#include "ResourceCompiler_EntityMap.h"
#include "ResourceBuilders/NavmeshBuilder.h"
#include "Tools/Entity/Serialization/EntityCollectionDescriptorReader.h"
#include "Engine/Core/Entity/Map/EntityMapDescriptor.h"
#include "Engine/Navmesh/Components/NavmeshComponent.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Time/Timers.h"
#include "Engine/Core/Entity/Collections/EntityCollection.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapCompiler::EntityMapCompiler()
        : Resource::Compiler( "EntityMapCompiler", s_version )
    {
        m_outputTypes.push_back( EntityMapDescriptor::GetStaticResourceTypeID() );
        m_virtualTypes.push_back( Navmesh::NavmeshData::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult EntityMapCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        EntityMapDescriptor map;
        map.m_ID = UUID::GenerateID();

        //-------------------------------------------------------------------------
        // Read collection
        //-------------------------------------------------------------------------

        Milliseconds elapsedTime = 0.0f;
        {
            ScopedSystemTimer timer( elapsedTime );

            if ( !EntityCollectionDescriptorReader::Read( ctx.m_typeRegistry, ctx.m_inputFilePath, map.m_collectionDescriptor ) )
            {
                return Resource::CompilationResult::Failure;
            }
        }
        Message( "Entity map read in: %.2fms", elapsedTime.ToFloat() );

        //-------------------------------------------------------------------------
        // Additional Resources
        //-------------------------------------------------------------------------

        auto const navmeshComponents = map.m_collectionDescriptor.GetComponentsOfType<Navmesh::NavmeshComponent>( ctx.m_typeRegistry, false );

        // If we have a navmesh component 
        if ( !navmeshComponents.empty() )
        {
            {
                ScopedSystemTimer timer( elapsedTime );

                // Log warning about invalid data
                if ( navmeshComponents.size() > 1 )
                {
                    Warning( "More than one navmesh component detected!" );
                }

                // Remove any values for the navmesh resource property
                // TODO: see if there is a smart way to avoid using strings for property access
                TypeSystem::PropertyPath const navmeshResourcePropertyPath( "m_pNavmeshData" );
                for ( auto i = navmeshComponents.size(); i > 0; i-- )
                {
                    navmeshComponents[0].m_pComponent->RemovePropertyValue( navmeshResourcePropertyPath );
                }

                // Set navmesh resource ptr
                ResourcePath navmeshResourcePath = ctx.m_resourceID.GetResourcePath();
                navmeshResourcePath.ReplaceExtension( Navmesh::NavmeshData::GetStaticResourceTypeID().ToString() );
                navmeshComponents[0].m_pComponent->m_properties.emplace_back( TypeSystem::PropertyDescriptor( ctx.m_typeRegistry, navmeshResourcePropertyPath, GetCoreTypeID( TypeSystem::CoreTypes::TResourcePtr ), TypeSystem::TypeID(), navmeshResourcePath.GetString() ) );

                // Generate navmesh
                FileSystem::Path navmeshFilePath = ctx.m_outputFilePath;
                navmeshFilePath.ReplaceExtension( Navmesh::NavmeshData::GetStaticResourceTypeID().ToString() );

                Navmesh::NavmeshBuilder navmeshBuilder;
                if ( !navmeshBuilder.Build( ctx, map.GetCollectionDescriptor(), navmeshFilePath ) )
                {
                    return Resource::CompilationResult::Failure;
                }
            }
            Message( "Navmesh built in: %.2fms", elapsedTime.ToFloat() );
        }

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( s_version, EntityMapDescriptor::GetStaticResourceTypeID() ) << map;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}
