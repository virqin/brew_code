// Serial.h

#include "LogRecord.h"

#ifndef __SERIAL_H__
#define __SERIAL_H__

class CSerial
{
public:
            CSerial();
            ~CSerial();

    BOOL    Open(int nPort = 1, int nBaud = 115200, int nDataBit = 8, int nStopBit = 1);
    BOOL    Close(void);

    int     ReadData(void *, int);
    int     SendData(const char *, int);
    int     WriteCommBytes(const char *, int);
    int     DLWriteCommBytes(const char *ucBytes, int length);
    int     ReadDataWaiting(void);

    int     m_nPort;
    BOOL IsOpened(void)
    {
        return(m_bOpened);
    }
    CLogRecord  m_log;

protected:
    BOOL        WriteCommByte(char);
    HANDLE      m_hIDComDev;
    OVERLAPPED  m_OverlappedRead, m_OverlappedWrite;
    BOOL        m_bOpened;
};

#endif
