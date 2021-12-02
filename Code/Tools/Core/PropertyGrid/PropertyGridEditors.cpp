#include "PropertyGridEditors.h"
#include "Tools/Core/Widgets/NumericEditors.h"
#include "Tools/Core/Resource/DataFilePicker.h"
#include "Tools/Core/Widgets/CurveEditor.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    constexpr static float const g_itemSpacing = 2.0f;
    constexpr static float const g_iconButtonWidth = 21.0f;

    //-------------------------------------------------------------------------

    PropertyEditor::PropertyEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
        : m_typeRegistry( typeRegistry )
        , m_propertyInfo( propertyInfo )
        , m_pPropertyInstance( m_pPropertyInstance )
        , m_coreType( GetCoreType( propertyInfo.m_typeID ) )
    {
        KRG_ASSERT( m_pPropertyInstance != nullptr );
    }

    bool PropertyEditor::UpdateAndDraw()
    {
        ImGui::PushID( m_pPropertyInstance );
        HandleExternalUpdate();
        bool const result = InternalUpdateAndDraw();
        ImGui::PopID();

        return result;
    }

    //-------------------------------------------------------------------------
    // Core Editors
    //-------------------------------------------------------------------------

    class EnumEditor final : public PropertyEditor
    {
    public:

        EnumEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
            , m_pEnumInfo( typeRegistry.GetEnumInfo( m_propertyInfo.m_typeID ) )
        {
            KRG_ASSERT( m_pEnumInfo != nullptr );
            EnumEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::BeginCombo( "##enumCombo", m_pEnumInfo->GetConstantLabel( m_value_imgui ).c_str() ) )
            {
                for ( auto const& enumValue : m_pEnumInfo->m_constants )
                {
                    bool const isSelected = ( enumValue.second == m_value_imgui );
                    if ( ImGui::Selectable( enumValue.first.c_str(), isSelected ) )
                    {
                        m_value_imgui = enumValue.second;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if ( isSelected )
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            return  m_value_cached != m_value_imgui;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;

            switch ( m_pEnumInfo->m_underlyingType )
            {
                case CoreTypeID::Uint8:
                {
                    *reinterpret_cast<uint8*>( m_pPropertyInstance ) = (uint8) m_value_cached;
                }
                break;

                case CoreTypeID::Int8:
                {
                    *reinterpret_cast<int8*>( m_pPropertyInstance ) = (int8) m_value_cached;
                }
                break;

                case CoreTypeID::Uint16:
                {
                    *reinterpret_cast<uint16*>( m_pPropertyInstance ) = (uint16) m_value_cached;
                }
                break;

                case CoreTypeID::Int16:
                {
                    *reinterpret_cast<int16*>( m_pPropertyInstance ) = (int16) m_value_cached;
                }
                break;

                case CoreTypeID::Uint32:
                {
                    *reinterpret_cast<uint32*>( m_pPropertyInstance ) = (uint32) m_value_cached;
                }
                break;

                case CoreTypeID::Int32:
                {
                    *reinterpret_cast<int32*>( m_pPropertyInstance ) = (int32) m_value_cached;
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

        virtual void ResetWorkingCopy() override
        {
            switch ( m_pEnumInfo->m_underlyingType )
            {
                case CoreTypeID::Uint8:
                {
                    m_value_cached = m_value_imgui = ( int64 ) *reinterpret_cast<uint8 const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int8:
                {
                    m_value_cached = m_value_imgui = ( int64 ) *reinterpret_cast<int8 const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint16:
                {
                    m_value_cached = m_value_imgui = ( int64 ) *reinterpret_cast<uint16 const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int16:
                {
                    m_value_cached = m_value_imgui = ( int64 ) *reinterpret_cast<int16 const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint32:
                {
                    m_value_cached = m_value_imgui = ( int64 ) *reinterpret_cast<uint32 const*>( m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int32:
                {
                    m_value_cached = m_value_imgui = ( int64 ) *reinterpret_cast<int32 const*>( m_pPropertyInstance );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

        virtual void HandleExternalUpdate() override
        {
            switch ( m_pEnumInfo->m_underlyingType )
            {
                case CoreTypeID::Uint8:
                {
                    auto actualValue = ( int64 ) * reinterpret_cast<uint8 const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Int8:
                {
                    auto actualValue = ( int64 ) * reinterpret_cast<int8 const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Uint16:
                {
                    auto actualValue = ( int64 ) * reinterpret_cast<uint16 const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Int16:
                {
                    auto actualValue = ( int64 ) * reinterpret_cast<int16 const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Uint32:
                {
                    auto actualValue = ( int64 ) * reinterpret_cast<uint32 const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                case CoreTypeID::Int32:
                {
                    auto actualValue = ( int64 ) * reinterpret_cast<int32 const*>( m_pPropertyInstance );
                    if ( actualValue != m_value_cached )
                    {
                        m_value_cached = m_value_imgui = actualValue;
                    }
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }

    private:

        EnumInfo const*     m_pEnumInfo = nullptr;
        int64               m_value_cached;
        int64               m_value_imgui;
    };

    //-------------------------------------------------------------------------

    class BoolEditor final : public PropertyEditor
    {
    public:

        BoolEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry,  propertyInfo, m_pPropertyInstance )
        {
            BoolEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::Checkbox( "##be", &m_value );
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            *reinterpret_cast<bool*>( m_pPropertyInstance ) = m_value;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value = *reinterpret_cast<bool*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            ResetWorkingCopy();
        }

    private:

        bool m_value;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class ScalarEditor final : public PropertyEditor
    {
    public:

        ScalarEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            ScalarEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( -1 );

            switch ( m_coreType )
            {
                case CoreTypeID::Int8:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S8, &m_value_imgui );
                }
                break;

                case CoreTypeID::Int16:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S16, &m_value_imgui );
                }
                break;

                case CoreTypeID::Int32:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S32, &m_value_imgui );
                }
                break;

                case CoreTypeID::Int64:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_S64, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint8:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U8, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint16:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U16, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint32:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U32, &m_value_imgui );
                }
                break;

                case CoreTypeID::Uint64:
                {
                    ImGui::InputScalar( "##scaed", ImGuiDataType_U64, &m_value_imgui );
                }
                break;

                case CoreTypeID::Float:
                {
                    ImGui::InputFloat( "##scaed", (float*) &m_value_imgui );
                }
                break;

                case CoreTypeID::Double:
                {
                    ImGui::InputDouble( "##scaed", (double*) &m_value_imgui );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }

            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<T*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<T*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            T const actualValue = *reinterpret_cast<T*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        T                   m_value_imgui;
        T                   m_value_cached;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class VectorEditor final : public PropertyEditor
    {
    public:

        VectorEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            VectorEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool valueChanged = false;

            switch ( m_coreType )
            {
                case CoreTypeID::Float2:
                {
                    if ( ImGuiX::InputFloat2( "F2Ed", (Float2&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                case CoreTypeID::Float3:
                {
                    if ( ImGuiX::InputFloat3( "F3Ed", (Float3&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                case CoreTypeID::Float4:
                {
                    if ( ImGuiX::InputFloat4( "F4Ed", (Float4&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                case CoreTypeID::Vector:
                {
                    if ( ImGuiX::InputFloat4( "VectorEd", (Vector&) m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<T*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<T*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            T const actualValue = *reinterpret_cast<T*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        T                   m_value_imgui;
        T                   m_value_cached;
    };

    //-------------------------------------------------------------------------

    class ColorEditor final : public PropertyEditor
    {
    public:

        ColorEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            ColorEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( -1 );
            ImGui::ColorEdit4( "##ce", &m_value_imgui.x );
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<Color*>( m_pPropertyInstance ) = Color( m_value_cached );
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = reinterpret_cast<Color*>( m_pPropertyInstance )->ToFloat4();
        }

        virtual void HandleExternalUpdate() override
        {
            Float4 const actualValue = reinterpret_cast<Color*>( m_pPropertyInstance )->ToFloat4();
            if ( !Vector( actualValue ).IsNearEqual4( m_value_cached ) )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        ImVec4 m_value_imgui;
        ImVec4 m_value_cached;
    };

    //-------------------------------------------------------------------------

    class RotationEditor final : public PropertyEditor
    {
    public:

        RotationEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            RotationEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::EulerAngles || m_coreType == CoreTypeID::Quaternion );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            return ImGuiX::InputFloat3( "Rotation", m_anglesInDegrees_imgui );
        }

        virtual void UpdateInstanceValue() override
        {
            m_anglesInDegrees_cached = m_anglesInDegrees_imgui;
            EulerAngles const newRotation( Degrees( m_anglesInDegrees_cached.m_x ).GetClamped180(), Degrees( m_anglesInDegrees_cached.m_y ).GetClamped180(), Degrees( m_anglesInDegrees_cached.m_z ).GetClamped180() );

            if ( m_coreType == CoreTypeID::EulerAngles )
            {
                *reinterpret_cast<EulerAngles*>( m_pPropertyInstance ) = newRotation;
            }
            else if( m_coreType == CoreTypeID::Quaternion )
            {
                *reinterpret_cast<Quaternion*>( m_pPropertyInstance ) = Quaternion( newRotation );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::EulerAngles )
            {
                m_anglesInDegrees_cached = m_anglesInDegrees_imgui = reinterpret_cast<EulerAngles*>( m_pPropertyInstance )->GetAsDegrees();
            }
            else if( m_coreType == CoreTypeID::Quaternion )
            {
                m_anglesInDegrees_cached = m_anglesInDegrees_imgui = reinterpret_cast<Quaternion*>( m_pPropertyInstance )->ToEulerAngles().GetAsDegrees();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            Float3 actualValue;
            if ( m_coreType == CoreTypeID::EulerAngles )
            {
                actualValue = reinterpret_cast<EulerAngles*>( m_pPropertyInstance )->GetAsDegrees();
            }
            else if ( m_coreType == CoreTypeID::Quaternion )
            {
                actualValue = reinterpret_cast<Quaternion*>( m_pPropertyInstance )->ToEulerAngles().GetAsDegrees();
            }

            if ( !Vector( actualValue ).IsNearEqual3( m_anglesInDegrees_cached ) )
            {
                m_anglesInDegrees_cached = m_anglesInDegrees_imgui = actualValue;
            }
        }

    private:

        Float3                m_anglesInDegrees_imgui;
        Float3                m_anglesInDegrees_cached;
    };

    //-------------------------------------------------------------------------

    class AngleEditor final : public PropertyEditor
    {
    public:

        AngleEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            AngleEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::Degrees || m_coreType == CoreTypeID::Radians );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const textAreaWidth = cellContentWidth - ( g_iconButtonWidth * 2 ) - ( g_itemSpacing * 3 );

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputFloat( "##ae", &m_valueInDegrees_imgui );
            bool valueChanged = ImGui::IsItemDeactivatedAfterEdit();

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::Button( KRG_ICON_ANGLE_DOWN "##ClampShortest" ) )
            {
                m_valueInDegrees_imgui = Degrees( m_valueInDegrees_imgui ).GetClamped180().ToFloat();
                valueChanged = true;
            }
            ImGuiX::ItemTooltip( "Clamp to Shortest Rotation [-180 : 180]" );

            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::Button( KRG_ICON_ANGLE_DOUBLE_DOWN "##ClampFull" ) )
            {
                m_valueInDegrees_imgui = Degrees( m_valueInDegrees_imgui ).GetClamped360().ToFloat();
                valueChanged = true;
            }
            ImGuiX::ItemTooltip( "Clamp to full rotation [-360 : 360]" );

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_valueInDegrees_cached = m_valueInDegrees_imgui;
            if ( m_coreType == CoreTypeID::Degrees )
            {
                *reinterpret_cast<Degrees*>( m_pPropertyInstance ) = Degrees( m_valueInDegrees_cached );
            }
            else if ( m_coreType == CoreTypeID::Radians )
            {
                *reinterpret_cast<Radians*>( m_pPropertyInstance ) = Degrees( m_valueInDegrees_cached );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::Degrees )
            {
                m_valueInDegrees_cached = m_valueInDegrees_imgui = reinterpret_cast<Degrees*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Radians )
            {
                m_valueInDegrees_cached = m_valueInDegrees_imgui = reinterpret_cast<Radians*>( m_pPropertyInstance )->ToDegrees().ToFloat();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            float actualValue = 0;

            if ( m_coreType == CoreTypeID::Degrees )
            {
                actualValue = reinterpret_cast<Degrees*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Radians )
            {
                actualValue = reinterpret_cast<Radians*>( m_pPropertyInstance )->ToDegrees().ToFloat();
            }

            if ( actualValue != m_valueInDegrees_cached )
            {
                m_valueInDegrees_cached = m_valueInDegrees_imgui = actualValue;
            }
        }

    private:

        float                m_valueInDegrees_imgui;
        float                m_valueInDegrees_cached;
    };

    //-------------------------------------------------------------------------

    class UUIDEditor final : public PropertyEditor
    {
    public:

        UUIDEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            UUIDEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const textAreaWidth = cellContentWidth - g_iconButtonWidth - ( g_itemSpacing * 2 );

            bool valueChanged = false;

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputText( "##ue", m_stringValue.data(), m_stringValue.length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::Button( KRG_ICON_REFRESH "##Generate" ) )
            {
                m_value_imgui = UUID::GenerateID();
                m_stringValue = m_value_imgui.ToString();
                valueChanged = true;
            }

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<UUID*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<UUID*>( m_pPropertyInstance );
            m_stringValue = m_value_imgui.ToString();
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<UUID*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
                m_stringValue = m_value_imgui.ToString();
            }
        }

    private:

        UUID            m_value_imgui;
        UUID            m_value_cached;
        UUIDString      m_stringValue;
    };

    //-------------------------------------------------------------------------

    class TimeEditor final : public PropertyEditor
    {
    public:

        TimeEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            TimeEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::Microseconds || m_coreType == CoreTypeID::Milliseconds || m_coreType == CoreTypeID::Seconds );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( -1 );
            if ( m_coreType == CoreTypeID::Microseconds )
            {
                ImGui::InputFloat( "##teus", &m_value_imgui, 0.0f, 0.0f, "%.2fus" );
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                ImGui::InputFloat( "#tems", &m_value_imgui, 0.0f, 0.0f, "%.2fms" );
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                ImGui::InputFloat( "##tes", &m_value_imgui, 0.0f, 0.0f, "%.2fs" );
            }
            
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;

            if ( m_coreType == CoreTypeID::Microseconds )
            {
                *reinterpret_cast<Microseconds*>( m_pPropertyInstance ) = Microseconds( m_value_cached );
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                *reinterpret_cast<Milliseconds*>( m_pPropertyInstance ) = Milliseconds( m_value_cached );
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                *reinterpret_cast<Seconds*>( m_pPropertyInstance ) = Seconds( m_value_cached );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::Microseconds )
            {
                m_value_cached = m_value_imgui = reinterpret_cast<Microseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                m_value_cached = m_value_imgui = reinterpret_cast<Milliseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                m_value_cached = m_value_imgui = reinterpret_cast<Seconds*>( m_pPropertyInstance )->ToFloat();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            float actualValue = 0;

            if ( m_coreType == CoreTypeID::Microseconds )
            {
                actualValue = reinterpret_cast<Microseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Milliseconds )
            {
                actualValue = reinterpret_cast<Milliseconds*>( m_pPropertyInstance )->ToFloat();
            }
            else if ( m_coreType == CoreTypeID::Seconds )
            {
                actualValue = reinterpret_cast<Seconds*>( m_pPropertyInstance )->ToFloat();
            }

            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        float                m_value_imgui;
        float                m_value_cached;
    };

    //-------------------------------------------------------------------------

    class PercentageEditor final : public PropertyEditor
    {
    public:

        PercentageEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            PercentageEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const textAreaWidth = cellContentWidth - g_iconButtonWidth - ( g_itemSpacing * 2 );

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputFloat( "##pe", &m_value_imgui, 0, 0, "%.2f%%" );
            bool valueChanged = ImGui::IsItemDeactivatedAfterEdit();

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::Button( KRG_ICON_ANGLE_DOWN "##ClampPercentage" ) )
            {
                m_value_imgui = ( Percentage( m_value_imgui / 100 ).GetClamped( true ) ).ToFloat() * 100;
                valueChanged = true;
            }
            ImGuiX::ItemTooltip( "Clamp [-100 : 100]" );

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<Percentage*>( m_pPropertyInstance ) = Percentage( m_value_cached / 100.0f );
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = reinterpret_cast<Percentage*>( m_pPropertyInstance )->ToFloat() * 100;
        }

        virtual void HandleExternalUpdate() override
        {
            auto actualValue = reinterpret_cast<Percentage*>( m_pPropertyInstance )->ToFloat() * 100;;
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        float  m_value_imgui;
        float  m_value_cached;
    };

    //-------------------------------------------------------------------------

    class TransformEditor final : public PropertyEditor
    {
    public:

        TransformEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            TransformEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::Transform || m_coreType == CoreTypeID::Matrix );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool transformUpdated = false;

            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 1, 2 ) );
            if ( ImGui::BeginTable( "Transform", 2, ImGuiTableFlags_None ) )
            {
                ImGui::TableSetupColumn( "Header", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 24 );
                ImGui::TableSetupColumn( "Values", ImGuiTableColumnFlags_NoHide );

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Rot" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( "R", m_rotation_imgui ) )
                    {
                        m_rotation_imgui.m_x = Degrees( m_rotation_imgui.m_x ).GetClamped180().ToFloat();
                        m_rotation_imgui.m_y = Degrees( m_rotation_imgui.m_y ).GetClamped180().ToFloat();
                        m_rotation_imgui.m_z = Degrees( m_rotation_imgui.m_z ).GetClamped180().ToFloat();
                        m_rotation_cached = m_rotation_imgui;
                        transformUpdated = true;
                    }
                }

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Pos" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( "T", m_translation_imgui ) )
                    {
                        m_translation_cached = m_translation_imgui;
                        transformUpdated = true;
                    }
                }

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Scl" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( "S", m_scale_imgui ) )
                    {
                        m_scale_imgui.m_x = Math::IsNearZero( m_scale_imgui.m_x ) ? 0.01f : m_scale_imgui.m_x;
                        m_scale_imgui.m_y = Math::IsNearZero( m_scale_imgui.m_y ) ? 0.01f : m_scale_imgui.m_y;
                        m_scale_imgui.m_z = Math::IsNearZero( m_scale_imgui.m_z ) ? 0.01f : m_scale_imgui.m_z;
                        m_scale_cached = m_scale_imgui;
                        transformUpdated = true;
                    }
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            return transformUpdated;
        }

        virtual void UpdateInstanceValue() override
        {
            EulerAngles const rot( m_rotation_cached );
            Quaternion const q( rot );

            if ( m_coreType == CoreTypeID::Transform )
            {
                *reinterpret_cast<Transform*>( m_pPropertyInstance ) = Transform( q, m_translation_cached, m_scale_cached );
            }
            else if ( m_coreType == CoreTypeID::Matrix )
            {
                *reinterpret_cast<Matrix*>( m_pPropertyInstance ) = Matrix( q, m_translation_cached, m_scale_cached );
            }
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::Transform )
            {
                auto const& transform = reinterpret_cast<Transform*>( m_pPropertyInstance );
                
                m_rotation_cached = m_rotation_imgui = transform->GetRotation().ToEulerAngles().GetAsDegrees();
                m_translation_cached = m_translation_imgui = transform->GetTranslation().ToFloat3();
                m_scale_cached = m_scale_imgui = transform->GetScale().ToFloat3();
            }
            else if ( m_coreType == CoreTypeID::Matrix )
            {
                auto const& matrix = reinterpret_cast<Matrix*>( m_pPropertyInstance );

                Quaternion q;
                Vector t, s;
                matrix->Decompose( q, t, s );

                m_rotation_cached = m_rotation_imgui = q.ToEulerAngles().GetAsDegrees();
                m_translation_cached = m_translation_imgui = t.ToFloat3();
                m_scale_cached = m_scale_imgui = s.ToFloat3();
            }
        }

        virtual void HandleExternalUpdate() override
        {
            Quaternion actualQ;
            Vector actualTranslation;
            Vector actualScale;

            // Get actual transform values
            //-------------------------------------------------------------------------

            if ( m_coreType == CoreTypeID::Transform )
            {
                auto const& transform = reinterpret_cast<Transform*>( m_pPropertyInstance );
                actualQ = transform->GetRotation();
                actualTranslation = transform->GetTranslation();
                actualScale = transform->GetScale();
            }
            else if ( m_coreType == CoreTypeID::Matrix )
            {
                auto const& matrix = reinterpret_cast<Matrix*>( m_pPropertyInstance );
                matrix->Decompose( actualQ, actualTranslation, actualScale );
            }

            // Update the cached (and the imgui transform) when the actual is sufficiently different
            //-------------------------------------------------------------------------

            EulerAngles const currentRotation( m_rotation_cached );
            Quaternion const currentQ( currentRotation );

            Radians const angularDistance = Quaternion::Distance( currentQ, actualQ );
            if ( angularDistance > Degrees( 1.0f ) )
            {
                m_rotation_cached = actualQ.ToEulerAngles().GetAsDegrees();
                m_rotation_imgui = m_rotation_cached;
            }

            if ( !actualTranslation.IsNearEqual3( m_translation_cached ) )
            {
                m_translation_cached = actualTranslation.ToFloat3();
                m_translation_imgui = m_translation_cached;
            }

            if ( !actualScale.IsNearEqual3( m_scale_cached ) )
            {
                m_scale_cached = actualScale.ToFloat3();
                m_scale_imgui = m_scale_cached;
            }
        }

    private:

        Float3                m_rotation_imgui;
        Float3                m_translation_imgui;
        Float3                m_scale_imgui;

        Float3                m_rotation_cached;
        Float3                m_translation_cached;
        Float3                m_scale_cached;
    };

    //-------------------------------------------------------------------------

    class ResourcePathEditor final : public PropertyEditor
    {
    public:

        ResourcePathEditor( TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
            , m_rawResourceDirectoryPath( rawResourceDirectoryPath )
        {
            ResourcePathEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::ResourcePath || m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr );

            //-------------------------------------------------------------------------

            if ( m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr )
            {
                for ( auto const& registeredResourceType : m_typeRegistry.GetRegisteredResourceTypes() )
                {
                    m_allowedResourceTypeIDs.emplace_back( registeredResourceType.second.m_resourceTypeID );
                }
            }
            else if ( m_coreType == CoreTypeID::TResourcePtr )
            {
                ResourceTypeID const allowedResourceTypeID = m_typeRegistry.GetResourceInfoForType( propertyInfo.m_templateArgumentTypeID )->m_resourceTypeID;
                allowedResourceTypeID.GetString( m_resourceTypeStr );

                m_allowedResourceTypeIDs.emplace_back( allowedResourceTypeID );
            }
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;

            //-------------------------------------------------------------------------

            bool valueChanged = false;

            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                float const textAreaWidth = cellContentWidth - ( g_iconButtonWidth * 2 ) - ( g_itemSpacing * 2 );

                ImGui::SetNextItemWidth( textAreaWidth );
                ImGui::InputText( "##pathstring", const_cast<char*>( m_value_imgui.GetString().data() ), m_value_imgui.GetString().length(), ImGuiInputTextFlags_ReadOnly );

                ImGui::SameLine( 0, g_itemSpacing );
                if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick", ImVec2( g_iconButtonWidth - 1, 0 ) ) )
                {
                    if ( DataFilePicker::PickFile( m_rawResourceDirectoryPath, m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
            }
            else if ( m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                constexpr static float const childWindowWidth = 36;

                ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
                ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 0 ) );
                ImGui::BeginChild( "##resourceType", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
                {
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextColored( Colors::LightPink.ToFloat4(), m_resourceTypeStr );
                }
                ImGui::EndChild();
                ImGui::PopStyleVar( 2 );

                //-------------------------------------------------------------------------

                ImGui::SameLine( 0, g_itemSpacing );
                ImGui::SetNextItemWidth( cellContentWidth - ( g_itemSpacing * 4 ) - ( g_iconButtonWidth * 2 ) - childWindowWidth );
                ImGui::InputText( "##resourcePath", const_cast<char*>( m_value_imgui.GetString().c_str() ), m_value_imgui.GetString().length(), ImGuiInputTextFlags_ReadOnly );

                ImGui::SameLine( 0, g_itemSpacing );
                if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
                {
                    if ( DataFilePicker::PickResourceFile( m_rawResourceDirectoryPath, m_allowedResourceTypeIDs, m_value_imgui ) )
                    {
                        valueChanged = true;
                    }
                }
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear" ) )
            {
                m_value_imgui.Clear();
                valueChanged = true;
            }

            //-------------------------------------------------------------------------

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;

            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                *reinterpret_cast<ResourcePath*>( m_pPropertyInstance ) = m_value_cached;
            }
            else if ( m_coreType == CoreTypeID::ResourceID )
            {
                *reinterpret_cast<ResourceID*>( m_pPropertyInstance ) = m_value_cached.IsValid() ? ResourceID( m_value_cached ) : ResourceID();
            }
            else if ( m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                *reinterpret_cast<Resource::ResourcePtr*>( m_pPropertyInstance ) = m_value_cached.IsValid() ? ResourceID( m_value_cached ) : ResourceID();
            }

            UpdateResourceTypeIDString();
        }

        virtual void ResetWorkingCopy() override
        {
            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                m_value_cached = m_value_imgui = *reinterpret_cast<ResourcePath*>( m_pPropertyInstance );
            }
            else if ( m_coreType == CoreTypeID::ResourceID )
            {
                ResourceID* pResourceID = reinterpret_cast<ResourceID*>( m_pPropertyInstance );
                m_value_cached = m_value_imgui = pResourceID->GetResourcePath();
            }
            else if ( m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                Resource::ResourcePtr* pResourcePtr = reinterpret_cast<Resource::ResourcePtr*>( m_pPropertyInstance );
                m_value_cached = m_value_imgui = pResourcePtr->GetResourcePath();
            }

            UpdateResourceTypeIDString();
        }

        void UpdateResourceTypeIDString()
        {
            if ( m_coreType == CoreTypeID::ResourceID || m_coreType == CoreTypeID::ResourcePtr )
            {
                if ( m_value_cached.IsValid() )
                {
                    ResourceID( m_value_cached ).GetResourceTypeID().GetString( m_resourceTypeStr );
                }
                else
                {
                    m_resourceTypeStr[0] = 0;
                }
            }
        }

        virtual void HandleExternalUpdate() override
        {
            ResourcePath const* pActualPath = nullptr;

            if ( m_coreType == CoreTypeID::ResourcePath )
            {
                pActualPath = reinterpret_cast<ResourcePath*>( m_pPropertyInstance );
            }
            else if ( m_coreType == CoreTypeID::ResourceID )
            {
                ResourceID* pResourceID = reinterpret_cast<ResourceID*>( m_pPropertyInstance );
                pActualPath = &pResourceID->GetResourcePath();
            }
            else if ( m_coreType == CoreTypeID::ResourcePtr || m_coreType == CoreTypeID::TResourcePtr )
            {
                Resource::ResourcePtr* pResourcePtr = reinterpret_cast<Resource::ResourcePtr*>( m_pPropertyInstance );
                pActualPath = &pResourcePtr->GetResourcePath();
            }

            if ( *pActualPath != m_value_cached )
            {
                m_value_cached = m_value_imgui = *pActualPath;
            }
        }

    private:

        FileSystem::Path const          m_rawResourceDirectoryPath;
        TVector<ResourceTypeID>         m_allowedResourceTypeIDs;
        char                            m_resourceTypeStr[5] = { 0 };
        ResourcePath                    m_value_imgui;
        ResourcePath                    m_value_cached;
    };

    //-------------------------------------------------------------------------

    class ResourceTypeIDEditor final : public PropertyEditor
    {
    public:

        ResourceTypeIDEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            ResourceTypeIDEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const childWindowWidth = 36;
            float const textAreaWidth = cellContentWidth - childWindowWidth - g_itemSpacing;

            bool valueChanged = false;

            //-------------------------------------------------------------------------

            ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 0 ) );
            ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextColored( Colors::LightPink.ToFloat4(), m_resourceTypeStr );
            }
            ImGui::EndChild();
            ImGui::PopStyleVar( 2 );

            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::BeginCombo( "##resTypeID", m_resourceTypeFriendlyName.c_str()) )
            {
                auto AddComboItem = [this, &valueChanged] ( ResourceInfo const& resourceInfo )
                {
                    bool const isSelected = ( m_value_imgui.m_ID == resourceInfo.m_resourceTypeID );
                    if ( ImGui::Selectable( resourceInfo.m_friendlyName.c_str(), isSelected ) )
                    {
                        if ( resourceInfo.m_resourceTypeID != m_value_imgui )
                        {
                            m_value_imgui = resourceInfo.m_resourceTypeID;
                            m_value_imgui.GetString( m_resourceTypeStr );
                            m_resourceTypeFriendlyName = resourceInfo.m_friendlyName;
                            valueChanged = true;
                        }
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if ( isSelected )
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                };

                //-------------------------------------------------------------------------

                AddComboItem( ResourceInfo() );

                //-------------------------------------------------------------------------

                auto const& registeredResourceTypes = m_typeRegistry.GetRegisteredResourceTypes();
                for ( auto& pair : registeredResourceTypes )
                {
                    AddComboItem( pair.second );
                }

                ImGui::EndCombo();
            }

            //-------------------------------------------------------------------------

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<ResourceTypeID*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<ResourceTypeID*>( m_pPropertyInstance );
            m_value_cached.GetString( m_resourceTypeStr );

            if ( m_value_cached.IsValid() )
            {
                m_resourceTypeFriendlyName = m_typeRegistry.GetResourceInfoForResourceType( m_value_cached )->m_friendlyName;
            }
            else
            {
                m_resourceTypeFriendlyName = "";
            }
        }

        virtual void HandleExternalUpdate() override
        {
            auto actualValue = *reinterpret_cast<ResourceTypeID*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                ResetWorkingCopy();
            }
        }

    private:

        ResourceTypeID              m_value_imgui;
        ResourceTypeID              m_value_cached;
        char                        m_resourceTypeStr[5];
        String                      m_resourceTypeFriendlyName;
    };

    //-------------------------------------------------------------------------

    class BitflagsEditor final : public PropertyEditor
    {
    public:

        BitflagsEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            BitflagsEditor::ResetWorkingCopy();
            KRG_ASSERT( m_coreType == CoreTypeID::BitFlags || m_coreType == CoreTypeID::TBitFlags );
        }

        virtual bool InternalUpdateAndDraw() override
        {
            bool valueChanged = false;

            if ( ImGui::BeginTable( "FlagsTable", 9, ImGuiTableFlags_SizingFixedFit ) )
            {
                if ( m_coreType == CoreTypeID::BitFlags )
                {
                    constexpr static char const* const rowLabels[4] = { "00-07", "08-15", "16-23", "24-31" };

                    for ( uint8 i = 0u; i < 4; i++ )
                    {
                        ImGui::TableNextRow();

                        for ( uint8 j = 0u; j < 8; j++ )
                        {
                            uint8 const flagIdx = i * 8 + j;
                            ImGui::TableNextColumn();

                            ImGui::PushID( &m_values_imgui[flagIdx] );
                            if ( ImGui::Checkbox( "##flag", &m_values_imgui[flagIdx]) )
                            {
                                valueChanged = true;
                            }
                            ImGui::PopID();
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text( rowLabels[i] );
                    }
                }
                else if ( m_coreType == CoreTypeID::TBitFlags )
                {
                    // Get enum type for specific flags
                    TypeID const enumTypeID = m_propertyInfo.m_templateArgumentTypeID;
                    EnumInfo const* pEnumInfo = m_typeRegistry.GetEnumInfo( enumTypeID );
                    KRG_ASSERT( pEnumInfo != nullptr );

                    //-------------------------------------------------------------------------

                    int32 flagCount = 0;

                    // For each label 
                    for ( auto const& constant : pEnumInfo->m_constants )
                    {
                        if ( ( flagCount % 2 ) == 0 )
                        {
                            ImGui::TableNextRow();
                        }

                        ImGui::TableNextColumn();
                        flagCount++;

                        //-------------------------------------------------------------------------

                        int64 const flagValue = constant.second;
                        KRG_ASSERT( flagValue >= 0 && flagValue <= 31 );
                        if ( ImGui::Checkbox( constant.first.c_str(), &m_values_imgui[flagValue] ) )
                        {
                            valueChanged = true;
                        }
                    }
                }

                ImGui::EndTable();
            }

            //-------------------------------------------------------------------------

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            auto pFlags = reinterpret_cast<BitFlags*>( m_pPropertyInstance );
            for ( uint8 i = 0; i < 32; i++ )
            {
                pFlags->SetFlag( i, m_values_imgui[i] );
            }

            m_cachedFlags = pFlags->Get();
        }

        virtual void ResetWorkingCopy() override
        {
            auto pFlags = reinterpret_cast<BitFlags*>( m_pPropertyInstance );
            for ( uint8 i = 0; i < 32; i++ )
            {
                m_values_imgui[i] = pFlags->IsFlagSet( i );
            }

            m_cachedFlags = pFlags->Get();
        }

        virtual void HandleExternalUpdate() override
        {
            auto pFlags = reinterpret_cast<BitFlags*>( m_pPropertyInstance );
            if ( pFlags->Get() != m_cachedFlags )
            {
                ResetWorkingCopy();
            }
        }

    private:

        bool        m_values_imgui[32];
        uint32      m_cachedFlags = 0;
    };

    //-------------------------------------------------------------------------

    class StringEditor final : public PropertyEditor
    {
    public:

        StringEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            StringEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            ImGui::SetNextItemWidth( -1 );
            ImGui::InputText( "##stringEd", m_buffer_imgui, 256);
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            strcpy_s( m_buffer_cached, 256, m_buffer_imgui );
            *reinterpret_cast<String*>( m_pPropertyInstance ) = m_buffer_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            String* pValue = reinterpret_cast<String*>( m_pPropertyInstance );
            strcpy_s( m_buffer_imgui, 256, pValue->c_str() );
            strcpy_s( m_buffer_cached, 256, pValue->c_str() );
        }

        virtual void HandleExternalUpdate() override
        {
            String* pActualValue = reinterpret_cast<String*>( m_pPropertyInstance );
            if ( strcmp( pActualValue->c_str(), m_buffer_cached ) != 0 )
            {
                strcpy_s( m_buffer_imgui, 256, pActualValue->c_str() );
                strcpy_s( m_buffer_cached, 256, pActualValue->c_str() );
            }
        }

    private:

        char    m_buffer_imgui[256];
        char    m_buffer_cached[256];
    };

    //-------------------------------------------------------------------------

    class StringIDEditor final : public PropertyEditor
    {
    public:

        StringIDEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            StringIDEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const childWindowWidth = 80;
            float const textAreaWidth = cellContentWidth - childWindowWidth - g_itemSpacing;

            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( childWindowWidth );
            ImGui::PushStyleColor( ImGuiCol_Text, Colors::LightGreen.ToUInt32_ABGR() );
            ImGui::InputText( "##IDString", m_IDString.data(), m_IDString.length(), ImGuiInputTextFlags_ReadOnly );
            ImGui::PopStyleColor();

            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::SameLine( 0, g_itemSpacing );
            ImGui::InputText( "##StringInput", m_buffer_imgui, 256 );
            return ImGui::IsItemDeactivatedAfterEdit();
        }

        virtual void UpdateInstanceValue() override
        {
            auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            strcpy_s( m_buffer_cached, 256, m_buffer_imgui );
            *pValue = StringID( m_buffer_cached );
            m_IDString.sprintf( "%u", pValue->GetID() );
        }

        virtual void ResetWorkingCopy() override
        {
            auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            strcpy_s( m_buffer_imgui, 256, pValue->c_str() );
            strcpy_s( m_buffer_cached, 256, pValue->c_str() );
            m_IDString.sprintf( "%u", pValue->GetID() );
        }

        virtual void HandleExternalUpdate() override
        {
            auto pActualValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            if ( strcmp( pActualValue->c_str(), m_buffer_cached ) != 0 )
            {
                strcpy_s( m_buffer_imgui, 256, pActualValue->c_str() );
                strcpy_s( m_buffer_cached, 256, pActualValue->c_str() );
                m_IDString.sprintf( "%u", pActualValue->GetID() );
            }
        }

    private:

        char    m_buffer_imgui[256];
        char    m_buffer_cached[256];
        String  m_IDString;
    };

    //-------------------------------------------------------------------------

    class TagEditor final : public PropertyEditor
    {
    public:

        TagEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            TagEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            //float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            //float const childWindowWidth = 80;
            //float const textAreaWidth = cellContentWidth - childWindowWidth - g_itemSpacing;

            ////-------------------------------------------------------------------------

            //ImGui::SetNextItemWidth( childWindowWidth );
            //ImGui::PushStyleColor( ImGuiCol_Text, Colors::LightGreen.ToUInt32_ABGR() );
            //ImGui::InputText( "##IDString", m_IDString.data(), m_IDString.length(), ImGuiInputTextFlags_ReadOnly );
            //ImGui::PopStyleColor();

            ////-------------------------------------------------------------------------

            //ImGui::SetNextItemWidth( textAreaWidth );
            //ImGui::SameLine( 0, g_itemSpacing );
            //ImGui::InputText( "##StringInput", m_buffer_imgui, 256 );
            //return ImGui::IsItemDeactivatedAfterEdit();

            ImGui::Text( "WIP" );
            return false;
        }

        virtual void UpdateInstanceValue() override
        {
            /*auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            strcpy_s( m_buffer_cached, 256, m_buffer_imgui );
            *pValue = StringID( m_buffer_cached );
            m_IDString.sprintf( "%u", pValue->GetID() );*/
        }

        virtual void ResetWorkingCopy() override
        {
            /*auto pValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            strcpy_s( m_buffer_imgui, 256, pValue->c_str() );
            strcpy_s( m_buffer_cached, 256, pValue->c_str() );
            m_IDString.sprintf( "%u", pValue->GetID() );*/
        }

        virtual void HandleExternalUpdate() override
        {
            /*auto pActualValue = reinterpret_cast<StringID*>( m_pPropertyInstance );
            if ( strcmp( pActualValue->c_str(), m_buffer_cached ) != 0 )
            {
                strcpy_s( m_buffer_imgui, 256, pActualValue->c_str() );
                strcpy_s( m_buffer_cached, 256, pActualValue->c_str() );
                m_IDString.sprintf( "%u", pActualValue->GetID() );
            }*/
        }

    private:
    };

    //-------------------------------------------------------------------------

    class IntRangeEditor final : public PropertyEditor
    {
    public:

        IntRangeEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            IntRangeEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const contentWidth = ImGui::GetContentRegionAvail().x;
            float const inputWidth = ( contentWidth - ( g_itemSpacing * 1 ) ) / 2;

            //-------------------------------------------------------------------------

            bool valueUpdated = false;

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputScalar( "##min", ImGuiDataType_S32, &m_value_imgui.m_start, 0, 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, g_itemSpacing );

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputScalar( "##max", ImGuiDataType_S32, &m_value_imgui.m_end, 0, 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            return valueUpdated;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<IntRange*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<IntRange*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<IntRange*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        IntRange m_value_imgui;
        IntRange m_value_cached;
    };

    //-------------------------------------------------------------------------

    class FloatRangeEditor final : public PropertyEditor
    {
    public:

        FloatRangeEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
        {
            FloatRangeEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const contentWidth = ImGui::GetContentRegionAvail().x;
            float const inputWidth = ( contentWidth - ( g_itemSpacing * 1 ) ) / 2;

            //-------------------------------------------------------------------------

            bool valueUpdated = false;

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##min", &m_value_imgui.m_start, 0, 0, "%.3f", 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, g_itemSpacing );

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##max", &m_value_imgui.m_end, 0, 0, "%.3f", 0 );
            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            return valueUpdated;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<FloatRange*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<FloatRange*>( m_pPropertyInstance );
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<FloatRange*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        FloatRange m_value_imgui;
        FloatRange m_value_cached;
    };

    //-------------------------------------------------------------------------

    class FloatCurveEditor final : public PropertyEditor
    {
    public:

        FloatCurveEditor( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
            : PropertyEditor( typeRegistry, propertyInfo, m_pPropertyInstance )
            , m_editor( m_value_imgui )
        {
            FloatCurveEditor::ResetWorkingCopy();
        }

        virtual bool InternalUpdateAndDraw() override
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const previewWidth = cellContentWidth - g_iconButtonWidth - ( g_itemSpacing * 2 );

            bool valueChanged = false;

            if ( ImGui::BeginChild( "##Preview", ImVec2( previewWidth, 140 ) ) )
            {
                valueChanged = m_editor.UpdateAndDraw();
            }
            ImGui::EndChild();

            ImGui::SameLine( 0, g_itemSpacing );
            if ( ImGui::Button( KRG_ICON_PENCIL "##OpenCurveEditor" ) )
            {
                ImGui::OpenPopup( "CurveEditor" );
            }
            ImGuiX::ItemTooltip( "Open Full Curve Editor" );

            //-------------------------------------------------------------------------

            bool isOpen = true;
            ImGui::SetNextWindowSize( ImVec2( 800, 600 ), ImGuiCond_FirstUseEver );
            if ( ImGui::BeginPopupModal( "CurveEditor", &isOpen ) )
            {
                valueChanged = m_editor.UpdateAndDraw();
                ImGui::EndPopup();
            }

            return valueChanged;
        }

        virtual void UpdateInstanceValue() override
        {
            m_value_cached = m_value_imgui;
            *reinterpret_cast<FloatCurve*>( m_pPropertyInstance ) = m_value_cached;
        }

        virtual void ResetWorkingCopy() override
        {
            m_value_cached = m_value_imgui = *reinterpret_cast<FloatCurve*>( m_pPropertyInstance );
            m_editor.ResetView();
        }

        virtual void HandleExternalUpdate() override
        {
            auto& actualValue = *reinterpret_cast<FloatCurve*>( m_pPropertyInstance );
            if ( actualValue != m_value_cached )
            {
                m_value_cached = m_value_imgui = actualValue;
            }
        }

    private:

        FloatCurve      m_value_imgui;
        FloatCurve      m_value_cached;
        CurveEditor     m_editor;
    };

    //-------------------------------------------------------------------------
    // Factory Method
    //-------------------------------------------------------------------------

    PropertyEditor* CreatePropertyEditor( TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath, PropertyInfo const& propertyInfo, Byte* m_pPropertyInstance )
    {
        if ( propertyInfo.IsEnumProperty() )
        {
            return KRG::New<EnumEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
        }
        else
        {
            CoreTypeID const coreType = GetCoreType( propertyInfo.m_typeID );
            switch ( coreType )
            {
                case CoreTypeID::Bool:
                {
                    return KRG::New<BoolEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int8:
                {
                    return KRG::New<ScalarEditor<int8>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int16:
                {
                    return KRG::New<ScalarEditor<int16>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int32:
                {
                    return KRG::New<ScalarEditor<int32>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Int64:
                {
                    return KRG::New<ScalarEditor<int64>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;
                
                case CoreTypeID::Uint8:
                {
                    return KRG::New<ScalarEditor<uint8>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint16:
                {
                    return KRG::New<ScalarEditor<uint16>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint32:
                {
                    return KRG::New<ScalarEditor<uint32>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Uint64:
                {
                    return KRG::New<ScalarEditor<uint64>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;
                
                case CoreTypeID::Float:
                {
                    return KRG::New<ScalarEditor<float>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Double:
                {
                    return KRG::New<ScalarEditor<double>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Color:
                {
                    return KRG::New<ColorEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::EulerAngles:
                case CoreTypeID::Quaternion:
                {
                    return KRG::New<RotationEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::UUID:
                {
                    return KRG::New<UUIDEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Microseconds:
                case CoreTypeID::Milliseconds:
                case CoreTypeID::Seconds:
                {
                    return KRG::New<TimeEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Degrees:
                case CoreTypeID::Radians:
                {
                    return KRG::New<AngleEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Matrix:
                case CoreTypeID::Transform:
                {
                    return KRG::New<TransformEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::ResourcePath:
                case CoreTypeID::ResourceID:
                case CoreTypeID::ResourcePtr:
                case CoreTypeID::TResourcePtr:
                {
                    return KRG::New<ResourcePathEditor>( typeRegistry, rawResourceDirectoryPath, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::ResourceTypeID:
                {
                    return KRG::New<ResourceTypeIDEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::BitFlags:
                case CoreTypeID::TBitFlags:
                {
                    return KRG::New<BitflagsEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::StringID:
                {
                    return KRG::New<StringIDEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Tag:
                {
                    return KRG::New<TagEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::String:
                {
                    return KRG::New<StringEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Float2:
                {
                    return KRG::New<VectorEditor<Float2>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Float3:
                {
                    return KRG::New<VectorEditor<Float3>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Float4:
                {
                    return KRG::New<VectorEditor<Float4>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Vector:
                {
                    return KRG::New<VectorEditor<Vector>>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::Percentage:
                {
                    return KRG::New<PercentageEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::IntRange:
                {
                    return KRG::New<IntRangeEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::FloatRange:
                {
                    return KRG::New<FloatRangeEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;

                case CoreTypeID::FloatCurve:
                {
                    return KRG::New<FloatCurveEditor>( typeRegistry, propertyInfo, m_pPropertyInstance );
                }
                break;
            }
        }

        return nullptr;
    }
}