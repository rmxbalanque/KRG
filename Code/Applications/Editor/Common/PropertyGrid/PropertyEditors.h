#pragma once
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class PropertyInstanceModel; class TypeRegistry; }

// Property editor context
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    struct Context
    {
        Context( TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath, TFunction<void( PropertyInstanceModel& )>& preChangeDelegate, TFunction<void( PropertyInstanceModel& )>& postChangeDelegate )
            : m_typeRegistry( typeRegistry )
            , m_sourceDataPath( sourceDataPath )
            , m_preChangeDelegate( preChangeDelegate )
            , m_postChangeDelegate( postChangeDelegate )
        {
            KRG_ASSERT( m_sourceDataPath.ExistsAndIsDirectory() );
        }

        TypeRegistry const&                             m_typeRegistry;
        FileSystem::Path const&                         m_sourceDataPath;
        TFunction<void( PropertyInstanceModel& )>       m_preChangeDelegate;
        TFunction<void( PropertyInstanceModel& )>       m_postChangeDelegate;
    };
}

// Factory Method
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    void CreatePropertyEditor( Context& ctx, PropertyInstanceModel& propertyModel );
}