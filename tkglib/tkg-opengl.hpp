#ifndef TKG_OPENGL_HPP
#define TKG_OPENGL_HPP

#include <string>
#include <cstdlib>
#include <GL/glu.h>
#include "tkg-geometry.hpp"

namespace tkg
{

class Color4
{
    public: // functions

        Color4(std::string str);
        Color4(double r=1.0, double g=1.0, double b=1.0, double a=1.0);

    public: // variables

        double rgba[4];
};

class Camera
{

};

inline void glVertex(const Point3 &p)
{
    glVertex3dv( p.vec );
}

inline void glDrawCross(const Point3 &p, double t, double r)
{
    glBegin(GL_LINES);
    glVertex3dv( (p + Point3::polar(r, t + 0.0*pi)).vec );
    glVertex3dv( (p + Point3::polar(r, t + 1.0*pi)).vec );
    glVertex3dv( (p + Point3::polar(r, t + 0.5*pi)).vec );
    glVertex3dv( (p + Point3::polar(r, t + 1.5*pi)).vec );
    glEnd();
}

inline void glArrow(const Point3 &p, double t, double r)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex( p + Point3( r,   0, 0).rotateZ(t) );
    glVertex( p + Point3(-r,   r, 0).rotateZ(t) );
    glVertex( p + Point3(-r/2, 0, 0).rotateZ(t) );
    glVertex( p + Point3(-r,  -r, 0).rotateZ(t) );
    glEnd();
}

// implements

inline Color4::Color4(double r, double g, double b, double a)
{
    rgba[0]=r; rgba[1]=g; rgba[2]=b; rgba[3]=a;
}

inline Color4::Color4(std::string str)
{
    if(str[0]=='#')
    {
        str = str.substr(1) + "FFFFFFFF";

        char *err;
        for(int i=0; i<4; i++)
        {
            rgba[i] = std::strtol(str.substr(i*2,2).c_str(), &err, 16) / 255.0;
        }
    }
}




/*
const double char_vector[2km././][] =
*/



//カメラ
//色クラス

/*
    opengl + geometry = tkg-opengl-geometry


    void myString(double x, double y, double z, const char *str)
    {
        glRasterPos3d(x, y, z);
        for(int i=0; str[i]; i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
        }
    }
 */



}


#endif // TKG_OPENGL_HPP
