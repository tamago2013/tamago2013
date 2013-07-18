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

    glwidget = new GLWidget();
    setCentralWidget(glwidget);

    QMenu *fps_menu;
    fps_menu = menuBar()->addMenu(tr("&FPS"));

    QAction *fps_action;
    fps_action = new QAction(tr("1 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_1) );
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps01()));
    fps_menu->addAction(fps_action);

    fps_action = new QAction(tr("5 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_2) );
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps05()));
    fps_menu->addAction(fps_action);

    fps_action = new QAction(tr("10 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_3) );
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps10()));
    fps_menu->addAction(fps_action);

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
    glwidget->update();
}

