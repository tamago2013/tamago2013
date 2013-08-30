#ifndef TKG_OPENGL_HPP
#define TKG_OPENGL_HPP

#include <string>
#include <cstdlib>

namespace tkg
{

class color4
{
    public: // functions

        color4(std::string str);
        color4(double r=1.0, double g=1.0, double b=1.0, double a=1.0);

    public: // variables

        double rgba[4];
};


// implements

inline color4::color4(double r, double g, double b, double a)
{
    rgba[0]=r; rgba[1]=g; rgba[2]=b; rgba[3]=a;
}

inline color4::color4(std::string str)
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
