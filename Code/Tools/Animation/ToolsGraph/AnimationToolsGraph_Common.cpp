#include "AnimationToolsGraph_Common.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ImColor GetColorForValueType( ValueType type )
    {
        static const ImColor colors[9] =
        {
            ImGuiX::ConvertColor( Colors::GhostWhite ),
            ImGuiX::ConvertColor( Colors::Yellow ),
            ImGuiX::ConvertColor( Colors::Orange ),
            ImGuiX::ConvertColor( Colors::Violet ),
            ImGuiX::ConvertColor( Colors::CornflowerBlue ),
            ImGuiX::ConvertColor( Colors::DeepSkyBlue ),
            ImGuiX::ConvertColor( Colors::Cyan ),
            ImGuiX::ConvertColor( Colors::HoneyDew ),
            ImGuiX::ConvertColor( Colors::LimeGreen ),
        };

        return colors[(uint8) type];
    }

    char const* GetNameForValueType( ValueType type )
    {
        switch ( type )
        {
            case ValueType::Bool:
            {
                return "Bool";
            }
            break;

            case ValueType::ID:
            {
                return "ID";
            }
            break;

            case ValueType::Int:
            {
                return "Int";
            }
            break;

            case ValueType::Float:
            {
                return "Float";
            }
            break;

            case ValueType::Vector:
            {
                return "Vector";
            }
            break;

            case ValueType::Target:
            {
                return "Target";
            }
            break;

            case ValueType::BoneMask:
            {
                return "Bone Mask";
            }
            break;

            case ValueType::Pose:
            {
                return "Pose";
            }
            break;
        }

        return "Unknown";
    }
}