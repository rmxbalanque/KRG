#pragma once
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { struct PropertyInfo; class PropertyInstanceModel; class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::PG
{
    struct Context
    {
        Context( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath )
            : m_typeRegistry( typeRegistry )
            , m_sourceDataPath( sourceDataPath )
        {
            KRG_ASSERT( m_sourceDataPath.IsExistingDirectory() );
        }

        TypeSystem::TypeRegistry const&                 m_typeRegistry;
        FileSystem::Path const&                         m_sourceDataPath;
        TFunction<void()>                               m_preChangeDelegate;
        TFunction<void()>                               m_postChangeDelegate;
    };

    void CreatePropertyEditor( Context& ctx, TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
}