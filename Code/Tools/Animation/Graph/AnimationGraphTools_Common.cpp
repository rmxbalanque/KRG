#include "AnimationGraphTools_Common.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ImColor GetColorForValueType( NodeValueType type )
    {
        static const ImColor colors[9] =
        {
            ImGuiX::ConvertColor( Colors::Red ),
            ImGuiX::ConvertColor( Colors::Yellow ),
            ImGuiX::ConvertColor( Colors::Orange ),
            ImGuiX::ConvertColor( Colors::Violet ),
            ImGuiX::ConvertColor( Colors::CornflowerBlue ),
            ImGuiX::ConvertColor( Colors::DeepSkyBlue ),
            ImGuiX::ConvertColor( Colors::Cyan ),
            ImGuiX::ConvertColor( Colors::HoneyDew ),
            ImGuiX::ConvertColor( Colors::LimeGreen ),
        };

        return colors[(uint8) type ];
    }

    char const* GetNameForValueType( NodeValueType type )
    {
        switch ( type )
        {
            case NodeValueType::Bool:
            {
                return "Bool";
            }
            break;

            case NodeValueType::ID:
            {
                return "ID";
            }
            break;

            case NodeValueType::Int:
            {
                return "Int";
            }
            break;

            case NodeValueType::Float:
            {
                return "Float";
            }
            break;

            case NodeValueType::Vector:
            {
                return "Vector";
            }
            break;

            case NodeValueType::Target:
            {
                return "Target";
            }
            break;

            case NodeValueType::BoneMask:
            {
                return "Bone Mask";
            }
            break;

            case NodeValueType::Pose:
            {
                return "Pose";
            }
            break;

            default:
            KRG_UNREACHABLE_CODE();
            break;
        }

        return "UNKNOWN";
    }
}