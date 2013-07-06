#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "geometry.hpp"

class Camera
{
    public:

        Camera();
        void update();
        void rotate(double x, double y);
        void zoom  (double d);

    private:

        double roth;
        double rotv;
        double dist;
        tkg::point3 pos;
};

#endif
