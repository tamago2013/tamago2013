#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "geometry.hpp"

class Camera
{
    public:

        Camera();
        void update();
        void scale    (double d);
        void rotate   (double x, double y);
        void translate(double x, double y);

    private:

        double roth;
        double rotv;
        double dist;
        tkg::point3 pos;
};

#endif
