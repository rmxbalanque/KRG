/****************************************************************************
** Meta object code from reading C++ file 'ResourceServerApplication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../ResourceServerApplication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceServerApplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRG__Resource__ResourceServerApplication_t {
    QByteArrayData data[9];
    char stringdata0[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRG__Resource__ResourceServerApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRG__Resource__ResourceServerApplication_t qt_meta_stringdata_KRG__Resource__ResourceServerApplication = {
    {
QT_MOC_LITERAL(0, 0, 40), // "KRG::Resource::ResourceServer..."
QT_MOC_LITERAL(1, 41, 15), // "OnServerStopped"
QT_MOC_LITERAL(2, 57, 0), // ""
QT_MOC_LITERAL(3, 58, 19), // "OnTrayIconActivated"
QT_MOC_LITERAL(4, 78, 33), // "QSystemTrayIcon::ActivationRe..."
QT_MOC_LITERAL(5, 112, 6), // "reason"
QT_MOC_LITERAL(6, 119, 24), // "OnServerBusyStateChanged"
QT_MOC_LITERAL(7, 144, 17), // "CreateMainContent"
QT_MOC_LITERAL(8, 162, 22) // "CreateStatusBarContent"

    },
    "KRG::Resource::ResourceServerApplication\0"
    "OnServerStopped\0\0OnTrayIconActivated\0"
    "QSystemTrayIcon::ActivationReason\0"
    "reason\0OnServerBusyStateChanged\0"
    "CreateMainContent\0CreateStatusBarContent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRG__Resource__ResourceServerApplication[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    1,   40,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,
       7,    0,   44,    2, 0x08 /* Private */,
       8,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KRG::Resource::ResourceServerApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ResourceServerApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnServerStopped(); break;
        case 1: _t->OnTrayIconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 2: _t->OnServerBusyStateChanged(); break;
        case 3: _t->CreateMainContent(); break;
        case 4: _t->CreateStatusBarContent(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRG::Resource::ResourceServerApplication::staticMetaObject = { {
    QMetaObject::SuperData::link<Tools::Application::staticMetaObject>(),
    qt_meta_stringdata_KRG__Resource__ResourceServerApplication.data,
    qt_meta_data_KRG__Resource__ResourceServerApplication,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRG::Resource::ResourceServerApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRG::Resource::ResourceServerApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRG__Resource__ResourceServerApplication.stringdata0))
        return static_cast<void*>(this);
    return Tools::Application::qt_metacast(_clname);
}

int KRG::Resource::ResourceServerApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tools::Application::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
