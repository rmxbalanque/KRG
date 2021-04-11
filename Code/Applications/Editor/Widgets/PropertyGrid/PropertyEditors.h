#pragma once
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class PropertyInstanceModel; class TypeRegistry; }

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

        TypeRegistry const&                 m_typeRegistry;
        FileSystem::Path const&             m_sourceDataPath;
    };
}

// Individual editors
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    bool CreateEditorBool( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorInt8( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorInt16( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorInt32( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorInt64( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorUInt8( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorUInt16( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorUInt32( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorUInt64( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorFloat( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorString( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorStringID( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorUUID( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorColor( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorFloat2( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorFloat3( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorFloat4( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorVector( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorQuaternion( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorTransform( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorMicroseconds( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorMilliseconds( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorSeconds( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorPercentage( Context& ctx, PropertyInstanceModel& propertyModel );

    bool CreateEditorDegrees( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorRadians( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorEulerAngles( Context& ctx, PropertyInstanceModel& propertyModel );

    //-------------------------------------------------------------------------

    bool CreateEditorDataPath( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorResourceTypeID( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorResourceID( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorResourcePtr( Context& ctx, PropertyInstanceModel& propertyModel );

    //-------------------------------------------------------------------------

    bool CreateEditorEnum( Context& ctx, PropertyInstanceModel& propertyModel );

    //-------------------------------------------------------------------------

    bool CreateEditorGenericFlags( Context& ctx, PropertyInstanceModel& propertyModel );
    bool CreateEditorSpecificFlags( Context& ctx, PropertyInstanceModel& propertyModel );
}

// Factory Method
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    bool CreatePropertyEditor( Context& ctx, PropertyInstanceModel& propertyModel );
}