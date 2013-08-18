#include <QApplication>
#include <QTextCodec>
#include "main.hpp"
#include "window.hpp"
#include "tkg-config.hpp"

#include <iostream>

void cmdanalyze()
{}

int main(int argc, char *argv[])
{
    // config

    tkg::ConfigFile config;
    config.load("../visualizer.conf");

    // window

    QApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    Window *window;

    window = new Window(config);
    window->show();
    window->init();

    int retval = app.exec();

    delete window;
    return retval;
}
