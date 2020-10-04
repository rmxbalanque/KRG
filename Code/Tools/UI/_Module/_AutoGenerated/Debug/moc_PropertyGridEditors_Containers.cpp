/****************************************************************************
** Meta object code from reading C++ file 'PropertyGridEditors_Containers.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/PropertyGrid/PropertyGridEditors_Containers.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyGridEditors_Containers.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__PropertyGrid__CollapsiblePropertyEditorHeader_t {
    QByteArrayData data[3];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__PropertyGrid__CollapsiblePropertyEditorHeader_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__PropertyGrid__CollapsiblePropertyEditorHeader_t qt_meta_stringdata_KRG__PropertyGrid__CollapsiblePropertyEditorHeader = {
    {
QT_MOC_LITERAL(0, 0, 50), // "KRG::PropertyGrid::Collapsibl..."
QT_MOC_LITERAL(1, 51, 7), // "Clicked"
QT_MOC_LITERAL(2, 59, 0) // ""

    },
    "KRG::PropertyGrid::CollapsiblePropertyEditorHeader\0"
    "Clicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__PropertyGrid__CollapsiblePropertyEditorHeader[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void KRG::PropertyGrid::CollapsiblePropertyEditorHeader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CollapsiblePropertyEditorHeader *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CollapsiblePropertyEditorHeader::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CollapsiblePropertyEditorHeader::Clicked)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::PropertyGrid::CollapsiblePropertyEditorHeader::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRG__PropertyGrid__CollapsiblePropertyEditorHeader.data,
    qt_meta_data_KRG__PropertyGrid__CollapsiblePropertyEditorHeader,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::PropertyGrid::CollapsiblePropertyEditorHeader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::PropertyGrid::CollapsiblePropertyEditorHeader::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__PropertyGrid__CollapsiblePropertyEditorHeader.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRG::PropertyGrid::CollapsiblePropertyEditorHeader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void KRG::PropertyGrid::CollapsiblePropertyEditorHeader::Clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_KRG__PropertyGrid__ContainerPropertyEditor_t {
    QByteArrayData data[1];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__PropertyGrid__ContainerPropertyEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__PropertyGrid__ContainerPropertyEditor_t qt_meta_stringdata_KRG__PropertyGrid__ContainerPropertyEditor = {
    {
QT_MOC_LITERAL(0, 0, 42) // "KRG::PropertyGrid::ContainerP..."

    },
    "KRG::PropertyGrid::ContainerPropertyEditor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__PropertyGrid__ContainerPropertyEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KRG::PropertyGrid::ContainerPropertyEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::PropertyGrid::ContainerPropertyEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<PropertyEditor::staticMetaObject>(),
    qt_meta_stringdata_KRG__PropertyGrid__ContainerPropertyEditor.data,
    qt_meta_data_KRG__PropertyGrid__ContainerPropertyEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::PropertyGrid::ContainerPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::PropertyGrid::ContainerPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__PropertyGrid__ContainerPropertyEditor.stringdata0))
        return static_cast<void*>(this);
    return PropertyEditor::qt_metacast(_clname);
}

int KRG::PropertyGrid::ContainerPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PropertyEditor::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KRG__PropertyGrid__StructurePropertyEditor_t {
    QByteArrayData data[1];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__PropertyGrid__StructurePropertyEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__PropertyGrid__StructurePropertyEditor_t qt_meta_stringdata_KRG__PropertyGrid__StructurePropertyEditor = {
    {
QT_MOC_LITERAL(0, 0, 42) // "KRG::PropertyGrid::StructureP..."

    },
    "KRG::PropertyGrid::StructurePropertyEditor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__PropertyGrid__StructurePropertyEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KRG::PropertyGrid::StructurePropertyEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::PropertyGrid::StructurePropertyEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ContainerPropertyEditor::staticMetaObject>(),
    qt_meta_stringdata_KRG__PropertyGrid__StructurePropertyEditor.data,
    qt_meta_data_KRG__PropertyGrid__StructurePropertyEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::PropertyGrid::StructurePropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::PropertyGrid::StructurePropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__PropertyGrid__StructurePropertyEditor.stringdata0))
        return static_cast<void*>(this);
    return ContainerPropertyEditor::qt_metacast(_clname);
}

int KRG::PropertyGrid::StructurePropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContainerPropertyEditor::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KRG__PropertyGrid__StaticArrayPropertyEditor_t {
    QByteArrayData data[1];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__PropertyGrid__StaticArrayPropertyEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__PropertyGrid__StaticArrayPropertyEditor_t qt_meta_stringdata_KRG__PropertyGrid__StaticArrayPropertyEditor = {
    {
QT_MOC_LITERAL(0, 0, 44) // "KRG::PropertyGrid::StaticArra..."

    },
    "KRG::PropertyGrid::StaticArrayPropertyEditor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__PropertyGrid__StaticArrayPropertyEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KRG::PropertyGrid::StaticArrayPropertyEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::PropertyGrid::StaticArrayPropertyEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ContainerPropertyEditor::staticMetaObject>(),
    qt_meta_stringdata_KRG__PropertyGrid__StaticArrayPropertyEditor.data,
    qt_meta_data_KRG__PropertyGrid__StaticArrayPropertyEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::PropertyGrid::StaticArrayPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::PropertyGrid::StaticArrayPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__PropertyGrid__StaticArrayPropertyEditor.stringdata0))
        return static_cast<void*>(this);
    return ContainerPropertyEditor::qt_metacast(_clname);
}

int KRG::PropertyGrid::StaticArrayPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContainerPropertyEditor::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KRG__PropertyGrid__DynamicArrayPropertyEditor_t {
    QByteArrayData data[1];
    char stringdata0[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__PropertyGrid__DynamicArrayPropertyEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__PropertyGrid__DynamicArrayPropertyEditor_t qt_meta_stringdata_KRG__PropertyGrid__DynamicArrayPropertyEditor = {
    {
QT_MOC_LITERAL(0, 0, 45) // "KRG::PropertyGrid::DynamicArr..."

    },
    "KRG::PropertyGrid::DynamicArrayPropertyEditor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__PropertyGrid__DynamicArrayPropertyEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KRG::PropertyGrid::DynamicArrayPropertyEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::PropertyGrid::DynamicArrayPropertyEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ContainerPropertyEditor::staticMetaObject>(),
    qt_meta_stringdata_KRG__PropertyGrid__DynamicArrayPropertyEditor.data,
    qt_meta_data_KRG__PropertyGrid__DynamicArrayPropertyEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::PropertyGrid::DynamicArrayPropertyEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::PropertyGrid::DynamicArrayPropertyEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__PropertyGrid__DynamicArrayPropertyEditor.stringdata0))
        return static_cast<void*>(this);
    return ContainerPropertyEditor::qt_metacast(_clname);
}

int KRG::PropertyGrid::DynamicArrayPropertyEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContainerPropertyEditor::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
