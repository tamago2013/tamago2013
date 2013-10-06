#include <GL/glu.h>
#include "camera.hpp"

Camera::Camera()
{
    mode = 2;
    roth = 0.0;
    rotv = 0.0;
    dist = 15.0;
}

void Camera::update()
{
    tkg::Point3 vec = tkg::Point3::polar(dist, rotv-tkg::pi/2, roth);
    tkg::Point3 cam = tkg::Point3::polar(dist, rotv, roth) + pos;
    gluLookAt(cam.x, cam.y, cam.z,  pos.x, pos.y, pos.z, vec.x, vec.y, vec.z);
}

void Camera::scale(double d)
{
    dist += d;
    if(dist < 1.0) dist = 1.0;
}

void Camera::rotate(double x, double y)
{
    roth += x;
    rotv += y;
    if(rotv<0)         rotv = 0;
    if(rotv>tkg::pi/2) rotv = tkg::pi/2;
}

void Camera::translate(double x, double y)
{
    tkg::Point3 tmp(y*dist/2, x*dist/2, 0);
    //tmp.rotY(rotv); // not grounding
    tmp = tmp.rotateZ(roth);
    pos = pos + tmp;
    //if(grounding==1) { camCent.z=0; }
}

void Camera::setpos(double x, double y, double t)
{
    if(mode>=1) pos = tkg::Point3(x,y,0);
    if(mode>=2) roth = t+3.141592;
}

void Camera::setmode(int m)
{
    mode = m;
}
