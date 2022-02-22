/*
        QTDEMO.h  -  defines for qt demo application

        The Structured Query Scripting Language
        Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

        Initial release: Aug 21
        Current release: Aug 21

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.

        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.

        You should have received a copy of the GNU Lesser General Public
        License along with this library; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef QTDEMO_H
#define QTDEMO_H

#include <QMainWindow>
#include <QMessageBox>
#include <QErrorMessage>
#include <QFileDialog>
#include <QToolBar>
#include <QTabBar>
#include <QToolButton>

#include <QThread>

#include <QPainter>
#include <QTextBlock>
#include <QPlainTextEdit>

#include "csqpc.h"

#define RESULTSTAB TextEdit	// TextEdit or PlainTextEdit

class codeEditor: public QPlainTextEdit {
    Q_OBJECT

public:
    codeEditor(bool useLines, QWidget *parent = 0);
    ~codeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    bool isUntitled();
    void setUntitled(bool);
    void setResults(QString);
    QString getResults();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumbers;
    bool untitled;
    bool useLines;
    QString results;
};

class lineNumberArea: public QWidget {
public:
    lineNumberArea(codeEditor *editor): QWidget(editor) {
        this->editor = editor;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        editor->lineNumberAreaPaintEvent(event);
    }

private:
    codeEditor *editor;
};

class container: public QWidget {
public:
    bool isEditor() {
        return property("EDITOR").toBool();
    }
    bool isHtml() {
        return property("HTML").toBool();
    }
};

typedef struct runnerenv {
    int width;
    fgw_loc_t *vars, txtvar;
} runnerenv_t;

class runner: public QThread {
    Q_OBJECT

public:
    runner(runnerenv_t *env, QTextDocument *document);
    ~runner();
    void run() override;

private:
    execinfo_t execinfo;
    runnerenv_t *env;
    QTextDocument *document;
    int len;

signals:
    void showError(QString);
    void showStatus(QString);
    void complete();
    void runnerId(void *);
};

namespace Ui {
    class mainWindow;
}

class mainWindow: public QMainWindow {
    Q_OBJECT

public:
    explicit mainWindow(fgw_loc_t *vars, char *inFile, int width, QWidget *parent = 0);
    ~mainWindow();
    
private:
    Ui::mainWindow *ui;
    QTabWidget* tabs;
    int untitledCount;
    QAction *runMenuAction;
    QAction *runBarAction;
    QAction *stopMenuAction;
    QAction *stopBarAction;
    runner *executionThread;
    pthread_t id;
    QWidget *resultsTab;
    QWidget *runningTab;
    runnerenv env;

    int addTab(QWidget *, QString);
    int tabCount();
    void setTabToolTip();
    void setRunning();
    void cancelRunning();

    void closeEvent(QCloseEvent*) override;

private slots:
    void updateToolTip();

    void createFile();
    void openFile();
    void openFile(const QString&);
    bool saveFile();
    bool saveFileAs();
    bool saveAllFiles();
    void closeFile();
    bool closeAllFiles();
    void closeTab(int);
    void switchTab(int);
    void closeWindow();
    void execute();
    void cancel();

    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotSelectAll();
    void slotUndo();

    void showError(QString);
    void showStatus(QString);
    void completeExecution();
    void runnerId(void *id);
};
#endif // QTDEMO_H
