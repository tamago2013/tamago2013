#ifndef MAP_VIEWER_HPP
#define MAP_VIEWER_HPP

#include <string>
#include <QObject>
#include <QThread>

struct MapInfo
{
    int    width,  height;
    double base_x, base_y;
    double unit_x, unit_y;
};

class MapLoader : public QObject
{
    Q_OBJECT

    public:

        MapLoader();
        ~MapLoader();

        void file(std::string path);

    public slots:

        void load();
        //void exit();

    signals:

        void send_make();

    public:

        uchar       *data;
        MapInfo     minfo;
        std::string fname;

};

class MapViewer : public QObject
{
    Q_OBJECT

    public:

        MapViewer(std::string path);
        ~MapViewer();

        bool good();
        void draw();
        void load();

    public slots:

        void make();

    signals:

        void send_load();
        //void send_exit();

    private:

        bool        state;
        uint        image;
        MapInfo     minfo;
        std::string fname;

        MapLoader *loader;
        QThread   *thread;

};

#endif // MAP_VIEWER_HPP
