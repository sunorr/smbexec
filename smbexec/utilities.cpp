#include <Windows.h>
#include <stdio.h>

#define __DEBUG__

void debug( char * fmt, ... )
{
    char szBuffer[10240] = {0};
#ifdef __DEBUG__
    va_list valist;
    int argn = 0;
    char *para = NULL;

    va_start( valist, fmt );
    //printf("[debug: file: %s, line: %d] ", __FILE__, __LINE__ );
    vsprintf( szBuffer, fmt, valist );
    OutputDebugString( szBuffer );
    vprintf( fmt, valist );
    va_end( valist );
#endif

}