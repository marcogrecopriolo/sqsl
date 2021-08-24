/****************************************************************************
** Meta object code from reading C++ file 'qtdemo.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtdemo.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtdemo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_codeEditor_t {
    QByteArrayData data[5];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_codeEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_codeEditor_t qt_meta_stringdata_codeEditor = {
    {
QT_MOC_LITERAL(0, 0, 10), // "codeEditor"
QT_MOC_LITERAL(1, 11, 25), // "updateLineNumberAreaWidth"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 20), // "highlightCurrentLine"
QT_MOC_LITERAL(4, 59, 20) // "updateLineNumberArea"

    },
    "codeEditor\0updateLineNumberAreaWidth\0"
    "\0highlightCurrentLine\0updateLineNumberArea"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_codeEditor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08 /* Private */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    2,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect, QMetaType::Int,    2,    2,

       0        // eod
};

void codeEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<codeEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateLineNumberAreaWidth(); break;
        case 1: _t->highlightCurrentLine(); break;
        case 2: _t->updateLineNumberArea((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject codeEditor::staticMetaObject = { {
    &QPlainTextEdit::staticMetaObject,
    qt_meta_stringdata_codeEditor.data,
    qt_meta_data_codeEditor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *codeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *codeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_codeEditor.stringdata0))
        return static_cast<void*>(this);
    return QPlainTextEdit::qt_metacast(_clname);
}

int codeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_runner_t {
    QByteArrayData data[6];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_runner_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_runner_t qt_meta_stringdata_runner = {
    {
QT_MOC_LITERAL(0, 0, 6), // "runner"
QT_MOC_LITERAL(1, 7, 9), // "showError"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 10), // "showStatus"
QT_MOC_LITERAL(4, 29, 8), // "complete"
QT_MOC_LITERAL(5, 38, 8) // "runnerId"

    },
    "runner\0showError\0\0showStatus\0complete\0"
    "runnerId"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_runner[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    1,   37,    2, 0x06 /* Public */,
       4,    0,   40,    2, 0x06 /* Public */,
       5,    1,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::VoidStar,    2,

       0        // eod
};

void runner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<runner *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->showStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->complete(); break;
        case 3: _t->runnerId((*reinterpret_cast< void*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (runner::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&runner::showError)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (runner::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&runner::showStatus)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (runner::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&runner::complete)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (runner::*)(void * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&runner::runnerId)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject runner::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_runner.data,
    qt_meta_data_runner,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *runner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *runner::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_runner.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int runner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void runner::showError(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void runner::showStatus(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void runner::complete()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void runner::runnerId(void * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_mainWindow_t {
    QByteArrayData data[25];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mainWindow_t qt_meta_stringdata_mainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "mainWindow"
QT_MOC_LITERAL(1, 11, 13), // "updateToolTip"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 10), // "createFile"
QT_MOC_LITERAL(4, 37, 8), // "openFile"
QT_MOC_LITERAL(5, 46, 8), // "saveFile"
QT_MOC_LITERAL(6, 55, 10), // "saveFileAs"
QT_MOC_LITERAL(7, 66, 12), // "saveAllFiles"
QT_MOC_LITERAL(8, 79, 9), // "closeFile"
QT_MOC_LITERAL(9, 89, 13), // "closeAllFiles"
QT_MOC_LITERAL(10, 103, 8), // "closeTab"
QT_MOC_LITERAL(11, 112, 9), // "switchTab"
QT_MOC_LITERAL(12, 122, 11), // "closeWindow"
QT_MOC_LITERAL(13, 134, 7), // "execute"
QT_MOC_LITERAL(14, 142, 6), // "cancel"
QT_MOC_LITERAL(15, 149, 7), // "slotCut"
QT_MOC_LITERAL(16, 157, 8), // "slotCopy"
QT_MOC_LITERAL(17, 166, 9), // "slotPaste"
QT_MOC_LITERAL(18, 176, 13), // "slotSelectAll"
QT_MOC_LITERAL(19, 190, 8), // "slotUndo"
QT_MOC_LITERAL(20, 199, 9), // "showError"
QT_MOC_LITERAL(21, 209, 10), // "showStatus"
QT_MOC_LITERAL(22, 220, 17), // "completeExecution"
QT_MOC_LITERAL(23, 238, 8), // "runnerId"
QT_MOC_LITERAL(24, 247, 2) // "id"

    },
    "mainWindow\0updateToolTip\0\0createFile\0"
    "openFile\0saveFile\0saveFileAs\0saveAllFiles\0"
    "closeFile\0closeAllFiles\0closeTab\0"
    "switchTab\0closeWindow\0execute\0cancel\0"
    "slotCut\0slotCopy\0slotPaste\0slotSelectAll\0"
    "slotUndo\0showError\0showStatus\0"
    "completeExecution\0runnerId\0id"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  129,    2, 0x08 /* Private */,
       3,    0,  130,    2, 0x08 /* Private */,
       4,    0,  131,    2, 0x08 /* Private */,
       4,    1,  132,    2, 0x08 /* Private */,
       5,    0,  135,    2, 0x08 /* Private */,
       6,    0,  136,    2, 0x08 /* Private */,
       7,    0,  137,    2, 0x08 /* Private */,
       8,    0,  138,    2, 0x08 /* Private */,
       9,    0,  139,    2, 0x08 /* Private */,
      10,    1,  140,    2, 0x08 /* Private */,
      11,    1,  143,    2, 0x08 /* Private */,
      12,    0,  146,    2, 0x08 /* Private */,
      13,    0,  147,    2, 0x08 /* Private */,
      14,    0,  148,    2, 0x08 /* Private */,
      15,    0,  149,    2, 0x08 /* Private */,
      16,    0,  150,    2, 0x08 /* Private */,
      17,    0,  151,    2, 0x08 /* Private */,
      18,    0,  152,    2, 0x08 /* Private */,
      19,    0,  153,    2, 0x08 /* Private */,
      20,    1,  154,    2, 0x08 /* Private */,
      21,    1,  157,    2, 0x08 /* Private */,
      22,    0,  160,    2, 0x08 /* Private */,
      23,    1,  161,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::VoidStar,   24,

       0        // eod
};

void mainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<mainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateToolTip(); break;
        case 1: _t->createFile(); break;
        case 2: _t->openFile(); break;
        case 3: _t->openFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: { bool _r = _t->saveFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->saveFileAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->saveAllFiles();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->closeFile(); break;
        case 8: { bool _r = _t->closeAllFiles();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 9: _t->closeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->switchTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->closeWindow(); break;
        case 12: _t->execute(); break;
        case 13: _t->cancel(); break;
        case 14: _t->slotCut(); break;
        case 15: _t->slotCopy(); break;
        case 16: _t->slotPaste(); break;
        case 17: _t->slotSelectAll(); break;
        case 18: _t->slotUndo(); break;
        case 19: _t->showError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 20: _t->showStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 21: _t->completeExecution(); break;
        case 22: _t->runnerId((*reinterpret_cast< void*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject mainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_mainWindow.data,
    qt_meta_data_mainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *mainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_mainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int mainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
