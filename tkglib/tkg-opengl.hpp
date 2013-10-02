#ifndef TKG_OPENGL_HPP
#define TKG_OPENGL_HPP

#include <string>
#include <cstdlib>
#include <GL/glu.h>
#include "tkg-geometry.hpp"

namespace tkg
{

static const char *vec[] =
{
    /* 0x20     */ "",
    /* 0x21  !  */ "",
    /* 0x22  "  */ "",
    /* 0x23  #  */ "",
    /* 0x24  $  */ "",
    /* 0x25  %  */ "",
    /* 0x26  &  */ "",
    /* 0x27  '  */ "",
    /* 0x28  (  */ "",
    /* 0x29  )  */ "",
    /* 0x2A  *  */ "",
    /* 0x2B  +  */ "",
    /* 0x2C  ,  */ "",
    /* 0x2D  -  */ "",
    /* 0x2E  .  */ "",
    /* 0x2F  /  */ "",

    /* 0x30  0  */ "AAMA MAMI MIAI AIAA",
    /* 0x31  1  */ "AIGA GAMI DEJE",
    /* 0x32  2  */ "",
    /* 0x33  3  */ "",
    /* 0x34  4  */ "",
    /* 0x35  5  */ "",
    /* 0x36  6  */ "",
    /* 0x37  7  */ "",
    /* 0x38  8  */ "",
    /* 0x39  9  */ "",
    /* 0x3A  :  */ "",
    /* 0x3B  ;  */ "",
    /* 0x3C  <  */ "",
    /* 0x3D  =  */ "",
    /* 0x3E  >  */ "",
    /* 0x3F  ?  */ "",

    ""
};

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

inline void glString(const std::string &str)
{
    double x=0,y=0;

    glBegin(GL_LINES);
    int c = 17;
    for(int j=0; vec[c][j]; j++)
    {
        if(j%5 != 3) continue;

        double y1 = - (vec[c][j-3] - 'A') / 12.0;
        double x1 =   (vec[c][j-2] - 'A') / 12.0 * 3;
        double y2 = - (vec[c][j-1] - 'A') / 12.0;
        double x2 =   (vec[c][j-0] - 'A') / 12.0 * 3;

        glVertex2d(-x1, y1);
        glVertex2d(-x2, y2);
    }
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
