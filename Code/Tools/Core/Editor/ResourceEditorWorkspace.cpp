#include "ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_DEFINE_GLOBAL_REGISTRY( ResourceEditorWorkspaceFactory );

    //-------------------------------------------------------------------------

    bool ResourceEditorWorkspaceFactory::CanCreateWorkspace( ResourceTypeID const& resourceTypeID )
    {
        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return true;
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return false;
    }

    EditorWorkspace* ResourceEditorWorkspaceFactory::TryCreateWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        auto resourceTypeID = resourceID.GetResourceTypeID();
        KRG_ASSERT( resourceTypeID.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return pCurrentFactory->CreateWorkspace( context, pWorld, resourceID );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return nullptr;
    }
}