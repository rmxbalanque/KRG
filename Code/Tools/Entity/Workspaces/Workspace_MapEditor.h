#pragma once

#include "Tools/Entity/EntityEditor/EntityEditor_BaseWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API EntityMapEditor final : public EntityEditorBaseWorkspace
    {
    public:

        EntityMapEditor( WorkspaceInitializationContext const& context, EntityWorld* pWorld );

        inline bool HasLoadedMap() const { return m_loadedMap.IsValid(); }
        inline ResourceID GetLoadedMap() const { return m_loadedMap; }

        void CreateNewMap();
        void SelectAndLoadMap();
        void LoadMap( TResourcePtr<EntityModel::EntityMapDescriptor> const& mapToLoad );
        void SaveMap();
        void SaveMapAs();

        void OnGamePreviewStarted();
        void OnGamePreviewEnded();

    private:

        virtual uint32 GetID() const override { return 0xFFFFFFFF; }
        virtual bool IsDirty() const override{ return false; } // TODO
        virtual bool Save() override;

    private:

        ResourceID                                      m_loadedMap;
        bool                                            m_isGamePreviewRunning = false;
    };
}