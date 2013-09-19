#ifndef TKG_UTILITY_HPP
#define TKG_UTILITY_HPP

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

namespace tkg
{

inline std::string strf(const char *format, ... )
{
    va_list list;
    char str[256];

    va_start(list, format);
    vsprintf(str, format, list);
    va_end(list);

    return std::string(str);
}

inline int parseInt(const std::string &str)
{
    return atoi( str.c_str() );
}

inline const char* parseStr(const std::string &str)
{
    return str.c_str();
}

inline const std::vector<std::string> parseArray(const std::string &str)
{
    std::vector<std::string> ret;
    for(std::string::size_type p=0,n=0; true; p=n+1)
    {
        n = str.find_first_of(",", p);
        if(n == std::string::npos)
        {
            ret.push_back( str.substr(p) );
            break;
        }

        ret.push_back( str.substr(p,n-p) );
    }
    return ret;
}

inline std::string removeComment(const std::string &_str)
{
    std::string str = _str;
    std::string::size_type s,t;
    while( true )
    {
        s = str.find("/*", s);
        if(s == std::string::npos) break;

        t = str.find("*/", s+2);
        str.erase(s, t-s+2);
    }
    return str;
}

}

#endif // TKG_UTILITY_HPP
