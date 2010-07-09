/****************************************************************************
** Meta object code from reading C++ file 'QComponentWidget.h'
**
** Created: Fri 9. Jul 17:13:27 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QComponentWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QComponentWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QComponentWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QComponentWidget[] = {
    "QComponentWidget\0\0refreshWidget()\0"
};

const QMetaObject QComponentWidget::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QComponentWidget,
      qt_meta_data_QComponentWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QComponentWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QComponentWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QComponentWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QComponentWidget))
        return static_cast<void*>(const_cast< QComponentWidget*>(this));
    return QDialog::qt_metacast(_clname);
}

int QComponentWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refreshWidget(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
