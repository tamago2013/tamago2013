#include <GL/gl.h>
#include <GL/glu.h>
#include "camera.hpp"
using namespace tkg;

#include <iostream>
using namespace std;

Camera::Camera()
{
    roth = 0.0;
    rotv = 0.0;
    dist = 15.0;
}

void Camera::update()
{
    point3 vec = point3::polar(dist, rotv-pi/2, roth);
    point3 cam = point3::polar(dist, rotv, roth) + pos;
    gluLookAt(cam.x, cam.y, cam.z,  pos.x, pos.y, pos.z, vec.x, vec.y, vec.z);
}

void Camera::rotate(double x, double y)
{
    roth += x;
    rotv += y;
}

void Camera::zoom(double d)
{
    cout << d << endl;
    dist += d;
}
