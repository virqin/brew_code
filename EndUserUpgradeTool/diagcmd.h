#ifndef DIAGCMD_H
#define DIAGCMD_H

/* Version Number Request/Response            */
#define DIAG_VERNO_F        0
/* Mobile Station ESN Request/Response        */
#define DIAG_ESN_F          1
/* DMSS status Request/Response               */
#define DIAG_STATUS_F       12 
/* Read NV item                               */
#define DIAG_NV_READ_F      38 
/* Write NV item                              */
#define DIAG_NV_WRITE_F     39 
/* Mode change request                        */
#define DIAG_CONTROL_F      41 
/* Switch to downloader                       */
#define DIAG_DLOAD_F        58 
/* Software Version Request/Response          */
#define DIAG_SW_VERSION_F   56
/* Software Extern Version Request/Response       */
#define DIAG_EXT_SW_VERSION_F   60
/*Communications Configuration Request Message*/
#define DIAG_COMMCONFIG_F   0x5c
/*Subsystem Dispatch Request Message*/
#define DIAG_SUBSYS_CMD_F   0x4B
/*SPC*/
#define DIAG_SPC_F          0x41
/* Clear User Info Request/Response           */
#define DIAG_DEL_USER_INFOR_F           135+2
/* Clear Call Timer Request/Response          */
#define DIAG_DEL_CALLTIME_F         135+3
/* Clear User Lock code Request/Response      */
#define DIAG_DEL_LOCKCODE_F         135+4

/* Software extend Request/Response           */
#define DIAG_SW_VERSION_EXTEND_F    0x7C

#define DIAG_PSW_NV_READ_F      146  //ruiju add 091120 //F350备份保密参数 使用扩展的Diag指令0x92读取

/* Write a block of data to memory            */
#define DLOAD_WRITE_F       1
/* Acknowledge receiving a packet             */
#define DLOAD_ACK_F         2
/* Acknowledge a bad packet                   */   
#define DLOAD_NAK_F         3
/* Erase a block of memory                    */   
#define DLOAD_ERASE_F       4
/* Begin execution at an address              */   
#define DLOAD_GO_F          5   
/* No operation, for debug                    */
#define DLOAD_NOP_F         6
/* Request implementation info                */
#define DLOAD_PREQ_F        7
/* Provide implementation info                */
#define DLOAD_PARAMS_F      8
/* Debug: dump a block of memory              */
#define DLOAD_DUMP_F        9
/* Reset the phone                            */   
#define DLOAD_RESET_F       10
/* Unlock access to secured ops               */
#define DLOAD_UNLOCK_F      11
/* Request software version info              */
#define DLOAD_VERREQ_F      12
/* Provide software version info              */
#define DLOAD_VERRSP_F      13
/* Turn phone power off                       */
#define DLOAD_PWROFF_F      14
/* Write with 32 bit address      ::add by xjh */
#define DLOAD_WRITE32_F     15
/* Go command for repair version   #define DLOAD_GO_REP_F   15 */
#define DLOAD_GO_REP_F      20
/* Change uart rate for high-speed download   */
#define DLOAD_CHANGE_RATE_F 16
/* Boot hello cmd*/
#define DLOAD_BOOT_F        85
/*--------------------------------------------------------------------*/
//2004年5月27日由刘晓军添加
/*  用于向锁网软件版本发送口令 */
#define DLOAD_CHECKPASS_F    17
/*  用于发送命令请求，要求手机返回版本号和锁网定制号 */
#define DLOAD_CHECKVER_F     18
/*  手机返回的版本号和锁网定制号所带的命令字 */
#define DLOAD_CHECKVER_RET_F 19
/*--------------------------------------------------------------------*/


#define SDP_HELLO_F     0x01
#define SDP_HELLORSP_F  0x02
#define SDP_READ_F      0x03
#define SDP_READRSP_F   0x04
#define SDP_SWRITE_F    0x05
#define SDP_SWRITERSP_F 0x06
#define SDP_WRITE_F     0x07
#define SDP_WRITERSP_F  0x08
#define SDP_NOP_F       0x09
#define SDP_NOPRSP_F    0x0A
#define SDP_RESET_F     0x0B
#define SDP_RESETRSP_F  0x0C
#define SDP_ERROR_F     0x0D
#define SDP_LOG_F       0x0E
#define SDP_UNLOCK_F    0x0F
#define SDP_UNLOCKRSP_F 0x10
#define SDP_POWER_F     0x11
#define SDP_POWERRSP_F  0x12
#define SDP_OPEN_F      0x13
#define SDP_OPENRSP_F   0x14
#define SDP_CLOSE_F     0x15
#define SDP_CLOSERSP_F  0x16
//lichenyang 20060318
#define SDP_SECURITY_F  0x17
#define SDP_SECURITYRSP_F   0x18
#define SDP_PARTITION_F 0x19
#define SDP_PATITIONRSP_F   0x1A
#define SDP_OPENMULTI_F 0x1B
#define SDP_OPENMULTIRSP_F  0x1C
#define SDP_ERASER_F    0x1D
#define SDP_ERASERRSP_F 0x1E
//lichenyang 20060318
#define SDP_HSDL_F      0xB0
#define SDP_HSDL_F_NEW 0X28
#define SDP_HSDL_RSP_F_NEW 0X29

/* PRL Read Request/Response */
#define DIAG_PRL_READ_F          73 
/* PRL Write Request/Response */
#define DIAG_PRL_WRITE_F         72 
/* Performs an Embedded File System
   (EFS) operation.                           */
#define DIAG_FS_OP_F        89 


typedef enum
{
    DIAG_SUBSYS_OEM                                         = 0,
    /* Reserved for OEM use */
    DIAG_SUBSYS_ZREX                                        = 1,
    /* ZREX */
    DIAG_SUBSYS_SD                                          = 2,
    /* System Determination */
    DIAG_SUBSYS_BT                                          = 3,
    /* Bluetooth */
    DIAG_SUBSYS_WCDMA                                       = 4,
    /* WCDMA */
    DIAG_SUBSYS_HDR                                         = 5,
    /* 1xEvDO */
    DIAG_SUBSYS_DIABLO                                      = 6,
    /* DIABLO */
    DIAG_SUBSYS_TREX                                        = 7,
    /* TREX - Off-target testing environments */
    DIAG_SUBSYS_GSM                                         = 8,
    /* GSM */
    DIAG_SUBSYS_UMTS                                        = 9,
    /* UMTS */
    DIAG_SUBSYS_HWTC                                        = 10,
    /* HWTC */
    DIAG_SUBSYS_FTM                                         = 11,
    /* Factory Test Mode */
    DIAG_SUBSYS_REX                                         = 12,
    /* Rex */
    DIAG_SUBSYS_OS                                          = DIAG_SUBSYS_REX,
    DIAG_SUBSYS_GPS                                         = 13,
    /* Global Positioning System */
    DIAG_SUBSYS_WMS                                         = 14,
    /* Wireless Messaging Service (WMS, SMS) */
    DIAG_SUBSYS_CM                                          = 15,
    /* Call Manager */
    DIAG_SUBSYS_HS                                          = 16,
    /* Handset */
    DIAG_SUBSYS_AUDIO_SETTINGS                              = 17,
    /* Audio Settings */
    DIAG_SUBSYS_DIAG_SERV                                   = 18,
    /* DIAG Services */
    DIAG_SUBSYS_FS                                          = 19,
    /* File System - EFS2 */
    DIAG_SUBSYS_PORT_MAP_SETTINGS                           = 20,
    /* Port Map Settings */
    DIAG_SUBSYS_MEDIAPLAYER                                 = 21,
    /* QCT Mediaplayer */
    DIAG_SUBSYS_QCAMERA                                     = 22,
    /* QCT QCamera */
    DIAG_SUBSYS_MOBIMON                                     = 23,
    /* QCT MobiMon */
    DIAG_SUBSYS_GUNIMON                                     = 24,
    /* QCT GuniMon */
    DIAG_SUBSYS_LSM                                         = 25,
    /* Location Services Manager */
    DIAG_SUBSYS_QCAMCORDER                                  = 26,
    /* QCT QCamcorder */
    DIAG_SUBSYS_MUX1X                                       = 27,
    /* Multiplexer */
    DIAG_SUBSYS_DATA1X                                      = 28,
    /* Data */
    DIAG_SUBSYS_SRCH1X                                      = 29,
    /* Searcher */
    DIAG_SUBSYS_CALLP1X                                     = 30,
    /* Call Processor */
    DIAG_SUBSYS_APPS                                        = 31,
    /* Applications */
    DIAG_SUBSYS_SETTINGS                                    = 32,
    /* Settings */
    DIAG_SUBSYS_GSDI                                        = 33,
    /* Generic SIM Driver Interface */
    DIAG_SUBSYS_TMC                                         = 34,
    /* Task Main Controller */
    DIAG_SUBSYS_USB                                         = 35,
    DIAG_SUBSYS_PM                                          = 36,
    DIAG_SUBSYS_DEBUG                                       = 37,
    DIAG_SUBSYS_QTV                                         = 38,
    DIAG_SUBSYS_CLKRGM                                      = 39,
    /* Clock Regime */
    DIAG_SUBSYS_DEVICES                                     = 40,
    DIAG_SUBSYS_WLAN                                        = 41,
    /* 802.11 Technology */
    DIAG_SUBSYS_PS_DATA_LOGGING                             = 42,
    /* Data Path Logging */
    DIAG_SUBSYS_PS                                          = DIAG_SUBSYS_PS_DATA_LOGGING,
    DIAG_SUBSYS_MFLO                                        = 43,
    DIAG_SUBSYS_DTV                                         = 44,
    /* Digital TV */
    DIAG_SUBSYS_RRC                                         = 45,
    /* WCDMA Radio Resource Control state */
    DIAG_SUBSYS_PROF                                        = 46,
    /* Miscellaneous Profiling Related */
    DIAG_SUBSYS_TCXOMGR                                     = 47,
    DIAG_SUBSYS_NV                                          = 48,
    DIAG_SUBSYS_AUTOCONFIG                                  = 49,
    DIAG_SUBSYS_PARAMS                                      = 50,
    /* Parameters required for debugging subsystems */
    DIAG_SUBSYS_MDDI                                        = 51,
    /* Mobile Display Digital Interface */
    DIAG_SUBSYS_DS_ATCOP                                    = 52,
    DIAG_SUBSYS_L4LINUX                                     = 53,
    /* L4/Linux */
    DIAG_SUBSYS_MVS                                         = 54,
    /* Multimode Voice Services */
    DIAG_SUBSYS_CNV                                         = 55,
    /* Compact NV */
    DIAG_SUBSYS_APIONE_PROGRAM                              = 56,
    /* apiOne */
    DIAG_SUBSYS_HIT                                         = 57,
    /* Hardware Integration Test */
    DIAG_SUBSYS_DRM                                         = 58,
    /* Digital Rights Management */
    DIAG_SUBSYS_DM                                          = 59,
    /* Device Management */
    DIAG_SUBSYS_UTS_DRIVER_CDMA_OP                          = 67,
    DIAG_SUBSYS_FC                                          = 60,
    /* Flow Controller */
    DIAG_SUBSYS_LAST,
    /* Subsystem IDs reserved for OEM use */
    DIAG_SUBSYS_RESERVED_OEM_0                              = 250,
    DIAG_SUBSYS_RESERVED_OEM_1                              = 251,
    DIAG_SUBSYS_RESERVED_OEM_2                              = 252,
    DIAG_SUBSYS_RESERVED_OEM_3                              = 253,
    DIAG_SUBSYS_RESERVED_OEM_4                              = 254,
    DIAG_SUBSYS_RESERVED_OEM_5                              = 255  /*pc-lint 726 taojie*/
}    diagpkt_subsys_cmd_enum_type;

#endif 

