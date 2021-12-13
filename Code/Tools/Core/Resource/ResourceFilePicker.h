#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceDatabase;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API ResourceFilePicker final
    {
        struct PickerOption
        {
            PickerOption( ResourceID const& resourceID );

            ResourceID                  m_resourceID;
            String                      m_filename;
            String                      m_parentFolder;
        };

    public:

        ResourceFilePicker( ResourceDatabase const& database );
        ~ResourceFilePicker();

        void SetInitialResourceID( ResourceID const& initialID );

        // Set file restrictions
        void SetTypeFilter( ResourceTypeID allowedResourceType );
        void ClearTypeFilter();

        // Draws the resource field - returns true if the value is changed
        bool Draw();

        // Get the result of the file selection
        ResourceID const& GetPickedResourceID() const { return m_resourceID; }

    private:

        void RefreshResourceList();

        // Draw the selection dialog, returns true if the dialog is closed
        bool DrawDialog();

    private:

        ResourceDatabase const&         m_database;
        ResourceID                      m_resourceID;
        ResourceTypeID                  m_allowedResourceTypeID;
        char                            m_filterBuffer[256];
        EventBindingID                  m_databaseUpdateEventBindingID;
        TVector<PickerOption>           m_knownResourceIDs;
        TVector<PickerOption>           m_filteredResourceIDs;
        ResourceID                      m_selectedID;
    };
}