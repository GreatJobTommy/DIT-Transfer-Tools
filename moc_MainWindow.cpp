/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "src/MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[24];
    char stringdata0[260];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 11), // "updateLists"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 7), // "addTask"
QT_MOC_LITERAL(4, 32, 14), // "onFilesDropped"
QT_MOC_LITERAL(5, 47, 5), // "files"
QT_MOC_LITERAL(6, 53, 10), // "ejectDrive"
QT_MOC_LITERAL(7, 64, 15), // "updateDashboard"
QT_MOC_LITERAL(8, 80, 12), // "updateDrives"
QT_MOC_LITERAL(9, 93, 14), // "updateProgress"
QT_MOC_LITERAL(10, 108, 12), // "updateErrors"
QT_MOC_LITERAL(11, 121, 14), // "settingChanged"
QT_MOC_LITERAL(12, 136, 3), // "key"
QT_MOC_LITERAL(13, 140, 5), // "value"
QT_MOC_LITERAL(14, 146, 15), // "onTaskCompleted"
QT_MOC_LITERAL(15, 162, 13), // "TransferTask*"
QT_MOC_LITERAL(16, 176, 4), // "task"
QT_MOC_LITERAL(17, 181, 12), // "onTaskFailed"
QT_MOC_LITERAL(18, 194, 12), // "onTaskPaused"
QT_MOC_LITERAL(19, 207, 18), // "onDriveReconnected"
QT_MOC_LITERAL(20, 226, 17), // "onProgressChanged"
QT_MOC_LITERAL(21, 244, 5), // "bytes"
QT_MOC_LITERAL(22, 250, 5), // "speed"
QT_MOC_LITERAL(23, 256, 3) // "eta"

    },
    "MainWindow\0updateLists\0\0addTask\0"
    "onFilesDropped\0files\0ejectDrive\0"
    "updateDashboard\0updateDrives\0"
    "updateProgress\0updateErrors\0settingChanged\0"
    "key\0value\0onTaskCompleted\0TransferTask*\0"
    "task\0onTaskFailed\0onTaskPaused\0"
    "onDriveReconnected\0onProgressChanged\0"
    "bytes\0speed\0eta"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08 /* Private */,
       3,    0,   85,    2, 0x08 /* Private */,
       4,    1,   86,    2, 0x08 /* Private */,
       6,    0,   89,    2, 0x08 /* Private */,
       7,    0,   90,    2, 0x08 /* Private */,
       8,    0,   91,    2, 0x08 /* Private */,
       9,    0,   92,    2, 0x08 /* Private */,
      10,    0,   93,    2, 0x08 /* Private */,
      11,    2,   94,    2, 0x08 /* Private */,
      14,    1,   99,    2, 0x08 /* Private */,
      17,    1,  102,    2, 0x08 /* Private */,
      18,    1,  105,    2, 0x08 /* Private */,
      19,    0,  108,    2, 0x08 /* Private */,
      20,    3,  109,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   12,   13,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong, QMetaType::LongLong,   21,   22,   23,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateLists(); break;
        case 1: _t->addTask(); break;
        case 2: _t->onFilesDropped((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 3: _t->ejectDrive(); break;
        case 4: _t->updateDashboard(); break;
        case 5: _t->updateDrives(); break;
        case 6: _t->updateProgress(); break;
        case 7: _t->updateErrors(); break;
        case 8: _t->settingChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 9: _t->onTaskCompleted((*reinterpret_cast< TransferTask*(*)>(_a[1]))); break;
        case 10: _t->onTaskFailed((*reinterpret_cast< TransferTask*(*)>(_a[1]))); break;
        case 11: _t->onTaskPaused((*reinterpret_cast< TransferTask*(*)>(_a[1]))); break;
        case 12: _t->onDriveReconnected(); break;
        case 13: _t->onProgressChanged((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TransferTask* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TransferTask* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< TransferTask* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
