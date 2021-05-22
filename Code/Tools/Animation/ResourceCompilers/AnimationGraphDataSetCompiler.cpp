#include "AnimationGraphDataSetCompiler.h"
#include "Engine/Animation/Graph/AnimationGraphDataset.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "Engine/Animation/Graph/AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationGraphDataSetCompiler::AnimationGraphDataSetCompiler()
        : Resource::Compiler( "GraphDataSetCompiler", s_version )
    {
        m_outputTypes.push_back( AnimationGraphDataSet::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult AnimationGraphDataSetCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        if ( !ctx.m_inputFilePath.Exists() )
        {
            return Error( "Referenced data set file doesnt exist: %s", ctx.m_inputFilePath.c_str() );
        }

        // Read json data
        //-------------------------------------------------------------------------

        JsonFileReader dataSetReader;
        if ( !dataSetReader.ReadFromFile( ctx.m_inputFilePath ) )
        {
            return Error( "Failed to data set file: %s", ctx.m_inputFilePath.c_str() );
        }

        if ( !dataSetReader.GetDocument().IsObject() )
        {
            return Error( "Malformed data set file: %s", ctx.m_inputFilePath.c_str() );
        }

        auto const& document = dataSetReader.GetDocument();

        //-------------------------------------------------------------------------
        // Read data set
        //-------------------------------------------------------------------------

        AnimationGraphDataSet dataSet;

        // Get data set info
        //-------------------------------------------------------------------------

        auto nameIter = document.FindMember( "Name" );
        if ( nameIter == document.MemberEnd() || !nameIter->value.IsString() )
        {
            return Error( "Missing Name string" );
        }

        auto animGraphDataPathIter = document.FindMember( "Graph" );
        if ( animGraphDataPathIter == document.MemberEnd() || !animGraphDataPathIter->value.IsString() )
        {
            return Error( "Missing Graph datapath" );
        }

        if ( !DataPath::IsValidDataPath( animGraphDataPathIter->value.GetString() ) )
        {
            return Error( "Invalid data path format for Graph string" );
        }

        ResourceID const animGraphResourceID( animGraphDataPathIter->value.GetString() );
        if ( animGraphResourceID.GetResourceTypeID() != AnimationGraph::GetStaticResourceTypeID() )
        {
            return Error( "Graph data path set to a non-animgraph resource" );
        }

        auto skeletonDataPathIter = document.FindMember( "Skeleton" );
        if ( skeletonDataPathIter == document.MemberEnd() || !skeletonDataPathIter->value.IsString() )
        {
            return Error( "Missing Skeleton string" );
        }

        if ( !DataPath::IsValidDataPath( skeletonDataPathIter->value.GetString() ) )
        {
            return Error( "Invalid data path format for Skeleton string" );
        }

        ResourceID const skeletonResourceID( skeletonDataPathIter->value.GetString() );
        if ( skeletonResourceID.GetResourceTypeID() != Skeleton::GetStaticResourceTypeID() )
        {
            return Error( "Skeleton data path set to a non-skeleton resource" );
        }

        dataSet.m_name = StringID( nameIter->value.GetString() );
        dataSet.m_pSkeleton = skeletonResourceID;

        // Read animations
        //-------------------------------------------------------------------------

        auto dataArrayIter = document.FindMember( "Data" );
        if ( dataArrayIter == document.MemberEnd() )
        {
            return Error( "Missing data array member" );
        }

        auto const& dataArray = dataArrayIter->value;
        if ( !dataArray.IsArray() )
        {
            return Error( "Missing data array member" );
        }

        for ( auto& dataValue : dataArray.GetArray() )
        {
            auto itemIDiter = dataValue.FindMember( "ID" );
            if ( itemIDiter == dataValue.MemberEnd() || !itemIDiter->value.IsString() )
            {
                return Error( "Missing item ID string" );
            }

            if ( !UUID::IsValidUUIDString( itemIDiter->value.GetString() ) )
            {
                return Error( "Invalid GUID format for item ID string" );
            }

            auto itemDataPathIter = dataValue.FindMember( "DataPath" );
            if ( itemDataPathIter == dataValue.MemberEnd() || !itemDataPathIter->value.IsString() )
            {
                return Error( "Missing item DataPath string" );
            }

            if ( !DataPath::IsValidDataPath( itemDataPathIter->value.GetString() ) )
            {
                return Error( "Invalid data path format for DataPath string" );
            }

            ResourceID const animResourceID( itemDataPathIter->value.GetString() );
            if ( animResourceID.GetResourceTypeID() != AnimationClip::GetStaticResourceTypeID() )
            {
                return Error( "Only data paths to animation clips are allowed in graph data set" );
            }

            dataSet.m_animationClips.emplace_back( animResourceID );
        }

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            Resource::ResourceHeader hdr( s_version, AnimationGraphDataSet::GetStaticResourceTypeID() );

            hdr.AddInstallDependency( dataSet.m_pSkeleton.GetResourceID() );

            for ( auto const& dataRecord : dataSet.m_animationClips )
            {
                hdr.AddInstallDependency( dataRecord.GetResourceID() );
            }

            archive << hdr << dataSet;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}