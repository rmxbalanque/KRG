#pragma once

#include "../_Module/API.h"
#include "System/Core/Math/Range.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/LoadingStatus.h"

//-------------------------------------------------------------------------

namespace KRG { class Entity; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct LoadingContext;

    //-------------------------------------------------------------------------
    // Internal loading group helper
    //-------------------------------------------------------------------------
    
    class EntityLoader
    {
        enum class Operation
        {
            Invalid = -1,
            Load,
            Unload,
            Reload
        };

    public:

        static EntityLoader Load( EntityModel::LoadingContext const& loadingContext, TVector<Entity*> const& entities ) { return EntityLoader( loadingContext, entities, Operation::Load ); }
        static EntityLoader Load( EntityModel::LoadingContext const& loadingContext, Entity* pEntity ) { return EntityLoader( loadingContext, pEntity, Operation::Load ); }
        static EntityLoader Unload( EntityModel::LoadingContext const& loadingContext, TVector<Entity*> const& entities ) { return EntityLoader( loadingContext, entities, Operation::Unload ); }
        static EntityLoader Unload( EntityModel::LoadingContext const& loadingContext, Entity* pEntity ) { return EntityLoader( loadingContext, pEntity, Operation::Unload ); }
        static EntityLoader Reload( EntityModel::LoadingContext const& loadingContext, TVector<Entity*> const& entities ) { return EntityLoader( loadingContext, entities, Operation::Reload ); }
        static EntityLoader Reload( EntityModel::LoadingContext const& loadingContext, Entity* pEntity ) { return EntityLoader( loadingContext, pEntity, Operation::Reload ); }

    public:

        EntityLoader( EntityLoader&& loader );
        virtual ~EntityLoader();

        // Loading Status
        inline bool IsLoading() const { return m_loadingStatus == LoadingStatus::Loading; }
        inline bool IsLoaded() const { return m_loadingStatus == LoadingStatus::Loaded; }
        inline bool IsUnloading() const { return m_loadingStatus == LoadingStatus::Unloading; }
        inline bool IsUnloaded() const { return m_loadingStatus == LoadingStatus::Unloaded || m_loadingStatus == LoadingStatus::Failed; }

        // Get the entities that the loader is operating on
        inline TVector<Entity*> const& GetEntities() const { KRG_ASSERT( !IsLoading() ); return m_entities; }

        // This will cancel any active loads and unload all entities that have been previously loaded
        void CancelOperation( LoadingContext const& loadingContext ) { Unload( loadingContext ); }

        // Update the loading status for the loader
        LoadingStatus UpdateLoading( LoadingContext const& loadingContext );

        // Reloading
        //-------------------------------------------------------------------------

        inline bool IsReloadOperation() const { return m_operation == Operation::Reload; }

        inline void SetEntitiesToReactivate( TInlineVector<Entity*, 5> const& entitiesToReactivate )
        {
            KRG_ASSERT( IsReloadOperation() );
            m_entitiesToReactivate = entitiesToReactivate;
        }

        inline TInlineVector<Entity*, 5> const& GetEntitiesToReactivate() const
        { 
            KRG_ASSERT( IsReloadOperation() );
            return m_entitiesToReactivate; 
        }

    private:
    
        EntityLoader( LoadingContext const& loadingContext, TVector<Entity*> const& entities, Operation operation );
        EntityLoader( LoadingContext const& loadingContext, Entity* pEntity, Operation operation );

        void Initialize( LoadingContext const& loadingContext );
        void Load( LoadingContext const& loadingContext );
        void Unload( LoadingContext const& loadingContext );

    private:

        TVector<Entity*>                    m_entities;
        TVector<Entity*>                    m_activeEntityLoadRequests;
        TInlineVector<Entity*, 5>           m_entitiesToReactivate;
        LoadingStatus                       m_loadingStatus = LoadingStatus::Unloaded;
        Operation                           m_operation = Operation::Invalid;
    };
}