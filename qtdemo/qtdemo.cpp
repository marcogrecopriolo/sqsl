/*
        QTDEMO.cpp  -  SQSL qt demo application

        The Structured Query Scripting Language
        Copyright (C) 1992-2021 Marco Greco (marco@4glworks.com)

        Initial release: Aug 2021
        Current release: Aug 2021

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

#include <signal.h>
#include <cascc.h>
#include <csqrc.h>
#include <csqpc.h>
#include <ctypc.h>
#include "qtdemo.h"
#include "ui_qtdemo.h"
#include <QApplication>

// TODO preferences
#define COLOUR_BKG Qt::white
#define COLOUR_HILITE (QColor(Qt::lightGray).lighter(120))
#define COLOUR_LINES Qt::darkGray
#define COLOUR_FONT Qt::black
#define SQSL_WIDTH 80

#define SQSL_FLAGS K_verbose|K_dml|K_noconf|K_preserve
#define ERRMSGSZ 256

// Not going to have a header file for one forward declaration
extern "C" char *parse_args(int argc, char **argv, fgw_loc_t *vars);

int main(int argc, char *argv[]) {
    fgw_loc_t vars;
    sigset_t sigset;

    QApplication app(argc, argv);
    char *inFile = parse_args(argc, argv, &vars);
    mainWindow *window = new mainWindow(&vars, inFile, SQSL_WIDTH);
    window->show();

    // we ignore SIGINT when not execute it and send it to the runner when we are
    signal(SIGINT, SIG_IGN);
    return app.exec();
}

mainWindow::mainWindow(fgw_loc_t *vars, char *inFile, int width, QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow) {

    // set up runner environment
    env.vars = vars;
    memset(&env.txtvar, 0, sizeof(env.txtvar));
    env.txtvar.loc_loctype=LOCMEMORY;
    env.txtvar.loc_indicator=-1;
    runningTab = NULL;
    env.width = width;
    id = NULL;

    // set up UI elements
    ui->setupUi(this);
    setAcceptDrops(true);
    setWindowIcon(QIcon(":/icons/sqsl.png"));
    setWindowTitle("SQSL QT demo");

    untitledCount = 0;

    // set up tab widget
    tabs = new QTabWidget(this);
    tabs->setMovable(true);
    tabs->setTabsClosable(true);
    tabs->setUsesScrollButtons(true);

    // TODO would be nice to have it floating
    QToolButton *newTabButton = new QToolButton;
    newTabButton->setText("+");
    newTabButton->setToolTip("New File");
    newTabButton->setAutoRaise(true);
    connect(newTabButton, SIGNAL(clicked()), this, SLOT(createFile()));
    tabs->setCornerWidget(newTabButton, Qt::TopRightCorner);

    createFile();
    if (inFile != NULL)
	openFile(QString(inFile));

    // set up menu bar
    QMenu *fileMenu = new QMenu("File");
    QMenu *editMenu = new QMenu("Edit");

    fileMenu->addAction("New File", this, SLOT(createFile()), Qt::CTRL + Qt::Key_N);
    fileMenu->addAction("Open File", this, SLOT(openFile()), Qt::CTRL + Qt::Key_O);
    fileMenu->addSeparator();
    fileMenu->addAction("Save File", this, SLOT(saveFile()), Qt::CTRL + Qt::Key_S);
    fileMenu->addAction("Save As...",this, SLOT(saveFileAs()), Qt::SHIFT + Qt::CTRL + Qt::Key_S);
    fileMenu->addSeparator();
    fileMenu->addAction("Close File", this, SLOT(closeFile()), Qt::CTRL + Qt::Key_W);
    fileMenu->addSeparator();
    runMenuAction = fileMenu->addAction("Run", this, SLOT(execute()), Qt::CTRL + Qt::Key_R);
    stopMenuAction = fileMenu->addAction("Stop", this, SLOT(cancel()), Qt::CTRL + Qt::Key_Delete);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, SLOT(closeWindow()), Qt::CTRL + Qt::Key_Q);

    editMenu->addAction("Cut", this, SLOT(slotCut()), Qt::CTRL + Qt::Key_X);
    editMenu->addAction("Copy", this, SLOT(slotCopy()), Qt::CTRL + Qt::Key_C);
    editMenu->addAction("Paste", this, SLOT(slotPaste()), Qt::CTRL + Qt::Key_V);
    editMenu->addAction("Undo", this, SLOT(slotUndo()), Qt::CTRL + Qt::Key_U);
    editMenu->addAction("Select All", this, SLOT(slotSelectAll()), Qt::CTRL + Qt::Key_A);

    ui->menuBar->addMenu(fileMenu);
    ui->menuBar->addMenu(editMenu);
    ui->menuBar->setVisible(true);

    // set up tool bar
    ui->mainToolBar->addAction(QIcon(":/icons/new.png"), "New File", this, SLOT(createFile()));
    ui->mainToolBar->addAction(QIcon(":/icons/open.png"), "Open File", this, SLOT(openFile()));
    ui->mainToolBar->addAction(QIcon(":/icons/save.png"), "Save File", this, SLOT(saveFile()));
    ui->mainToolBar->addAction(QIcon(":/icons/save_as.png"), "Save File As", this, SLOT(saveFileAs()));
    ui->mainToolBar->addSeparator();
    runBarAction = ui->mainToolBar->addAction(QIcon(":/icons/run.png"), "Run", this, SLOT(execute()));
    stopBarAction = ui->mainToolBar->addAction(QIcon(":/icons/stop.png"), "Stop", this, SLOT(cancel()));
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(QIcon(":/icons/copy.png"), "Copy", this, SLOT(slotCopy()));
    ui->mainToolBar->addAction(QIcon(":/icons/cut.png"), "Cut", this, SLOT(slotCut()));
    ui->mainToolBar->addAction(QIcon(":/icons/paste.png"), "Paste", this, SLOT(slotPaste()));
    ui->mainToolBar->addAction(QIcon(":/icons/undo.png"), "Undo", this, SLOT(slotUndo()));

    setCentralWidget(tabs);
    tabs->currentWidget()->setFocus();
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(switchTab(int)));
    cancelRunning();
}

mainWindow::~mainWindow() {
    fgw_sqlrelease(env.vars);
    delete ui;
}

int mainWindow::tabCount() {
    return tabs->count();
}

int mainWindow::addTab(QWidget *widget, QString title) {
     return tabs->addTab(widget, title);
}

// file menu and tab management slots

void mainWindow::createFile() {
    int index;

    codeEditor *newTextEdit = new codeEditor(true);
    if (untitledCount++ == 0)
	index = addTab(newTextEdit, "untitled");
    else
	index = addTab(newTextEdit, "untitled " + QString::number(untitledCount));
    newTextEdit->setResults("");
    tabs->setCurrentIndex(index);
    setTabToolTip();
    connect(newTextEdit, SIGNAL(textChanged()), this, SLOT(updateToolTip()));
}

void mainWindow::openFile() {
    QString filepath = QFileDialog::getOpenFileName(this, "Choose file", "");
    if (filepath.isEmpty())
        return;
    openFile(filepath);
}

void mainWindow::openFile(const QString& filepath) {
    QString filename = filepath.section("/", -1, -1);
    QFile file(filepath);

    if (file.open(QIODevice::ReadOnly)) {
        container *c = (container *)tabs->currentWidget();
        codeEditor *curEditor = (codeEditor *)c;

        if (c->isEditor() && curEditor->document()->isEmpty() && curEditor->isUntitled())
            delete tabs->widget(tabs->currentIndex());

        codeEditor* editor = new codeEditor(true);
        QString content = file.readAll();
        file.close();
	editor->setUntitled(false);
        editor->appendPlainText(content);
        int index = addTab(editor, filename);
        tabs->setCurrentIndex(index);
        setTabToolTip();
        connect(editor, SIGNAL(textChanged()), this, SLOT(updateToolTip()));
    } else {
        (new QErrorMessage(this))->showMessage("Cannot open file!");
        return;
    }
}

bool mainWindow::saveFile() {
    container *c = (container *)tabs->currentWidget();
    codeEditor *curEditor = (codeEditor *)c;

    if (c->isEditor() && curEditor->isUntitled()) {
        return saveFileAs();
    }
    QString filepath = tabs->tabText(tabs->currentIndex());
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
	if (c->isEditor())
	    file.write(curEditor->document()->toPlainText().toUtf8());
#if RESULTSTAB == TextEdit
	else {
	    if (c->isHtml())
		file.write(((QTextEdit *)c)->toHtml().toUtf8());
	    else
		file.write(((QTextEdit *)c)->toPlainText().toUtf8());
	}
#endif
        file.close();
        setTabToolTip();
    } else {
        (new QErrorMessage(this))->showMessage("Cannot save file!");
        return false;
    }
    return true;
}

bool mainWindow::saveFileAs() {
    QString filename = tabs->tabText(tabs->currentIndex());
    QString filepath = QFileDialog::getSaveFileName(this, "Save " + filename, filename);
    container *c = (container *)tabs->currentWidget();
    codeEditor *curEditor = (codeEditor *)c;

    if (filepath.isEmpty())
        return false;
    if (QFileInfo(filepath).suffix().isEmpty())
        filepath.append(".sqsl");
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
	if (c->isEditor()) {
	    file.write(curEditor->document()->toPlainText().toUtf8());
	    file.close();
	    curEditor->setUntitled(false);
	    curEditor->setResults("");
	}
#if RESULTSTAB == TextEdit
	else {
	    if (c->isHtml())
		file.write(((QTextEdit *)c)->toHtml().toUtf8());
	    else
		file.write(((QTextEdit *)c)->toPlainText().toUtf8());
	    file.close();
	}
#endif
    } else {
        (new QErrorMessage(this))->showMessage("Cannot save file!");
        return false;
    }
    filename = filepath.section("/", -1, -1);
    tabs->tabBar()->setTabText(tabs->currentIndex(), filename);
    setTabToolTip();
    return true;
}

bool mainWindow::saveAllFiles() {
    int current_index = tabs->currentIndex();
    for (int i = 0; i < tabCount(); ++i) {
        container *c = (container *)tabs->widget(i);
        tabs->setCurrentIndex(i);

        if (c->isEditor()) {
	    if (((codeEditor *)c)->document()->isModified() && !saveFile())
		return false;
	 }
#if RESULTSTAB == TextEdit
	else {
	    if (((QTextEdit *)c)->document()->isModified() && !saveFile())
		return false;
	}
#endif
    }
    tabs->setCurrentIndex(current_index);
    return true;
}

void mainWindow::closeTab(int idx) {
    container *c = (container *)tabs->widget(idx);
    bool check;

    if (c->isEditor())
	check = ((codeEditor *)c)->document()->isModified();
#if RESULTSTAB == TextEdit
    else 
	check = ((QTextEdit *)c)->document()->isModified();
#endif
    if (check) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Saving changes", "Save changes before closing?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes && !saveFile())
	    return;
    }
    delete tabs->widget(idx);

    if (tabCount() == 0)
        createFile();
    tabs->currentWidget()->setFocus();
}

void mainWindow::switchTab(int idx) {
    if (idx >= 0) {
	container *c = (container *)tabs->widget(idx);
	codeEditor *editor = (codeEditor *)c;
	bool runEnabled = c->isEditor() && !editor->isReadOnly();

	runMenuAction->setEnabled(runEnabled);
	runBarAction->setEnabled(runEnabled);
    }
}

void mainWindow::closeFile() {
    closeTab(tabs->currentIndex());
}

bool mainWindow::closeAllFiles() {
    bool checker = false;
    for (int i = 0; i < tabCount(); ++i) { // QTabWidget guarantees the consistency of indices?
        container *c = (container *)tabs->widget(i);

        if (c->isEditor()) {
	    if (((codeEditor *)c)->document()->isModified()) {
		checker = true;
		break;
	    }
        }
#if RESULTSTAB == TextEdit
	else {
	    if (((QTextEdit *)c)->document()->isModified()) {
		checker = true;
		break;
	    }
	}
#endif
    }
    if (checker) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Save all changes", "Save all changes before closing?",
            QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes && !saveAllFiles())
	    return false;
    }
    while (tabCount() > 0)
        delete tabs->widget(0);

    return true;
}

void mainWindow::closeWindow() {
    if (closeAllFiles())
	QApplication::quit();
}

void mainWindow::closeEvent(QCloseEvent *event) {
    if (closeAllFiles())
	event->accept();
    else
	event->ignore();
}

// sqsl execution slots

void mainWindow::execute() {
    // for completeness
    container *c = (container *)tabs->currentWidget();
    if (!c->isEditor())
	return;

    container *newResults = NULL;
    runningTab = tabs->widget(tabs->currentIndex());
    codeEditor *editor = (codeEditor *) runningTab;
    
    setRunning();
    if (editor->getResults() != "") {
	for (int i = 0; i < tabCount(); ++i)
	    if (tabs->tabText(i) == editor->getResults()) {
		newResults = (container *) tabs->widget(i);
		break;
	    }
    }

    if (newResults == NULL) {
	int index;
	QString results;
#if RESULTSTAB == TextEdit
	QTextEdit *newBrowser;

	newBrowser = new QTextEdit();
	newBrowser->setReadOnly(true);
	resultsTab = newBrowser;
#elif RESULTSTAB == PlainTextEdit
	codeEditor *newBrowser;

	newBrowser = new codeEditor(false);
        newBrowser->setReadOnly(true);
	resultsTab = newBrowser;
#endif
	results = "results for " + tabs->tabBar()->tabText(tabs->currentIndex());
       	index = addTab(newBrowser, results);
	tabs->setCurrentIndex(index);
	editor->setResults(results);
    } else {
	resultsTab = newResults;
	tabs->setCurrentIndex(tabs->indexOf(newResults));
    }
    executionThread = new runner(&env, editor->document());
    connect(executionThread, SIGNAL(complete()), this, SLOT(completeExecution()));
    connect(executionThread, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
    connect(executionThread, SIGNAL(showStatus(QString)), this, SLOT(showStatus(QString)));
    connect(executionThread, SIGNAL(runnerId(void *)), this, SLOT(runnerId(void *)));
    showStatus("");
    executionThread->start();
}

// FIXME this is dodgy, but we need the runner to get out of stuck system call (hence we need to signal the
// runner thread), and there's no way to know the runner thread id, so we use signal and slots
void mainWindow::runnerId(void *id) {
    this->id = (pthread_t) id;
    disconnect(executionThread, SIGNAL(runnerId(void *)), this, SLOT(runnerId(void *)));
}

void mainWindow::completeExecution() {
    if (((container *) resultsTab)->isEditor()) {
	((codeEditor *)resultsTab)->clear();
	if (env.txtvar.loc_size > 0)
	    ((codeEditor *)resultsTab)->appendPlainText(QString::fromLocal8Bit(env.txtvar.loc_buffer, env.txtvar.loc_size));
    }
#if RESULTSTAB == TextEdit
    else {
	((QTextEdit *)resultsTab)->clear();
	((QTextEdit *)resultsTab)->setProperty("HTML", false);
	if (env.txtvar.loc_size > 0)
	    ((QTextEdit *)resultsTab)->append(QString::fromLocal8Bit(env.txtvar.loc_buffer, env.txtvar.loc_size));
    }
#endif
    fgw_freetext(&env.txtvar);
    cancelRunning();
    runningTab = NULL;
    resultsTab = NULL;
    disconnect(executionThread, SIGNAL(complete()), this, SLOT(completeExecution()));
    disconnect(executionThread, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
    disconnect(executionThread, SIGNAL(showStatus(QString)), this, SLOT(showStatus(QString)));
    id = NULL;
    delete executionThread;
}

void mainWindow::cancel() {
#ifdef NT
    kill(_getpid(), SIGINT);
#else
    pthread_t thread = id;

    if (thread != NULL)
	pthread_kill(thread, SIGINT);
#endif
}

void mainWindow::showError(QString s) {
    ui->statusBar->showMessage(s);
}

void mainWindow::showStatus(QString s) {
    ui->statusBar->showMessage(s);
}

void mainWindow::setRunning() {
    codeEditor *editor = (codeEditor *) tabs->currentWidget();
    tabs->tabBar()->tabButton(tabs->currentIndex(), QTabBar::RightSide)->setEnabled(false);
    editor->setReadOnly(true);
    runMenuAction->setVisible(false);
    runBarAction->setVisible(false);
    stopMenuAction->setVisible(true);
    stopBarAction->setVisible(true);
}

void mainWindow::cancelRunning() {
    if (runningTab != NULL) {
	int index = tabs->indexOf(runningTab);
	if (index >= 0) 
	    tabs->tabBar()->tabButton(index, QTabBar::RightSide)->setEnabled(true);

	// for completeness
	container *c = (container *) runningTab;
	if (c->isEditor()) {
	    codeEditor *editor = (codeEditor *) runningTab;
	    editor->setReadOnly(false);
	}
    }
    runMenuAction->setVisible(true);
    runBarAction->setVisible(true);
    stopMenuAction->setVisible(false);
    stopBarAction->setVisible(false);
}

// edit menu slots

void mainWindow::slotCopy() {
    container *c = (container *)tabs->currentWidget();
    if (c->isEditor())
	((codeEditor *) tabs->currentWidget())->copy();
#if RESULTSTAB == TextEdit
    else
	((QTextEdit *) tabs->currentWidget())->copy();
#endif
}

void mainWindow::slotCut() {
    container *c = (container *)tabs->currentWidget();
    if (c->isEditor())
	((codeEditor *) tabs->currentWidget())->cut();
#if RESULTSTAB == TextEdit
    else
	((QTextEdit *) tabs->currentWidget())->cut();
#endif
}

void mainWindow::slotSelectAll() {
    container *c = (container *)tabs->currentWidget();
    if (c->isEditor())
	((codeEditor *) tabs->currentWidget())->selectAll();
#if RESULTSTAB == TextEdit
    else
	((QTextEdit *) tabs->currentWidget())->selectAll();
#endif
}

void mainWindow::slotPaste() {
    container *c = (container *)tabs->currentWidget();
    if (c->isEditor())
	((codeEditor *) tabs->currentWidget())->paste();
#if RESULTSTAB == TextEdit
    else
	((QTextEdit *) tabs->currentWidget())->paste();
#endif
}

void mainWindow::slotUndo() {
    container *c = (container *)tabs->currentWidget();
    if (c->isEditor())
	((codeEditor *) tabs->currentWidget())->undo();
#if RESULTSTAB == TextEdit
    else
	((QTextEdit *) tabs->currentWidget())->undo();
#endif
}

// editor tab state change

void mainWindow::setTabToolTip() {
    bool changed;
    QString tip = tabs->tabBar()->tabText(tabs->currentIndex());
    container *c = (container *) tabs->currentWidget();
    
    if (c->isEditor())
	changed = ((codeEditor *) c)->document()->isModified();
#if RESULTSTAB == TextEdit
    else
	changed = ((QTextEdit *) c)->document()->isModified();
#endif
    if (changed)
	tip.append(" - changed");
    else
	tip.append(" - not changed");
    tabs->setTabToolTip(tabs->currentIndex(), tip);
}

void mainWindow::updateToolTip() {
    setTabToolTip();
    disconnect(sender(), SIGNAL(textChanged()), this, SLOT(updateToolTip()));
}

// SQSL runner

runner::runner(runnerenv_t *env, QTextDocument *document) {
    this->env = env;
    this->document = document;
    execinfo.context = (void *) this;
}

runner::~runner() {
}

void runner::run() {
    emit runnerId((void *) pthread_self());
    int r = fgw_sqlexec(document->toPlainText().toUtf8().data(), document->toPlainText().size(),  0, SQSL_FLAGS, env->width,
			env->vars, &env->txtvar, &execinfo);
    if (r != 0) {
	char errmsg[ERRMSGSZ];

	fgw_errmsg(r, &execinfo.errorinfo, (char *) errmsg, sizeof(errmsg));
	emit showError(errmsg);
    }
    emit complete();
}

// SQSL interface

void sqsl_numrows(int rows, execinfo_t *execinfo) {
    runner *r = (runner *) execinfo->context;

    emit r->showStatus("Rows processed: " + QString::number(rows));
}

int sqsl_asktouch(int touch, execinfo_t *execinfo) {
    return 1;
}

int sqsl_promptpasswd(int tok, int opts, char *txt, char *e_buf, int *len, int verbose, execinfo_t *execinfo) {
    return RC_NWNDW;
}

int sqsl_picklist(int tok, int opts, char *txt, char *e_buf, int *len, char *sep, char *quotes, int verbose, execinfo_t *execinfo) {
    return RC_NWNDW;
}

// code editor

codeEditor::codeEditor(bool useLines, QWidget *parent) : QPlainTextEdit(parent) {
    untitled = true;
    this->useLines = useLines;
    setProperty("EDITOR", true);
    QPalette p = palette();
    p.setColor(QPalette::Base, COLOUR_BKG);
    p.setColor(QPalette::Text, COLOUR_FONT);
    setPalette(p);

    lineNumbers = new lineNumberArea(this);
    if (useLines) {
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    }
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth();
    highlightCurrentLine();
}

codeEditor::~codeEditor() {
    if (this->useLines) {
	disconnect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth()));
	disconnect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    }
    disconnect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
}

bool codeEditor::isUntitled() {
    return untitled;
}

void codeEditor::setUntitled(bool u) {
    untitled = u;
}

QString codeEditor::getResults() {
    return results;
}

void codeEditor::setResults(QString r) {
    results = r;
}

void codeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(COLOUR_HILITE);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

int codeEditor::lineNumberAreaWidth() {
    if (!useLines)
	return 0;
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void codeEditor::updateLineNumberAreaWidth() {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void codeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumbers->scroll(0, dy);
    else
        lineNumbers->update(0, rect.y(), lineNumbers->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void codeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumbers->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void codeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    if (!useLines)
	return;
    QPainter painter(lineNumbers);
    painter.fillRect(event->rect(), COLOUR_BKG);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(COLOUR_LINES);
            painter.drawText(0, top,
                             lineNumbers->width(),
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
