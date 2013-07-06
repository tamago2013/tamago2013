#include <QApplication>
#include "window.hpp"

int main(int argc, char *argv[])
{
    // 初期化
    QApplication app(argc, argv);
    MainWindow *window = new MainWindow();

    // メインループ
    int ret = app.exec();

    // 終了
    if(window) delete window;
    return ret;
}
