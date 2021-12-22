#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Resource/ResourceID.h"

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

        // Get the raw resource data path
        FileSystem::Path const& GetRawResourceDirectoryPath() const;

        // Draws the resource field - returns true if the value is changed
        bool DrawPicker( ResourceTypeID resourceTypeID, ResourceID const* pResourceID );

        // Get the selected ID - Only use if DrawPicker returns true
        inline ResourceID const GetSelectedResourceID() const { return m_selectedID; }

    private:

        // Draw the selection dialog, returns true if the dialog is closed
        bool DrawDialog( ResourceTypeID resourceTypeID, ResourceID const* pResourceID );

        void RefreshResourceList( ResourceTypeID resourceTypeID );

    private:

        ResourceDatabase const&         m_database;
        char                            m_filterBuffer[256];
        TVector<PickerOption>           m_knownResourceIDs;
        TVector<PickerOption>           m_filteredResourceIDs;
        ResourceID                      m_selectedID;
        bool                            m_initializeFocus = false;
    };
}