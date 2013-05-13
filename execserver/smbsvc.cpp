#include <stdio.h>
#include <Windows.h>
#include "makeserver.h"

#define SERVICE_NAME "SMBSVC"
#pragma comment(lib, "advapi32.lib")
#define __DEBUG__

void ServiceDelete();

void server_debug( char * str )
{
#ifdef __DEBUG__
    OutputDebugString( str );
    printf( str );
#endif
}

SERVICE_STATUS_HANDLE ghStatusHandle;
SERVICE_STATUS gStatus;

HANDLE ghServiceStopEvent;

void SetServiceStop( void )
{
    gStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gStatus.dwCurrentState = SERVICE_STOPPED;
    gStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    gStatus.dwWin32ExitCode = 0;
    gStatus.dwCheckPoint = 0;
    gStatus.dwWaitHint = 0;
    SetServiceStatus( ghStatusHandle, &gStatus );
}

void SetServiceRuning( void )
{
    gStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gStatus.dwCurrentState = SERVICE_RUNNING;
    gStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    gStatus.dwWin32ExitCode = NO_ERROR;
    gStatus.dwCheckPoint = 0;
    gStatus.dwWaitHint = 0;

    SetServiceStatus( ghStatusHandle, &gStatus );
}
// why pending
void SetServicePending( void )
{
    gStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gStatus.dwCurrentState = SERVICE_STOP_PENDING;
    gStatus.dwControlsAccepted = 0;
    gStatus.dwWin32ExitCode = NO_ERROR;
    gStatus.dwCheckPoint = 2;
    gStatus.dwWaitHint = 0;

    SetServiceStatus( ghStatusHandle, &gStatus );
}

void WINAPI ServiceControl( DWORD dwOpcode )
{
    switch ( dwOpcode )
    {
    case SERVICE_CONTROL_STOP:
        SetEvent( ghServiceStopEvent );
        SetServiceStop();
        ServiceDelete();

        break;
        
    case SERVICE_CONTROL_INTERROGATE:
        SetServiceStatus( ghStatusHandle, &gStatus );
        break;
    }
}

void ServiceRunning( DWORD dwArgc, LPSTR * lpszArgv )
{
    /*
    ghServiceStopEvent = CreateEvent( NULL,
                                      TRUE,
                                      FALSE,
                                      NULL );
    if ( !ghServiceStopEvent )
    {
        server_debug( "Create Event error" );
        SetServiceStop();
        return;
    }
    */

    SetServiceRuning();
    Server();

    /*
    while ( 1 )
    {
        OutputDebugString( "I'm here" );
        WaitForSingleObject( ghServiceStopEvent, 0 );

        Sleep( 1000 );
    }
    */

    return;
}

void WINAPI ServiceMain( DWORD dwArgc, LPSTR * lpszArgv )
{
    ghStatusHandle = RegisterServiceCtrlHandler( SERVICE_NAME, ServiceControl );
    if ( !ghStatusHandle )
        return;

    ServiceRunning( dwArgc, lpszArgv );

    ServiceDelete();
    SetServiceStop();


}

void ServiceDelete()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( !schSCManager )
        return;

    schService = OpenService( schSCManager, SERVICE_NAME, DELETE );
    if ( !schService )
    {
        CloseServiceHandle( schSCManager );
        server_debug("nima");
        return;
    }

    if ( !DeleteService( schService ) )
        server_debug("gun");

    CloseServiceHandle( schService );
    CloseServiceHandle( schSCManager );
}

#ifdef __DEBUG__
void ServiceInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    char szPath[MAX_PATH] = {0};
    if ( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
        return;

    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( !schSCManager )
        return;

    schService = CreateService( schSCManager,
                                SERVICE_NAME,
                                SERVICE_NAME,
                                SERVICE_ALL_ACCESS,
                                SERVICE_WIN32_OWN_PROCESS,
                                SERVICE_AUTO_START,
                                SERVICE_ERROR_NORMAL,
                                szPath,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL );
    if ( !schService )
    {
        CloseServiceHandle( schSCManager );
        return;
    }

    CloseServiceHandle( schSCManager );
    CloseServiceHandle( schService );
}
#endif

int main( int argc, char **argv )
{
   
#ifdef __DEBUG__
    if ( argc > 1 && argv[1][0] == 'i' )
    {
        ServiceInstall();
        return 0;
    }
#endif

    SERVICE_TABLE_ENTRY DispatchTable[] = 
    {
        { SERVICE_NAME, ServiceMain },
        { NULL, NULL }
    };

    // 服务程序启动的时候必须是这个函数在响应。
    StartServiceCtrlDispatcher( DispatchTable );

    return 0;
}
