
#include <Windows.h>
#include <stdio.h>

BOOL ReleaseSource( DWORD SourcID, TCHAR *szSourcName, TCHAR *szGroup, TCHAR *szChangePath )
{
	HRSRC hRsrc;
	hRsrc = FindResource( NULL, MAKEINTRESOURCE( SourcID ), szGroup );

	if ( hRsrc == NULL )
	{
		printf( "FindResource error: %d\n", GetLastError() );
		return FALSE;
	}

	DWORD dSize = SizeofResource( NULL, hRsrc );
	if ( dSize == 0 )
	{
		printf( "SizeofResource error: %d\n", GetLastError() );
		return FALSE;
	}

	HGLOBAL hGlobal = LoadResource( NULL, hRsrc );
	if ( hGlobal == NULL )
	{
		printf( "LoadResource error: %d\n", GetLastError() );
		return FALSE;
	}

	VOID *pBuffer = LockResource( hGlobal );
	if ( pBuffer == NULL )
	{
		printf( "LockResource error: %d\n", GetLastError() );
		return FALSE;
	}

	TCHAR szCurrentPath[1024];
	GetCurrentDirectory( sizeof(szCurrentPath), szCurrentPath );

	if ( szChangePath != NULL )
	{
		if ( !SetCurrentDirectory( szChangePath ) )
		{
			printf( "SetCurrentDirectroy error: %d\n", GetLastError() );
		}
	}

	FILE *fp = fopen( szSourcName, TEXT("wb") );
	if ( fp == NULL )
	{
		printf( "OpenFile error!\n" );
		return FALSE;
	}

//	GetCurrentDirectory( sizeof(szCurrentPath), szCurrentPath );
	if ( dSize == fwrite( pBuffer, 1, dSize, fp ) )
	{
		fclose( fp );
		
		DWORD a = sizeof(pBuffer);		
		SetCurrentDirectory( szCurrentPath );
		return TRUE;
	}
	else
	{
		fclose( fp );
		SetCurrentDirectory( szCurrentPath );
		return FALSE;
	}
}