#include "window.hpp"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <cstdio>
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    printf("constructor\n");
    resize(800, 600);
    setWindowTitle("visualizer");

    m_glWidget = new GLWidget();
    setCentralWidget(m_glWidget);

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    m_timer->start(50);

    /*
    new_act = new QAction(tr("&New"), this);
    new_act->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_N) );
    new_act->setStatusTip(tr("create new file"));
    connect(new_act, SIGNAL(triggered()), this, SLOT(aaa()));

    open_act = new QAction(tr("&Bool"), this);
    open_act->setCheckable(true);
    connect(open_act, SIGNAL(triggered(bool)), this, SLOT(bbb(bool)));


    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(new_act);
    file_menu->addAction(open_act);

    statusBar()->showMessage(tr("message"));
    */

    show();
}

MainWindow::~MainWindow()
{
    printf("destructor\n");
}

void MainWindow::aaa()
{
    statusBar()->showMessage(tr("selected"));
}

void MainWindow::onTimer()
{
    m_glWidget->update();
}

