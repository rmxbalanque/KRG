/****************************************************************************
** Meta object code from reading C++ file 'ResourceServer_RequestsView_Completed.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Views/ResourceServer_RequestsView_Completed.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceServer_RequestsView_Completed.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__Resource__CompletedRequestsViewModel_t {
    QByteArrayData data[1];
    char stringdata0[42];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__CompletedRequestsViewModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__CompletedRequestsViewModel_t qt_meta_stringdata_KRG__Resource__CompletedRequestsViewModel = {
    {
QT_MOC_LITERAL(0, 0, 41) // "KRG::Resource::CompletedReque..."

    },
    "KRG::Resource::CompletedRequestsViewModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__CompletedRequestsViewModel[] = {

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

void KRG::Resource::CompletedRequestsViewModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::CompletedRequestsViewModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractTableModel::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__CompletedRequestsViewModel.data,
    qt_meta_data_KRG__Resource__CompletedRequestsViewModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::CompletedRequestsViewModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::CompletedRequestsViewModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__CompletedRequestsViewModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int KRG::Resource::CompletedRequestsViewModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_KRG__Resource__CompletedRequestsViewWidget_t {
    QByteArrayData data[15];
    char stringdata0[260];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__CompletedRequestsViewWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__CompletedRequestsViewWidget_t qt_meta_stringdata_KRG__Resource__CompletedRequestsViewWidget = {
    {
QT_MOC_LITERAL(0, 0, 42), // "KRG::Resource::CompletedReque..."
QT_MOC_LITERAL(1, 43, 19), // "RequestClearHistory"
QT_MOC_LITERAL(2, 63, 0), // ""
QT_MOC_LITERAL(3, 64, 17), // "OnRequestsUpdated"
QT_MOC_LITERAL(4, 82, 35), // "TVector<const CompilationRequ..."
QT_MOC_LITERAL(5, 118, 9), // "pRequests"
QT_MOC_LITERAL(6, 128, 18), // "OnSelectionChanged"
QT_MOC_LITERAL(7, 147, 14), // "QItemSelection"
QT_MOC_LITERAL(8, 162, 8), // "selected"
QT_MOC_LITERAL(9, 171, 10), // "deselected"
QT_MOC_LITERAL(10, 182, 10), // "OnCopyArgs"
QT_MOC_LITERAL(11, 193, 17), // "OnExploreToSource"
QT_MOC_LITERAL(12, 211, 22), // "OnExploreToDestination"
QT_MOC_LITERAL(13, 234, 21), // "OnCustomMenuRequested"
QT_MOC_LITERAL(14, 256, 3) // "pos"

    },
    "KRG::Resource::CompletedRequestsViewWidget\0"
    "RequestClearHistory\0\0OnRequestsUpdated\0"
    "TVector<const CompilationRequest*>*\0"
    "pRequests\0OnSelectionChanged\0"
    "QItemSelection\0selected\0deselected\0"
    "OnCopyArgs\0OnExploreToSource\0"
    "OnExploreToDestination\0OnCustomMenuRequested\0"
    "pos"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__CompletedRequestsViewWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   50,    2, 0x08 /* Private */,
       6,    2,   53,    2, 0x08 /* Private */,
      10,    0,   58,    2, 0x08 /* Private */,
      11,    0,   59,    2, 0x08 /* Private */,
      12,    0,   60,    2, 0x08 /* Private */,
      13,    1,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 7,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   14,

       0        // eod
};

void KRG::Resource::CompletedRequestsViewWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CompletedRequestsViewWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->RequestClearHistory(); break;
        case 1: _t->OnRequestsUpdated((*reinterpret_cast< TVector<const CompilationRequest*>*(*)>(_a[1]))); break;
        case 2: _t->OnSelectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 3: _t->OnCopyArgs(); break;
        case 4: _t->OnExploreToSource(); break;
        case 5: _t->OnExploreToDestination(); break;
        case 6: _t->OnCustomMenuRequested((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QItemSelection >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CompletedRequestsViewWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CompletedRequestsViewWidget::RequestClearHistory)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::CompletedRequestsViewWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__CompletedRequestsViewWidget.data,
    qt_meta_data_KRG__Resource__CompletedRequestsViewWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::CompletedRequestsViewWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::CompletedRequestsViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__CompletedRequestsViewWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRG::Resource::CompletedRequestsViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void KRG::Resource::CompletedRequestsViewWidget::RequestClearHistory()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
