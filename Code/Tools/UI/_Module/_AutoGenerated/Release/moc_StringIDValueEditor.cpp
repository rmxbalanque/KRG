/****************************************************************************
** Meta object code from reading C++ file 'StringIDValueEditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/ValueEditors/StringIDValueEditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StringIDValueEditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__ValueEditors__StringIDEditor_t {
    QByteArrayData data[6];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__ValueEditors__StringIDEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__ValueEditors__StringIDEditor_t qt_meta_stringdata_KRG__ValueEditors__StringIDEditor = {
    {
QT_MOC_LITERAL(0, 0, 33), // "KRG::ValueEditors::StringIDEd..."
QT_MOC_LITERAL(1, 34, 14), // "OnValueChanged"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 8), // "StringID"
QT_MOC_LITERAL(4, 59, 8), // "newValue"
QT_MOC_LITERAL(5, 68, 8) // "OldValue"

    },
    "KRG::ValueEditors::StringIDEditor\0"
    "OnValueChanged\0\0StringID\0newValue\0"
    "OldValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__ValueEditors__StringIDEditor[] = {

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

void KRG::ValueEditors::StringIDEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StringIDEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnValueChanged((*reinterpret_cast< StringID(*)>(_a[1])),(*reinterpret_cast< StringID(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StringIDEditor::*)(StringID , StringID );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StringIDEditor::OnValueChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::ValueEditors::StringIDEditor::staticMetaObject = { {
    QMetaObject::SuperData::link<ValueEditorBase::staticMetaObject>(),
    qt_meta_stringdata_KRG__ValueEditors__StringIDEditor.data,
    qt_meta_data_KRG__ValueEditors__StringIDEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::ValueEditors::StringIDEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::ValueEditors::StringIDEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__ValueEditors__StringIDEditor.stringdata0))
        return static_cast<void*>(this);
    return ValueEditorBase::qt_metacast(_clname);
}

int KRG::ValueEditors::StringIDEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KRG::ValueEditors::StringIDEditor::OnValueChanged(StringID _t1, StringID _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
