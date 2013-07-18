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

    QActionGroup *fps_group = new QActionGroup(this);
    fps_group->setExclusive(true);

    QAction *fps_action;
    fps_action = new QAction(tr("1 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_1) );
    fps_action->setCheckable(true);
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps01()));
    fps_menu->addAction(fps_action);
    fps_group->addAction(fps_action);

    fps_action = new QAction(tr("5 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_2) );
    fps_action->setCheckable(true);
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps05()));
    fps_menu->addAction(fps_action);
    fps_group->addAction(fps_action);

    fps_action = new QAction(tr("10 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_3) );
    fps_action->setCheckable(true);
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps10()));
    fps_menu->addAction(fps_action);
    fps_group->addAction(fps_action);

    fps_action = new QAction(tr("20 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_4) );
    fps_action->setCheckable(true);
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps20()));
    fps_menu->addAction(fps_action);
    fps_group->addAction(fps_action);

    fps_action = new QAction(tr("30 fps"), this);
    fps_action->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_5) );
    fps_action->setCheckable(true);
    connect(fps_action, SIGNAL(triggered()), this, SLOT(fps30()));
    fps_menu->addAction(fps_action);
    fps_group->addAction(fps_action);


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
*/

    status_message();

    show();
}

MainWindow::~MainWindow()
{
    printf("destructor\n");
}

void MainWindow::status_message()
{
    statusBar()->showMessage(tr("message"));
}
