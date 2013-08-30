#ifndef TKG_GEOMETRY_HPP
#define TKG_GEOMETRY_HPP

#include <cmath>

namespace tkg
{

const double pi = 3.14159265358979323846;

class Point3
{
	public:

        union
		{
			double vec[3];
			struct { double x,y,z; };
		};

        Point3(double t = 0)                 : x(t), y(t), z(t) {}
        Point3(double x, double y, double z) : x(x), y(y), z(z) {}

        Point3(double *v)
		{
            for(int i=0; i<3; i++) { vec[i] = v[i]; }
		}

        static Point3 polar(double r, double t)
		{
            return Point3(r*cos(t), r*sin(t), 0);
		}

        static Point3 polar(double r, double v, double h)
		{
            return Point3(r*sin(v)*cos(h), r*sin(v)*sin(h), r*cos(v));
		}


        Point3 operator+(const Point3 &t) const { return Point3(x+t.x, y+t.y, z+t.z); }
        Point3 operator-(const Point3 &t) const { return Point3(x-t.x, y-t.y, z-t.z); }


		double abs() const
		{
			return sqrt(x*x + y*y + z*z);
		}

        /*
		void rotX(double rad)
		{
			double ty=y, tz=z;
			y = ty*cos(rad) - tz*sin(rad);
			z = ty*sin(rad) + tz*cos(rad);
		}

        void rotY(double rad)
		{
			double tz=z, tx=x;
			z = tz*cos(rad) - tx*sin(rad);
			x = tz*sin(rad) + tx*cos(rad);
		}

        void rotZ(double rad)
		{
			double tx=x, ty=y;
			x = tx*cos(rad) - ty*sin(rad);
			y = tx*sin(rad) + ty*cos(rad);
		}
        */

        Point3 rotateZ(double rad)
        {
            Point3 ret;
            ret.x = x*cos(rad) - y*sin(rad);
            ret.y = x*sin(rad) + y*cos(rad);
            ret.z = z;
            return ret;
        }

/*
        friend ostream& operator<<(ostream &out, const Point3 &t)
		{
			return (out << "(" << t.x << "," << t.y << "," << t.z << ")");
        }
*/

};

}

#endif // TKG_GEOMETRY_HPP
