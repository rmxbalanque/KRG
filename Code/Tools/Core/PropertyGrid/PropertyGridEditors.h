#pragma once
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/Function.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { struct PropertyInfo; class PropertyInstanceModel; class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::PG
{
    struct Context
    {
        Context( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath )
            : m_typeRegistry( typeRegistry )
            , m_rawResourceDirectoryPath( rawResourceDirectoryPath )
        {
            KRG_ASSERT( FileSystem::Exists( m_rawResourceDirectoryPath ) );
        }

        TypeSystem::TypeRegistry const&                 m_typeRegistry;
        FileSystem::Path const&                         m_rawResourceDirectoryPath;
        TFunction<void()>                               m_preChangeDelegate;
        TFunction<void()>                               m_postChangeDelegate;
    };

    void CreatePropertyEditor( Context& ctx, TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
}