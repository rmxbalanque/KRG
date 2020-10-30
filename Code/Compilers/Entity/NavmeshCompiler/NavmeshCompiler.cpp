#include "NavmeshCompiler.h"
#include "Tools/Entity/ToolEntityCollectionConverter.h"
#include "Tools/Entity/ToolEntityCollection.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/error/en.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Platform/Platform/Platform_Win32.h"
#include "Engine/Physics/Components/PhysicsGeometryComponent.h"
#include "Tools/Entity/Serialization/EntityCollectionReader.h"
#include "Tools/Entity/Serialization/EntityCollectionWriter.h"

#include "System/TypeSystem/CoreTypeIDs.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    NavmeshCompiler::NavmeshCompiler()
        : Resource::Compiler( "NavmeshCompiler", VERSION )
    {
        m_outputTypes.push_back( 'NAV' );
    }

    Resource::CompilationResult NavmeshCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        NavmeshResourceDescriptor resourceDescriptor;
        if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
        {
            return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
        }

        // Validate map resource ptr
        //-------------------------------------------------------------------------

        if ( !resourceDescriptor.m_map.IsValid() )
        {
            return Error( "Invalid source map : %s", resourceDescriptor.m_map.GetResourceID().c_str() );
        }

        // Read file
        //-------------------------------------------------------------------------
        
        DataPath const& mapDataPath = resourceDescriptor.m_map.GetResourceID().GetDataPath();
        FileSystemPath const mapFilePath = DataPath::ToFileSystemPath( ctx.m_sourceDataPath, mapDataPath );

        EntityCollectionDescriptor entityCollection;
        if ( !EntityCollectionReader::ReadCollection( ctx.m_typeRegistry, mapFilePath, entityCollection ) )
        {
            return Resource::CompilationResult::Failure;
        }

        //-------------------------------------------------------------------------

        ToolEntityCollection toolEntityCollection( ctx.m_typeRegistry );
        if ( !ToolEntityCollectionConverter::ConvertToToolsFormat( ctx.m_typeRegistry, entityCollection, toolEntityCollection ) )
        {
            return Resource::CompilationResult::Failure;
        }

        EntityCollectionDescriptor entityCollectionTest;
        if ( !ToolEntityCollectionConverter::ConvertFromToolsFormat( ctx.m_typeRegistry, toolEntityCollection, entityCollectionTest ) )
        {
            return Resource::CompilationResult::Failure;
        }

        // Collect all collision geometry
        //-------------------------------------------------------------------------

        THashMap<DataPath, TVector<Transform>> CollisionGeometry;

        auto foundPhysicsComponents = toolEntityCollection.GetAllComponentsOfType( Physics::PhysicsGeometryComponent::GetStaticTypeID() );
        for ( auto pPhysicsComponent : foundPhysicsComponents )
        {
            auto pProperty = pPhysicsComponent->GetProperty( TypeSystem::PropertyPath( "m_pPhysicsGeometry" ) );
            KRG_ASSERT( pProperty != nullptr );

            Resource::ResourcePtr geometryPtr = pProperty->GetValue<Resource::ResourcePtr>();
            if ( geometryPtr.IsValid() )
            {
                CollisionGeometry[geometryPtr.GetResourceID().GetDataPath()].push_back( pPhysicsComponent->GetWorldTransform() );
            }
        }

        // Serialize
        //-------------------------------------------------------------------------

        /*FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::Archive::Binary archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( VERSION, EntityMap::GetStaticResourceTypeID() ) << map;
            return CompilationSucceeded( ctx );
        }
        else*/
        {
            return CompilationFailed( ctx );
        }
    }
}
