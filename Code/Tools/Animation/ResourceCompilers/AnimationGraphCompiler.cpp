#include "AnimationGraphCompiler.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Graph.h"
#include "Tools/Animation/Graph/AnimationGraphTools_AnimationGraph.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace Graph;

    //-------------------------------------------------------------------------

    AnimationGraphCompiler::AnimationGraphCompiler()
        : Resource::Compiler( "GraphCompiler", s_version )
    {
        m_outputTypes.push_back( AnimationGraphDefinition::GetStaticResourceTypeID() );
        m_outputTypes.push_back( AnimationGraphVariation::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationGraphCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        auto const resourceTypeID = ctx.m_resourceID.GetResourceTypeID();
        if ( resourceTypeID == AnimationGraphDefinition::GetStaticResourceTypeID() )
        {
            return CompileDefinition( ctx );
        }
        else if ( resourceTypeID == AnimationGraphVariation::GetStaticResourceTypeID() )
        {
            return CompileVariation( ctx );
        }

        KRG_UNREACHABLE_CODE();
        return CompilationFailed( ctx );
    }

    Resource::CompilationResult AnimationGraphCompiler::CompileDefinition( Resource::CompileContext const& ctx ) const
    {
        JsonFileReader jsonReader;
        if ( !jsonReader.ReadFromFile( ctx.m_inputFilePath ) )
        {
            return Error( "Failed to read animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        ToolsAnimationGraph toolsGraph;
        if ( !toolsGraph.Load( ctx.m_typeRegistry, jsonReader.GetDocument() ) )
        {
            return Error( "Malformed animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Compile
        //-------------------------------------------------------------------------

        ToolsGraphCompilationContext context;
        if ( !toolsGraph.Compile( context ) )
        {
            // Dump log
            for ( auto const& logEntry : context.GetLog() )
            {
                if ( logEntry.m_severity == Log::Severity::Error )
                {
                    Error( "%s", logEntry.m_message.c_str() );
                }
                else if ( logEntry.m_severity == Log::Severity::Warning )
                {
                    Warning( "%s", logEntry.m_message.c_str() );
                }
                else if ( logEntry.m_severity == Log::Severity::Message )
                {
                    Message( "%s", logEntry.m_message.c_str() );
                }
            }

            return Resource::CompilationResult::Failure;
        }

        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( s_version, AnimationGraphDefinition::GetStaticResourceTypeID() );
            archive << context;

            // Node settings type descs
            TypeSystem::TypeDescriptorCollection settingsTypeDescriptors;
            for ( auto pSettings : context.m_nodeSettings )
            {
                auto& desc = settingsTypeDescriptors.m_descriptors.emplace_back();
                TypeSystem::Serialization::CreateTypeDescriptorFromNativeType( ctx.m_typeRegistry, pSettings, desc );
            }
            archive << settingsTypeDescriptors;

            // Node settings data
            cereal::BinaryOutputArchive& settingsArchive = *archive.GetOutputArchive();
            for ( auto pSettings : context.m_nodeSettings )
            {
                pSettings->Save( settingsArchive );
            }

            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    Resource::CompilationResult AnimationGraphCompiler::CompileVariation( Resource::CompileContext const& ctx ) const
    {
        AnimationGraphVariationResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Load anim graph
        //-------------------------------------------------------------------------

        FileSystem::Path graphFilePath;
        if ( !ctx.ConvertDataPathToFilePath( resourceDescriptor.m_graphDataPath, graphFilePath ) )
        {
            return Error( "invalid graph data path: %s", resourceDescriptor.m_graphDataPath.c_str() );
        }

        JsonFileReader jsonReader;
        if ( !jsonReader.ReadFromFile( graphFilePath ) )
        {
            return Error( "Failed to read animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        ToolsAnimationGraph toolsGraph;
        if ( !toolsGraph.Load( ctx.m_typeRegistry, jsonReader.GetDocument() ) )
        {
            return Error( "Malformed animation graph file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Create requested data set resource
        //-------------------------------------------------------------------------

        StringID const variationID = resourceDescriptor.m_variationID.IsValid() ? resourceDescriptor.m_variationID : StringID( AnimationGraphVariation::s_pDefaultVariationName );
        String dataSetFileName;
        dataSetFileName.sprintf( "%s_%s.%s", graphFilePath.GetFileNameWithoutExtension().c_str(), variationID.c_str(), AnimationGraphDataSet::GetStaticResourceTypeID().ToString().c_str() );

        FileSystem::Path dataSetFilePath = graphFilePath.GetParentDirectory();
        dataSetFilePath.Append( dataSetFileName );
        DataPath const dataSetDataPath = DataPath::FromFileSystemPath( ctx.m_sourceDataPath, dataSetFilePath );

        if ( !GenerateVirtualDataSetResource( ctx, variationID, dataSetDataPath ) )
        {
            return Error( "Failed to create data set: %s", dataSetDataPath.c_str() );
        }

        // Create variation resource and serialize
        //-------------------------------------------------------------------------

        AnimationGraphVariation variation;
        variation.m_pGraphDefinition = ResourceID( resourceDescriptor.m_graphDataPath );
        variation.m_pDataSet = ResourceID( dataSetDataPath );

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, AnimationGraphDataSet::GetStaticResourceTypeID() );
            hdr.AddInstallDependency( variation.m_pDataSet.GetResourceID() );
            hdr.AddInstallDependency( variation.m_pGraphDefinition.GetResourceID() );

            archive << hdr;
            archive << variation;

            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    //-------------------------------------------------------------------------

    bool AnimationGraphCompiler::GenerateVirtualDataSetResource( Resource::CompileContext const& ctx, StringID const& variationID, DataPath const& dataSetPath ) const
    {
        //if ( !ctx.m_inputFilePath.Exists() )
        //{
        //    return Error( "Referenced data set file doesnt exist: %s", ctx.m_inputFilePath.c_str() );
        //}

        //// Read json data
        ////-------------------------------------------------------------------------

        //JsonFileReader dataSetReader;
        //if ( !dataSetReader.ReadFromFile( ctx.m_inputFilePath ) )
        //{
        //    return Error( "Failed to data set file: %s", ctx.m_inputFilePath.c_str() );
        //}

        //if ( !dataSetReader.GetDocument().IsObject() )
        //{
        //    return Error( "Malformed data set file: %s", ctx.m_inputFilePath.c_str() );
        //}

        //auto const& document = dataSetReader.GetDocument();

        ////-------------------------------------------------------------------------
        //// Read data set
        ////-------------------------------------------------------------------------

        //AnimationGraphDataSet dataSet;

        //// Get data set info
        ////-------------------------------------------------------------------------

        //auto nameIter = document.FindMember( "Name" );
        //if ( nameIter == document.MemberEnd() || !nameIter->value.IsString() )
        //{
        //    return Error( "Missing Name string" );
        //}

        //auto animGraphDataPathIter = document.FindMember( "Graph" );
        //if ( animGraphDataPathIter == document.MemberEnd() || !animGraphDataPathIter->value.IsString() )
        //{
        //    return Error( "Missing Graph datapath" );
        //}

        //if ( !DataPath::IsValidDataPath( animGraphDataPathIter->value.GetString() ) )
        //{
        //    return Error( "Invalid data path format for Graph string" );
        //}

        //ResourceID const animGraphResourceID( animGraphDataPathIter->value.GetString() );
        //if ( animGraphResourceID.GetResourceTypeID() != AnimationGraphDefinition::GetStaticResourceTypeID() )
        //{
        //    return Error( "Graph data path set to a non-animgraph resource" );
        //}

        //auto skeletonDataPathIter = document.FindMember( "Skeleton" );
        //if ( skeletonDataPathIter == document.MemberEnd() || !skeletonDataPathIter->value.IsString() )
        //{
        //    return Error( "Missing Skeleton string" );
        //}

        //if ( !DataPath::IsValidDataPath( skeletonDataPathIter->value.GetString() ) )
        //{
        //    return Error( "Invalid data path format for Skeleton string" );
        //}

        //ResourceID const skeletonResourceID( skeletonDataPathIter->value.GetString() );
        //if ( skeletonResourceID.GetResourceTypeID() != Skeleton::GetStaticResourceTypeID() )
        //{
        //    return Error( "Skeleton data path set to a non-skeleton resource" );
        //}

        //dataSet.m_name = StringID( nameIter->value.GetString() );
        //dataSet.m_pSkeleton = skeletonResourceID;

        //// Read animations
        ////-------------------------------------------------------------------------

        //auto dataArrayIter = document.FindMember( "Data" );
        //if ( dataArrayIter == document.MemberEnd() )
        //{
        //    return Error( "Missing data array member" );
        //}

        //auto const& dataArray = dataArrayIter->value;
        //if ( !dataArray.IsArray() )
        //{
        //    return Error( "Missing data array member" );
        //}

        //for ( auto& dataValue : dataArray.GetArray() )
        //{
        //    auto itemIDiter = dataValue.FindMember( "ID" );
        //    if ( itemIDiter == dataValue.MemberEnd() || !itemIDiter->value.IsString() )
        //    {
        //        return Error( "Missing item ID string" );
        //    }

        //    if ( !UUID::IsValidUUIDString( itemIDiter->value.GetString() ) )
        //    {
        //        return Error( "Invalid GUID format for item ID string" );
        //    }

        //    auto itemDataPathIter = dataValue.FindMember( "DataPath" );
        //    if ( itemDataPathIter == dataValue.MemberEnd() || !itemDataPathIter->value.IsString() )
        //    {
        //        return Error( "Missing item DataPath string" );
        //    }

        //    if ( !DataPath::IsValidDataPath( itemDataPathIter->value.GetString() ) )
        //    {
        //        return Error( "Invalid data path format for DataPath string" );
        //    }

        //    ResourceID const animResourceID( itemDataPathIter->value.GetString() );
        //    if ( animResourceID.GetResourceTypeID() != AnimationClip::GetStaticResourceTypeID() )
        //    {
        //        return Error( "Only data paths to animation clips are allowed in graph data set" );
        //    }

        //    dataSet.m_animationClips.emplace_back( animResourceID );
        //}

        ////-------------------------------------------------------------------------

        //FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        //Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        //if ( archive.IsValid() )
        //{
        //    Resource::ResourceHeader hdr( s_version, AnimationGraphDataSet::GetStaticResourceTypeID() );

        //    hdr.AddInstallDependency( dataSet.m_pSkeleton.GetResourceID() );

        //    for ( auto const& dataRecord : dataSet.m_animationClips )
        //    {
        //        hdr.AddInstallDependency( dataRecord.GetResourceID() );
        //    }

        //    archive << hdr << dataSet;
        //    return CompilationSucceeded( ctx );
        //}
        //else
        //{
        //    return CompilationFailed( ctx );
        //}

        return false;
    }
}
