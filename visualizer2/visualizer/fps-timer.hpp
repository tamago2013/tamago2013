#ifndef FPS_TIMER_HPP
#define FPS_TIMER_HPP

#include <QTimer>
#include <vector>
#include <string>
class QMenu;
class QTimer;

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

#endif // FPSTIMER_HPP
