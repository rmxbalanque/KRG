#include "PropertyGrid.h"
#include "PropertyEditors.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    PropertyGrid::PropertyGrid( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {
        m_pTypeInstanceModel = KRG::New<TypeInstanceModel>( m_typeRegistry, m_typeRegistry.GetTypeInfo( TypeID( "KRG::TestComponent" ) ) );
    }

    PropertyGrid::~PropertyGrid()
    {
        KRG::Delete( m_pTypeInstanceModel );
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::SetTypeToEdit( TypeInstanceModel* pTypeInstance )
    {
        //m_pTypeInstance = pTypeInstance;
        m_isDirty = false;
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::DrawGrid()
    {
        if ( m_pTypeInstanceModel == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGuiTableFlags const flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH | ImGuiTableFlags_Resizable;
        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 4 ) );
        if ( ImGui::BeginTable( "PropertyGrid", 2, flags ) )
        {
            ImGui::TableSetupColumn( "Property", ImGuiTableColumnFlags_NoHide );
            ImGui::TableSetupColumn( "##Value", ImGuiTableColumnFlags_NoHide );
            ImGui::TableHeadersRow();

            //-------------------------------------------------------------------------

            for ( auto& prop : m_pTypeInstanceModel->GetProperties() )
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text( prop.GetFriendlyName() );

                ImGui::TableNextColumn();
                PG::CreatePropertyEditor( prop );
            }
            
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

    
}