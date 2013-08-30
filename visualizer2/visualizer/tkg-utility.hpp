#ifndef TKG_UTILITY_HPP
#define TKG_UTILITY_HPP

#include <cstdlib>
#include <string>

namespace tkg
{

inline int parseInt(std::string str)
{
    return atoi( str.c_str() );
}

inline const char* parseStr(std::string str)
{
    return str.c_str();
}

}
#endif // TKGUTILITY_HPP
