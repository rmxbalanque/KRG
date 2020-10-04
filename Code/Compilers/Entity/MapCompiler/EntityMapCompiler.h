#pragma once

#include "Tools/Entity/ToolEntityCollection.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "Tools/Core/TypeSystem/DynamicTypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityMap;

    //-------------------------------------------------------------------------

    class EntityMapCompiler : public Resource::Compiler
    {
        static const S32 VERSION = 1;

    public:

        EntityMapCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    private:

        inline void ConvertProperties( TVector<Byte>& propertyDataBuffer, TVector<Serialization::SerializedDynamicPropertyValue> const& stringProperties, TVector<Serialization::SerializedPropertyValue>& outProperties ) const
        {
            for ( auto const& stringProperty : stringProperties )
            {
                if ( stringProperty.IsOnlyRelevantForTools() )
                {
                    continue;
                }

                TypeSystem::ConvertStringToByteArray( stringProperty.m_typeID, stringProperty.m_value, propertyDataBuffer );
                outProperties.push_back( Serialization::SerializedPropertyValue( stringProperty.m_path, propertyDataBuffer ) );
            }
        };

        inline bool ConvertComponent( TypeSystem::TypeRegistry const& typeRegistry, TVector<Byte>& propertyDataBuffer, ToolEntityComponent const& component, TVector<Serialization::SerializedEntityComponent>& components ) const
        {
            KRG_ASSERT( !component.IsSpatialComponent() && component.GetChildComponents().empty() );

            auto& serializedComponent = components.emplace_back( Serialization::SerializedEntityComponent() );
            serializedComponent.m_ID = component.GetID();
            serializedComponent.m_name = component.GetName();
            serializedComponent.m_typeID = component.GetTypeID();

            Serialization::SerializedDynamicType serializedType;
            if ( !SerializeDynamicType( typeRegistry, component.GetTypeInstance(), serializedType ) )
            {
                Error( "Failed to reflect component %s", serializedComponent.m_name.ToString() );
                return false;
            }

            ConvertProperties( propertyDataBuffer, serializedType.m_properties, serializedComponent.m_propertyValues );
            return true;
        }

        inline bool ConvertSpatialComponent( TypeSystem::TypeRegistry const& typeRegistry, TVector<Byte>& propertyDataBuffer, ToolEntityComponent const& component, UUID const& parentComponentID, TVector<Serialization::SerializedSpatialEntityComponent>& serializedComponents ) const
        {
            KRG_ASSERT( component.IsSpatialComponent() );

            auto& serializedSpatialComponent = serializedComponents.emplace_back( Serialization::SerializedSpatialEntityComponent() );
            serializedSpatialComponent.m_ID = component.GetID();
            serializedSpatialComponent.m_name = component.GetName();
            serializedSpatialComponent.m_typeID = component.GetTypeID();
            serializedSpatialComponent.m_spatialParentID = parentComponentID;
            serializedSpatialComponent.m_attachmentSocketID = component.GetAttachmentSocketID();

            Serialization::SerializedDynamicType serializedType;
            if ( !SerializeDynamicType( typeRegistry, component.GetTypeInstance(), serializedType ) )
            {
                Error( "Failed to reflect component %s", serializedSpatialComponent.m_name.ToString() );
                return false;
            }

            ConvertProperties( propertyDataBuffer, serializedType.m_properties, serializedSpatialComponent.m_propertyValues );

            //-------------------------------------------------------------------------

            for ( auto const& childComponent : component.GetChildComponents() )
            {
                ConvertSpatialComponent( typeRegistry, propertyDataBuffer, childComponent, component.GetID(), serializedComponents );
            }

            return true;
        }

        inline bool ConvertEntity( TypeSystem::TypeRegistry const& typeRegistry, Serialization::EntityArchive& entityArchive, TVector<Byte>& propertyDataBuffer, ToolEntity const& entity, UUID const& parentEntityID ) const
        {
            Serialization::SerializedEntity serializedEntity;
            serializedEntity.m_ID = entity.GetID();
            serializedEntity.m_name = entity.GetName();
            serializedEntity.m_spatialParentID = parentEntityID;
            serializedEntity.m_attachmentSocketID = entity.GetAttachmentSocketID();

            for ( auto const& toolSystem : entity.GetSystems() )
            {
                KRG_ASSERT( toolSystem.IsValid() );

                Serialization::SerializedEntitySystem serializedSystem;
                serializedSystem.m_typeID = toolSystem.GetTypeID();
                serializedEntity.m_systems.push_back( serializedSystem );
            }

            for ( auto const& toolComponent : entity.GetComponents() )
            {
                if ( toolComponent.IsSpatialComponent() )
                {
                    if ( !ConvertSpatialComponent( typeRegistry, propertyDataBuffer, toolComponent, UUID(), serializedEntity.m_spatialComponents ) )
                    {
                        return false;
                    }
                }
                else
                {
                    if ( !ConvertComponent( typeRegistry, propertyDataBuffer, toolComponent, serializedEntity.m_components ) )
                    {
                        return false;
                    }
                }
            }

            // Store entity
            //-------------------------------------------------------------------------

            entityArchive << serializedEntity;

            // Process child entities
            //-------------------------------------------------------------------------
            // Child entity MUST always be serialized AFTER the parent

            for ( auto const& childEntity : entity.GetChildEntities() )
            {
                ConvertEntity( typeRegistry, entityArchive, propertyDataBuffer, childEntity, entity.GetID() );
            }

            return true;
        }
    };
}
