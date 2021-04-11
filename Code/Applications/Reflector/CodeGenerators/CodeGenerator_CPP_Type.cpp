#include "CodeGenerator_CPP_Type.h"
#include "System/TypeSystem/TypeID.h"
#include "System/TypeSystem/TypeRegistry.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace CPP
    {
        //-------------------------------------------------------------------------
        // Helper Methods
        //-------------------------------------------------------------------------

        static void GenerateFactoryMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            if ( !type.IsAbstract() )
            {
                file << "                    return KRG::New<" << type.m_namespace << type.m_name << ">();\n";
            }
            else
            {
                file << "                    KRG_HALT(); // Error! Trying to instantiate an abstract type!\n";
                file << "                    return nullptr;\n";
            }
            file << "                }\n\n";
        }

        static void GenerateDynamicArrayAccessorMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pType );\n";

            for ( auto& propertyDesc : type.m_properties )
            {
                if ( propertyDesc.IsDynamicArrayProperty() )
                {
                    file << "                    if ( arrayID == " << propertyDesc.m_propertyID << " )\n";
                    file << "                    {\n";
                    file << "                        if ( ( arrayIdx + 1 ) >= pActualType->" << propertyDesc.m_name << ".size() )\n";
                    file << "                        {\n";
                    file << "                            pActualType->" << propertyDesc.m_name << ".resize( arrayIdx + 1 );\n";
                    file << "                        }\n\n";
                    file << "                        return (Byte*) &pActualType->" << propertyDesc.m_name << "[arrayIdx];\n";
                    file << "                    }\n\n";
                }
            }

            file << "                    // We should never get here since we are asking for a ptr to an invalid property\n";
            file << "                    KRG_UNREACHABLE_CODE();\n";
            file << "                    return nullptr;\n";
            file << "                }\n\n";
        }

        static void GenerateExpectedResourceTypeMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pType );\n";

            for ( auto& propertyDesc : type.m_properties )
            {
                if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
                {
                    file << "                    if ( propertyID == " << propertyDesc.m_propertyID << " )\n";
                    file << "                    {\n";
                    file << "                        return " << propertyDesc.m_templateArgTypeName << "::GetStaticResourceTypeID();\n";
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

        static void GenerateSerializeFunction( std::stringstream& file, TypeDescriptor const& type, String const& exportMacro, TVector<TypeDescriptor> const& parentDescs )
        {
            file << "    template<class Archive>\n";
            file << "    " << exportMacro;
            if ( !exportMacro.empty() ) file << " ";
            file << "void serialize( Archive& archive, " << type.m_namespace << type.m_name << "& type )\n";
            file << "    {\n";

            if ( !type.m_properties.empty() || !type.m_parents.empty() )
            {
                file << "        archive( ";
                String serializationList;

                for ( auto& parent : parentDescs )
                {
                    serializationList = serializationList + "cereal::base_class<" + parent.m_namespace + parent.m_name + ">( &type ), ";
                }

                for ( auto& prop : type.m_properties )
                {
                    serializationList = serializationList + "KRG_NVP( " + prop.m_name + " ), ";
                }

                file.write( serializationList.c_str(), serializationList.size() - 2 );
                file << " );\n";
            }

            file << "    }\n";
        }

        //-------------------------------------------------------------------------
        // Resource Management
        //-------------------------------------------------------------------------

        static void GenerateLoadResourcesMethod( ReflectionDatabase const& database, std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            file << "                    KRG_ASSERT( pResourceSystem != nullptr );\n";
            file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pType );\n\n";

            for ( auto& propertyDesc : type.m_properties )
            {
                if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
                {
                    if ( propertyDesc.IsArrayProperty() )
                    {
                        if ( propertyDesc.IsDynamicArrayProperty() )
                        {
                            file << "                    for ( auto& resourcePtr : pActualType->" << propertyDesc.m_name << " )\n";
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
                                file << "                    if ( pActualType->" << propertyDesc.m_name << "[" << i << "].IsValid() )\n";
                                file << "                    {\n";
                                file << "                        pResourceSystem->LoadResource( pActualType->" << propertyDesc.m_name << "[" << i << "], requesterID );\n";
                                file << "                    }\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    if ( pActualType->" << propertyDesc.m_name << ".IsValid() )\n";
                        file << "                    {\n";
                        file << "                        pResourceSystem->LoadResource( pActualType->" << propertyDesc.m_name << ", requesterID );\n";
                        file << "                    }\n\n";
                    }
                }
                else if ( !IsCoreType( propertyDesc.m_typeID ) && !propertyDesc.IsEnumProperty() && !propertyDesc.IsBitFlagsProperty() )
                {
                    if ( propertyDesc.IsArrayProperty() )
                    {
                        if ( propertyDesc.IsDynamicArrayProperty() )
                        {
                            file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name << " )\n";
                            file << "                    {\n";
                            file << "                        " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );\n";
                            file << "                    }\n\n";
                        }
                        else // Static array
                        {
                            for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                            {
                                file << "                    " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name << "[" << i << "] );\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name << " );\n\n";
                    }
                }
            }

            file << "                }\n\n";
        }

        static void GenerateUnloadResourcesMethod( ReflectionDatabase const& database, std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            file << "                    KRG_ASSERT( pResourceSystem != nullptr );\n";
            file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pType );\n\n";

            for ( auto& propertyDesc : type.m_properties )
            {
                if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
                {
                    if ( propertyDesc.IsArrayProperty() )
                    {
                        if ( propertyDesc.IsDynamicArrayProperty() )
                        {
                            file << "                    for ( auto& resourcePtr : pActualType->" << propertyDesc.m_name << " )\n";
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
                                file << "                    if ( pActualType->" << propertyDesc.m_name << "[" << i << "].IsValid() )\n";
                                file << "                    {\n";
                                file << "                        pResourceSystem->UnloadResource( pActualType->" << propertyDesc.m_name << "[" << i << "], requesterID );\n";
                                file << "                    }\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    if ( pActualType->" << propertyDesc.m_name << ".IsValid() )\n";
                        file << "                    {\n";
                        file << "                        pResourceSystem->UnloadResource( pActualType->" << propertyDesc.m_name << ", requesterID );\n";
                        file << "                    }\n\n";
                    }
                }
                else if ( !IsCoreType( propertyDesc.m_typeID ) && !propertyDesc.IsEnumProperty() && !propertyDesc.IsBitFlagsProperty() )
                {
                    if ( propertyDesc.IsArrayProperty() )
                    {
                        if ( propertyDesc.IsDynamicArrayProperty() )
                        {
                            file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name << " )\n";
                            file << "                    {\n";
                            file << "                        " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );\n";
                            file << "                    }\n\n";
                        }
                        else // Static array
                        {
                            for ( auto i = 0; i < propertyDesc.m_arraySize; i++ )
                            {
                                file << "                    " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name << "[" << i << "] );\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->" << propertyDesc.m_name << " );\n\n";
                    }
                }
            }

            file << "                }\n\n";
        }

        static void GenerateResourceLoadingStatusMethod( ReflectionDatabase const& database, std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pType );\n";
            file << "                    LoadingStatus status = LoadingStatus::Loaded;\n\n";

            for ( auto& propertyDesc : type.m_properties )
            {
                if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
                {
                    if ( propertyDesc.IsArrayProperty() )
                    {
                        if ( propertyDesc.IsDynamicArrayProperty() )
                        {
                            file << "                    for ( auto const& resourcePtr : pActualType->" << propertyDesc.m_name << " )\n";
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
                                file << "                    if ( !pActualType->" << propertyDesc.m_name << "[" << i << "].IsValid() || pActualType->" << propertyDesc.m_name << "[" << i << "].HasLoadingFailed() )\n";
                                file << "                    {\n";
                                file << "                        status = LoadingStatus::Failed;\n";
                                file << "                    }\n";
                                file << "                    else if ( pActualType->" << propertyDesc.m_name << "[" << i << "].IsUnloaded() || pActualType->" << propertyDesc.m_name << "[" << i << "].IsLoading() )\n";
                                file << "                    {\n";
                                file << "                        return LoadingStatus::Loading;\n";
                                file << "                    }\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    if ( !pActualType->" << propertyDesc.m_name << ".IsValid() || pActualType->" << propertyDesc.m_name << ".HasLoadingFailed() )\n";
                        file << "                    {\n";
                        file << "                        status = LoadingStatus::Failed;\n";
                        file << "                    }\n";
                        file << "                    else if ( pActualType->" << propertyDesc.m_name << ".IsUnloaded() || pActualType->" << propertyDesc.m_name << ".IsLoading() )\n";
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
                            file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name << " )\n";
                            file << "                    {\n";
                            file << "                        status = " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );\n";
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
                                file << "                    status = " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->" << propertyDesc.m_name << "[" << i << "] ); \n";
                                file << "                    if ( status == LoadingStatus::Loading )\n";
                                file << "                    {\n";
                                file << "                        return LoadingStatus::Loading;\n";
                                file << "                    }\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    status = " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->" << propertyDesc.m_name << " );\n";
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

        static void GenerateResourceUnloadingStatusMethod( ReflectionDatabase const& database, std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";
            file << "                    auto pActualType = reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pType );\n";
            file << "                    LoadingStatus status = LoadingStatus::Unloading;\n\n";

            for ( auto& propertyDesc : type.m_properties )
            {
                if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr )
                {
                    if ( propertyDesc.IsArrayProperty() )
                    {
                        if ( propertyDesc.IsDynamicArrayProperty() )
                        {
                            file << "                    for ( auto const& resourcePtr : pActualType->" << propertyDesc.m_name << " )\n";
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
                                file << "                    KRG_ASSERT( !pActualType->" << propertyDesc.m_name << "[" << i << "].IsLoading() );\n";
                                file << "                    if ( !pActualType->" << propertyDesc.m_name << "[" << i << "].IsUnloaded() )\n";
                                file << "                    {\n";
                                file << "                        return LoadingStatus::Unloading;\n";
                                file << "                    }\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    KRG_ASSERT( !pActualType->" << propertyDesc.m_name << ".IsLoading() );\n";
                        file << "                    if ( !pActualType->" << propertyDesc.m_name << ".IsUnloaded() )\n";
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
                            file << "                    for ( auto& propertyValue : pActualType->" << propertyDesc.m_name << " )\n";
                            file << "                    {\n";
                            file << "                        status = " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );\n";
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
                                file << "                    status = " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->" << propertyDesc.m_name << "[" << i << "] ); \n";
                                file << "                    if ( status != LoadingStatus::Unloaded )\n";
                                file << "                    {\n";
                                file << "                        return LoadingStatus::Unloading;\n";
                                file << "                    }\n\n";
                            }
                        }
                    }
                    else
                    {
                        file << "                    status = " << propertyDesc.m_typeName << "::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->" << propertyDesc.m_name << " );\n";
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

        static void GenerateLoadMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "\n";
            file << "    void " << type.m_namespace << type.m_name << "::Load( EntityModel::LoadingContext const& context, UUID requesterID )\n";
            file << "    {\n";

            if ( !type.m_properties.empty() )
            {
                file << "        " << type.m_namespace << type.m_name << "::StaticTypeInfo->m_pTypeHelper->LoadResources( context.m_pResourceSystem, requesterID, this );\n";
                file << "        m_status = Status::Loading;\n";
            }
            else
            {
                file << "        m_status = Status::Loaded;\n";
            }

            file << "    }\n";
        }

        static void GenerateUnloadMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "\n";
            file << "    void " << type.m_namespace << type.m_name << "::Unload( EntityModel::LoadingContext const& context, UUID requesterID )\n";
            file << "    {\n";

            if ( !type.m_properties.empty() )
            {
                file << "        " << type.m_namespace << type.m_name << "::StaticTypeInfo->m_pTypeHelper->UnloadResources( context.m_pResourceSystem, requesterID, this );\n";
            }

            file << "        m_status = Status::Unloaded;\n";
            file << "    }\n";
        }

        static void GenerateUpdateLoadingMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "\n";
            file << "    void " << type.m_namespace << type.m_name << "::UpdateLoading()\n";
            file << "    {\n";
            file << "        if( m_status == Status::Loading )\n";
            file << "        {\n";

            {
                if ( !type.m_properties.empty() )
                {
                    // Wait for resources to be loaded
                    file << "            auto const resourceLoadingStatus = " << type.m_namespace << type.m_name << "::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( this );\n";
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

        static void GenerateGetTypeInfoMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "\n";
            file << "    TypeSystem::TypeInfo const* " << type.m_namespace << type.m_name << "::GetTypeInfo() const\n";
            file << "    {\n";
            file << "        return " << type.m_namespace << type.m_name << "::StaticTypeInfo;\n";
            file << "    }\n";
        }

        //-------------------------------------------------------------------------
        // Type Registration
        //-------------------------------------------------------------------------

        static void GeneratePropertyRegistrationMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            auto GeneratePropertyRegistrationCode = [&file, type] ( PropertyDescriptor const& prop )
            {
                String const templateSpecializationString = prop.m_templateArgTypeName.empty() ? String() : "<" + prop.m_templateArgTypeName + ">";

                file << "\n";
                file << "            //-------------------------------------------------------------------------\n\n";
                file << "            propertyInfo.m_ID = StringID( \"" << prop.m_name << "\" );\n";
                file << "            propertyInfo.m_typeID = TypeSystem::TypeID( \"" << prop.m_typeName << "\" );\n";
                file << "            propertyInfo.m_parentTypeID = " << type.m_ID << ";\n";
                file << "            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( \"" << prop.m_templateArgTypeName << "\" );\n";

                // Abstract types cannot have default values since they cannot be instantiated
                if ( type.IsAbstract() )
                {
                    file << "            propertyInfo.m_pDefaultValue = nullptr;\n";
                }
                else
                {
                    file << "            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->" << prop.m_name << ";\n";

                    file << "            propertyInfo.m_offset = offsetof( " << type.m_namespace << type.m_name << ", " << prop.m_name << " );\n";

                    if ( prop.IsDynamicArrayProperty() )
                    {
                        file << "            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->" << prop.m_name << ".data();\n";
                        file << "            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->" << prop.m_name << ".size();\n";
                        file << "            propertyInfo.m_arrayElementSize = (int32) sizeof( " << prop.m_typeName << templateSpecializationString << " );\n";
                        file << "            propertyInfo.m_size = sizeof( TVector<" << prop.m_typeName << templateSpecializationString << "> );\n";
                    }
                    else if ( prop.IsStaticArrayProperty() )
                    {
                        file << "            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->" << prop.m_name << ";\n";
                        file << "            propertyInfo.m_arraySize = " << prop.GetArraySize() << ";\n";
                        file << "            propertyInfo.m_arrayElementSize = (int32) sizeof( " << prop.m_typeName << templateSpecializationString << " );\n";
                        file << "            propertyInfo.m_size = sizeof( " << prop.m_typeName << templateSpecializationString << " ) * " << prop.GetArraySize() << ";\n";
                    }
                    else
                    {
                        file << "            propertyInfo.m_size = sizeof( " << prop.m_typeName << templateSpecializationString << " );\n";
                    }

                    file << "            propertyInfo.m_flags.Set( " << prop.m_flags << " );\n";
                    file << "            m_properties.emplace_back( propertyInfo );\n";
                    file << "            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );\n";
                }
            };

            //-------------------------------------------------------------------------

            file << "        template<>\n";
            file << "        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace << type.m_name << "> >( void const* pDefaultTypeInstance )\n";
            file << "        {\n";
            
            // Create the default type instance to use as a reference for default values
            if ( !type.IsAbstract() )
            {
                file << "            KRG_ASSERT( pDefaultTypeInstance != nullptr );\n";
                file << "            " << type.m_namespace << type.m_name << " const* pActualDefaultTypeInstance = ( " << type.m_namespace << type.m_name << " const* ) pDefaultTypeInstance;\n\n";
            }

            //-------------------------------------------------------------------------
            
            file << "            PropertyInfo propertyInfo;\n";

            for ( auto& prop : type.m_properties )
            {
                GeneratePropertyRegistrationCode( prop );
            }

            file << "        }\n";
        }

        static void GenerateTypeRegistrationMethod( std::stringstream& file, TypeDescriptor const& type, TVector<TypeDescriptor> const& parentDescs )
        {
            file << "                {\n";

            // Create default type instance
            if ( !type.IsAbstract() )
            {
                file << "                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );\n";
                file << "                    pDefaultTypeInstance = KRG::Alloc( sizeof( " << type.m_namespace << type.m_name << " ), alignof( " << type.m_namespace << type.m_name << " ) );\n";
                file << "                    new ( pDefaultTypeInstance ) " << type.m_namespace << type.m_name << ";\n\n";
            }

            // Create type info
            file << "                    TypeSystem::TypeInfo typeInfo;\n";
            file << "                    typeInfo.m_ID = TypeSystem::TypeID( \"" << type.m_namespace << type.m_name << "\" );\n";
            file << "                    typeInfo.m_size = sizeof( " << type.m_namespace << type.m_name << " );\n";
            file << "                    typeInfo.m_alignment = alignof( " << type.m_namespace << type.m_name << " );\n";
            file << "                    typeInfo.m_pTypeHelper = &StaticTypeHelper; \n";

            // Add type metadata
            if ( type.IsEntity() )
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
                    file << "                    pParentType = " << parent.m_namespace << parent.m_name << "::StaticTypeInfo;\n";
                    file << "                    KRG_ASSERT( pParentType != nullptr );\n";
                    file << "                    typeInfo.m_parentTypes.push_back( pParentType );\n\n";
                }
            }

            file << "                    // Register properties and type\n";
            file << "                    //-------------------------------------------------------------------------\n\n";

            if ( !type.m_properties.empty() )
            {
                file << "                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace << type.m_name << "> >( DefaultTypeInstancePtr );\n";
            }

            file << "                    " << type.m_namespace << type.m_name << "::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );\n";
            file << "                }\n\n";
        }

        static void GenerateTypeUnregistrationMethod( std::stringstream& file, TypeDescriptor const& type )
        {
            file << "                {\n";

            // Unregister type
            file << "                    auto const ID = TypeSystem::TypeID( \"" << type.m_namespace << type.m_name << "\" );\n";
            file << "                    typeRegistry.UnregisterType( ID );\n\n";

            // Destroy default type instance
            if ( !type.IsAbstract() )
            {
                file << "                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );\n";
                file << "                    reinterpret_cast<" << type.m_namespace << type.m_name << "*>( pDefaultTypeInstance )->~" << type.m_name << "();\n";
                file << "                    KRG::Free( pDefaultTypeInstance );\n";
            }

            file << "                }\n\n";
        }

        //-------------------------------------------------------------------------
        // File generation
        //-------------------------------------------------------------------------

        static void GenerateHeaderFile( std::stringstream& file, ReflectionDatabase const& database, String const& exportMacro, TypeDescriptor const& type, TVector<TypeDescriptor> const& parentDescs )
        {
            file << "\n";
            file << "//-------------------------------------------------------------------------\n";
            file << "// TypeHelper: " << type.m_namespace << type.m_name << "\n";
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
            file << "            class " << exportMacro << " TTypeHelper<" << type.m_namespace << type.m_name << "> : public ITypeHelper\n";
            file << "            {\n";
            file << "                static TTypeHelper<" << type.m_namespace << type.m_name << "> StaticTypeHelper;\n\n";
            file << "                static void const* DefaultTypeInstancePtr;\n\n";

            file << "            public:\n\n";

            file << "                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )\n";
            GenerateTypeRegistrationMethod( file, type, parentDescs );

            file << "                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )\n";
            GenerateTypeUnregistrationMethod( file, type );

            file << "                virtual void* CreateType() const override final\n";
            GenerateFactoryMethod( file, type );

            file << "                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final\n";
            GenerateLoadResourcesMethod( database, file, type );

            file << "                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final\n";
            GenerateUnloadResourcesMethod( database, file, type );

            file << "                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final\n";
            GenerateResourceLoadingStatusMethod( database, file, type );

            file << "                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final\n";
            GenerateResourceUnloadingStatusMethod( database, file, type );

            file << "                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final\n";
            GenerateDynamicArrayAccessorMethod( file, type );

            file << "                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final\n";
            GenerateExpectedResourceTypeMethod( file, type );

            file << "            };\n";
            file << "        }\n";
            file << "    }\n";
            file << "}\n";
        }

        static void GenerateCodeFile( std::stringstream& file, TypeDescriptor const& type, TVector<TypeDescriptor> const& parentDescs )
        {
            file << "\n";
            file << "//-------------------------------------------------------------------------\n";
            file << "// TypeHelper: " << type.m_namespace << type.m_name << "\n";
            file << "//-------------------------------------------------------------------------\n\n";
            file << "namespace KRG\n";
            file << "{\n";

            // Generate static typeinfo
            file << "    TypeSystem::TypeInfo const* " << type.m_namespace << type.m_name << "::StaticTypeInfo = nullptr;\n";

            // Define static type helper
            file << "    namespace TypeSystem\n";
            file << "    {\n";
            file << "        namespace TypeHelpers\n";
            file << "        {\n";

            file << "            void const* TTypeHelper<" << type.m_namespace << type.m_name << ">::DefaultTypeInstancePtr = nullptr;\n\n";
            file << "            TTypeHelper<" << type.m_namespace << type.m_name << "> TTypeHelper<" << type.m_namespace << type.m_name << ">::StaticTypeHelper;\n";

            file << "        }\n";
            file << "    }\n";

            // Generate entity component methods
            if ( type.IsEntityComponent() )
            {
                GenerateGetTypeInfoMethod( file, type );
                GenerateLoadMethod( file, type );
                GenerateUnloadMethod( file, type );
                GenerateUpdateLoadingMethod( file, type );
            }

            file << "}\n";
        }

        //-------------------------------------------------------------------------

        namespace TypeGenerator
        {
            void Generate( ReflectionDatabase const& database, std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, TypeDescriptor const& type, TVector<TypeDescriptor> const& parentDescs )
            {
                GenerateHeaderFile( headerFile, database, exportMacro, type, parentDescs );
                GenerateCodeFile( codeFile, type, parentDescs );
            }

            void GenerateRegistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type )
            {
                file << "    TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace << type.m_name << ">::RegisterType( typeRegistry );\n";
            }

            void GenerateUnregistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type )
            {
                file << "    TypeSystem::TypeHelpers::TTypeHelper<" << type.m_namespace << type.m_name << ">::UnregisterType( typeRegistry );\n";
            }
        }
    }
}