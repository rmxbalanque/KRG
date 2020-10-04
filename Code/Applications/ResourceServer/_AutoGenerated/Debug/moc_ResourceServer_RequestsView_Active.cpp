/****************************************************************************
** Meta object code from reading C++ file 'ResourceServer_RequestsView_Active.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Views/ResourceServer_RequestsView_Active.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceServer_RequestsView_Active.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__Resource__ActiveRequestsViewModel_t {
    QByteArrayData data[1];
    char stringdata0[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__ActiveRequestsViewModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__ActiveRequestsViewModel_t qt_meta_stringdata_KRG__Resource__ActiveRequestsViewModel = {
    {
QT_MOC_LITERAL(0, 0, 38) // "KRG::Resource::ActiveRequests..."

    },
    "KRG::Resource::ActiveRequestsViewModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__ActiveRequestsViewModel[] = {

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

void KRG::Resource::ActiveRequestsViewModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::ActiveRequestsViewModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractTableModel::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__ActiveRequestsViewModel.data,
    qt_meta_data_KRG__Resource__ActiveRequestsViewModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::ActiveRequestsViewModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::ActiveRequestsViewModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__ActiveRequestsViewModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int KRG::Resource::ActiveRequestsViewModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KRG__Resource__ActiveRequestsViewWidget_t {
    QByteArrayData data[6];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__ActiveRequestsViewWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__ActiveRequestsViewWidget_t qt_meta_stringdata_KRG__Resource__ActiveRequestsViewWidget = {
    {
QT_MOC_LITERAL(0, 0, 39), // "KRG::Resource::ActiveRequests..."
QT_MOC_LITERAL(1, 40, 17), // "OnRequestsUpdated"
QT_MOC_LITERAL(2, 58, 0), // ""
QT_MOC_LITERAL(3, 59, 35), // "TVector<const CompilationRequ..."
QT_MOC_LITERAL(4, 95, 16), // "pPendingRequests"
QT_MOC_LITERAL(5, 112, 15) // "pActiveRequests"

    },
    "KRG::Resource::ActiveRequestsViewWidget\0"
    "OnRequestsUpdated\0\0"
    "TVector<const CompilationRequest*>*\0"
    "pPendingRequests\0pActiveRequests"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__ActiveRequestsViewWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,

       0        // eod
};

void KRG::Resource::ActiveRequestsViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ActiveRequestsViewWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnRequestsUpdated((*reinterpret_cast< TVector<const CompilationRequest*>*(*)>(_a[1])),(*reinterpret_cast< TVector<const CompilationRequest*>*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::ActiveRequestsViewWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__ActiveRequestsViewWidget.data,
    qt_meta_data_KRG__Resource__ActiveRequestsViewWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::ActiveRequestsViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::ActiveRequestsViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__ActiveRequestsViewWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRG::Resource::ActiveRequestsViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
