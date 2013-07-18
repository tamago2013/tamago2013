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


    private slots:

        void fps01() { fps=1;  glwidget->setfps(fps); }
        void fps05() { fps=5;  glwidget->setfps(fps); }
        void fps10() { fps=10; glwidget->setfps(fps); }
        void fps20() { fps=20; glwidget->setfps(fps); }
        void fps30() { fps=30; glwidget->setfps(fps); }

    private:

        int fps;

        GLWidget *glwidget;

        void status_message();
};

#endif
