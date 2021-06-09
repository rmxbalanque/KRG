#include "CodeGenerator_CPP_Type.h"
#include "System/TypeSystem/TypeID.h"
#include "System/TypeSystem/TypeRegistry.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG::CPP
{
    //-------------------------------------------------------------------------
        // Factory/Serialization Methods
        //-------------------------------------------------------------------------

    static void GenerateSerializeFunction( std::stringstream& file, ReflectedType const& type, String const& exportMacro, TVector<ReflectedType> const& parentDescs )
    {
        file << "    template<class Archive>\n";
        file << "    " << exportMacro.c_str();
        if ( !exportMacro.empty() ) file << " ";
        file << "void serialize( Archive& archive, " << type.m_namespace.c_str() << type.m_name.c_str() << "& type )\n";
        file << "    {\n";

        if ( !type.m_properties.empty() || !type.m_parents.empty() )
        {
            file << "        archive( ";
            String serializationList;

            for ( auto& parent : parentDescs )
            {
                serializationList = serializationList + "cereal::base_class<" + parent.m_namespace.c_str() + parent.m_name.c_str() + ">( &type ), ";
            }

            for ( auto& prop : type.m_properties )
            {
                serializationList = serializationList + "KRG_NVP( " + prop.m_name.c_str() + " ), ";
            }

            file.write( serializationList.c_str(), serializationList.size() - 2 );
            file << " );\n";
        }

        file << "    }\n";
    }

    static void GenerateCreationMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual IRegisteredType* CreateType() const override final\n";
        file << "                {\n";
        if ( !type.IsAbstract() )
        {
            file << "                    return KRG::New<" << type.m_namespace.c_str() << type.m_name.c_str() << ">();\n";
        }
        else
        {
            file << "                    KRG_HALT(); // Error! Trying to instantiate an abstract type!\n";
            file << "                    return nullptr;\n";
        }
        file << "                }\n\n";
    }

    static void GenerateInPlaceCreationMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final\n";
        file << "                {\n";
        if ( !type.IsAbstract() )
        {
            file << "                    KRG_ASSERT( pAllocatedMemory != nullptr );\n";
            file << "                    new( pAllocatedMemory ) " << type.m_namespace.c_str() << type.m_name.c_str() << "();\n";
        }
        else
        {
            file << "                    KRG_HALT(); // Error! Trying to instantiate an abstract type!\n";
        }
        file << "                }\n\n";
    }

    //-------------------------------------------------------------------------
    // Array Methods
    //-------------------------------------------------------------------------

    static void GenerateArrayAccessorMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pType );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.IsDynamicArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        if ( ( arrayIdx + 1 ) >= pActualType->" << propertyDesc.m_name.c_str() << ".size() )\n";
                file << "                        {\n";
                file << "                            pActualType->" << propertyDesc.m_name.c_str() << ".resize( arrayIdx + 1 );\n";
                file << "                        }\n\n";
                file << "                        return (Byte*) &pActualType->" << propertyDesc.m_name.c_str() << "[arrayIdx];\n";
                file << "                    }\n\n";
            }
            else if ( propertyDesc.IsStaticArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        return (Byte*) &pActualType->" << propertyDesc.m_name.c_str() << "[arrayIdx];\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                    return nullptr;\n";
        file << "                }\n\n";
    }

    static void GenerateArraySizeMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << " const*>( pTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.IsDynamicArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        return pActualType->" << propertyDesc.m_name.c_str() << ".size();\n";
                file << "                    }\n\n";
            }
            else if ( propertyDesc.IsStaticArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        return " << propertyDesc.GetArraySize() << ";\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                    return 0;\n";
        file << "                }\n\n";
    }

    static void GenerateArrayElementSizeMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final\n";
        file << "                {\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            String const templateSpecializationString = propertyDesc.m_templateArgTypeName.empty() ? String() : "<" + propertyDesc.m_templateArgTypeName + ">";

            if ( propertyDesc.IsArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        return sizeof( " << propertyDesc.m_typeName.c_str() << templateSpecializationString.c_str() << " );\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                    return 0;\n";
        file << "                }\n\n";
    }

    static void GenerateArrayClearMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.IsDynamicArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        pActualType->" << propertyDesc.m_name.c_str() << ".clear();\n";
                file << "                        return;\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                }\n\n";
    }

    static void GenerateAddArrayElementMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.IsDynamicArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        pActualType->" << propertyDesc.m_name.c_str() << ".emplace_back();\n";
                file << "                        return;\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                }\n\n";
    }

    static void GenerateRemoveArrayElementMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.IsDynamicArrayProperty() )
            {
                file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        pActualType->" << propertyDesc.m_name.c_str() << ".erase( pActualType->" << propertyDesc.m_name.c_str() << ".begin() + arrayIdx );\n";
                file << "                        return;\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                }\n\n";
    }

    //-------------------------------------------------------------------------
    // Default Value Methods
    //-------------------------------------------------------------------------

    static void GenerateAreAllPropertiesEqualMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final\n";
        file << "                {\n";
        file << "                    auto pTypeHelper = " << type.m_namespace.c_str() << type.m_name.c_str() << "::s_pTypeInfo->m_pTypeHelper;\n";
        file << "                    auto pType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << " const*>( pTypeInstance );\n";
        file << "                    auto pOtherType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << " const*>( pOtherTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            file << "                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, " << propertyDesc.m_propertyID << " ) )\n";
            file << "                    {\n";
            file << "                       return false;\n";
            file << "                    }\n\n";
        }

        file << "                    return true;\n";
        file << "                }\n\n";
    }

    static void GenerateIsPropertyEqualMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final\n";
        file << "                {\n";
        file << "                    auto pType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << " const*>( pTypeInstance );\n";
        file << "                    auto pOtherType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << " const*>( pOtherTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            InlineString<255> propertyTypeName = propertyDesc.m_typeName.c_str();
            if ( !propertyDesc.m_templateArgTypeName.empty() )
            {
                propertyTypeName += "<";
                propertyTypeName += propertyDesc.m_templateArgTypeName.c_str();
                propertyTypeName += ">";
            }

            //-------------------------------------------------------------------------

            file << "                    if ( propertyID == " << propertyDesc.m_propertyID << " )\n";
            file << "                    {\n";

            // Arrays
            if ( propertyDesc.IsArrayProperty() )
            {
                // Handle individual element comparison
                //-------------------------------------------------------------------------

                file << "                        // Compare array elements\n";
                file << "                        if ( arrayIdx != InvalidIndex )\n";
                file << "                        {\n";

                // If it's a dynamic array check the sizes first
                if ( propertyDesc.IsDynamicArrayProperty() )
                {
                    file << "                            if ( arrayIdx >= pOtherType->" << propertyDesc.m_name.c_str() << ".size() )\n";
                    file << "                            {\n";
                    file << "                                return false;\n";
                    file << "                            }\n\n";
                }

                if ( propertyDesc.IsStructureProperty() )
                {
                    file << "                            return " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->" << propertyDesc.m_name.c_str() << "[arrayIdx], &pOtherType->" << propertyDesc.m_name.c_str() << "[arrayIdx] );\n";
                }
                else
                {
                    file << "                            return pType->" << propertyDesc.m_name.c_str() << "[arrayIdx] == pOtherType->" << propertyDesc.m_name.c_str() << "[arrayIdx];\n";
                }
                file << "                        }\n";

                // Handle array comparison
                //-------------------------------------------------------------------------

                file << "                        else // Compare entire array contents\n";
                file << "                        {\n";

                // If it's a dynamic array check the sizes first
                if ( propertyDesc.IsDynamicArrayProperty() )
                {
                    file << "                            if ( pType->" << propertyDesc.m_name.c_str() << ".size() != pOtherType->" << propertyDesc.m_name.c_str() << ".size() )\n";
                    file << "                            {\n";
                    file << "                                return false;\n";
                    file << "                            }\n\n";

                    file << "                            for ( size_t i = 0; i < pType->" << propertyDesc.m_name.c_str() << ".size(); i++ )\n";
                }
                else
                {
                    file << "                            for ( size_t i = 0; i < " << propertyDesc.GetArraySize() << "; i++ )\n";
                }

                file << "                            {\n";

                if ( propertyDesc.IsStructureProperty() )
                {
                    file << "                                if( !" << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->" << propertyDesc.m_name.c_str() << "[i], &pOtherType->" << propertyDesc.m_name.c_str() << "[i] ) )\n";
                    file << "                                {\n";
                    file << "                                    return false;\n";
                    file << "                                }\n";
                }
                else
                {
                    file << "                                if( pType->" << propertyDesc.m_name.c_str() << "[i] != pOtherType->" << propertyDesc.m_name.c_str() << "[i] )\n";
                    file << "                                {\n";
                    file << "                                    return false;\n";
                    file << "                                }\n";
                }

                file << "                            }\n\n";

                file << "                            return true;\n";
                file << "                        }\n";
            }
            else // Non-Array properties
            {
                if ( propertyDesc.IsStructureProperty() )
                {
                    file << "                        return " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->" << propertyDesc.m_name.c_str() << ", &pOtherType->" << propertyDesc.m_name.c_str() << " );\n";
                }
                else
                {
                    file << "                        return pType->" << propertyDesc.m_name.c_str() << " == pOtherType->" << propertyDesc.m_name.c_str() << ";\n";
                }
            }

            file << "                    }\n\n";
        }

        file << "                    return false;\n";
        file << "                }\n\n";
    }

    static void GenerateSetToDefaultValueMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final\n";
        file << "                {\n";
        file << "                    auto pDefaultType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << " const*>( GetDefaultTypeInstancePtr() );\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pTypeInstance );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            file << "                    if ( propertyID == " << propertyDesc.m_propertyID << " )\n";
            file << "                    {\n";

            if ( propertyDesc.IsStaticArrayProperty() )
            {
                for ( auto i = 0u; i < propertyDesc.GetArraySize(); i++ )
                {
                    file << "                        pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "] = pDefaultType->" << propertyDesc.m_name.c_str() << "[" << i << "];\n";
                }
            }
            else
            {
                file << "                        pActualType->" << propertyDesc.m_name.c_str() << " = pDefaultType->" << propertyDesc.m_name.c_str() << ";\n";
            }

            file << "                        return;\n";
            file << "                    }\n\n";
        }

        file << "                }\n\n";
    }

    //-------------------------------------------------------------------------
    // Resource Methods
    //-------------------------------------------------------------------------

    static void GenerateExpectedResourceTypeMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pType );\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
            {
                file << "                    if ( propertyID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        return " << propertyDesc.m_templateArgTypeName.c_str() << "::GetStaticResourceTypeID();\n";
                file << "                    }\n\n";
            }
            else if ( propertyDesc.m_typeID == CoreTypes::ResourcePtr )
            {
                file << "                    if ( propertyID == " << propertyDesc.m_propertyID << " )\n";
                file << "                    {\n";
                file << "                        return ResourceTypeID();\n";
                file << "                    }\n\n";
            }
        }

        file << "                    // We should never get here since we are asking for a resource type of an invalid property\n";
        file << "                    KRG_UNREACHABLE_CODE();\n";
        file << "                    return ResourceTypeID();\n";
        file << "                }\n\n";
    }

    static void GenerateLoadResourcesMethod( ReflectionDatabase const& database, std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final\n";
        file << "                {\n";
        file << "                    KRG_ASSERT( pResourceSystem != nullptr );\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pType );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto& resourcePtr : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        if ( resourcePtr.IsValid() )\n";
                        file << "                        {\n";
                        file << "                            pResourceSystem->LoadResource( resourcePtr, requesterID );\n";
                        file << "                        }\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    if ( pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsValid() )\n";
                            file << "                    {\n";
                            file << "                        pResourceSystem->LoadResource( pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "], requesterID );\n";
                            file << "                    }\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    if ( pActualType->" << propertyDesc.m_name.c_str() << ".IsValid() )\n";
                    file << "                    {\n";
                    file << "                        pResourceSystem->LoadResource( pActualType->" << propertyDesc.m_name.c_str() << ", requesterID );\n";
                    file << "                    }\n\n";
                }
            }
            else if ( !IsCoreType( propertyDesc.m_typeID ) && !propertyDesc.IsEnumProperty() && !propertyDesc.IsBitFlagsProperty() )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "] );\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name.c_str() << " );\n\n";
                }
            }
        }

        file << "                }\n\n";
    }

    static void GenerateUnloadResourcesMethod( ReflectionDatabase const& database, std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final\n";
        file << "                {\n";
        file << "                    KRG_ASSERT( pResourceSystem != nullptr );\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pType );\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto& resourcePtr : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        if ( resourcePtr.IsValid() )\n";
                        file << "                        {\n";
                        file << "                            pResourceSystem->UnloadResource( resourcePtr, requesterID );\n";
                        file << "                        }\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    if ( pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsValid() )\n";
                            file << "                    {\n";
                            file << "                        pResourceSystem->UnloadResource( pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "], requesterID );\n";
                            file << "                    }\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    if ( pActualType->" << propertyDesc.m_name.c_str() << ".IsValid() )\n";
                    file << "                    {\n";
                    file << "                        pResourceSystem->UnloadResource( pActualType->" << propertyDesc.m_name.c_str() << ", requesterID );\n";
                    file << "                    }\n\n";
                }
            }
            else if ( !IsCoreType( propertyDesc.m_typeID ) && !propertyDesc.IsEnumProperty() && !propertyDesc.IsBitFlagsProperty() )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "] );\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name.c_str() << " );\n\n";
                }
            }
        }

        file << "                }\n\n";
    }

    static void GenerateResourceLoadingStatusMethod( ReflectionDatabase const& database, std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pType );\n";
        file << "                    LoadingStatus status = LoadingStatus::Loaded;\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto const& resourcePtr : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        if ( !resourcePtr.IsValid() || resourcePtr.HasLoadingFailed() )\n";
                        file << "                        {\n";
                        file << "                            status = LoadingStatus::Failed;\n";
                        file << "                        }\n";
                        file << "                        else if ( resourcePtr.IsUnloaded() || resourcePtr.IsLoading() )\n";
                        file << "                        {\n";
                        file << "                            return LoadingStatus::Loading;\n";
                        file << "                        }\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    if ( !pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsValid() || pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].HasLoadingFailed() )\n";
                            file << "                    {\n";
                            file << "                        status = LoadingStatus::Failed;\n";
                            file << "                    }\n";
                            file << "                    else if ( pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsUnloaded() || pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsLoading() )\n";
                            file << "                    {\n";
                            file << "                        return LoadingStatus::Loading;\n";
                            file << "                    }\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    if ( !pActualType->" << propertyDesc.m_name.c_str() << ".IsValid() || pActualType->" << propertyDesc.m_name.c_str() << ".HasLoadingFailed() )\n";
                    file << "                    {\n";
                    file << "                        status = LoadingStatus::Failed;\n";
                    file << "                    }\n";
                    file << "                    else if ( pActualType->" << propertyDesc.m_name.c_str() << ".IsUnloaded() || pActualType->" << propertyDesc.m_name.c_str() << ".IsLoading() )\n";
                    file << "                    {\n";
                    file << "                        return LoadingStatus::Loading;\n";
                    file << "                    }\n\n";
                }
            }
            else if ( !IsCoreType( propertyDesc.m_typeID ) && !propertyDesc.IsEnumProperty() && !propertyDesc.IsBitFlagsProperty() )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        status = " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );\n";
                        file << "                        if ( status == LoadingStatus::Loading )\n";
                        file << "                        {\n";
                        file << "                            return LoadingStatus::Loading;\n";
                        file << "                        }\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    status = " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "] ); \n";
                            file << "                    if ( status == LoadingStatus::Loading )\n";
                            file << "                    {\n";
                            file << "                        return LoadingStatus::Loading;\n";
                            file << "                    }\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    status = " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->" << propertyDesc.m_name.c_str() << " );\n";
                    file << "                    if ( status == LoadingStatus::Loading )\n";
                    file << "                    {\n";
                    file << "                        return LoadingStatus::Loading;\n";
                    file << "                    }\n\n";
                }
            }
        }

        file << "                    return status;\n";
        file << "                }\n\n";
    }

    static void GenerateResourceUnloadingStatusMethod( ReflectionDatabase const& database, std::stringstream& file, ReflectedType const& type )
    {
        file << "                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final\n";
        file << "                {\n";
        file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pType );\n";
        file << "                    LoadingStatus status = LoadingStatus::Unloading;\n\n";

        for ( auto& propertyDesc : type.m_properties )
        {
            if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto const& resourcePtr : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        KRG_ASSERT( !resourcePtr.IsLoading() );\n";
                        file << "                        if ( !resourcePtr.IsUnloaded() )\n";
                        file << "                        {\n";
                        file << "                            return LoadingStatus::Unloading;\n";
                        file << "                        }\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    KRG_ASSERT( !pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsLoading() );\n";
                            file << "                    if ( !pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "].IsUnloaded() )\n";
                            file << "                    {\n";
                            file << "                        return LoadingStatus::Unloading;\n";
                            file << "                    }\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    KRG_ASSERT( !pActualType->" << propertyDesc.m_name.c_str() << ".IsLoading() );\n";
                    file << "                    if ( !pActualType->" << propertyDesc.m_name.c_str() << ".IsUnloaded() )\n";
                    file << "                    {\n";
                    file << "                        return LoadingStatus::Unloading;\n";
                    file << "                    }\n\n";
                }
            }
            else if ( !IsCoreType( propertyDesc.m_typeID ) && !propertyDesc.IsEnumProperty() && !propertyDesc.IsBitFlagsProperty() )
            {
                if ( propertyDesc.IsArrayProperty() )
                {
                    if ( propertyDesc.IsDynamicArrayProperty() )
                    {
                        file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name.c_str() << " )\n";
                        file << "                    {\n";
                        file << "                        status = " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );\n";
                        file << "                        if ( status != LoadingStatus::Unloaded )\n";
                        file << "                        {\n";
                        file << "                            return LoadingStatus::Unloading;\n";
                        file << "                        }\n";
                        file << "                    }\n\n";
                    }
                    else // Static array
                    {
                        for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                        {
                            file << "                    status = " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->" << propertyDesc.m_name.c_str() << "[" << i << "] ); \n";
                            file << "                    if ( status != LoadingStatus::Unloaded )\n";
                            file << "                    {\n";
                            file << "                        return LoadingStatus::Unloading;\n";
                            file << "                    }\n\n";
                        }
                    }
                }
                else
                {
                    file << "                    status = " << propertyDesc.m_typeName.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->" << propertyDesc.m_name.c_str() << " );\n";
                    file << "                    if ( status != LoadingStatus::Unloaded )\n";
                    file << "                    {\n";
                    file << "                        return LoadingStatus::Unloading;\n";
                    file << "                    }\n\n";
                }
            }
        }

        file << "                    return LoadingStatus::Unloaded;\n";
        file << "                }\n\n";
    }

    //-------------------------------------------------------------------------
    // Entity Methods
    //-------------------------------------------------------------------------

    static void GenerateLoadMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "\n";
        file << "    void " << type.m_namespace.c_str() << type.m_name.c_str() << "::Load( EntityModel::LoadingContext const& context, UUID requesterID )\n";
        file << "    {\n";

        if ( !type.m_properties.empty() )
        {
            file << "        " << type.m_namespace.c_str() << type.m_name.c_str() << "::s_pTypeInfo->m_pTypeHelper->LoadResources( context.m_pResourceSystem, requesterID, this );\n";
            file << "        m_status = Status::Loading;\n";
        }
        else
        {
            file << "        m_status = Status::Loaded;\n";
        }

        file << "    }\n";
    }

    static void GenerateUnloadMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "\n";
        file << "    void " << type.m_namespace.c_str() << type.m_name.c_str() << "::Unload( EntityModel::LoadingContext const& context, UUID requesterID )\n";
        file << "    {\n";

        if ( !type.m_properties.empty() )
        {
            file << "        " << type.m_namespace.c_str() << type.m_name.c_str() << "::s_pTypeInfo->m_pTypeHelper->UnloadResources( context.m_pResourceSystem, requesterID, this );\n";
        }

        file << "        m_status = Status::Unloaded;\n";
        file << "    }\n";
    }

    static void GenerateUpdateLoadingMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "\n";
        file << "    void " << type.m_namespace.c_str() << type.m_name.c_str() << "::UpdateLoading()\n";
        file << "    {\n";
        file << "        if( m_status == Status::Loading )\n";
        file << "        {\n";

        {
            if ( !type.m_properties.empty() )
            {
                // Wait for resources to be loaded
                file << "            auto const resourceLoadingStatus = " << type.m_namespace.c_str() << type.m_name.c_str() << "::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( this );\n";
                file << "            if ( resourceLoadingStatus == LoadingStatus::Loading )\n";
                file << "            {\n";
                file << "                return; // Something is still loading so early-out\n";
                file << "            }\n\n";

                // Set status
                file << "            if ( resourceLoadingStatus == LoadingStatus::Failed )\n";
                file << "            {\n";
                file << "                m_status = EntityComponent::Status::LoadingFailed;\n";
                file << "            }\n";
                file << "            else\n";
                file << "            {\n";
                file << "                m_status = EntityComponent::Status::Loaded;\n";
                file << "            }\n";
            }
            else
            {
                file << "            m_status = EntityComponent::Status::Loaded;\n";
            }
        }

        file << "        }\n";
        file << "    }\n";
    }

    //-------------------------------------------------------------------------
    // Type Registration Methods
    //-------------------------------------------------------------------------

    static void GeneratePropertyRegistrationMethod( std::stringstream& file, ReflectedType const& type )
    {
        auto GeneratePropertyRegistrationCode = [&file, type] ( ReflectedProperty const& prop )
        {
            String const templateSpecializationString = prop.m_templateArgTypeName.empty() ? String() : "<" + prop.m_templateArgTypeName + ">";

            file << "\n";
            file << "            //-------------------------------------------------------------------------\n\n";
            file << "            propertyInfo.m_ID = StringID( \"" << prop.m_name.c_str() << "\" );\n";
            file << "            propertyInfo.m_typeID = TypeSystem::TypeID( \"" << prop.m_typeName.c_str() << "\" );\n";
            file << "            propertyInfo.m_parentTypeID = " << type.m_ID << ";\n";
            file << "            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( \"" << prop.m_templateArgTypeName.c_str() << "\" );\n";

            // Abstract types cannot have default values since they cannot be instantiated
            if ( type.IsAbstract() )
            {
                file << "            propertyInfo.m_pDefaultValue = nullptr;\n";
            }
            else
            {
                file << "            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->" << prop.m_name.c_str() << ";\n";

                file << "            propertyInfo.m_offset = offsetof( " << type.m_namespace.c_str() << type.m_name.c_str() << ", " << prop.m_name.c_str() << " );\n";

                if ( prop.IsDynamicArrayProperty() )
                {
                    file << "            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->" << prop.m_name.c_str() << ".data();\n";
                    file << "            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->" << prop.m_name.c_str() << ".size();\n";
                    file << "            propertyInfo.m_arrayElementSize = (int32) sizeof( " << prop.m_typeName.c_str() << templateSpecializationString.c_str() << " );\n";
                    file << "            propertyInfo.m_size = sizeof( TVector<" << prop.m_typeName.c_str() << templateSpecializationString.c_str() << "> );\n";
                }
                else if ( prop.IsStaticArrayProperty() )
                {
                    file << "            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->" << prop.m_name.c_str() << ";\n";
                    file << "            propertyInfo.m_arraySize = " << prop.GetArraySize() << ";\n";
                    file << "            propertyInfo.m_arrayElementSize = (int32) sizeof( " << prop.m_typeName.c_str() << templateSpecializationString.c_str() << " );\n";
                    file << "            propertyInfo.m_size = sizeof( " << prop.m_typeName.c_str() << templateSpecializationString.c_str() << " ) * " << prop.GetArraySize() << ";\n";
                }
                else
                {
                    file << "            propertyInfo.m_size = sizeof( " << prop.m_typeName.c_str() << templateSpecializationString.c_str() << " );\n";
                }

                file << "            propertyInfo.m_flags.Set( " << prop.m_flags << " );\n";
                file << "            m_properties.emplace_back( propertyInfo );\n";
                file << "            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );\n";
            }
        };

        //-------------------------------------------------------------------------

        file << "        template<>\n";
        file << "        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << "> >( IRegisteredType const* pDefaultTypeInstance )\n";
        file << "        {\n";

        // Create the default type instance to use as a reference for default values
        if ( !type.IsAbstract() )
        {
            file << "            KRG_ASSERT( pDefaultTypeInstance != nullptr );\n";
            file << "            " << type.m_namespace.c_str() << type.m_name.c_str() << " const* pActualDefaultTypeInstance = ( " << type.m_namespace.c_str() << type.m_name.c_str() << " const* ) pDefaultTypeInstance;\n\n";
        }

        //-------------------------------------------------------------------------

        file << "            PropertyInfo propertyInfo;\n";

        for ( auto& prop : type.m_properties )
        {
            GeneratePropertyRegistrationCode( prop );
        }

        file << "        }\n";
    }

    static void GenerateTypeRegistrationMethod( std::stringstream& file, ReflectedType const& type, TVector<ReflectedType> const& parentDescs )
    {
        file << "                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )\n";
        file << "                {\n";

        // Create default type instance
        if ( !type.IsAbstract() )
        {
            file << "                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );\n";
            file << "                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( " << type.m_namespace.c_str() << type.m_name.c_str() << " ), alignof( " << type.m_namespace.c_str() << type.m_name.c_str() << " ) );\n";
            file << "                    new ( pDefaultTypeInstance ) " << type.m_namespace.c_str() << type.m_name.c_str() << ";\n\n";
        }

        // Create type info
        file << "                    TypeSystem::TypeInfo typeInfo;\n";
        file << "                    typeInfo.m_ID = TypeSystem::TypeID( \"" << type.m_namespace.c_str() << type.m_name.c_str() << "\" );\n";
        file << "                    typeInfo.m_size = sizeof( " << type.m_namespace.c_str() << type.m_name.c_str() << " );\n";
        file << "                    typeInfo.m_alignment = alignof( " << type.m_namespace.c_str() << type.m_name.c_str() << " );\n";
        file << "                    typeInfo.m_pTypeHelper = &StaticTypeHelper; \n";

        // Add type metadata
        if ( type.IsAbstract() )
        {
            file << "                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Abstract );\n\n";
        }
        else if ( type.IsEntity() )
        {
            file << "                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Entity );\n\n";
        }
        else if ( type.IsEntityComponent() )
        {
            file << "                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );\n\n";
        }
        else if ( type.IsEntitySystem() )
        {
            file << "                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntitySystem );\n\n";
        }
        else
        {
            file << "\n";
        }

        // Add interfaces
        if ( !type.m_parents.empty() )
        {
            file << "                    // Parent Types \n";
            file << "                    //-------------------------------------------------------------------------\n\n";
            file << "                    TypeSystem::TypeInfo const* pParentType = nullptr;\n\n";

            for ( auto& parent : parentDescs )
            {
                file << "                    pParentType = " << parent.m_namespace.c_str() << parent.m_name.c_str() << "::s_pTypeInfo;\n";
                file << "                    KRG_ASSERT( pParentType != nullptr );\n";
                file << "                    typeInfo.m_parentTypes.push_back( pParentType );\n\n";
            }
        }

        file << "                    // Register properties and type\n";
        file << "                    //-------------------------------------------------------------------------\n\n";

        if ( !type.m_properties.empty() )
        {
            file << "                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << "> >( s_pDefaultTypeInstancePtr );\n";
        }

        file << "                    " << type.m_namespace.c_str() << type.m_name.c_str() << "::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );\n";
        file << "                }\n\n";
    }

    static void GenerateTypeUnregistrationMethod( std::stringstream& file, ReflectedType const& type )
    {
        file << "                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )\n";
        file << "                {\n";

        // Unregister type
        file << "                    auto const ID = TypeSystem::TypeID( \"" << type.m_namespace.c_str() << type.m_name.c_str() << "\" );\n";
        file << "                    typeRegistry.UnregisterType( ID );\n\n";

        // Destroy default type instance
        if ( !type.IsAbstract() )
        {
            file << "                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );\n";
            file << "                    reinterpret_cast<" << type.m_namespace.c_str() << type.m_name.c_str() << "*>( pDefaultTypeInstance )->~" << type.m_name.c_str() << "();\n";
            file << "                    KRG::Free( pDefaultTypeInstance );\n";
        }

        file << "                }\n\n";
    }

    //-------------------------------------------------------------------------
    // File generation
    //-------------------------------------------------------------------------

    static void GenerateHeaderFile( std::stringstream& file, ReflectionDatabase const& database, String const& exportMacro, ReflectedType const& type, TVector<ReflectedType> const& parentDescs )
    {
        file << "\n";
        file << "//-------------------------------------------------------------------------\n";
        file << "// TypeHelper: " << type.m_namespace.c_str() << type.m_name.c_str() << "\n";
        file << "//-------------------------------------------------------------------------\n\n";
        file << "namespace KRG\n";
        file << "{\n";

        GenerateSerializeFunction( file, type, exportMacro, parentDescs );

        file << "\n";
        file << "    //-------------------------------------------------------------------------\n\n";

        file << "    namespace TypeSystem\n";
        file << "    {\n";

        GeneratePropertyRegistrationMethod( file, type );

        file << "\n";
        file << "        //-------------------------------------------------------------------------\n\n";

        file << "        namespace TypeHelpers\n";
        file << "        {\n";
        file << "            template<>\n";
        file << "            class " << exportMacro.c_str() << " TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << "> final : public ITypeHelper\n";
        file << "            {\n";
        file << "                static TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << "> StaticTypeHelper;\n\n";
        file << "                static IRegisteredType const* s_pDefaultTypeInstancePtr;\n\n";

        file << "            public:\n\n";

        file << "                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }\n\n";

        GenerateTypeRegistrationMethod( file, type, parentDescs );
        GenerateTypeUnregistrationMethod( file, type );
        GenerateCreationMethod( file, type );
        GenerateInPlaceCreationMethod( file, type );
        GenerateLoadResourcesMethod( database, file, type );
        GenerateUnloadResourcesMethod( database, file, type );
        GenerateResourceLoadingStatusMethod( database, file, type );
        GenerateResourceUnloadingStatusMethod( database, file, type );
        GenerateExpectedResourceTypeMethod( file, type );
        GenerateArrayAccessorMethod( file, type );
        GenerateArraySizeMethod( file, type );
        GenerateArrayElementSizeMethod( file, type );
        GenerateArrayClearMethod( file, type );
        GenerateAddArrayElementMethod( file, type );
        GenerateRemoveArrayElementMethod( file, type );
        GenerateAreAllPropertiesEqualMethod( file, type );
        GenerateIsPropertyEqualMethod( file, type );
        GenerateSetToDefaultValueMethod( file, type );

        file << "            };\n";
        file << "        }\n";
        file << "    }\n";
        file << "}\n";
    }

    static void GenerateCodeFile( std::stringstream& file, ReflectedType const& type, TVector<ReflectedType> const& parentDescs )
    {
        file << "\n";
        file << "//-------------------------------------------------------------------------\n";
        file << "// TypeHelper: " << type.m_namespace.c_str() << type.m_name.c_str() << "\n";
        file << "//-------------------------------------------------------------------------\n\n";
        file << "namespace KRG\n";
        file << "{\n";

        // Generate static typeinfo
        file << "    TypeSystem::TypeInfo const* " << type.m_namespace.c_str() << type.m_name.c_str() << "::s_pTypeInfo = nullptr;\n";

        // Define static type helper
        file << "    namespace TypeSystem\n";
        file << "    {\n";
        file << "        namespace TypeHelpers\n";
        file << "        {\n";

        file << "            IRegisteredType const* TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << ">::s_pDefaultTypeInstancePtr = nullptr;\n\n";
        file << "            TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << "> TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << ">::StaticTypeHelper;\n";

        file << "        }\n";
        file << "    }\n";

        // Generate entity component methods
        if ( type.IsEntityComponent() )
        {
            GenerateLoadMethod( file, type );
            GenerateUnloadMethod( file, type );
            GenerateUpdateLoadingMethod( file, type );
        }

        file << "}\n";
    }

    //-------------------------------------------------------------------------

    namespace TypeGenerator
    {
        void Generate( ReflectionDatabase const& database, std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, ReflectedType const& type, TVector<ReflectedType> const& parentDescs )
        {
            GenerateHeaderFile( headerFile, database, exportMacro, type, parentDescs );
            GenerateCodeFile( codeFile, type, parentDescs );
        }

        void GenerateRegistrationFunctionCall( std::stringstream& file, ReflectedType const& type )
        {
            file << "    TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << ">::RegisterType( typeRegistry );\n";
        }

        void GenerateUnregistrationFunctionCall( std::stringstream& file, ReflectedType const& type )
        {
            file << "    TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace.c_str() << type.m_name.c_str() << ">::UnregisterType( typeRegistry );\n";
        }
    }
}