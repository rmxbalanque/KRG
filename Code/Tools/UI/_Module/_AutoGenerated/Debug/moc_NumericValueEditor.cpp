/****************************************************************************
** Meta object code from reading C++ file 'NumericValueEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/ValueEditors/NumericValueEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NumericValueEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__ValueEditors__SignedIntegerEditor_t {
    QByteArrayData data[6];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__SignedIntegerEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__SignedIntegerEditor_t qt_meta_stringdata_KRG__ValueEditors__SignedIntegerEditor = {
    {
QT_MOC_LITERAL(0, 0, 38), // "KRG::ValueEditors::SignedInte..."
QT_MOC_LITERAL(1, 39, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 5), // "int64"
QT_MOC_LITERAL(4, 61, 8), // "newValue"
QT_MOC_LITERAL(5, 70, 8) // "OldValue"

    },
    "KRG::ValueEditors::SignedIntegerEditor\0"
    "OnValueChanged\0\0int64\0newValue\0OldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__SignedIntegerEditor[] = {

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
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,

       0        // eod
};

void KRG::ValueEditors::SignedIntegerEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SignedIntegerEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< int64(*)>(_a[1])),(*reinterpret_cast< int64(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SignedIntegerEditor::*)(int64 , int64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SignedIntegerEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::SignedIntegerEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__SignedIntegerEditor.data,
    qt_meta_data_KRG__ValueEditors__SignedIntegerEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::SignedIntegerEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::SignedIntegerEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__SignedIntegerEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::SignedIntegerEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ValueEditorBase::qt_metacall(_c, _id, _a);
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
void KRG::ValueEditors::SignedIntegerEditor::OnValueChanged(int64 _t1, int64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KRG__ValueEditors__UnsignedIntegerEditor_t {
    QByteArrayData data[6];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__UnsignedIntegerEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__UnsignedIntegerEditor_t qt_meta_stringdata_KRG__ValueEditors__UnsignedIntegerEditor = {
    {
QT_MOC_LITERAL(0, 0, 40), // "KRG::ValueEditors::UnsignedIn..."
QT_MOC_LITERAL(1, 41, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 56, 0), // ""
QT_MOC_LITERAL(3, 57, 6), // "uint64"
QT_MOC_LITERAL(4, 64, 8), // "newValue"
QT_MOC_LITERAL(5, 73, 8) // "OldValue"

    },
    "KRG::ValueEditors::UnsignedIntegerEditor\0"
    "OnValueChanged\0\0uint64\0newValue\0"
    "OldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__UnsignedIntegerEditor[] = {

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
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,

       0        // eod
};

void KRG::ValueEditors::UnsignedIntegerEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UnsignedIntegerEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< uint64(*)>(_a[1])),(*reinterpret_cast< uint64(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UnsignedIntegerEditor::*)(uint64 , uint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UnsignedIntegerEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::UnsignedIntegerEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__UnsignedIntegerEditor.data,
    qt_meta_data_KRG__ValueEditors__UnsignedIntegerEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::UnsignedIntegerEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::UnsignedIntegerEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__UnsignedIntegerEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::UnsignedIntegerEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ValueEditorBase::qt_metacall(_c, _id, _a);
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
void KRG::ValueEditors::UnsignedIntegerEditor::OnValueChanged(uint64 _t1, uint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KRG__ValueEditors__FloatingPointEditor_t {
    QByteArrayData data[5];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__FloatingPointEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__FloatingPointEditor_t qt_meta_stringdata_KRG__ValueEditors__FloatingPointEditor = {
    {
QT_MOC_LITERAL(0, 0, 38), // "KRG::ValueEditors::FloatingPo..."
QT_MOC_LITERAL(1, 39, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 8), // "newValue"
QT_MOC_LITERAL(4, 64, 8) // "OldValue"

    },
    "KRG::ValueEditors::FloatingPointEditor\0"
    "OnValueChanged\0\0newValue\0OldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__FloatingPointEditor[] = {

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
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,

       0        // eod
};

void KRG::ValueEditors::FloatingPointEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FloatingPointEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FloatingPointEditor::*)(double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FloatingPointEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::FloatingPointEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__FloatingPointEditor.data,
    qt_meta_data_KRG__ValueEditors__FloatingPointEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::FloatingPointEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::FloatingPointEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__FloatingPointEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::FloatingPointEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ValueEditorBase::qt_metacall(_c, _id, _a);
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
void KRG::ValueEditors::FloatingPointEditor::OnValueChanged(double _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
