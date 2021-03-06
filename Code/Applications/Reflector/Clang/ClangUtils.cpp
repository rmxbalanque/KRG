#include "ClangUtils.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ClangUtils
    {
        void GetDiagnostics( CXTranslationUnit& TU, TVector<String>& diagnostics )
        {
            auto const numDiagnostics = clang_getNumDiagnostics( TU );
            for ( auto i = 0u; i < numDiagnostics; i++ )
            {
                CXDiagnostic diagnostic = clang_getDiagnostic( TU, i );
                diagnostics.push_back( GetString( clang_formatDiagnostic( diagnostic, clang_defaultDiagnosticDisplayOptions() ) ) );
            }
        }

        FileSystemPath GetHeaderPathForCursor( CXCursor cr )
        {
            CXFile pFile;
            CXSourceRange const cursorRange = clang_getCursorExtent( cr );
            clang_getExpansionLocation( clang_getRangeStart( cursorRange ), &pFile, nullptr, nullptr, nullptr );

            FileSystemPath HeaderFilePath;
            if ( pFile != nullptr )
            {
                HeaderFilePath = FileSystemPath( ClangUtils::GetString( clang_getFileName( pFile ) ) );
            }

            return HeaderFilePath;
        }

        bool GetQualifiedNameForType( clang::QualType type, String& qualifiedName )
        {
            clang::Type const* pType = type.getTypePtr();

            if ( pType->isPointerType() || pType->isReferenceType() )
            {
                auto p = pType->getPointeeType();
                clang::Type const* pClangType2 = p.getTypePtr();
            }
            else if ( pType->isRecordType() )
            {
                clang::RecordDecl const* pRecordDecl = pType->getAs<clang::RecordType>()->getDecl();
                KRG_ASSERT( pRecordDecl != nullptr );
                qualifiedName = pRecordDecl->getQualifiedNameAsString().c_str();
            }
            else if ( pType->isEnumeralType() )
            {
                clang::NamedDecl const* pNamedDecl = pType->getAs<clang::EnumType>()->getDecl();
                KRG_ASSERT( pNamedDecl != nullptr );
                qualifiedName = pNamedDecl->getQualifiedNameAsString().c_str();
            }
            else if ( pType->getTypeClass() == clang::Type::Typedef )
            {
                clang::NamedDecl const* pNamedDecl = pType->getAs<clang::TypedefType>()->getDecl();
                KRG_ASSERT( pNamedDecl != nullptr );
                qualifiedName = pNamedDecl->getQualifiedNameAsString().c_str();
            }
            else if ( pType->isBooleanType() )
            {
                qualifiedName = "bool";
            }
            else if ( pType->isArrayType() )
            {
                auto elementType = pType->castAsArrayTypeUnsafe()->getElementType();
                if ( !GetQualifiedNameForType( elementType, qualifiedName ) )
                {
                    return false;
                }
            }
            else if ( pType->isBuiltinType() )
            {
                auto const* pBT = pType->getAs<clang::BuiltinType>();
                switch ( pBT->getKind() )
                {
                    case clang::BuiltinType::Char_S:
                    qualifiedName = "KRG::int8";
                    break;

                    case clang::BuiltinType::Char_U:
                    qualifiedName = "KRG::uint8";
                    break;

                    case clang::BuiltinType::UChar:
                    qualifiedName = "KRG::uint8";
                    break;

                    case clang::BuiltinType::SChar:
                    qualifiedName = "KRG::int8";
                    break;

                    case clang::BuiltinType::Char16:
                    qualifiedName = "KRG::uint16";
                    break;

                    case clang::BuiltinType::Char32:
                    qualifiedName = "KRG::uint32";
                    break;

                    case clang::BuiltinType::UShort:
                    qualifiedName = "KRG::uint16";
                    break;

                    case clang::BuiltinType::Short:
                    qualifiedName = "KRG::int16";
                    break;

                    case clang::BuiltinType::UInt:
                    qualifiedName = "KRG::uint32";
                    break;

                    case clang::BuiltinType::Int:
                    qualifiedName = "KRG::int32";
                    break;

                    case clang::BuiltinType::ULongLong:
                    qualifiedName = "KRG::uint64";
                    break;

                    case clang::BuiltinType::LongLong:
                    qualifiedName = "KRG::int64";
                    break;

                    case clang::BuiltinType::Float:
                    qualifiedName = "float";
                    break;

                    case clang::BuiltinType::Double:
                    qualifiedName = "double";
                    break;

                    default:
                    {
                        return false;
                    }
                };
            }
            else
            {
                return false;
            }

            // Convert standard library types to KRG types
            if ( qualifiedName == "eastl::basic_string" )
            {
                qualifiedName = "KRG::String";
            }
            else  if ( qualifiedName == "eastl::string" )
            {
                qualifiedName = "KRG::String";
            }
            else if ( qualifiedName == "eastl::vector" )
            {
                qualifiedName = "KRG::TVector";
            }
            return true;
        }
    }
}
