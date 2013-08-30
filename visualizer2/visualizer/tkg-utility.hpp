#ifndef TKG_UTILITY_HPP
#define TKG_UTILITY_HPP

#include <cstdlib>
#include <string>

namespace tkg
{

struct color { double rgba[4]; };

inline int parseInt(std::string str)
{
    return atoi( str.c_str() );
}

inline const char* parseStr(std::string str)
{
    return str.c_str();
}

inline color parseColor(std::string str)
{
    color ret;
    char *err;

    str = str.substr(1) + "FFFFFFFF";
    for(int i=0; i<4; i++)
    {
        ret.rgba[i] = std::strtol(str.substr(i*2,2).c_str(), &err, 16) / 255.0;
    }
    return  ret;
}

}
#endif // TKGUTILITY_HPP
