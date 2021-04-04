#pragma once

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class PropertyInstanceModel; }

// Individual editors
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    void CreateEditorBool( PropertyInstanceModel& model );
    void CreateEditorInt( PropertyInstanceModel& model );
    void CreateEditorFloat( PropertyInstanceModel& model );

    void CreateEditorString( PropertyInstanceModel& model );
    void CreateEditorStringID( PropertyInstanceModel& model );
    void CreateEditorUUID( PropertyInstanceModel& model );
    void CreateEditorColor( PropertyInstanceModel& model );

    void CreateEditorFloat2( PropertyInstanceModel& model );
    void CreateEditorFloat3( PropertyInstanceModel& model );
    void CreateEditorFloat4( PropertyInstanceModel& model );

    void CreateEditorQuaternion( PropertyInstanceModel& model );
    void CreateEditorTransform( PropertyInstanceModel& model );
    void CreateEditorMatrix( PropertyInstanceModel& model );

    void CreateEditorMicroseconds( PropertyInstanceModel& model );
    void CreateEditorMilliseconds( PropertyInstanceModel& model );
    void CreateEditorSeconds( PropertyInstanceModel& model );

    void CreateEditorPercentage( PropertyInstanceModel& model );

    void CreateEditorDegrees( PropertyInstanceModel& model );
    void CreateEditorRadians( PropertyInstanceModel& model );
    void CreateEditorEulerAngles( PropertyInstanceModel& model );

    void CreateEditorDataPath( PropertyInstanceModel& model );
    void CreateEditorResourceID( PropertyInstanceModel& model );
    void CreateEditorResourcePtr( PropertyInstanceModel& model );

    void CreateEditorGenericFlags( PropertyInstanceModel& model );
    void CreateEditorSpecificFlags( PropertyInstanceModel& model );

    void CreateEditorEnum( PropertyInstanceModel& model );

    void CreateEditorStaticArray( PropertyInstanceModel& model );
    void CreateEditorDynamicArray( PropertyInstanceModel& model );
}

// Factory Method
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    void CreatePropertyEditor( PropertyInstanceModel& model );
}