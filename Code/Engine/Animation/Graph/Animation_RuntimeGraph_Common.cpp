#include "Animation_RuntimeGraph_Common.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    Color GetColorForValueType( ValueType type )
    {
        static const Color colors[9] =
        {
            Colors::GhostWhite,
            Colors::Yellow,
            Colors::Orange,
            Colors::Violet,
            Colors::CornflowerBlue,
            Colors::DeepSkyBlue,
            Colors::Cyan,
            Colors::HoneyDew,
            Colors::LimeGreen,
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