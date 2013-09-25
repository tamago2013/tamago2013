#ifndef TKG_DEBUG_HPP
#define TKG_DEBUG_HPP

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

namespace tkg
{

inline void debug(const char *format, ... )
{
    va_list list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
    fflush( stdout );
}


}

#endif // TKG_DEBUG_HPP
