#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#define PIPE_NAME_STDIN "pipe\\SMBEXEC"
#define PIPE_NAME_STDOUT "pipe\\SMBEXEC2"
#define REMOTE "admin$"
#define LOCAL "%windir%"
#define BUFFERSIZE 1024
#define WAIT_PIPE_TIME 20000


int PipeRecv( HANDLE hPipe, char * szBuffer, DWORD dwSize );
int PipeSend( HANDLE hPipe, char * szBuffer, DWORD dwSize );
void ClosePipe( HANDLE hPipe );
HANDLE CreateXPipe( char * szPipeName );
HANDLE PipeConnect( char * szPipeName );

#define NAME_LEN 32
#define PASSWD_LEN 32
#define CMD_LEN 1024

typedef struct __LOGINFO__
{
    char szUserName[NAME_LEN];
    char szPassword[PASSWD_LEN];
    char szExcuteCmd[CMD_LEN];
}LOGINFO, *PLOGINFO;

#endif
