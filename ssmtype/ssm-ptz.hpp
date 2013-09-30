//
//ssm-ptz.hpp
//
//YSD 2013/9/30
//

#include <ssm.hpp>

#define SNAME_PTZ "ptz"

namespace ysd {

typedef struct {

    double pan;     //[deg]
    double tilt;    //[deg]
    double zoom;
    double brightness;
    bool autofocus;

}PTZ;

}
