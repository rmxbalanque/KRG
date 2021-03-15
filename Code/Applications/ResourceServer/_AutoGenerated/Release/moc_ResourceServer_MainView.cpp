/****************************************************************************
** Meta object code from reading C++ file 'ResourceServer_MainView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Views/ResourceServer_MainView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceServer_MainView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__Resource__MainViewWidget_t {
    QByteArrayData data[11];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__MainViewWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__MainViewWidget_t qt_meta_stringdata_KRG__Resource__MainViewWidget = {
    {
QT_MOC_LITERAL(0, 0, 29), // "KRG::Resource::MainViewWidget"
QT_MOC_LITERAL(1, 30, 15), // "ChangePageIndex"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 5), // "int32"
QT_MOC_LITERAL(4, 53, 7), // "pageIdx"
QT_MOC_LITERAL(5, 61, 17), // "OnChangePageIndex"
QT_MOC_LITERAL(6, 79, 18), // "CreateCompilerPage"
QT_MOC_LITERAL(7, 98, 8), // "QWidget*"
QT_MOC_LITERAL(8, 107, 15), // "ResourceServer&"
QT_MOC_LITERAL(9, 123, 14), // "resourceServer"
QT_MOC_LITERAL(10, 138, 18) // "CreateRequestsPage"

    },
    "KRG::Resource::MainViewWidget\0"
    "ChangePageIndex\0\0int32\0pageIdx\0"
    "OnChangePageIndex\0CreateCompilerPage\0"
    "QWidget*\0ResourceServer&\0resourceServer\0"
    "CreateRequestsPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__MainViewWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   37,    2, 0x08 /* Private */,
       6,    1,   40,    2, 0x08 /* Private */,
      10,    1,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    0x80000000 | 7, 0x80000000 | 8,    9,
    0x80000000 | 7, 0x80000000 | 8,    9,

       0        // eod
};

void KRG::Resource::MainViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainViewWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ChangePageIndex((*reinterpret_cast< int32(*)>(_a[1]))); break;
        case 1: _t->OnChangePageIndex((*reinterpret_cast< int32(*)>(_a[1]))); break;
        case 2: { QWidget* _r = _t->CreateCompilerPage((*reinterpret_cast< ResourceServer(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = std::move(_r); }  break;
        case 3: { QWidget* _r = _t->CreateRequestsPage((*reinterpret_cast< ResourceServer(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainViewWidget::*)(int32 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainViewWidget::ChangePageIndex)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::MainViewWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__MainViewWidget.data,
    qt_meta_data_KRG__Resource__MainViewWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::MainViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::MainViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__MainViewWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRG::Resource::MainViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void KRG::Resource::MainViewWidget::ChangePageIndex(int32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
