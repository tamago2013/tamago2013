#ifndef TKG_OPENGL_GEOMETRY_HPP
#define TKG_OPENGL_GEOMETRY_HPP

#include <GL/glu.h>
#include "tkg-opengl.hpp"
#include "tkg-geometry.hpp"

namespace tkg
{

inline void glVertex(const Point3 &p)
{
    glVertex3dv( p.vec );
}

inline void glDrawCross(const Point3 &p, double theta, double r)
{
	glBegin(GL_LINES);
    glVertex3dv( (p + Point3::polar(r, theta + 0.0*pi)).vec );
    glVertex3dv( (p + Point3::polar(r, theta + 1.0*pi)).vec );
    glVertex3dv( (p + Point3::polar(r, theta + 0.5*pi)).vec );
    glVertex3dv( (p + Point3::polar(r, theta + 1.5*pi)).vec );
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


}

#endif TKG_OPENGL_GEOMETRY_HPP
