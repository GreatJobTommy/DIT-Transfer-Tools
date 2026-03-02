/****************************************************************************
** Meta object code from reading C++ file 'DriveMonitor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "src/DriveMonitor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DriveMonitor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DriveMonitor_t {
    QByteArrayData data[13];
    char stringdata0[145];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DriveMonitor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DriveMonitor_t qt_meta_stringdata_DriveMonitor = {
    {
QT_MOC_LITERAL(0, 0, 12), // "DriveMonitor"
QT_MOC_LITERAL(1, 13, 14), // "driveConnected"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "path"
QT_MOC_LITERAL(4, 34, 17), // "driveDisconnected"
QT_MOC_LITERAL(5, 52, 16), // "driveReconnected"
QT_MOC_LITERAL(6, 69, 10), // "driveAdded"
QT_MOC_LITERAL(7, 80, 5), // "drive"
QT_MOC_LITERAL(8, 86, 12), // "driveRemoved"
QT_MOC_LITERAL(9, 99, 8), // "lastFile"
QT_MOC_LITERAL(10, 108, 6), // "offset"
QT_MOC_LITERAL(11, 115, 18), // "onDirectoryChanged"
QT_MOC_LITERAL(12, 134, 10) // "pollDrives"

    },
    "DriveMonitor\0driveConnected\0\0path\0"
    "driveDisconnected\0driveReconnected\0"
    "driveAdded\0drive\0driveRemoved\0lastFile\0"
    "offset\0onDirectoryChanged\0pollDrives"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DriveMonitor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       5,    0,   60,    2, 0x06 /* Public */,
       6,    1,   61,    2, 0x06 /* Public */,
       8,    1,   64,    2, 0x06 /* Public */,
       5,    3,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   74,    2, 0x08 /* Private */,
      12,    0,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::LongLong,    3,    9,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

       0        // eod
};

void DriveMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DriveMonitor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->driveConnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->driveDisconnected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->driveReconnected(); break;
        case 3: _t->driveAdded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->driveRemoved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->driveReconnected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        case 6: _t->onDirectoryChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->pollDrives(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DriveMonitor::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DriveMonitor::driveConnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DriveMonitor::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DriveMonitor::driveDisconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DriveMonitor::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DriveMonitor::driveReconnected)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DriveMonitor::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DriveMonitor::driveAdded)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DriveMonitor::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DriveMonitor::driveRemoved)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DriveMonitor::*)(const QString & , const QString & , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DriveMonitor::driveReconnected)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DriveMonitor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DriveMonitor.data,
    qt_meta_data_DriveMonitor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DriveMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DriveMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DriveMonitor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DriveMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void DriveMonitor::driveConnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DriveMonitor::driveDisconnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DriveMonitor::driveReconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DriveMonitor::driveAdded(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DriveMonitor::driveRemoved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DriveMonitor::driveReconnected(const QString & _t1, const QString & _t2, qint64 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
