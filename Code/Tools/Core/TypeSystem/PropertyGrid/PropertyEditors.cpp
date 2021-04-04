#include "PropertyEditors.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    void CreateEditorBool( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        bool v = 0; // Get Value
        if ( ImGui::Checkbox( labelBuffer, &v ) )
        {
            // Set Value 
        }
    }

    void CreateEditorInt( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        int32 v = 0; // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputInt( labelBuffer, &v ) )
        {
            // Set Value 
        }
    }

    void CreateEditorFloat( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        float v = 0; // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( labelBuffer, &v ) )
        {
            // Set Value 
        }
    }

    void CreateEditorDouble( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        double v = 0; // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputDouble( labelBuffer, &v ) )
        {
            // Set Value 
        }
    }

    void CreateEditorFloat2( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat2( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorFloat3( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorFloat4( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat4( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorEulerAngles( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorQuaternion( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat4( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorColor( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 1, 0, 0, 1 );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::ColorEdit4( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorTransform( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorMatrix( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat3( labelBuffer, &v.x ) )
        {
            // Set Value 
        }
    }

    void CreateEditorString( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputText( labelBuffer, labelBuffer, 256, ImGuiInputTextFlags_ReadOnly );
    }

    void CreateEditorStringID( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImGui::Text( "342432423423" );

        ImGui::SameLine();
        ImGui::InputText( labelBuffer, labelBuffer, 256, ImGuiInputTextFlags_ReadOnly );
    }

    void CreateEditorUUID( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImGui::InputText( labelBuffer, labelBuffer, 256, ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine();
        ImGui::Button( KRG_ICON_REFRESH );
    }

    void CreateEditorDataPath( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 

        ImGui::InputText( labelBuffer, labelBuffer, 256, ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine();
        ImGui::Button( KRG_ICON_CROSSHAIRS );
    }

    void CreateEditorResourceID( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 

        ImGui::Text( "SMSH" );

        ImGui::SameLine();
        ImGui::InputText( labelBuffer, labelBuffer, 256, ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine();
        ImGui::Button( KRG_ICON_CROSSHAIRS );
    }

    void CreateEditorResourcePtr( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImVec4 v( 0, 0, 0, 0 ); // Get Value 

        ImGui::Text( "SMSH" );

        ImGui::SameLine();
        ImGui::InputText( labelBuffer, labelBuffer, 256, ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine();
        ImGui::Button( KRG_ICON_CROSSHAIRS );
    }

    //-------------------------------------------------------------------------

    void CreateEditorEnum( PropertyInstanceModel& model )
    {
        char labelBuffer[256];
        Printf( labelBuffer, 256, "##%s", model.GetFriendlyName() );

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::BeginCombo( labelBuffer, "Test" ) )
        {
            ImGui::EndCombo();
        }
    }

    void CreateEditorDynamicArray( PropertyInstanceModel& model )
    {
        ImGui::Text( "DYNAMIC ARRAY" );
    }

    void CreateEditorStaticArray( PropertyInstanceModel& model )
    {
        ImGui::Text( "STATIC ARRAY" );
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    void CreatePropertyEditor( PropertyInstanceModel& model )
    {
        if ( model.IsArray() )
        {
            if ( model.IsDynamicArray() )
            {
                CreateEditorDynamicArray( model );
            }
            else
            {
                CreateEditorStaticArray( model );
            }
        }
        else if ( model.IsEnum() )
        {
            CreateEditorEnum( model );
        }
        else if ( model.IsCoreType() )
        {
            CoreTypes const coreType = GetCoreType( model.GetTypeID() );

            switch ( coreType )
            {
                case CoreTypes::Bool:
                {
                    CreateEditorBool( model );
                }
                break;

                case CoreTypes::Int32:
                {
                    CreateEditorInt( model );
                }
                break;

                case CoreTypes::Float:
                {
                    CreateEditorFloat( model );
                }
                break;

                case CoreTypes::Float2:
                {
                    CreateEditorFloat2( model );
                }
                break;

                case CoreTypes::Float3:
                {
                    CreateEditorFloat3( model );
                }
                break;

                case CoreTypes::Float4:
                {
                    CreateEditorFloat4( model );
                }
                break;

                case CoreTypes::Color:
                {
                    CreateEditorColor( model );
                }
                break;

                case CoreTypes::UUID:
                {
                    CreateEditorUUID( model );
                }
                break;

                case CoreTypes::String:
                {
                    CreateEditorString( model );
                }
                break;

                case CoreTypes::StringID:
                {
                    CreateEditorStringID( model );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    CreateEditorQuaternion( model );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    CreateEditorEulerAngles( model );
                }
                break;

                case CoreTypes::Transform:
                {
                    CreateEditorTransform( model );
                }
                break;

                case CoreTypes::Matrix:
                {
                    CreateEditorMatrix( model );
                }
                break;

                case CoreTypes::DataPath:
                {
                    CreateEditorDataPath( model );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    CreateEditorResourceID( model );
                }
                break;

                case CoreTypes::ResourcePtr:
                {
                    CreateEditorResourcePtr( model );
                }
                break;
            }
        }
        else // Compound Type
        {
            
        }
    }
}