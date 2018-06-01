/****************************************************************************
** Meta object code from reading C++ file 'pcscompress.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pcscompress.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pcscompress.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_pcsCompress_t {
    QByteArrayData data[17];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_pcsCompress_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_pcsCompress_t qt_meta_stringdata_pcsCompress = {
    {
QT_MOC_LITERAL(0, 0, 11), // "pcsCompress"
QT_MOC_LITERAL(1, 12, 21), // "clickedOpenFileAction"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 21), // "clickPointCloudButton"
QT_MOC_LITERAL(4, 57, 20), // "clickPCOctTreeButton"
QT_MOC_LITERAL(5, 78, 19), // "clickTwoFrameButton"
QT_MOC_LITERAL(6, 98, 16), // "changeRefFrameId"
QT_MOC_LITERAL(7, 115, 19), // "changeTargetFrameId"
QT_MOC_LITERAL(8, 135, 17), // "changeOctCellSize"
QT_MOC_LITERAL(9, 153, 16), // "changeClusterNum"
QT_MOC_LITERAL(10, 170, 7), // "changeU"
QT_MOC_LITERAL(11, 178, 18), // "changeMDegreeCheby"
QT_MOC_LITERAL(12, 197, 13), // "changeNscales"
QT_MOC_LITERAL(13, 211, 9), // "trainMatP"
QT_MOC_LITERAL(14, 221, 14), // "getSparseMatch"
QT_MOC_LITERAL(15, 236, 15), // "getMotionVector"
QT_MOC_LITERAL(16, 252, 4) // "test"

    },
    "pcsCompress\0clickedOpenFileAction\0\0"
    "clickPointCloudButton\0clickPCOctTreeButton\0"
    "clickTwoFrameButton\0changeRefFrameId\0"
    "changeTargetFrameId\0changeOctCellSize\0"
    "changeClusterNum\0changeU\0changeMDegreeCheby\0"
    "changeNscales\0trainMatP\0getSparseMatch\0"
    "getMotionVector\0test"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_pcsCompress[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a /* Public */,
       3,    0,   90,    2, 0x0a /* Public */,
       4,    0,   91,    2, 0x0a /* Public */,
       5,    0,   92,    2, 0x0a /* Public */,
       6,    0,   93,    2, 0x0a /* Public */,
       7,    0,   94,    2, 0x0a /* Public */,
       8,    0,   95,    2, 0x0a /* Public */,
       9,    0,   96,    2, 0x0a /* Public */,
      10,    0,   97,    2, 0x0a /* Public */,
      11,    0,   98,    2, 0x0a /* Public */,
      12,    0,   99,    2, 0x0a /* Public */,
      13,    0,  100,    2, 0x0a /* Public */,
      14,    0,  101,    2, 0x0a /* Public */,
      15,    0,  102,    2, 0x0a /* Public */,
      16,    0,  103,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void pcsCompress::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        pcsCompress *_t = static_cast<pcsCompress *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->clickedOpenFileAction(); break;
        case 1: _t->clickPointCloudButton(); break;
        case 2: _t->clickPCOctTreeButton(); break;
        case 3: _t->clickTwoFrameButton(); break;
        case 4: _t->changeRefFrameId(); break;
        case 5: _t->changeTargetFrameId(); break;
        case 6: _t->changeOctCellSize(); break;
        case 7: _t->changeClusterNum(); break;
        case 8: _t->changeU(); break;
        case 9: _t->changeMDegreeCheby(); break;
        case 10: _t->changeNscales(); break;
        case 11: _t->trainMatP(); break;
        case 12: _t->getSparseMatch(); break;
        case 13: _t->getMotionVector(); break;
        case 14: _t->test(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject pcsCompress::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_pcsCompress.data,
      qt_meta_data_pcsCompress,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *pcsCompress::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *pcsCompress::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_pcsCompress.stringdata0))
        return static_cast<void*>(const_cast< pcsCompress*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int pcsCompress::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
