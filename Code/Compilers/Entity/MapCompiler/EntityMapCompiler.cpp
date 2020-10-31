#include "EntityMapCompiler.h"
#include "ResourceBuilders/NavmeshBuilder.h"
#include "Tools/Entity/Serialization/EntityCollectionReader.h"
#include "System/Entity/Map/EntityMapDescriptor.h"
#include "Engine/Navmesh/Components/NavmeshComponent.h"
#include "System/Navmesh/NavmeshData.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapCompiler::EntityMapCompiler()
        : Resource::Compiler( "EntityMapCompiler", VERSION )
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

        if ( !EntityCollectionReader::ReadCollection( ctx.m_typeRegistry, ctx.m_inputFilePath, map.m_collectionTemplate ) )
        {
            return Resource::CompilationResult::Failure;
        }

        //-------------------------------------------------------------------------
        // Additional Resources
        //-------------------------------------------------------------------------

        auto const navmeshComponents = map.m_collectionTemplate.GetComponentsOfType<Navmesh::NavmeshComponent>( ctx.m_typeRegistry, false );

        // If we have a navmesh component 
        if ( !navmeshComponents.empty() )
        {
            // Log warning about invalid data
            if ( navmeshComponents.size() > 1 )
            {
                Warning( "More than one navmesh component detected!" );
            }

            // Remove any values for the navmesh resource property
            TypeSystem::PropertyPath const navmeshResourcePropertyPath( "m_pNavmeshData" );
            for ( auto i = navmeshComponents.size(); i > 0; i-- )
            {
                navmeshComponents[0]->RemovePropertyValue( navmeshResourcePropertyPath );
            }

            // Set navmesh resource ptr
            DataPath navmeshResourceDataPath = ctx.m_resourceID.GetDataPath();
            navmeshResourceDataPath.ReplaceExtension( Navmesh::NavmeshData::GetStaticResourceTypeID().ToString() );
            navmeshComponents[0]->m_propertyValues.emplace_back( PropertyDescriptor( navmeshResourcePropertyPath, TypeSystem::GetCoreTypeID( TypeSystem::CoreTypes::TResourcePtr ), navmeshResourceDataPath.c_str() ) );

            // Generate navmesh
            auto navmeshResourcePath = ctx.m_outputFilePath;
            navmeshResourcePath.ReplaceExtension( Navmesh::NavmeshData::GetStaticResourceTypeID().ToString() );

            Navmesh::NavmeshBuilder navmeshBuilder;
            navmeshBuilder.Build( ctx, map.GetCollection(), navmeshResourcePath );
        }

        //-------------------------------------------------------------------------
        // Serialize
        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( ctx.m_outputFilePath );
        Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
        if ( archive.IsValid() )
        {
            archive << Resource::ResourceHeader( VERSION, EntityMapDescriptor::GetStaticResourceTypeID() ) << map;
            return CompilationSucceeded( ctx );
        }
        else
        {
            return CompilationFailed( ctx );
        }
    }
}
