#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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

        void fps01() { glwidget->setfps( 1); }
        void fps05() { glwidget->setfps( 5); }
        void fps10() { glwidget->setfps(10); }

    private:

        GLWidget *glwidget;

        QMenu *file_menu;
        QAction *new_act;
        QAction *open_act;
};

#endif
