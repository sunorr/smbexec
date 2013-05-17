#include <Windows.h>
#include <WinNetWk.h>
#include <string.h>
#include <stdio.h>
#include "makeclient.h"
#include "utilities.h"
#include "getopt.h"
#include "releasesource.h"
#include "resource.h"

#define SERVER_DIR "%WINDIR%\\execserver.exe"
#define SERVICE_NAME "SMBSVC"
#define ADMIN "admin$"
#define MAX_CMDLEN 8191

#pragma comment(lib, "mpr.lib")

#define __DEBUG__
#ifdef __DEBUG__
//#define REMOTE_COMP "192.168.79.129"
#define REMOTE_COMP "10.16.101.47"
//#define SERVER  "d:\\My Documents\\Project\\smbexec\\Debug\\execserver.exe"
#define SERVER  "..\\Debug\\execserver.exe"
//#define REMOTE_PATH1  "\\\\192.168.79.129\\admin$\\execserver.exe"
#define REMOTE_PATH1  "\\\\10.16.101.47\\admin$\\execserver.exe"
#endif




int InstallRemoteService( char *szComputerName )
{
    /*
#ifdef __DEBUG__
    char szRemotePath[MAX_PATH] = {0};
    sprintf( szRemotePath, "\\\\%s\\%s\\execserver.exe", szComputerName, ADMIN );
    if ( !CopyFile( SERVER, REMOTE_PATH1, FALSE ) )
    {
        if ( !CopyFile( "execserver.exe", REMOTE_PATH1, FALSE ) )
        {
            debug( "copy file failed\n" );
            return -1;
        }
    }
#endif 
    */

    char szRemotePath[MAX_PATH] = {0};
    sprintf( szRemotePath, "\\\\%s\\%s", szComputerName, ADMIN );
    ReleaseSource( IDR_EXE1, "execserver.exe", "EXE", szRemotePath );

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



void usage()
{
    printf( "" );
    exit( -1 );
}

int main( int argc, char ** argv )
{
    NETRESOURCE nr = {0};
    DWORD dwRetVal = 0;

    int c = 0;

    char host[32] = {0};

    LOGINFO LogInfo = {0};

    while ( ( c = getopt( argc, argv, "h:u:p:e:") ) != -1 )
    {
        switch ( c )
        {
        // 目标ip
        case 'h':
            strncpy( host, optarg, 32 );
        	break;

        // 用户名
        case 'u':
            strncpy( LogInfo.szUserName, optarg, NAME_LEN );
        	break;

        // 密码
        case 'p':
            strncpy( LogInfo.szPassword, optarg, PASSWD_LEN );
        	break;

        // cmd
        case 'e':
            strncpy( LogInfo.szExcuteCmd, optarg, CMD_LEN );
        	break;

        default:
            break;
        }
    }

    char szRemoteName[MAX_PATH] = {0};
    if ( host[0] )
        sprintf( szRemoteName, "\\\\%s\\%s", host, ADMIN );
    else
        usage();

    if ( !LogInfo.szUserName[0] || !LogInfo.szPassword[0] )
        usage();

    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = NULL;
    nr.lpRemoteName = szRemoteName;
    nr.lpProvider =  NULL;


    dwRetVal = WNetAddConnection3( NULL, &nr, LogInfo.szPassword, LogInfo.szUserName, 0 );
    if ( dwRetVal != NO_ERROR )
    {
        int a = GetLastError();
        return -1;
    }

    InstallRemoteService( host );

    Client( host, &LogInfo );

    WNetCancelConnection( nr.lpRemoteName, TRUE );

    return 0;


}