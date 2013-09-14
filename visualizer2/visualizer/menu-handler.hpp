#ifndef MENU_HANDLER_HPP
#define MENU_HANDLER_HPP

#include <QObject>

#include <QTimer>
#include <vector>
#include <string>

struct MenuElement
{
    std::string title;
    int         value;
    bool        check;

    MenuElement(const std::string &str, int v, bool c) : title(str), value(v), check(c) {}
    bool operator<(const MenuElement &m) const { return value < m.value; }
};

class MenuHandler : public QObject
{
    Q_OBJECT

    public: // functions

        MenuHandler(QObject *parent);
        ~MenuHandler();

    public slots:

        virtual void receive(int val);

    public: // variables

        std::string title;
        std::vector<MenuElement> list;
};

class ViewMenuHandler : public MenuHandler
{
    Q_OBJECT

    public: // functions

        ViewMenuHandler(QObject *parent);
        ~ViewMenuHandler();

    public slots:

        void receive(int val);

    public:

        int value;
};

class FpsMenuHandler : public MenuHandler
{
    Q_OBJECT

    public: // functions

        FpsMenuHandler(QObject *parent);
        ~FpsMenuHandler();

    public slots:

        void receive(int val);

    public:

        QTimer *timer;
};

/*
class FPSTimer : public QTimer
{
    Q_OBJECT

    public:  // functions

        FPSTimer(const std::vector<std::string> &fps);
        std::vector<int> getFPS();

    public slots:

        void setFPS(int fps);

    private: // variables

        std::vector<int> fpslist;
};
*/

#endif // MENU_HANDLER_HPP
