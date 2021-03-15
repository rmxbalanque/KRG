#include "ClangVisitors_Structure.h"
#include "ClangVisitors_Enum.h"
#include "Applications/Reflector/ReflectorSettingsAndUtils.h"
#include "Applications/Reflector/Database/ReflectionDatabase.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "ClangVisitors_Macro.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            struct FieldName
            {
                String              m_name;
                TVector<FieldName>  m_templateArgs;

                inline void GetFlattenedTemplateArgs( String& flattenedArgs ) const
                {
                    if ( !m_templateArgs.empty() )
                    {
                        for ( auto arg : m_templateArgs )
                        {
                            flattenedArgs.append( arg.m_name );

                            if ( !arg.m_templateArgs.empty() )
                            {
                                flattenedArgs.append( "<" );
                                arg.GetFlattenedTemplateArgs( flattenedArgs );
                                flattenedArgs.append( ">" );
                            }

                            flattenedArgs.append( ", " );
                        }

                        flattenedArgs = flattenedArgs.substr( 0, flattenedArgs.length() - 2 );
                    }
                }

                bool AllowsTemplateArguments() const
                {
                    if ( m_name == "KRG::String" )
                    {
                        return false;
                    }

                    return true;
                }
            };

            static void GetFieldTypeName( ClangParserContext* pContext, TypeDescriptor* pType, CXType type, FieldName& name )
            {
                auto const pFieldQualType = ClangUtils::GetQualType( type );

                // Get typename
                if ( !ClangUtils::GetQualifiedNameForType( pFieldQualType, name.m_name ) )
                {
                    String typeSpelling = ClangUtils::GetString( clang_getTypeSpelling( type ) );
                    return pContext->LogError( "Failed to qualify typename for member: %s in class: %s and of type: %s", name.m_name.c_str(), pType->m_name.c_str(), typeSpelling.c_str() );
                }

                if ( name.AllowsTemplateArguments() )
                {
                    auto const numTemplateArguments = clang_Type_getNumTemplateArguments( type );
                    if ( numTemplateArguments > 0 )
                    {
                        // We only support one template arg for now
                        auto const argType = clang_Type_getTemplateArgumentAsType( type, 0 );

                        FieldName argName;
                        GetFieldTypeName( pContext, pType, argType, argName );
                        name.m_templateArgs.push_back( argName );
                    }
                }
            }

            static void GetAllDerivedProperties( ReflectionDatabase const* pDatabase, TVector<TypeSystem::TypeID> const& parentTypes, TVector<PropertyDescriptor>& results )
            {
                for ( auto const& parentID : parentTypes )
                {
                    TypeDescriptor const* pParentDesc = pDatabase->GetType( parentID );
                    if ( pParentDesc != nullptr )
                    {
                        GetAllDerivedProperties( pDatabase, pParentDesc->m_parents, results );
                        for ( auto& parentProperty : pParentDesc->m_properties )
                        {
                            results.push_back( parentProperty );
                        }
                    }
                }
            }

            //-------------------------------------------------------------------------

            CXChildVisitResult VisitStructureContents( CXCursor cr, CXCursor parent, CXClientData pClientData )
            {
                auto pContext = reinterpret_cast<ClangParserContext*>( pClientData );
                auto pClass = reinterpret_cast<TypeDescriptor*>( pContext->m_pCurrentEntry );

                uint32 const lineNumber = ClangUtils::GetLineNumberForCursor( cr );
                CXCursorKind kind = clang_getCursorKind( cr );
                switch ( kind )
                {
                    // Add base class
                    case CXCursor_CXXBaseSpecifier:
                    {
                        // Get qualified base type
                        clang::CXXBaseSpecifier* pBaseSpecifier = ( clang::CXXBaseSpecifier* ) cr.data[0];
                        String fullyQualifiedName;
                        if ( !ClangUtils::GetQualifiedNameForType( pBaseSpecifier->getType(), fullyQualifiedName ) )
                        {
                            pContext->LogError( "Failed to qualify typename for base class: %s, baseclass = %s", pClass->m_name.c_str(), ClangUtils::GetCursorDisplayName( cr ).c_str() );
                            return CXChildVisit_Break;
                        }

                        auto const parentID = TypeSystem::TypeID( fullyQualifiedName );
                        TVector<TypeSystem::TypeID> parentIDs = { parentID };
                        GetAllDerivedProperties( pContext->m_pDatabase, parentIDs, pClass->m_properties );

                        // Remove duplicate properties added via the parent property traversal
                        eastl::sort( pClass->m_properties.begin(), pClass->m_properties.end() );
                        pClass->m_properties.erase( eastl::unique( pClass->m_properties.begin(), pClass->m_properties.end() ), pClass->m_properties.end() );

                        pClass->m_parents.push_back( parentID );
                        break;
                    }

                    // Extract property info
                    case CXCursor_FieldDecl:
                    {
                        // Should this field be exported?
                        auto const iter = std::find( pContext->m_exposedPropertyMacros.begin(), pContext->m_exposedPropertyMacros.end(), ( ExposedPropertyMacro( pClass->m_headerID, lineNumber ) ) );
                        if ( iter != pContext->m_exposedPropertyMacros.end() )
                        {
                            pClass->m_properties.push_back( PropertyDescriptor( ClangUtils::GetCursorDisplayName( cr ) ) );
                            PropertyDescriptor& propertyDesc = pClass->m_properties.back();

                            auto type = clang_getCursorType( cr );
                            auto const pFieldQualType = ClangUtils::GetQualType( type );
                            auto typeSpelling = ClangUtils::GetString( clang_getTypeSpelling( type ) );

                            // Check if template parameter
                            if ( pFieldQualType->isTemplateTypeParmType() )
                            {
                                pContext->LogError( "Cannot expose template argument member (%s) in class (%s)!", propertyDesc.m_name.c_str(), pClass->m_name.c_str() );
                                return CXChildVisit_Break;
                            }

                            // Check if this field is a c-style array
                            //-------------------------------------------------------------------------

                            if ( pFieldQualType->isArrayType() )
                            {
                                if ( pFieldQualType->isVariableArrayType() || pFieldQualType->isIncompleteArrayType() )
                                {
                                    pContext->LogError( "Variable size array properties are not supported! Please change to TVector or fixed size!" );
                                    return CXChildVisit_Break;
                                }

                                auto const pArrayType = ( clang::ConstantArrayType* ) pFieldQualType.getTypePtr();
                                propertyDesc.m_flags.SetFlag( PropertyInfo::Flags::IsArray );
                                propertyDesc.m_arraySize = (int32) pArrayType->getSize().getSExtValue();

                                // Set property type to array type
                                type = clang_getElementType( type );
                            }

                            // Get field typename
                            //-------------------------------------------------------------------------

                            FieldName fieldName;
                            GetFieldTypeName( pContext, pClass, type, fieldName );
                            KRG_ASSERT( !fieldName.m_name.empty() );
                            auto fieldTypeID = StringID( fieldName.m_name );

                            // Additional processing for special types
                            //-------------------------------------------------------------------------

                            if ( fieldTypeID == GetCoreTypeID( CoreTypes::TVector ) )
                            {
                                // We need to flag this in advance as we are about to change the field type ID
                                propertyDesc.m_flags.SetFlag( PropertyInfo::Flags::IsDynamicArray );

                                // We need to remove the TVector type from the property info as we allow for templated types to be contained within arrays
                                auto const& newName = fieldName.m_templateArgs.front();
                                fieldName = newName;
                                fieldTypeID = StringID( fieldName.m_name );
                            }
                            else if ( fieldTypeID == GetCoreTypeID( CoreTypes::String ) )
                            {
                                // We need to clear the template args since we have a type alias and clang is detected the template args for eastl::basic_string
                                fieldName.m_templateArgs.clear();
                            }

                            //-------------------------------------------------------------------------

                            // Set property typename and validate
                            // If it is a templated type, we only support one level of specialization for exposed properties, so flatten the type
                            propertyDesc.m_typeName = fieldName.m_name;
                            propertyDesc.m_typeID = fieldTypeID;

                            if ( !fieldName.m_templateArgs.empty() )
                            {
                                String flattenedArgs;
                                fieldName.GetFlattenedTemplateArgs( flattenedArgs );
                                propertyDesc.m_templateArgTypeName = flattenedArgs;
                            }

                            // Check for unsupported types
                            //-------------------------------------------------------------------------

                            // Core Types
                            if ( IsCoreType( propertyDesc.m_typeID ) )
                            {
                                // Check if this field is a generic resource ptr
                                if ( propertyDesc.m_typeID == CoreTypes::ResourcePtr )
                                {
                                    pContext->LogError( "Generic resource pointers are not allowed to be exposed, please use a TResourcePtr instead! ( property: %s in class: %s )", propertyDesc.m_name.c_str(), pClass->m_name.c_str() );
                                    return CXChildVisit_Break;
                                }

                                if ( propertyDesc.m_typeID == CoreTypes::TResourcePtr && propertyDesc.m_templateArgTypeName == "KRG::Resource::IResource" )
                                {
                                    pContext->LogError( "Generic resource pointers ( TResourcePtr<IResource> ) are not allowed to be exposed, please use a specific resource type instead! ( property: %s in class: %s )", propertyDesc.m_name.c_str(), pClass->m_name.c_str() );
                                    return CXChildVisit_Break;
                                }

                                // Bit flags
                                if ( propertyDesc.m_typeID == CoreTypes::BitFlags )
                                {
                                    propertyDesc.m_flags.SetFlag( PropertyInfo::Flags::IsBitFlags );
                                }
                                else if ( propertyDesc.m_typeID == CoreTypes::TBitFlags )
                                {
                                    propertyDesc.m_flags.SetFlag( PropertyInfo::Flags::IsBitFlags );

                                    // Perform validation on the enum type for the bit-flags
                                    TypeDescriptor const* pFlagTypeDesc = pContext->m_pDatabase->GetType( propertyDesc.m_templateArgTypeName );
                                    if ( pFlagTypeDesc == nullptr || !pFlagTypeDesc->IsEnum() )
                                    {
                                        pContext->LogError( "Unsupported type encountered: %s for bitflags property: %s in class: %s", propertyDesc.m_typeName.c_str(), propertyDesc.m_name.c_str(), pClass->m_name.c_str() );
                                        return CXChildVisit_Break;
                                    }
                                }
                            }
                            else // Non-Core Types
                            {
                                // Non-core types must have a valid type descriptor
                                TypeDescriptor const* pPropertyTypeDesc = pContext->m_pDatabase->GetType( propertyDesc.m_typeID );
                                if ( pPropertyTypeDesc == nullptr )
                                {
                                    pContext->LogError( "Unsupported type encountered: %s for property: %s in class: %s", propertyDesc.m_typeName.c_str(), propertyDesc.m_name.c_str(), pClass->m_name.c_str() );
                                    return CXChildVisit_Break;
                                }

                                // Check for enum types - bitflags are a special case and are not an enum
                                if ( pPropertyTypeDesc->IsEnum() )
                                {
                                    propertyDesc.m_flags.SetFlag( PropertyInfo::Flags::IsEnum );
                                }

                                // Check if this field is a entity type
                                if ( pPropertyTypeDesc->IsEntity() || pPropertyTypeDesc->IsEntityComponent() || pPropertyTypeDesc->IsEntitySystem() )
                                {
                                    pContext->LogError( "Entities may not contain other entities, please use a EntityPtr instead ( property: %s in class: %s )", propertyDesc.m_name.c_str(), pClass->m_name.c_str() );
                                    return CXChildVisit_Break;
                                }
                            }

                            //-------------------------------------------------------------------------

                            // Remove exposed property macro from the list
                            if ( pClass->IsEntityComponent() || pClass->IsEntitySystem() )
                            {
                                pContext->m_exposedPropertyMacros.erase( iter );
                            }
                        }

                        break;
                    }

                    default: break;
                }

                return CXChildVisit_Continue;
            }

            CXChildVisitResult VisitStructure( ClangParserContext* pContext, CXCursor& cr, FileSystemPath const& headerFilePath, HeaderID const headerID )
            {
                auto cursorName = ClangUtils::GetCursorDisplayName( cr );

                String fullyQualifiedCursorName;
                if ( !ClangUtils::GetQualifiedNameForType( clang_getCursorType( cr ), fullyQualifiedCursorName ) )
                {
                    pContext->LogError( "Failed to get qualified type for cursor: %s", fullyQualifiedCursorName.c_str() );
                    return CXChildVisit_Break;
                }

                //-------------------------------------------------------------------------

                TypeRegistrationMacro macro;
                if ( pContext->ShouldRegisterType( cr, &macro ) )
                {
                    if ( !pContext->IsInEngineNamespace() )
                    {
                        pContext->LogError( "You cannot register types for reflection that are outside the engine namespace (%s). Type: %s, File: %s", Reflection::Settings::g_engineNamespace, fullyQualifiedCursorName.c_str(), headerFilePath.c_str() );
                        return CXChildVisit_Break;
                    }

                    KRG_ASSERT( macro.IsValid() );

                    // Modules
                    if ( macro.IsModuleMacro() )
                    {
                        String const moduleName = pContext->GetCurrentNamespace() + cursorName;

                        if ( !pContext->IsValidModuleName( cursorName ) )
                        {
                            String const errorMessage = Reflection::Utils::GetModuleNameErrorMessage();
                            pContext->LogError( "Invalid module name: %s (%s) - %s", moduleName.c_str(), headerFilePath.c_str(), errorMessage.c_str() );
                            return CXChildVisit_Break;
                        }

                        if ( headerFilePath.ToString().find( Reflection::Settings::g_moduleHeaderFilePath ) == String::npos )
                        {
                            pContext->LogError( "Detected module registration macro in a non-module header: %s (%s) - module registration macro may only exist in a file named 'Module.h'", moduleName.c_str(), headerFilePath.c_str() );
                            return CXChildVisit_Break;
                        }

                        if ( !pContext->SetModuleClassName( headerFilePath, moduleName ) )
                        {
                            // Could not find originating project for detected registered module class
                            pContext->LogError( "Cant find the source project for this module class: %s", headerFilePath.c_str() );
                            return CXChildVisit_Break;
                        }
                    }
                    // Resources
                    else if ( macro.IsResourceMacro() )
                    {
                        String const resourceTypeIDString = macro.m_macroContents.substr( 1, macro.m_macroContents.length() - 2 );

                        TypeID typeID = pContext->GenerateTypeID( fullyQualifiedCursorName );

                        ResourceDesc resource;
                        resource.m_typeID = pContext->GenerateTypeID( fullyQualifiedCursorName );
                        resource.m_resourceTypeID = ResourceTypeID( resourceTypeIDString );
                        resource.m_headerID = headerID;
                        resource.m_className = cursorName;
                        resource.m_namespace = pContext->GetCurrentNamespace();
                        resource.m_isVirtual = macro.m_type == ReflectionMacro::RegisterVirtualResource;

                        pContext->m_pDatabase->RegisterResource( &resource );
                    }
                    else // Classes or structs
                    {
                        KRG_ASSERT( !macro.IsEnumMacro() );

                        // Prevent entity components and systems being defined outside the game or engine layers
                        if ( macro.IsEntityComponentMacro() || macro.IsEntitySystemMacro() )
                        {
                            if ( !Reflection::Utils::IsFileInGameLayer( headerFilePath ) && !Reflection::Utils::IsFileInEngineLayer( headerFilePath ) )
                            {
                                pContext->LogError( "Entity Components and Systems are not allowed to be defined outside of the game or engine layers: %s", headerFilePath.c_str() );
                                return CXChildVisit_Break;
                            }
                        }

                        auto cursorType = clang_getCursorType( cr );
                        auto* pRecordDecl = ( clang::CXXRecordDecl* ) cr.data[0];

                        TypeID typeID = pContext->GenerateTypeID( fullyQualifiedCursorName );
                        TypeDescriptor classDescriptor( typeID, cursorName );
                        classDescriptor.m_headerID = headerID;
                        classDescriptor.m_namespace = pContext->GetCurrentNamespace();
                        classDescriptor.m_flags.SetFlag( TypeDescriptor::Flags::IsEntity, ( cursorName == Reflection::Settings::g_baseEntityClassName ) );
                        classDescriptor.m_flags.SetFlag( TypeDescriptor::Flags::IsEntityComponent, ( macro.IsEntityComponentMacro() || cursorName == Reflection::Settings::g_baseEntityComponentClassName ) );
                        classDescriptor.m_flags.SetFlag( TypeDescriptor::Flags::IsEntitySystem, ( macro.IsEntitySystemMacro() || cursorName == Reflection::Settings::g_baseEntitySystemClassName ) );
                        classDescriptor.m_flags.SetFlag( TypeDescriptor::Flags::IsAbstract, pRecordDecl->isAbstract() );

                        pContext->m_pCurrentEntry = &classDescriptor;
                        clang_visitChildren( cr, VisitStructureContents, pContext );

                        pContext->m_pDatabase->RegisterType( &classDescriptor );
                    }
                }

                return CXChildVisit_Continue;
            }
        }
    }
}