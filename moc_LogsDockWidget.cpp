/****************************************************************************
** Meta object code from reading C++ file 'LogsDockWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.15)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "src/LogsDockWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LogsDockWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.15. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LogsDockWidget_t {
    QByteArrayData data[9];
    char stringdata0[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LogsDockWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LogsDockWidget_t qt_meta_stringdata_LogsDockWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "LogsDockWidget"
QT_MOC_LITERAL(1, 15, 19), // "onSearchTextChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 4), // "text"
QT_MOC_LITERAL(4, 41, 26), // "onDestinationFilterChanged"
QT_MOC_LITERAL(5, 68, 20), // "onLevelFilterChanged"
QT_MOC_LITERAL(6, 89, 20), // "onClearButtonClicked"
QT_MOC_LITERAL(7, 110, 19), // "onAutoScrollChanged"
QT_MOC_LITERAL(8, 130, 13) // "updateDisplay"

    },
    "LogsDockWidget\0onSearchTextChanged\0\0"
    "text\0onDestinationFilterChanged\0"
    "onLevelFilterChanged\0onClearButtonClicked\0"
    "onAutoScrollChanged\0updateDisplay"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LogsDockWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x08 /* Private */,
       4,    0,   47,    2, 0x08 /* Private */,
       5,    0,   48,    2, 0x08 /* Private */,
       6,    0,   49,    2, 0x08 /* Private */,
       7,    0,   50,    2, 0x08 /* Private */,
       8,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LogsDockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LogsDockWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onSearchTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->onDestinationFilterChanged(); break;
        case 2: _t->onLevelFilterChanged(); break;
        case 3: _t->onClearButtonClicked(); break;
        case 4: _t->onAutoScrollChanged(); break;
        case 5: _t->updateDisplay(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LogsDockWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_LogsDockWidget.data,
    qt_meta_data_LogsDockWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LogsDockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LogsDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LogsDockWidget.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int LogsDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
