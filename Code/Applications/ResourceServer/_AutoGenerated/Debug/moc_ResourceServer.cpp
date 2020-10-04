/****************************************************************************
** Meta object code from reading C++ file 'ResourceServer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../ResourceServer/ResourceServer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__Resource__ResourceServer_t {
    QByteArrayData data[13];
    char stringdata0[241];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__ResourceServer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__ResourceServer_t qt_meta_stringdata_KRG__Resource__ResourceServer = {
    {
QT_MOC_LITERAL(0, 0, 29), // "KRG::Resource::ResourceServer"
QT_MOC_LITERAL(1, 30, 21), // "ActiveRequestsUpdated"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 35), // "TVector<const CompilationRequ..."
QT_MOC_LITERAL(4, 89, 15), // "pendingRequests"
QT_MOC_LITERAL(5, 105, 14), // "activeRequests"
QT_MOC_LITERAL(6, 120, 24), // "CompletedRequestsUpdated"
QT_MOC_LITERAL(7, 145, 17), // "completedRequests"
QT_MOC_LITERAL(8, 163, 16), // "BusyStateChanged"
QT_MOC_LITERAL(9, 180, 15), // "OnThreadStarted"
QT_MOC_LITERAL(10, 196, 15), // "OnThreadStopped"
QT_MOC_LITERAL(11, 212, 20), // "OnCleanupOldRequests"
QT_MOC_LITERAL(12, 233, 7) // "Execute"

    },
    "KRG::Resource::ResourceServer\0"
    "ActiveRequestsUpdated\0\0"
    "TVector<const CompilationRequest*>*\0"
    "pendingRequests\0activeRequests\0"
    "CompletedRequestsUpdated\0completedRequests\0"
    "BusyStateChanged\0OnThreadStarted\0"
    "OnThreadStopped\0OnCleanupOldRequests\0"
    "Execute"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__ResourceServer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   49,    2, 0x06 /* Public */,
       6,    1,   54,    2, 0x06 /* Public */,
       8,    0,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   58,    2, 0x0a /* Public */,
      10,    0,   59,    2, 0x0a /* Public */,
      11,    0,   60,    2, 0x0a /* Public */,
      12,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 3,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KRG::Resource::ResourceServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ResourceServer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ActiveRequestsUpdated((*reinterpret_cast< TVector<const CompilationRequest*>*(*)>(_a[1])),(*reinterpret_cast< TVector<const CompilationRequest*>*(*)>(_a[2]))); break;
        case 1: _t->CompletedRequestsUpdated((*reinterpret_cast< TVector<const CompilationRequest*>*(*)>(_a[1]))); break;
        case 2: _t->BusyStateChanged(); break;
        case 3: _t->OnThreadStarted(); break;
        case 4: _t->OnThreadStopped(); break;
        case 5: _t->OnCleanupOldRequests(); break;
        case 6: _t->Execute(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ResourceServer::*)(TVector<CompilationRequest const*> const * , TVector<CompilationRequest const*> const * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ResourceServer::ActiveRequestsUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ResourceServer::*)(TVector<CompilationRequest const*> const * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ResourceServer::CompletedRequestsUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ResourceServer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ResourceServer::BusyStateChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::ResourceServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__ResourceServer.data,
    qt_meta_data_KRG__Resource__ResourceServer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::ResourceServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::ResourceServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__ResourceServer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "FileSystem::IFileSystemChangeListener"))
        return static_cast< FileSystem::IFileSystemChangeListener*>(this);
    return QObject::qt_metacast(_clname);
}

int KRG::Resource::ResourceServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void KRG::Resource::ResourceServer::ActiveRequestsUpdated(TVector<CompilationRequest const*> const * _t1, TVector<CompilationRequest const*> const * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KRG::Resource::ResourceServer::CompletedRequestsUpdated(TVector<CompilationRequest const*> const * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KRG::Resource::ResourceServer::BusyStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
