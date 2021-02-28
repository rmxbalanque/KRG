/****************************************************************************
** Meta object code from reading C++ file 'EntityComponentPickerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../MapEditor/EntityComponentPickerWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EntityComponentPickerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__EntityComponentPicker__Model_t {
    QByteArrayData data[1];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__EntityComponentPicker__Model_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__EntityComponentPicker__Model_t qt_meta_stringdata_KRG__EntityComponentPicker__Model = {
    {
QT_MOC_LITERAL(0, 0, 33) // "KRG::EntityComponentPicker::M..."

    },
    "KRG::EntityComponentPicker::Model"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__EntityComponentPicker__Model[] = {

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

void KRG::EntityComponentPicker::Model::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::EntityComponentPicker::Model::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractItemModel::staticMetaObject>(),
    qt_meta_stringdata_KRG__EntityComponentPicker__Model.data,
    qt_meta_data_KRG__EntityComponentPicker__Model,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::EntityComponentPicker::Model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::EntityComponentPicker::Model::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__EntityComponentPicker__Model.stringdata0))
        return static_cast<void*>(this);
    return QAbstractItemModel::qt_metacast(_clname);
}

int KRG::EntityComponentPicker::Model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KRG__EntityComponentPickerWidget_t {
    QByteArrayData data[5];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__EntityComponentPickerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__EntityComponentPickerWidget_t qt_meta_stringdata_KRG__EntityComponentPickerWidget = {
    {
QT_MOC_LITERAL(0, 0, 32), // "KRG::EntityComponentPickerWidget"
QT_MOC_LITERAL(1, 33, 19), // "OnComponentSelected"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 27), // "const TypeSystem::TypeInfo*"
QT_MOC_LITERAL(4, 82, 18) // "pComponentTypeInfo"

    },
    "KRG::EntityComponentPickerWidget\0"
    "OnComponentSelected\0\0const TypeSystem::TypeInfo*\0"
    "pComponentTypeInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__EntityComponentPickerWidget[] = {

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
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void KRG::EntityComponentPickerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EntityComponentPickerWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnComponentSelected((*reinterpret_cast< const TypeSystem::TypeInfo*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EntityComponentPickerWidget::*)(TypeSystem::TypeInfo const * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EntityComponentPickerWidget::OnComponentSelected)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::EntityComponentPickerWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRG__EntityComponentPickerWidget.data,
    qt_meta_data_KRG__EntityComponentPickerWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::EntityComponentPickerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::EntityComponentPickerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__EntityComponentPickerWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRG::EntityComponentPickerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::EntityComponentPickerWidget::OnComponentSelected(TypeSystem::TypeInfo const * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
