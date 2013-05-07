#include <Windows.h>
#include <WinNetWk.h>

#pragma comment(lib, "mpr.lib")

int main( int argc, char ** argv )
{
    NETRESOURCE nr = {0};

    DWORD dwRetVal = 0;

    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = NULL;
    nr.lpRemoteName = "\\\\10.20.60.25\\admin$";
    nr.lpProvider =  NULL;


    dwRetVal = WNetAddConnection3( NULL, &nr, "nsfocus", "administrator", 0 );
    if ( dwRetVal != NO_ERROR )
    {
        int a = GetLastError();
        return -1;
    }


    WNetCancelConnection( nr.lpRemoteName, TRUE );

    return 0;



}