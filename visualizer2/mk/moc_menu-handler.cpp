/****************************************************************************
** Meta object code from reading C++ file 'menu-handler.hpp'
**
** Created: Wed Sep 18 17:21:02 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../visualizer/menu-handler.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'menu-handler.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ToggleMenuHandler[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   19,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ToggleMenuHandler[] = {
    "ToggleMenuHandler\0\0val\0receive(bool)\0"
};

void ToggleMenuHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ToggleMenuHandler *_t = static_cast<ToggleMenuHandler *>(_o);
        switch (_id) {
        case 0: _t->receive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ToggleMenuHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ToggleMenuHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ToggleMenuHandler,
      qt_meta_data_ToggleMenuHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ToggleMenuHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ToggleMenuHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ToggleMenuHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToggleMenuHandler))
        return static_cast<void*>(const_cast< ToggleMenuHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int ToggleMenuHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_SelectMenuHandler[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   19,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SelectMenuHandler[] = {
    "SelectMenuHandler\0\0val\0receive(int)\0"
};

void SelectMenuHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SelectMenuHandler *_t = static_cast<SelectMenuHandler *>(_o);
        switch (_id) {
        case 0: _t->receive((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SelectMenuHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SelectMenuHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SelectMenuHandler,
      qt_meta_data_SelectMenuHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SelectMenuHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SelectMenuHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SelectMenuHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SelectMenuHandler))
        return static_cast<void*>(const_cast< SelectMenuHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int SelectMenuHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FpsMenuHandler[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   16,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FpsMenuHandler[] = {
    "FpsMenuHandler\0\0val\0receive(int)\0"
};

void FpsMenuHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FpsMenuHandler *_t = static_cast<FpsMenuHandler *>(_o);
        switch (_id) {
        case 0: _t->receive((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FpsMenuHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FpsMenuHandler::staticMetaObject = {
    { &SelectMenuHandler::staticMetaObject, qt_meta_stringdata_FpsMenuHandler,
      qt_meta_data_FpsMenuHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FpsMenuHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FpsMenuHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FpsMenuHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FpsMenuHandler))
        return static_cast<void*>(const_cast< FpsMenuHandler*>(this));
    return SelectMenuHandler::qt_metacast(_clname);
}

int FpsMenuHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SelectMenuHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
