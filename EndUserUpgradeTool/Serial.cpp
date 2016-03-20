// Serial.cpp

#include "stdafx.h"
#include "Serial.h"

CSerial::CSerial()
{
	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
}

CSerial::~CSerial()
{
	Close();
}

BOOL CSerial::Open( int nPort, int nBaud, int nDataBit, int nStopBit )
{
	if( m_bOpened ) return( TRUE );

	char szPort[15];
	char szComParams[50];
	DCB dcb;

	wsprintf( szPort, "\\\\.\\COM%d", nPort );
	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
	if( m_hIDComDev == INVALID_HANDLE_VALUE ) return( FALSE );

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = -1;
	CommTimeOuts.ReadTotalTimeoutMultiplier = -1;
	CommTimeOuts.ReadTotalTimeoutConstant = 30000;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 1;
	CommTimeOuts.WriteTotalTimeoutConstant = 50;

	wsprintf( szComParams, "COM%d: %d, n, %d, %d", nPort, nBaud, nDataBit, nStopBit);

	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );
	BuildCommDCB( szComParams, &dcb ); 
	if( !SetCommState( m_hIDComDev, &dcb ) ||
		!SetupComm( m_hIDComDev, 0x1000, 0x1000 ) ||
		!SetCommMask( m_hIDComDev, EV_BREAK|EV_CTS|EV_DSR|EV_RLSD ) ||
		!SetCommTimeouts( m_hIDComDev, &CommTimeOuts ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL ){
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return( FALSE );
	}

	m_bOpened = TRUE;
	PurgeComm( m_hIDComDev, PURGE_TXABORT | PURGE_RXABORT |
                           PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	m_nPort = nPort;	// ¼ÇÂ¼¶Ë¿ÚºÅ

	m_log.OpenPort(nPort,TRUE);
	return( m_bOpened );
}

BOOL CSerial::Close( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( TRUE );

	PurgeComm( m_hIDComDev, PURGE_TXABORT | PURGE_RXABORT |
                           PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
	if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	m_log.OpenPort(m_nPort,FALSE);	
	return( TRUE );
}

BOOL CSerial::WriteCommByte( char ucByte )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;
	
	PurgeComm(m_hIDComDev, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) 
			dwBytesWritten = 0;
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return( TRUE );
}

int CSerial::WriteCommBytes(const char * ucBytes, int length )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	PurgeComm(m_hIDComDev, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) ucBytes, length, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 10000 ) ) 
			dwBytesWritten = 0;
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	m_log.WriteLog((void*)ucBytes,length,TRUE);	
	return( dwBytesWritten );
}
int CSerial::DLWriteCommBytes(const char * ucBytes, int length )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;
	
//	PurgeComm(m_hIDComDev, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR | PURGE_TXABORT);

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) ucBytes, length, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 10000 ) ) 
			dwBytesWritten = 0;
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	m_log.WriteLog((void*)ucBytes,length,TRUE);	
	return( dwBytesWritten );
}
int CSerial::SendData( const char *buffer, int size )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwBytesWritten = 0;

	for( int i=0; i<size; i++ )
	{
		WriteCommByte( buffer[i] );
		dwBytesWritten++;
	}

	return( (int) dwBytesWritten );
}
                                 
int CSerial::ReadDataWaiting( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	return( (int) ComStat.cbInQue );
}

int CSerial::ReadData( void *buffer, int limit )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if( !ComStat.cbInQue ) return( 0 );

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( limit < (int) dwBytesRead ) dwBytesRead = (DWORD) limit;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead );
	if( !bReadStatus )
	{
		if( GetLastError() == ERROR_IO_PENDING )
		{
			WaitForSingleObject( m_OverlappedRead.hEvent, 2000 );
			return( (int) dwBytesRead );
		}
		return( 0 );
	}

	m_log.WriteLog(buffer,dwBytesRead,FALSE);	
	return( (int) dwBytesRead );
}
