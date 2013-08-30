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
#include "fps-timer.hpp"
#include "tkg-config.hpp"
#include "ssm-message.hpp"

#include <QTableWidget>

Window::Window(tkg::ConfigFile &conf) : QMainWindow()
{
    resize(900, 600);
    setWindowTitle("visualizer");


    // Menu
    m_fps = menuBar()->addMenu(tr("&FPS"));
    m_ssm = menuBar()->addMenu(tr("&SSM"));

    w_viewer  = new WidgetGL (this, conf);
    w_camera1 = new WidgetIMG(this, conf["Camera1"]);
    w_camera2 = new WidgetIMG(this, conf["Camera2"]);
    w_status  = new WidgetMSG();
    w_message = new WidgetMSG();
    w_control = new WidgetMSG();

    /*
    w_camera1->setFixedWidth(320);
    w_camera2->setFixedWidth(320);
    */


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
    //connect(viewer->get_vssm(), SIGNAL(send_status (const char*)), status,  SLOT(add_message(const char*)));
    //connect(viewer->get_vssm(), SIGNAL(send_message(const char*)), message, SLOT(add_message(const char*)));

    w_viewer ->init();
    w_camera1->init();
    w_camera2->init();

    return true;
}

/*
void Window::test_add_menu_SSM(QObject* obj, const char* str)
{
    QAction *view00 = new QAction(tr("non-display"), this);
    //view00->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_1) );
    view00->setCheckable(true);

    QAction *view01 = new QAction(tr("point only"), this);
    //view01->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_2) );
    view01->setCheckable(true);

    QAction *view10 = new QAction(tr("laser only"), this);
    //view10->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_3) );
    view10->setCheckable(true);


    QAction *view11 = new QAction(tr("point + laser"), this);
    //view11->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_4) );
    view11->setCheckable(true);
    view11->setChecked(true);

    QMenu *stream_menu = new QMenu(tr(str), this);
    stream_menu->addAction(view00);
    stream_menu->addAction(view01);
    stream_menu->addAction(view10);
    stream_menu->addAction(view11);

    QActionGroup *stream_group = new QActionGroup(this);
    stream_group->setExclusive(true);
    stream_group->addAction(view00);
    stream_group->addAction(view01);
    stream_group->addAction(view10);
    stream_group->addAction(view11);

    ssm_menu->addMenu(stream_menu);

    QSignalMapper *signal_mapper = new QSignalMapper(this);
    connect(view00, SIGNAL(triggered()), signal_mapper, SLOT(map()));
    connect(view01, SIGNAL(triggered()), signal_mapper, SLOT(map()));
    connect(view10, SIGNAL(triggered()), signal_mapper, SLOT(map()));
    connect(view11, SIGNAL(triggered()), signal_mapper, SLOT(map()));
    signal_mapper->setMapping(view00, 0);
    signal_mapper->setMapping(view01, 1);
    signal_mapper->setMapping(view10, 2);
    signal_mapper->setMapping(view11, 3);
    connect(signal_mapper, SIGNAL(mapped(int)), obj, SLOT(set_view_state(int)));
}
*/

void Window::addMenuFPS(FPSTimer *obj, const char *str)
{
    QMenu         *menu   = new QMenu(tr(str), this);
    QActionGroup  *group  = new QActionGroup(this);
    QSignalMapper *mapper = new QSignalMapper(this);

    m_fps->addMenu(menu);
    group->setExclusive(true);

    int fps_default = 1;
    std::vector<int> fps = obj->getFPS();
    if(!fps.empty())
    {
        fps_default = fps.front();
        std::sort(fps.begin(), fps.end());
    }

    for(uint i=0; i<fps.size(); i++)
    {
        char str[256];
        std::snprintf(str, 256, "%2d fps", fps[i]);

        QAction *action = new QAction(tr(str), this);
        action->setCheckable(true);
        menu  ->addAction(action);
        group ->addAction(action);
        mapper->setMapping(action, fps[i]);
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));

        if(fps[i] == fps_default)
        {
            obj->setFPS(fps[i]);
            action->setChecked(true);
        }
    }
    connect(mapper, SIGNAL(mapped(int)), obj, SLOT(setFPS(int)));
}


void addMenuView(QObject  *obj, const char *str, const std::vector< std::pair<std::string, int> > &list)
{

}
