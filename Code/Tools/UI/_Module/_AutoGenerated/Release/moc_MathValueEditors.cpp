/****************************************************************************
** Meta object code from reading C++ file 'MathValueEditors.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/ValueEditors/MathValueEditors.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MathValueEditors.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__ValueEditors__Float2Editor_t {
    QByteArrayData data[6];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__Float2Editor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__Float2Editor_t qt_meta_stringdata_KRG__ValueEditors__Float2Editor = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KRG::ValueEditors::Float2Editor"
QT_MOC_LITERAL(1, 32, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 6), // "Float2"
QT_MOC_LITERAL(4, 55, 8), // "newValue"
QT_MOC_LITERAL(5, 64, 8) // "oldValue"

    },
    "KRG::ValueEditors::Float2Editor\0"
    "OnValueChanged\0\0Float2\0newValue\0"
    "oldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__Float2Editor[] = {

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

void KRG::ValueEditors::Float2Editor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Float2Editor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< const Float2(*)>(_a[1])),(*reinterpret_cast< const Float2(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Float2Editor::*)(Float2 const & , Float2 const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Float2Editor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::Float2Editor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__Float2Editor.data,
    qt_meta_data_KRG__ValueEditors__Float2Editor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::Float2Editor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::Float2Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__Float2Editor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::Float2Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::Float2Editor::OnValueChanged(Float2 const & _t1, Float2 const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KRG__ValueEditors__Float3Editor_t {
    QByteArrayData data[6];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__Float3Editor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__Float3Editor_t qt_meta_stringdata_KRG__ValueEditors__Float3Editor = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KRG::ValueEditors::Float3Editor"
QT_MOC_LITERAL(1, 32, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 6), // "Float3"
QT_MOC_LITERAL(4, 55, 8), // "newValue"
QT_MOC_LITERAL(5, 64, 8) // "oldValue"

    },
    "KRG::ValueEditors::Float3Editor\0"
    "OnValueChanged\0\0Float3\0newValue\0"
    "oldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__Float3Editor[] = {

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

void KRG::ValueEditors::Float3Editor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Float3Editor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< const Float3(*)>(_a[1])),(*reinterpret_cast< const Float3(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Float3Editor::*)(Float3 const & , Float3 const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Float3Editor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::Float3Editor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__Float3Editor.data,
    qt_meta_data_KRG__ValueEditors__Float3Editor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::Float3Editor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::Float3Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__Float3Editor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::Float3Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::Float3Editor::OnValueChanged(Float3 const & _t1, Float3 const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KRG__ValueEditors__Float4Editor_t {
    QByteArrayData data[6];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__Float4Editor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__Float4Editor_t qt_meta_stringdata_KRG__ValueEditors__Float4Editor = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KRG::ValueEditors::Float4Editor"
QT_MOC_LITERAL(1, 32, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 6), // "Float4"
QT_MOC_LITERAL(4, 55, 8), // "newValue"
QT_MOC_LITERAL(5, 64, 8) // "oldValue"

    },
    "KRG::ValueEditors::Float4Editor\0"
    "OnValueChanged\0\0Float4\0newValue\0"
    "oldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__Float4Editor[] = {

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

void KRG::ValueEditors::Float4Editor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Float4Editor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< const Float4(*)>(_a[1])),(*reinterpret_cast< const Float4(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Float4Editor::*)(Float4 const & , Float4 const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Float4Editor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::Float4Editor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__Float4Editor.data,
    qt_meta_data_KRG__ValueEditors__Float4Editor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::Float4Editor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::Float4Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__Float4Editor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::Float4Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::Float4Editor::OnValueChanged(Float4 const & _t1, Float4 const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KRG__ValueEditors__RotationEditor_t {
    QByteArrayData data[6];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__RotationEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__RotationEditor_t qt_meta_stringdata_KRG__ValueEditors__RotationEditor = {
    {
QT_MOC_LITERAL(0, 0, 33), // "KRG::ValueEditors::RotationEd..."
QT_MOC_LITERAL(1, 34, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 11), // "EulerAngles"
QT_MOC_LITERAL(4, 62, 8), // "newValue"
QT_MOC_LITERAL(5, 71, 8) // "oldValue"

    },
    "KRG::ValueEditors::RotationEditor\0"
    "OnValueChanged\0\0EulerAngles\0newValue\0"
    "oldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__RotationEditor[] = {

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

void KRG::ValueEditors::RotationEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RotationEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< const EulerAngles(*)>(_a[1])),(*reinterpret_cast< const EulerAngles(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RotationEditor::*)(EulerAngles const & , EulerAngles const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RotationEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::RotationEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__RotationEditor.data,
    qt_meta_data_KRG__ValueEditors__RotationEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::RotationEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::RotationEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__RotationEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::RotationEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::RotationEditor::OnValueChanged(EulerAngles const & _t1, EulerAngles const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_KRG__ValueEditors__TransformEditor_t {
    QByteArrayData data[6];
    char stringdata0[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__TransformEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__TransformEditor_t qt_meta_stringdata_KRG__ValueEditors__TransformEditor = {
    {
QT_MOC_LITERAL(0, 0, 34), // "KRG::ValueEditors::TransformE..."
QT_MOC_LITERAL(1, 35, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 6), // "Matrix"
QT_MOC_LITERAL(4, 58, 8), // "newValue"
QT_MOC_LITERAL(5, 67, 8) // "oldValue"

    },
    "KRG::ValueEditors::TransformEditor\0"
    "OnValueChanged\0\0Matrix\0newValue\0"
    "oldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__TransformEditor[] = {

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

void KRG::ValueEditors::TransformEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TransformEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< const Matrix(*)>(_a[1])),(*reinterpret_cast< const Matrix(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TransformEditor::*)(Matrix const & , Matrix const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TransformEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::TransformEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__TransformEditor.data,
    qt_meta_data_KRG__ValueEditors__TransformEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::TransformEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::TransformEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__TransformEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::TransformEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::TransformEditor::OnValueChanged(Matrix const & _t1, Matrix const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
