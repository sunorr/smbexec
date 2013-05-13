#include <Windows.h>
#include <WinNetWk.h>
#include <string.h>
#include <stdio.h>
#include "makeclient.h"
#include "utilities.h"

#define SERVER_DIR "%WINDIR%\\execserver.exe"
#define SERVICE_NAME "SMBSVC"

#pragma comment(lib, "mpr.lib")

#define __DEBUG__
#ifdef __DEBUG__
#define REMOTE_COMP "192.168.79.129"
#define SERVER  "d:\\My Documents\\Project\\smbexec\\Debug\\execserver.exe"
#define REMOTE_PATH1  "\\\\192.168.79.129\\admin$\\execserver.exe"
#endif




int InstallRemoteService( char *szComputerName )
{
#ifdef __DEBUG__
    if ( !CopyFile( SERVER, REMOTE_PATH1, FALSE ) )
    {
        debug( "copy file failed\n" );
        return -1;
    }
#endif 

    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager( szComputerName, NULL, SC_MANAGER_ALL_ACCESS );
    if ( !schSCManager )
    {
        debug( "open service manager failed: %d\n", GetLastError() );
        return -1;
    }


    schService = CreateService( schSCManager,
        SERVICE_NAME,
        SERVICE_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        SERVER_DIR,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL );
    if ( !schService )
    {
        if ( GetLastError() != ERROR_SERVICE_EXISTS )
        {
            debug( "create service failed: %d\n", GetLastError() );
            CloseServiceHandle( schSCManager );
            return -1;
        }
    }

    schService = OpenService( schSCManager, SERVICE_NAME, GENERIC_ALL );
    if ( !schService )
    {
        debug( "open service failed: %d\n", GetLastError() );
        CloseServiceHandle( schSCManager );
        return -1;
    }

    StartService( schService, 0, NULL );

    CloseServiceHandle( schSCManager );
    CloseServiceHandle( schService );

    return 0;
}


int main( int argc, char ** argv )
{
    NETRESOURCE nr = {0};

    DWORD dwRetVal = 0;

    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = NULL;
    //nr.lpRemoteName = "\\\\10.16.101.47\\admin$";
    nr.lpRemoteName = "\\\\192.168.79.129\\admin$";
    nr.lpProvider =  NULL;


    //dwRetVal = WNetAddConnection3( NULL, &nr, "123qwe!@#", "administrator", 0 );
    dwRetVal = WNetAddConnection3( NULL, &nr, "123qwe", "administrator", 0 );
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

    InstallRemoteService( REMOTE_COMP );

    Client( REMOTE_COMP );


    WNetCancelConnection( nr.lpRemoteName, TRUE );

    return 0;


}