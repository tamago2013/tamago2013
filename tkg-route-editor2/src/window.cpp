#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QScrollArea>
#include <QSignalMapper>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableView>
#include <QStandardItemModel>
#include <algorithm>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "menu-handler.hpp"
#include "tkg-config.hpp"
#include "tkg-debug.hpp"

Window::Window(tkg::ConfigFile &conf) : QMainWindow()
{
    tkg::debug("new Window\n");

    setWindowTitle(conf["Layout"]["title"].c_str());

    int window_width  = 800;
    int window_height = 600;

    std::vector<std::string> size;
    size = tkg::parseArray(conf["Layout"]["window-size"]);
    if(size.size() == 2)
    {
        window_width  = tkg::parseInt(size[0]);
        window_height = tkg::parseInt(size[1]);
    }
    resize(window_width, window_height);

    if(conf["Layout"]["window-maximized"] == "true")
    {
        setWindowState(Qt::WindowMaximized);
    }


    // Menu
    /*
    m_fps = menuBar()->addMenu(tr("&FPS"));
    m_ssm = menuBar()->addMenu(tr("&View"));
    */

    w_viewer  = new WidgetGL (this, conf);
    w_table   = new QTableWidget(0,6);

    QStringList header;
    header << tr("No.") << tr("f") << tr("x") << tr("y") << tr("r") << tr("s");
    w_table->setHorizontalHeaderLabels(header);
    w_table->verticalHeader()->hide();
    w_table->setFixedWidth(400);
    w_table->setColumnWidth(0, 50);
    w_table->setColumnWidth(1, 30);
    w_table->setColumnWidth(2, 80);
    w_table->setColumnWidth(3, 80);
    w_table->setColumnWidth(4, 80);
    w_table->setColumnWidth(5, 80);


    //w_status ->setMaximumHeight(1);
    //w_message->setMaximumHeight(1);

    //QScrollArea *a_scroll = new QScrollArea;
    //a_scroll->setWidget(w_message);
    //w_message->adjustSize();
    //a_scroll->setWidgetResizable(true);

    QSplitter *s_widget = new QSplitter(Qt::Vertical);
    QSplitter *s_viewer = new QSplitter(Qt::Horizontal);
   //\QSplitter *s_others = new QSplitter(Qt::Horizontal);

    s_viewer->addWidget(w_viewer);
    s_viewer->addWidget(w_table);
    //s_others->addWidget(w_status);
    //s_others->addWidget(a_scroll);
    s_widget->addWidget(s_viewer);
    //s_widget->addWidget(s_others);

    setCentralWidget(s_widget);
}

Window::~Window()
{
    tkg::debug("delete Window\n");
}

bool Window::init()
{
    w_viewer->init();
    return true;
}

void Window::addMenuView(SelectMenuHandler *handler)
{
    QMenu         *menu   = new QMenu(tr(handler->title.c_str()), this);
    QActionGroup  *group  = new QActionGroup(this);
    QSignalMapper *mapper = new QSignalMapper(this);

    m_ssm->addMenu(menu);
    group->setExclusive(true);

    for(uint i=0; i<handler->list.size(); i++)
    {
        QAction *action = new QAction(tr(handler->list[i].title.c_str()), this);
        action->setCheckable(true);
        if(handler->list[i].value == handler->value)
        {
            action->setChecked(true);
            handler->receive(handler->list[i].value);
        }
        menu  ->addAction(action);
        group ->addAction(action);
        mapper->setMapping(action, handler->list[i].value);
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    }
    connect(mapper, SIGNAL(mapped(int)), handler, SLOT(receive(int)));
}


void Window::addMenuView(ToggleMenuHandler *handler)
{
    QAction *action = new QAction(tr(handler->title.c_str()), this);

    m_ssm->addAction(action);

    action->setCheckable(true);
    if(handler->value)
    {
        action->setChecked(true);
        handler->receive(true);
    }
    connect(action, SIGNAL(toggled(bool)), handler, SLOT(receive(bool)));
}

