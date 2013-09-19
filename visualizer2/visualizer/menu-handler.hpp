#ifndef MENU_HANDLER_HPP
#define MENU_HANDLER_HPP

#include <QObject>

#include <QTimer>
#include <vector>
#include <string>

class ToggleMenuHandler : public QObject
{
    Q_OBJECT

    public: // functions

        ToggleMenuHandler(QObject *parent);
        ~ToggleMenuHandler();

    public slots:

        virtual void receive(bool val);

    public: // variables

        bool        value;
        std::string title;
};


struct SelectMenuElement
{
    std::string title;
    int         value;

    SelectMenuElement(const std::string &str, int v) : title(str), value(v) {}
    bool operator<(const SelectMenuElement &m) const { return value < m.value; }
};

class SelectMenuHandler : public QObject
{
    Q_OBJECT

    public: // functions

        SelectMenuHandler(QObject *parent);
        ~SelectMenuHandler();

    public slots:

        virtual void receive(int val);

    public: // variables

        int         value;
        std::string title;
        std::vector<SelectMenuElement> list;
};

class FpsMenuHandler : public SelectMenuHandler
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
