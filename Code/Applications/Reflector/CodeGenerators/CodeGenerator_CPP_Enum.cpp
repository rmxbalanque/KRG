#include "CodeGenerator_CPP_Enum.h"
#include "System/TypeSystem/TypeID.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        static char const* s_typeSetterFunctionPrototype = "SetValue( void* pEnum, TypeSystem::EnumInfo const* pEnumInfo, UID label )";
        static char const* s_typeRegistrationFunctionPrototype = "RegisterEnum( TypeSystem::TypeRegistry& typeRegistry )";
        static char const* s_typeUnregistrationFunctionPrototype = "UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry )";
    }

    //-------------------------------------------------------------------------

    namespace CPP
    {
        static String GenerateEnumHelperName( TypeDescriptor const& type )
        {
            KRG_ASSERT( type.IsEnum() );
            String enumHelperName = "EnumHelper_" + type.m_namespace + type.m_name;
            StringUtils::ReplaceAllOccurrencesInPlace( enumHelperName, "::", "_" );
            return enumHelperName;
        }

        static void GenerateHeaderFile( std::stringstream& file, String const& exportMacro, TypeDescriptor const& type )
        {
            auto const enumHelperName = GenerateEnumHelperName( type );

            file << "\n";
            file << "//-------------------------------------------------------------------------\n";
            file << "// Enum Helper: " << type.m_namespace << type.m_name << "\n";
            file << "//-------------------------------------------------------------------------\n\n";
            file << "namespace KRG\n";
            file << "{\n";
            file << "    namespace TypeSystem\n";
            file << "    {\n";
            file << "        namespace EnumHelpers\n";
            file << "        {\n";

            file << "            class " << enumHelperName << "\n";
            file << "            {\n";
            file << "            public:\n\n";
            file << "                static void " << s_typeRegistrationFunctionPrototype << ";\n";
            file << "                static void " << s_typeUnregistrationFunctionPrototype << ";\n";
            file << "            };\n";
            file << "        }\n";
            file << "    }\n";
            file << "}\n";
        }

        static void GenerateCodeFile( std::stringstream& file, TypeDescriptor const& type )
        {
            auto const enumHelperName = GenerateEnumHelperName( type );
           
            file << "\n";
            file << "//-------------------------------------------------------------------------\n";
            file << "// Enum Helper: " << type.m_namespace << type.m_name << "\n";
            file << "//-------------------------------------------------------------------------\n\n";
            file << "namespace KRG\n";
            file << "{\n";
            file << "    namespace TypeSystem\n";
            file << "    {\n";
            file << "        namespace EnumHelpers\n";
            file << "        {\n";

            // Registration function
            //-------------------------------------------------------------------------

            file << "            void " << enumHelperName << "::" << s_typeRegistrationFunctionPrototype << "\n";
            file << "            {\n";
            file << "                TypeSystem::TypeInfo typeInfo;\n";
            file << "                typeInfo.m_ID = TypeSystem::TypeID( \"" << type.m_namespace << type.m_name << "\" );\n";
            file << "                typeInfo.m_size = sizeof( " << type.m_namespace << type.m_name << " );\n";
            file << "                typeInfo.m_alignment = alignof( " << type.m_namespace << type.m_name << " );\n";
            file << "                typeRegistry.RegisterType( typeInfo );\n\n";
            file << "                TypeSystem::EnumInfo enumInfo;\n";
            file << "                enumInfo.m_ID = TypeSystem::TypeID( \"" << type.m_namespace << type.m_name << "\" );\n";

            switch ( type.m_underlyingType )
            {
                case TypeSystem::CoreTypes::U8:
                file << "                enumInfo.m_underlyingType = TypeSystem::CoreTypes::U8;\n";
                break;

                case TypeSystem::CoreTypes::S8:
                file << "                enumInfo.m_underlyingType = TypeSystem::CoreTypes::S8;\n";
                break;

                case TypeSystem::CoreTypes::U16:
                file << "                enumInfo.m_underlyingType = TypeSystem::CoreTypes::U16;\n";
                break;

                case TypeSystem::CoreTypes::S16:
                file << "                enumInfo.m_underlyingType = TypeSystem::CoreTypes::S16;\n";
                break;

                case TypeSystem::CoreTypes::U32:
                file << "                enumInfo.m_underlyingType = TypeSystem::CoreTypes::U32;\n";
                break;

                case TypeSystem::CoreTypes::S32:
                file << "                enumInfo.m_underlyingType = TypeSystem::CoreTypes::S32;\n";
                break;

                default:
                KRG_HALT();
                break;
            }

            for ( auto const& c : type.m_constants )
            {
                file << "                enumInfo.m_constants.insert( TPair<StringID, S64>( StringID( \"" << c.second.m_label << "\" ), " << c.second.m_value << " ) );\n";
            }

            file << "\n";
            file << "                typeRegistry.RegisterEnum( enumInfo );\n";
            file << "            }\n\n";

            // Unregistration function
            //-------------------------------------------------------------------------

            file << "            void " << enumHelperName << "::" << s_typeUnregistrationFunctionPrototype << "\n";
            file << "            {\n";
            file << "                auto const ID = TypeSystem::TypeID( \"" << type.m_namespace << type.m_name << "\" );\n";
            file << "                typeRegistry.UnregisterType( ID );\n";
            file << "                typeRegistry.UnregisterEnum( ID );\n";
            file << "            }\n";
            file << "        }\n";
            file << "    }\n";
            file << "}\n";
        }

        //-------------------------------------------------------------------------

        namespace EnumGenerator
        {
            void Generate( std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, TypeDescriptor const& type )
            {
                KRG_ASSERT( type.IsEnum() );
                GenerateHeaderFile( headerFile, exportMacro, type );
                GenerateCodeFile( codeFile, type );
            }

            void GenerateRegistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type )
            {
                KRG_ASSERT( type.IsEnum() );

                file << "    TypeSystem::EnumHelpers::" << GenerateEnumHelperName( type ) << "::RegisterEnum( typeRegistry );\n";
            }

            void GenerateUnregistrationFunctionCall( std::stringstream& file, TypeDescriptor const& type )
            {
                KRG_ASSERT( type.IsEnum() );
                file << "    TypeSystem::EnumHelpers::" << GenerateEnumHelperName( type ) << "::UnregisterEnum( typeRegistry );\n";
            }
        }
    }
}