#include "ClangVisitors_Enum.h"
#include "Applications/Reflector/Database/ReflectionDatabase.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            static CXChildVisitResult VisitEnumContents( CXCursor cr, CXCursor parent, CXClientData pClientData )
            {
                auto pContext = reinterpret_cast<ClangParserContext*>( pClientData );

                CXCursorKind kind = clang_getCursorKind( cr );
                if ( kind == CXCursor_EnumConstantDecl )
                {
                    auto pEnum = reinterpret_cast<TypeDescriptor*>( pContext->m_pCurrentEntry );
                    clang::EnumConstantDecl* pEnumConstantDecl = ( clang::EnumConstantDecl* ) cr.data[0];

                    EnumConstantDescriptor constant;
                    constant.m_label = ClangUtils::GetCursorDisplayName( cr );

                    auto const& initVal = pEnumConstantDecl->getInitVal();
                    constant.m_value = (S32) initVal.getExtValue();

                    pEnum->AddValue( constant );
                }

                return CXChildVisit_Continue;
            }

            CXChildVisitResult VisitEnum( ClangParserContext* pContext, CXCursor cr, HeaderID const headerID )
            {
                auto cursorName = ClangUtils::GetCursorDisplayName( cr );

                String fullyQualifiedCursorName;
                if ( !ClangUtils::GetQualifiedNameForType( clang_getCursorType( cr ), fullyQualifiedCursorName ) )
                {
                    pContext->LogError( "Failed to get qualified type for cursor: %s", cursorName.c_str() );
                    return CXChildVisit_Break;
                }

                //-------------------------------------------------------------------------

                clang::EnumDecl* pEnumDecl = ( clang::EnumDecl* ) cr.data[0];
                clang::QualType integerType = pEnumDecl->getIntegerType();

                if ( integerType.isNull() )
                {
                    pContext->LogError( "Failed to get underlying type for enum: %s. You must specify the underlying integer type for exposed enums!", fullyQualifiedCursorName.c_str() );
                    return CXChildVisit_Break;
                }

                CoreTypes underlyingCoreType;

                auto const* pBT = integerType.getTypePtr()->getAs<clang::BuiltinType>();
                switch ( pBT->getKind() )
                {
                    case clang::BuiltinType::UChar:
                    underlyingCoreType = CoreTypes::U8;
                    break;

                    case clang::BuiltinType::SChar:
                    underlyingCoreType = CoreTypes::S8;
                    break;

                    case clang::BuiltinType::UShort:
                    underlyingCoreType = CoreTypes::U16;
                    break;

                    case clang::BuiltinType::Short:
                    underlyingCoreType = CoreTypes::S16;
                    break;

                    case clang::BuiltinType::UInt:
                    underlyingCoreType = CoreTypes::U32;
                    break;

                    case clang::BuiltinType::Int:
                    underlyingCoreType = CoreTypes::S32;
                    break;

                    case clang::BuiltinType::ULongLong:
                    case clang::BuiltinType::LongLong:
                    {
                        pContext->LogError( "64bit enum detected: %s. This is not supported!", fullyQualifiedCursorName.c_str() );
                        return CXChildVisit_Break;
                    }
                    break;

                    default:
                    {
                        pContext->LogError( "Unknown underlying type for enum: %s", fullyQualifiedCursorName.c_str() );
                        return CXChildVisit_Break;
                    }
                    break;
                }

                //-------------------------------------------------------------------------

                auto enumTypeID = pContext->GenerateTypeID( fullyQualifiedCursorName );
                if ( !pContext->IsInEngineNamespace() || pContext->m_pDatabase->IsTypeRegistered( enumTypeID ) )
                {
                    return CXChildVisit_Continue;
                }

                if ( pContext->ShouldRegisterType( cr ) )
                {
                    TypeDescriptor enumDescriptor( enumTypeID, cursorName );
                    enumDescriptor.m_headerID = headerID;
                    enumDescriptor.m_namespace = pContext->GetCurrentNamespace();
                    enumDescriptor.m_flags.SetFlag( TypeDescriptor::Flags::IsEnum );
                    enumDescriptor.m_underlyingType = underlyingCoreType;
                    pContext->m_pCurrentEntry = &enumDescriptor;

                    clang_visitChildren( cr, VisitEnumContents, pContext );

                    pContext->m_pDatabase->RegisterType( &enumDescriptor );
                }

                return CXChildVisit_Continue;
            }
        }
    }
}