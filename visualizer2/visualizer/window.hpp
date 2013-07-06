#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QTimer>
#include <QMainWindow>
#include "glwidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        MainWindow(QWidget *parent = 0);
        ~MainWindow();
        
    protected slots:

        void onTimer();

    private slots:

        void aaa();
        void bbb(bool){}

    private:

        QTimer *m_timer;
        GLWidget *m_glWidget;

        QMenu *file_menu;
        QAction *new_act;
        QAction *open_act;
};

#endif
