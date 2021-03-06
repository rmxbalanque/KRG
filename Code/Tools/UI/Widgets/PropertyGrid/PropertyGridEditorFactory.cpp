#include "PropertyGridEditorFactory.h"
#include "PropertyGridEditors_Containers.h"
#include "PropertyGridEditors_CoreTypes.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        PropertyEditor* CreateEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance != nullptr );

            // Arrays
            if ( pPropertyInstance->IsArray() )
            {
                if ( pPropertyInstance->IsStaticArray() )
                {
                    return new StaticArrayPropertyEditor( context, pPropertyInstance );
                }
                else
                {
                    return new DynamicArrayPropertyEditor( context, pPropertyInstance );
                }
            }
            else if ( pPropertyInstance->IsEnum() )
            {
                return new EnumPropertyEditor( context, pPropertyInstance );
            }
            // Structures
            else if ( !pPropertyInstance->IsCoreType() )
            {
                return new StructurePropertyEditor( context, pPropertyInstance );
            }
            else // Core types
            {
                auto const CoreType = TypeSystem::GetCoreType( pPropertyInstance->GetTypeID() );
                switch ( CoreType )
                {
                    case TypeSystem::CoreTypes::Bool:
                    {
                        return new BoolPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Uint8:
                    case TypeSystem::CoreTypes::Uint16:
                    case TypeSystem::CoreTypes::Uint32:
                    case TypeSystem::CoreTypes::Uint64:
                    case TypeSystem::CoreTypes::Int8:
                    case TypeSystem::CoreTypes::Int16:
                    case TypeSystem::CoreTypes::Int32:
                    case TypeSystem::CoreTypes::Int64:
                    case TypeSystem::CoreTypes::Float:
                    case TypeSystem::CoreTypes::Double:
                    case TypeSystem::CoreTypes::Radians:
                    case TypeSystem::CoreTypes::Degrees:
                    case TypeSystem::CoreTypes::Microseconds:
                    case TypeSystem::CoreTypes::Milliseconds:
                    case TypeSystem::CoreTypes::Seconds:
                    case TypeSystem::CoreTypes::Percentage:
                    {
                        return new NumericPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Color:
                    {
                        return new ColorPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::String:
                    {
                        return new StringPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::UUID:
                    {
                        return new UUIDPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::StringID:
                    {
                        return new StringIDPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Float2:
                    {
                        return new Float2PropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Float3:
                    {
                        return new Float3PropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Float4:
                    {
                        return new Float4PropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::DataPath:
                    {
                        return new DataPathPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::ResourceID:
                    case TypeSystem::CoreTypes::ResourcePtr:
                    case TypeSystem::CoreTypes::TResourcePtr:
                    {
                        return new ResourcePathPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Transform:
                    case TypeSystem::CoreTypes::Matrix:
                    {
                        return new TransformPropertyEditor( context, pPropertyInstance );
                    }
                    break;

                    case TypeSystem::CoreTypes::Quaternion:
                    case TypeSystem::CoreTypes::EulerAngles:
                    {
                        return new RotationPropertyEditor( context, pPropertyInstance );
                    }
                    break;
                }
            }

            return nullptr;
        }
    }
}