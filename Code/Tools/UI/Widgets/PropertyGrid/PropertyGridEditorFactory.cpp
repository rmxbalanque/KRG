#include "PropertyGridEditorFactory.h"
#include "PropertyGridEditors_Containers.h"
#include "PropertyGridEditors_CoreTypes.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        PropertyEditor* CreateEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance != nullptr );

            // Arrays
            if ( pPropertyInstance->IsArrayProperty() )
            {
                if ( pPropertyInstance->IsStaticArrayProperty() )
                {
                    return new StaticArrayPropertyEditor( context, pPropertyInstance );
                }
                else
                {
                    return new DynamicArrayPropertyEditor( context, pPropertyInstance );
                }
            }
            else if ( pPropertyInstance->IsEnumProperty() )
            {
                return new EnumPropertyEditor( context, pPropertyInstance );
            }
            // Structures
            else if ( !pPropertyInstance->IsCoreTypeProperty() )
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

                    case TypeSystem::CoreTypes::U8:
                    case TypeSystem::CoreTypes::U16:
                    case TypeSystem::CoreTypes::U32:
                    case TypeSystem::CoreTypes::U64:
                    case TypeSystem::CoreTypes::S8:
                    case TypeSystem::CoreTypes::S16:
                    case TypeSystem::CoreTypes::S32:
                    case TypeSystem::CoreTypes::S64:
                    case TypeSystem::CoreTypes::F32:
                    case TypeSystem::CoreTypes::F64:
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