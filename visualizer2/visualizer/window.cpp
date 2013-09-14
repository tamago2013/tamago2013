#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QScrollArea>
#include <QSignalMapper>
#include "window.hpp"
#include "widget-gl.hpp"
#include "widget-msg.hpp"
#include "widget-img.hpp"
#include "menu-handler.hpp"
#include "tkg-config.hpp"
#include "ssm-message.hpp"

#include <QTableWidget>

Window::Window(tkg::ConfigFile &conf) : QMainWindow()
{
    setWindowTitle(conf["Layout"]["title"].c_str());

    if(conf["Layout"]["window-size"] == "maximized")
    {
        resize(900, 600);
        setWindowState(Qt::WindowMaximized);
    }
    else
    {
        std::vector<std::string> size;
        size = tkg::parseArray(conf["Layout"]["window-size"]);

        if(size.size() == 2)
        {
            int width  = tkg::parseInt(size[0]);
            int height = tkg::parseInt(size[1]);
            resize(width, height);
        }
    }

    // Menu
    m_fps = menuBar()->addMenu(tr("&FPS"));
    m_ssm = menuBar()->addMenu(tr("&View"));

    w_viewer  = new WidgetGL (this, conf);
    w_camera1 = new WidgetIMG(this, conf["Camera1"]);
    w_camera2 = new WidgetIMG(this, conf["Camera2"]);
    w_status  = new WidgetMSG();
    w_message = new WidgetMSG();
    w_control = new WidgetMSG();

    w_status ->setMaximumHeight(150);
    w_message->setMaximumHeight(150);
    w_control->setMaximumHeight(150);

    QScrollArea *m_scrollArea = new QScrollArea;
    m_scrollArea->setWidget(w_message);
    w_message->adjustSize();
    m_scrollArea->setWidgetResizable(true);

    QSplitter *s_widget = new QSplitter(Qt::Vertical);
    QSplitter *s_camera = new QSplitter(Qt::Vertical);
    QSplitter *s_viewer = new QSplitter(Qt::Horizontal);
    QSplitter *s_others = new QSplitter(Qt::Horizontal);

    s_camera->resize( QSize(320,480) );

    s_camera->addWidget(w_camera1);
    s_camera->addWidget(w_camera2);
    s_viewer->addWidget(w_viewer);
    s_viewer->addWidget(s_camera);
    s_others->addWidget(w_status);
    s_others->addWidget(m_scrollArea);
    //s_others->addWidget(w_control);
    s_widget->addWidget(s_viewer);
    s_widget->addWidget(s_others);

    setCentralWidget(s_widget);

    // ssm init
    smTarget(w_message);
    smInit();



    QAction *image_free = new QAction(tr("free"), this);
    image_free->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_8) );
    image_free->setCheckable(true);

    QAction *image_lock_xy = new QAction(tr("lock (x,y)"), this);
    image_lock_xy->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_9) );
    image_lock_xy->setCheckable(true);

    QAction *image_lock_xyt = new QAction(tr("lock (x,y,theta)"), this);
    image_lock_xyt->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_0) );
    image_lock_xyt->setCheckable(true);
    image_lock_xyt->setChecked(true);

    QMenu *image_menu;
    image_menu = menuBar()->addMenu(tr("&Camera"));
    image_menu->addAction(image_free);
    image_menu->addAction(image_lock_xy);
    image_menu->addAction(image_lock_xyt);

    QActionGroup *image_group = new QActionGroup(this);
    image_group->setExclusive(true);
    image_group->addAction(image_free);
    image_group->addAction(image_lock_xy);
    image_group->addAction(image_lock_xyt);

    QSignalMapper *signal_mapper2 = new QSignalMapper(this);
    connect(image_free,     SIGNAL(triggered()), signal_mapper2, SLOT(map()));
    connect(image_lock_xy,  SIGNAL(triggered()), signal_mapper2, SLOT(map()));
    connect(image_lock_xyt, SIGNAL(triggered()), signal_mapper2, SLOT(map()));
    signal_mapper2->setMapping(image_free,     0);
    signal_mapper2->setMapping(image_lock_xy,  1);
    signal_mapper2->setMapping(image_lock_xyt, 2);
    connect(signal_mapper2, SIGNAL(mapped(int)), w_viewer->get_camera(), SLOT(setmode(int)));
}

Window::~Window()
{
    smEnd();
}

bool Window::init()
{
    w_viewer ->init();
    w_camera1->init();
    w_camera2->init();

    return true;
}

void Window::addMenuFps(MenuHandler *handler)
{
    QMenu         *menu   = new QMenu(tr(handler->title.c_str()), this);
    QActionGroup  *group  = new QActionGroup(this);
    QSignalMapper *mapper = new QSignalMapper(this);

    m_fps->addMenu(menu);
    group->setExclusive(true);

    std::sort(handler->list.begin(), handler->list.end());

    for(uint i=0; i<handler->list.size(); i++)
    {
        QAction *action = new QAction(tr(handler->list[i].title.c_str()), this);
        action->setCheckable(true);
        if(handler->list[i].check)
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

void Window::addMenuView(MenuHandler *handler)
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
        if(handler->list[i].check)
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

