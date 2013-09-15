#include <QApplication>
#include <QTextCodec>
#include "main.hpp"
#include "window.hpp"
#include "tkg-config.hpp"

std::string config_file = "visualizer.conf";

bool optAnalyze(int argc, char **argv)
{
    int opt;
    while((opt = getopt(argc, argv, "g:h")) != -1)
    {
        switch(opt)
        {
            case 'g':
                config_file = optarg;
                break;

            case 'h':
                printf("\n");
                printf("\t[options]\n");
                printf("\t-g <FileName> : config file.\n");
                printf("\n");
                return false;

            default: // (opt=='?')
                printf("help : %s -h\n", argv[0]);
                return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    // config

    if(!optAnalyze(argc, argv)) return 0;

    tkg::ConfigFile config;
    if(!config.load(config_file))
    {
        printf("error: can't open config file.\n");
        return 0;
    }

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
