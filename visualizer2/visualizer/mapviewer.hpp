#ifndef MAPVIEWER_HPP
#define MAPVIEWER_HPP

class MapViewer
{
    public:

        MapViewer();
        ~MapViewer();

        bool read_cmap(const char *dirname);
        void draw();

    private:

        int width, height;
        double base_x, base_y;
        double unit_x, unit_y;
        char *data;
};


#endif // MAPVIEWER_HPP
