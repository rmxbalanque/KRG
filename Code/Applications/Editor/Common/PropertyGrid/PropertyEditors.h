#pragma once
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { struct PropertyInfo; class PropertyInstanceModel; class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    struct Context
    {
        Context( TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath )
            : m_typeRegistry( typeRegistry )
            , m_sourceDataPath( sourceDataPath )
        {
            KRG_ASSERT( m_sourceDataPath.ExistsAndIsDirectory() );
        }

        TypeRegistry const&                             m_typeRegistry;
        FileSystem::Path const&                         m_sourceDataPath;
        TFunction<void()>                               m_preChangeDelegate;
        TFunction<void()>                               m_postChangeDelegate;
    };

    void CreatePropertyEditor( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
}