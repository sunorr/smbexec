#include <Windows.h>
#include <WinNetWk.h>
#include <string.h>
#include <stdio.h>
#include "makeclient.h"

#pragma comment(lib, "mpr.lib")

#define __DEBUG__

int main( int argc, char ** argv )
{
    NETRESOURCE nr = {0};

    DWORD dwRetVal = 0;

    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = NULL;
    nr.lpRemoteName = "\\\\10.16.101.47\\admin$";
    //nr.lpRemoteName = "\\\\192.168.79.129\\admin$";
    nr.lpProvider =  NULL;


    dwRetVal = WNetAddConnection3( NULL, &nr, "123qwe!@#", "administrator", 0 );
    //dwRetVal = WNetAddConnection3( NULL, &nr, "123qwe", "administrator", 0 );
    if ( dwRetVal != NO_ERROR )
    {
        int a = GetLastError();
        return -1;
    }

    char szFileName[MAX_PATH] = {0};
    sprintf( szFileName, "%s\\test", nr.lpRemoteName );

    HANDLE hFile = NULL;
    hFile = CreateFile( szFileName, 
                        GENERIC_WRITE,
                        0,
                        NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL );

    Client( "10.16.101.47" );


    WNetCancelConnection( nr.lpRemoteName, TRUE );

    return 0;


}