// Packet.h: interface for the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKET_H__0E289BC1_1EBD_4CFD_B3C7_8698569F8B6D__INCLUDED_)
#define AFX_PACKET_H__0E289BC1_1EBD_4CFD_B3C7_8698569F8B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serial.h"
#include "Diagpkt.h"
#include "Diagcmd.h"
#include "CRC.h"
#include "flashdef.h"

class CPacket:public CSerial
{
public:
                            CPacket();
    virtual                 ~CPacket();

    //Attribute Valiable
public:
    byte                    plen[4];
    byte                    CRC[2]; 
    BOOL                    TransHSDLstartCommand(int length, byte CRC1, byte CRC2);
    BOOL                    MakeCRC2bytes(char FilePath[], byte &CRC1, byte &CRC2);
    bool                    waitsign;//lichenyang 200611
    StreamWrite_ret_type    m_retPacket;
    diag_req_pkt_type       m_ReqPacket;
    diag_rsp_pkt_type       m_RspPacket;
    tx_pkt_buf_type         m_TxPacket;
    rx_pkt_buf_type         m_RxPacket;
    CString                 m_strErrInfo;
    CString                 m_strTempInfo;
    protocol_type           m_Protocol;
    byte                    Buf[1024];//lichenyang 20060318
    int                     nLen;//lichenyang 20060318
    byte                    m_bOverridePtl;

    //operation
public:
    //////////////////////////////////////////////////////////////////////////
	BOOL					TransHSDLstartCommandNew(int length);
    BOOL                    NvItemReadPSW(word item, nv_item_type &item_data, word &status);
   	int						MakeCheckSum(char FilePath[], dword offset, dword length);

	//efs2  xiezhuo add
    BOOL                    EFS2ReadDir(char *buf, int nSeqNo, efs2_diag_readdir_rsp_type &rData);
    BOOL                    EFS2Rename(CString strOld, CString strNew);
    uint32                  m_pEFS2DirectoryPoint;
    BOOL                    EFS2CloseDir();
    BOOL                    EFS2CloseDir(int32 nDir);
    BOOL                    EFS2OpenDir(CString strDir);
    BOOL                    EFS2RmDir(CString strPath);
    BOOL                    EFS2MkDir(CString strPath);
    BOOL                    EFS2Query(diag_sub_efs2_rsp_type &queryType);
    BOOL                    EFS2FileClose();
    BOOL                    EFS2FileWrite(byte *buf, int nLen, int nOffset);    
    BOOL                    EFS2FileRead(byte *buf, int &nLen, int nOffset);
    BOOL                    EFS2FileOpen(CString strPath, int openMode);
    BOOL                    EFS2Hello(diag_sub_efs2_req_type helloType);
    BOOL                    DivideSubsysPacket();
    word                    CombineSubsysPacket();


    BOOL                    PRLwrite(LPCTSTR lpszFileName, BOOL &NAM);
    //////////////////////////////////////////////////////////////////////////
    BOOL                    GetSWExtend(char retSW[]);
    BOOL                    Eraser();
    int                     DDP0x7C(); //chenjie 20071127
    BOOL                    OpenMulti(byte type, byte *writeBuf, int nTotalLen);
    BOOL                    partition(byte *writeBuf, int nTotalLen);
    BOOL                    Security();
    BOOL                    IniFlexfile();
    BOOL                    HandWithMobile(byte Imeibypri[128]);
    BOOL                    ReadIMEI(byte ImeiNVbyte[15]);
    BOOL                    Verify_Download_Security(byte *input);
    BOOL                    EnterDMSS();
    CString                 EFSOperError(byte status);
    BOOL                    DivideFSPacket();
    word                    CombineFSPacket();
    BOOL                    WriteFileSequenceBlock(byte seqnum,
                                                   byte more,
                                                   diag_fs_data_block_type seqblock,
                                                   diag_fs_write_rsp_type &fs_write,
                                                   byte &status);
    BOOL                    WriteFileFirstBlock(byte more,
                                                diag_fs_write_begin_req_type beginblock,
                                                diag_fs_write_rsp_type &fs_write,
                                                byte &status);
    BOOL                    ReadFileSequenceBlock(byte seqnum, diag_fs_read_rsp_type &fs_read);
    BOOL                    ReadFileFirstBlock(diag_fs_filename_type filename, diag_fs_read_rsp_type &fs_read);
    BOOL                    ReadSpaceAvailiable(dword &space);
    BOOL                    RemoteFileLink(dword address, dword size, diag_fs_filename_type filename);
    BOOL                    SetFileAttributes(diag_fs_filename_type filename,
                                              diag_fs_attributes_type attr,
                                              dword cdate);
    BOOL                    GetFileAttributes(diag_fs_filename_type filename,
                                              diag_fs_get_attrib_rsp_type &fs_get_attrib);
    BOOL                    RemoveDir(diag_fs_filename_type filename);
    BOOL                    CreateDirectory(diag_fs_filename_type filename);
    BOOL                    RemoveFile(diag_fs_filename_type filename);
    BOOL                    IterativeFileList(int seqnum,
                                              diag_fs_filename_type filename,
                                              diag_fs_iter_rsp_type &fs_iter,
                                              BOOL &bDone);
    BOOL                    IterativeDirList(int seqnum,
                                             diag_fs_filename_type filename,
                                             diag_fs_iter_rsp_type &fs_iter,
                                             BOOL &bDone);
    BOOL                    DisplayFileList(diag_fs_filename_type filename,
                                            diag_fs_disp_files_rsp_type &fs_disp_files);
    BOOL                    DisplayDirList(diag_fs_filename_type filename,
                                           diag_fs_disp_dirs_rsp_type &fs_disp_dirs);
    void                    SleepMe();
    void                    DLCombinePacket(const byte *blockbuf, int address, int len);
    BOOL                    DLRxTxPacket(dword wait_ms);
    BOOL                    StreamWriteAsy(const byte *blockbuf, int address, int length);
    BOOL                    CheckPacketVal(word start, word end);
    BOOL                    DLDividePacket();
    BOOL                    DLRxPacket();
    BOOL                    DLTxPacket();
    BOOL                    LogMe(const byte *buf, int len, bool bWrite);
    void                    LogCls();
    BOOL                    CombinePacketDLP();
    BOOL                    CombinePacketDMP();
    BOOL                    CombinePacketSDP();
    BOOL                    DividePacketDLP();
    BOOL                    DividePacketDMP();
    BOOL                    DividePacketSDP();
    BOOL                    Hello();
    BOOL                    OpenPacket(int mode);
    BOOL                    ClosePacket();
    BOOL                    SimpleWrite(int address, int length);
    BOOL                    StreamWrite(const byte *blockbuf, int address, int length);
    BOOL                    Nop();
    BOOL                    ResetSDP();
    BOOL                    Down();
    BOOL                    Write32(const byte *blockbuf, int address, int length);
    BOOL                    ChangeUartRate(int baudrate);
    BOOL                    Write(const byte *blockbuf, dword address, dword length);
    BOOL                    Erase(dword address, dword length);
    BOOL                    Go_Rep(word segment, word offset);
    BOOL                    Go(word segment, word offset);
    BOOL                    ParamRequest(dload_params_rsp_type &params);
    BOOL                    ResetDLP();
    BOOL                    PowerDown();
    BOOL                    Unlock(const byte *seccode);
    BOOL                    SwitchDownloader();
    BOOL                    DoNoop();
    BOOL                    NvItemRead(word item, nv_item_type &item_data, word &status);
    BOOL                    NvItemWrite(word item, nv_item_type item_data, word &status);
    BOOL                    AT_QCDMG();
    BOOL                    AT_DLOAD();
    CString GetErrInfo() const
    {
        return m_strErrInfo;
    }
    void SetErrInfo(CString strErrInfo)
    {
        m_strErrInfo = strErrInfo;
    }
    int GetPacketProtocol() const
    {
        return m_Protocol;
    }

    BOOL    CommConfig();
    BOOL    SendSpc(CString spc);

    void    CheckSendPassWord() ; 
    BOOL    CheckSoftVer() ; 

    ////////////////////////////////////////////////////////////
    //DMSS functions
    BOOL    CommunicateWithMobile();
    BOOL    GetEsnNum(dword &esn);
    BOOL    EnterOfflineMode(void);
    BOOL    ResetDMP(void);
    BOOL    GetMobileStatus(diag_status_rsp_type &status);
    BOOL    GetSoftwareVersion(diag_sw_ver_rsp_type &version);
    BOOL    GetExtSoftwareVersion(diag_sw_ver_rsp_type &version);

    BOOL    DmssCommand(byte dmss_ID);
    BOOL    GetConnectState();
    BOOL    GetSoftVersion(void);

    //other functions
    CString MapNVOperateError(nv_stat_enum_type stat);
    CString MapDloadError(word reason);

    ///////////////////////////////////////////////////////////
    //CPacket functions
    void    EmptyVaiables();
    dword   CombinePacket();
    BOOL    DividePacket(); 
    BOOL    RxTxPacket(byte retry = 1, dword wait_ms = 2000);
    dword   PacketCRCMake(byte *buffer, dword PacketLen);
    BOOL    PacketCRCCheck(byte *buffer, word PacketLen);
};

#endif // !defined(AFX_PACKET_H__0E289BC1_1EBD_4CFD_B3C7_8698569F8B6D__INCLUDED_)
