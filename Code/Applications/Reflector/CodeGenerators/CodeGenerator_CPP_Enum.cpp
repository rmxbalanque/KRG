#include "CodeGenerator_CPP_Enum.h"
#include "System/TypeSystem/TypeID.h"
#include <sstream>

//-------------------------------------------------------------------------

namespace KRG::CPP
{
    constexpr static char const* s_typeRegistrationFunctionPrototype = "RegisterEnum( TypeSystem::TypeRegistry& typeRegistry )";
    constexpr static char const* s_typeUnregistrationFunctionPrototype = "UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry )";

    //-------------------------------------------------------------------------

    static String GenerateEnumHelperName( ReflectedType const& type )
    {
        KRG_ASSERT( type.IsEnum() );
        String enumHelperName = "EnumHelper_" + type.m_namespace + type.m_name;
        StringUtils::ReplaceAllOccurrencesInPlace( enumHelperName, "::", "_" );
        return enumHelperName;
    }

    static void GenerateHeaderFile( std::stringstream& file, String const& exportMacro, ReflectedType const& type )
    {
        auto const enumHelperName = GenerateEnumHelperName( type );

        file << "\n";
        file << "//-------------------------------------------------------------------------\n";
        file << "// Enum Helper: " << type.m_namespace.c_str() << type.m_name.c_str() << "\n";
        file << "//-------------------------------------------------------------------------\n\n";

        if ( type.m_isDevOnly )
        {
            file << "#if KRG_DEVELOPMENT_TOOLS\n";
        }

        file << "namespace KRG::TypeSystem::EnumHelpers\n";
        file << "{\n";
        file << "    class " << enumHelperName.c_str() << "\n";
        file << "    {\n";
        file << "    public:\n\n";
        file << "        static void " << s_typeRegistrationFunctionPrototype << ";\n";
        file << "        static void " << s_typeUnregistrationFunctionPrototype << ";\n";
        file << "    };\n";
        file << "}\n";

        if ( type.m_isDevOnly )
        {
            file << "#endif\n";
        }
    }

    static void GenerateCodeFile( std::stringstream& file, ReflectedType const& type )
    {
        auto const enumHelperName = GenerateEnumHelperName( type );

        file << "\n";
        file << "//-------------------------------------------------------------------------\n";
        file << "// Enum Helper: " << type.m_namespace.c_str() << type.m_name.c_str() << "\n";
        file << "//-------------------------------------------------------------------------\n\n";

        if ( type.m_isDevOnly )
        {
            file << "#if KRG_DEVELOPMENT_TOOLS\n";
        }

        file << "namespace KRG::TypeSystem::EnumHelpers\n";
        file << "{\n";

        // Registration function
        //-------------------------------------------------------------------------

        file << "    void " << enumHelperName.c_str() << "::" << s_typeRegistrationFunctionPrototype << "\n";
        file << "    {\n";
        file << "        TypeSystem::TypeInfo typeInfo;\n";
        file << "        typeInfo.m_ID = TypeSystem::TypeID( \"" << type.m_namespace.c_str() << type.m_name.c_str() << "\" );\n";
        file << "        typeInfo.m_size = sizeof( " << type.m_namespace.c_str() << type.m_name.c_str() << " );\n";
        file << "        typeInfo.m_alignment = alignof( " << type.m_namespace.c_str() << type.m_name.c_str() << " );\n";
        file << "        typeRegistry.RegisterType( typeInfo );\n\n";
        file << "        TypeSystem::EnumInfo enumInfo;\n";
        file << "        enumInfo.m_ID = TypeSystem::TypeID( \"" << type.m_namespace.c_str() << type.m_name.c_str() << "\" );\n";

        switch ( type.m_underlyingType )
        {
            case TypeSystem::CoreTypeID::Uint8:
            file << "        enumInfo.m_underlyingType = TypeSystem::CoreTypeID::Uint8;\n";
            break;

            case TypeSystem::CoreTypeID::Int8:
            file << "        enumInfo.m_underlyingType = TypeSystem::CoreTypeID::Int8;\n";
            break;

            case TypeSystem::CoreTypeID::Uint16:
            file << "        enumInfo.m_underlyingType = TypeSystem::CoreTypeID::Uint16;\n";
            break;

            case TypeSystem::CoreTypeID::Int16:
            file << "        enumInfo.m_underlyingType = TypeSystem::CoreTypeID::Int16;\n";
            break;

            case TypeSystem::CoreTypeID::Uint32:
            file << "        enumInfo.m_underlyingType = TypeSystem::CoreTypeID::Uint32;\n";
            break;

            case TypeSystem::CoreTypeID::Int32:
            file << "        enumInfo.m_underlyingType = TypeSystem::CoreTypeID::Int32;\n";
            break;

            default:
            KRG_HALT();
            break;
        }

        for ( auto const& c : type.m_enumConstants )
        {
            file << "        enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( \"" << c.second.m_label.c_str() << "\" ), " << c.second.m_value << " ) );\n";
        }

        file << "\n";
        file << "        typeRegistry.RegisterEnum( enumInfo );\n";
        file << "    }\n\n";

        // Unregistration function
        //-------------------------------------------------------------------------

        file << "    void " << enumHelperName.c_str() << "::" << s_typeUnregistrationFunctionPrototype << "\n";
        file << "    {\n";
        file << "        auto const ID = TypeSystem::TypeID( \"" << type.m_namespace.c_str() << type.m_name.c_str() << "\" );\n";
        file << "        typeRegistry.UnregisterType( ID );\n";
        file << "        typeRegistry.UnregisterEnum( ID );\n";
        file << "    }\n";
        file << "}\n";

        if ( type.m_isDevOnly )
        {
            file << "#endif\n";
        }
    }

    //-------------------------------------------------------------------------

    namespace EnumGenerator
    {
        void Generate( std::stringstream& headerFile, std::stringstream& codeFile, String const& exportMacro, ReflectedType const& type )
        {
            KRG_ASSERT( type.IsEnum() );
            GenerateHeaderFile( headerFile, exportMacro, type );
            GenerateCodeFile( codeFile, type );
        }

        void GenerateRegistrationFunctionCall( std::stringstream& file, ReflectedType const& type )
        {
            KRG_ASSERT( type.IsEnum() );

            if ( type.m_isDevOnly )
            {
                file << "    #if KRG_DEVELOPMENT_TOOLS\n";
            }

            file << "    TypeSystem::EnumHelpers::" << GenerateEnumHelperName( type ).c_str() << "::RegisterEnum( typeRegistry );\n";

            if ( type.m_isDevOnly )
            {
                file << "    #endif\n";
            }
        }

        void GenerateUnregistrationFunctionCall( std::stringstream& file, ReflectedType const& type )
        {
            KRG_ASSERT( type.IsEnum() );

            if ( type.m_isDevOnly )
            {
                file << "    #if KRG_DEVELOPMENT_TOOLS\n";
            }

            file << "    TypeSystem::EnumHelpers::" << GenerateEnumHelperName( type ).c_str() << "::UnregisterEnum( typeRegistry );\n";

            if ( type.m_isDevOnly )
            {
                file << "    #endif\n";
            }
        }
    }
}