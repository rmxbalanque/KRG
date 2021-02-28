/****************************************************************************
** Meta object code from reading C++ file 'StringValueEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/ValueEditors/StringValueEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StringValueEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__ValueEditors__StringEditor_t {
    QByteArrayData data[6];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__StringEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__StringEditor_t qt_meta_stringdata_KRG__ValueEditors__StringEditor = {
    {
QT_MOC_LITERAL(0, 0, 31), // "KRG::ValueEditors::StringEditor"
QT_MOC_LITERAL(1, 32, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 6), // "String"
QT_MOC_LITERAL(4, 55, 8), // "newValue"
QT_MOC_LITERAL(5, 64, 8) // "oldValue"

    },
    "KRG::ValueEditors::StringEditor\0"
    "OnValueChanged\0\0String\0newValue\0"
    "oldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__StringEditor[] = {

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

void KRG::ValueEditors::StringEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StringEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< const String(*)>(_a[1])),(*reinterpret_cast< const String(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StringEditor::*)(String const & , String const & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StringEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::StringEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__StringEditor.data,
    qt_meta_data_KRG__ValueEditors__StringEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::StringEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::StringEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__StringEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::StringEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::StringEditor::OnValueChanged(String const & _t1, String const & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
