#ifndef TKG_OPENGL_HPP
#define TKG_OPENGL_HPP

#include <string>
#include <cstdlib>

namespace tkg
{

const char *str[] =
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

    /* 0x30  0  */ "0060 ",
    /* 0x31  1  */ "",
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
