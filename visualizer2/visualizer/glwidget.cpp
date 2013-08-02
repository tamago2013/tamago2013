#include <GL/glu.h>
#include "glwidget.hpp"
using namespace tkg;

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    printf("GL constructor\n");
    setFocusPolicy(Qt::StrongFocus);

    mapviewer.read_cmap("/home/ena8781/roboken/map/tc2013");

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(100);
}

GLWidget::~GLWidget()
{
    printf("GL destructor\n");

    endSSM();
}

void GLWidget::initializeGL()
{
    // 背景色の指定
    glClearColor(0, 0, 0, 0);
}

void GLWidget::resizeGL(int w, int h)
{
    width  = w;
    height = h;
    aspect = (double)w/h;
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);   //  カラーバッファをクリア
    /*
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    */

    glViewport(0, 0, width, height);

    // カメラの設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500);
    camera.update();

    // モデルの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 地面の描画
    const double gsize = 10.0;
    double gr_x = 0;
    double gr_y = 0;

    glColor3d(0.2,0.2,0.2);
    glBegin(GL_POLYGON);
    glVertex3d(gr_x+gsize, gr_y+gsize, 0.0);
    glVertex3d(gr_x-gsize, gr_y+gsize, 0.0);
    glVertex3d(gr_x-gsize, gr_y-gsize, 0.0);
    glVertex3d(gr_x+gsize, gr_y-gsize, 0.0);
    glEnd();

    glColor3d(0.4,0.4,0.4);
    glBegin(GL_LINES);
    for(int i=-gsize; i<=gsize; i++)
    {
        glVertex3d(gr_x+i*1.0, gr_y-gsize, 0.0);
        glVertex3d(gr_x+i*1.0, gr_y+gsize, 0.0);
        glVertex3d(gr_x-gsize, gr_y+i*1.0, 0.0);
        glVertex3d(gr_x+gsize, gr_y+i*1.0, 0.0);
    }
    glEnd();

    // 各種データの描画
    mapviewer.draw();
    ssmviewer.draw();

    glFlush();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    double dx = 0;
    double dy = 0;
    printf("%d\n", event->key());
    if(event->key() == Qt::Key_Up   ) dy -= 0.1;
    if(event->key() == Qt::Key_Down ) dy += 0.1;
    if(event->key() == Qt::Key_Left ) dx -= 0.1;
    if(event->key() == Qt::Key_Right) dx += 0.1;



    if(event->modifiers() & Qt::ControlModifier)
    {
        camera.scale(dy*10);
    }
    else if(event->modifiers() & Qt::ShiftModifier)
    {
        camera.rotate(dx,dy);
    }
    else // (NoModifier)
    {
        camera.translate(dx,dy);
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{

}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    double dd = (double) event->delta() / 120;
    camera.scale(-dd);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    double dx = (double) (mouse_prev_x - event->x()) / width;
    double dy = (double) (mouse_prev_y - event->y()) / height;

    if(mouse_prev_b == Qt::LeftButton)
    {
        camera.translate(dx,dy);
    }

    if(mouse_prev_b == Qt::RightButton)
    {
        camera.rotate(dx,dy);
    }


    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
    mouse_prev_b = event->button();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_prev_x = event->x();
    mouse_prev_y = event->y();
    mouse_prev_b = event->button();
}

