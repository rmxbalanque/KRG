#include "ResourceCompiler_EntityCollection.h"
#include "ResourceBuilders/NavmeshBuilder.h"
#include "Engine/Core/Entity/EntityDescriptors.h"
#include "Engine/Core/Entity/EntitySerialization.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityCollectionCompiler::EntityCollectionCompiler()
        : Resource::Compiler( "EntityMapCompiler", s_version )
    {
        m_outputTypes.push_back( EntityCollectionDescriptor::GetStaticResourceTypeID() );
        m_outputTypes.push_back( EntityMapDescriptor::GetStaticResourceTypeID() );
        m_virtualTypes.push_back( Navmesh::NavmeshData::GetStaticResourceTypeID() );
    }

    Resource::CompilationResult EntityCollectionCompiler::Compile( Resource::CompileContext const& ctx ) const
    {
        if ( ctx.m_resourceID.GetResourceTypeID() == EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            return CompileMap( ctx );
        }

        return CompileCollection( ctx );
    }

    Resource::CompilationResult EntityCollectionCompiler::CompileCollection( Resource::CompileContext const& ctx ) const
    {
        EntityCollectionDescriptor collectionDesc;

        //-------------------------------------------------------------------------
        // Read collection
        //-------------------------------------------------------------------------

        Milliseconds elapsedTime = 0.0f;
        {
            ScopedTimer<PlatformClock> timer( elapsedTime );

            if ( !Serialization::ReadEntityCollectionFromFile( ctx.m_typeRegistry, ctx.m_inputFilePath, collectionDesc ) )
            {
                return Resource::CompilationResult::Failure;
            }
        }
        Message( "Entity collection read in: %.2fms", elapsedTime.ToFloat() );

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        KRG::Serialization::BinaryFileArchive archive( KRG::Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( s_version, EntityCollectionDescriptor::GetStaticResourceTypeID() ) << collectionDesc;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }

    Resource::CompilationResult EntityCollectionCompiler::CompileMap( Resource::CompileContext const& ctx ) const
    {
        EntityMapDescriptor map;

        //-------------------------------------------------------------------------
        // Read collection
        //-------------------------------------------------------------------------

        Milliseconds elapsedTime = 0.0f;
        {
            ScopedTimer<PlatformClock> timer( elapsedTime );

            if ( !Serialization::ReadEntityCollectionFromFile( ctx.m_typeRegistry, ctx.m_inputFilePath, map ) )
            {
                return Resource::CompilationResult::Failure;
            }
        }
        Message( "Entity map read in: %.2fms", elapsedTime.ToFloat() );

        //-------------------------------------------------------------------------
        // Navmesh
        //-------------------------------------------------------------------------

        #if KRG_NAVPOWER
        auto const navmeshComponents = map.GetComponentsOfType<Navmesh::NavmeshComponent>( ctx.m_typeRegistry, false );
        if ( !navmeshComponents.empty() )
        {
            {
                ScopedTimer<PlatformClock> timer( elapsedTime );

                // Log warning about invalid data
                if ( navmeshComponents.size() > 1 )
                {
                    Warning( "More than one navmesh component detected! Only using the first one!" );
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
                navmeshComponents[0].m_pComponent->m_properties.emplace_back( TypeSystem::PropertyDescriptor( ctx.m_typeRegistry, navmeshResourcePropertyPath, GetCoreTypeID( TypeSystem::CoreTypeID::TResourcePtr ), TypeSystem::TypeID(), navmeshResourcePath.GetString() ) );

                // Create navmesh component and generate navmesh
                auto pNavmeshComponent = navmeshComponents[0].m_pComponent->CreateTypeInstance<Navmesh::NavmeshComponent>( ctx.m_typeRegistry );
                KRG_ASSERT( pNavmeshComponent != nullptr );

                // Generate navmesh
                FileSystem::Path navmeshFilePath = ctx.m_outputFilePath;
                navmeshFilePath.ReplaceExtension( Navmesh::NavmeshData::GetStaticResourceTypeID().ToString() );

                Navmesh::NavmeshBuilder navmeshBuilder;
                bool const navmeshBuildResult = navmeshBuilder.Build( ctx, map, navmeshFilePath, pNavmeshComponent );


                // Delete component instance
                KRG::Delete( pNavmeshComponent );

                if ( !navmeshBuildResult )
                {
                    return Resource::CompilationResult::Failure;
                }
            }
            Message( "Navmesh built in: %.2fms", elapsedTime.ToFloat() );
        }
        #endif

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        KRG::Serialization::BinaryFileArchive archive( KRG::Serialization::Mode::Write, ctx.m_outputFilePath );
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
