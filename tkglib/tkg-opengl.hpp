#ifndef TKG_OPENGL_HPP
#define TKG_OPENGL_HPP

#include <string>
#include <cstdlib>
#include <GL/glu.h>
#include "tkg-geometry.hpp"

namespace tkg
{

// vector string
static const double vspos[19] = {0, 3, 4, 6, 8, 9, 12, 15, 16, 18, 20, 21, 24, 27, 28, 30, 32, 33, 36};

static const char *vsdata[] =
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

    /* 0x30  0  */ "CGAEACCAKAMCMEKGCGKA",
    /* 0x31  1  */ "DAADMD MAMG",
    /* 0x32  2  */ "CAACAECGEGMAMG",
    /* 0x33  3  */ "CAACAECGEGGEGC GEIGKGMEMCKA",
    /* 0x34  4  */ "HGHAAEME",
    /* 0x35  5  */ "AGAAGAGEIGKGMEMA",
    /* 0x36  6  */ "CGAEACCAKAMCMEKGIGGEGCIA",
    /* 0x37  7  */ "CAAAAGCGMC",
    /* 0x38  8  */ "GCEACAACAECGEGGEIGKGMEMCKAIAGCGE",
    /* 0x39  9  */ "KAMCMEKGCGAEACCAEAGCGEEG",
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
    public:

        Camera(double d=0.0) : dist(d), rotv(0.0), roth(0.0), pos(0.0) {}

        void scale    (double d)           { dist = std::max(1.0, dist+d); }
        void rotate   (double x, double y) { rotv = std::max(0.0, std::min(rotv+y, pi)); roth += x; }
        void translate(double x, double y)
        {
            Point3 tmp(y*dist/2, x*dist/2, 0);
            pos = pos + tmp.rotateZ(roth);
        }

        void setLookAt()
        {
            tkg::Point3 vec = tkg::Point3::polar(dist, rotv-tkg::pi/2, roth);
            tkg::Point3 cam = tkg::Point3::polar(dist, rotv, roth) + pos;
            gluLookAt(cam.x, cam.y, cam.z,  pos.x, pos.y, pos.z, vec.x, vec.y, vec.z);
        }

    private:

        double dist;
        double rotv;
        double roth;
        tkg::Point3 pos;
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

inline void glString(const std::string &str, const Point3 &p, double s, double v, double h)
{
    const double div = 15.0;
    double bx = - s / 2.0;
    double by = - s / 2.0 * str.size();

    glBegin(GL_LINES);
    for(uint i=0; i<str.size(); i++)
    {
        int ch = str[i] - 0x20;
        for(int j=0,c=0; vsdata[ch][j]; j++)
        {
            if(vsdata[ch][j] == ' ')
            {
                c = 0;
                continue;
            }
            if(++c != 4) continue;

            c = 2;
            double x1 = s * vspos[vsdata[ch][j-3]-'A'] / div;
            double y1 = s * vspos[vsdata[ch][j-2]-'A'] / div;
            double x2 = s * vspos[vsdata[ch][j-1]-'A'] / div;
            double y2 = s * vspos[vsdata[ch][j-0]-'A'] / div;

            tkg::Point3 p1(bx + x1, by + y1 + s*i);
            tkg::Point3 p2(bx + x2, by + y2 + s*i);
            glVertex(p + p1.rotateY(v).rotateZ(h));
            glVertex(p + p2.rotateY(v).rotateZ(h));
        }
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
