#ifndef DIAGPKT_H
#define DIAGPKT_H

#include "comdef.h"
#include "nv.h"
#include "subItem.h"

#define PRL_WRITE_BLOCK_SIZE   120  
#define DIAG_NV_ITEM_SIZE 128
//#define SRAM_WRITE_BLOCK_SIZE (0x1000)
//#define SRAM_WRITE_BLOCK_SIZE (0x03F9)
#define SRAM_WRITE_BLOCK_SIZE   (0x0400)
//#define FLASH_WRITE_BLOCK_SIZE    (0x4000)
#define FLASH_WRITE_BLOCK_SIZE  (0x0400)

#define HS_TRANSMIT_BLOCK_SIZE  0x010000
#define HS_RESPONSE_BLOCK_SIZE  0x020000
#define HS_DEVICE_MEMORY_SIZE   0x020000
#define HS_TRANSMIT_BLOCK_SIZE_NEW  0x020000

//#define DIAG_MAX_RX_PKT_SIZ (DIAG_NV_ITEM_SIZE * 2)   change by xjh
#define DIAG_MAX_RX_PKT_SIZ (SRAM_WRITE_BLOCK_SIZE * 2)
//#define DIAG_MAX_RX_PKT_SIZ (1024)
//#define DIAG_MAX_TX_PKT_SIZ (FLASH_WRITE_BLOCK_SIZE * 2)
#define DIAG_MAX_TX_PKT_SIZ (FLASH_WRITE_BLOCK_SIZE * 2)

#define DIAG_OFFLINE_DIGITAL 2  
#define DIAG_OFFLINE_ANALOG  6
#define VERSION_SIZE        60  
//版本的编译日期、时间、长度。ZTE自己定义的 
#define COMPI_DATE_STRLEN   12
#define COMPI_TIME_STRLEN   9
#define SW_VERSION_STRLEN   31//21//lichenyang 20080228
#define DIAG_CHECKPASS_SIZE 4

#define VERNO_DATE_STRLEN 12
#define VERNO_TIME_STRLEN  9
#define VERNO_DIR_STRLEN   17
#define VERNO_DIR_STRLEN2   31 //modify by lujuan from 21 to 31 20051224

typedef enum
{ 
	ARMPRG_QDLD_RSP_BUF_CHECK = 0,
	ARMPRG_QDLD_RSP_BUF_HANDSHAKE =1,
	ARMPRG_QDLD_RSP_NUM = 2
}armprg_qdld_rsp_type;

typedef enum
{
	ARMPRG_QDLD_RET_FAIL = 0,
	ARMPRG_QDLD_RET_SUCCESS = 1,
	ARMPRG_QDLD_RET_NUM = 2
}armprg_qdld_ret_type;

typedef  struct
{
    byte    cmd_code;                      
    char    comp_date[VERNO_DATE_STRLEN];
    char    comp_time[VERNO_TIME_STRLEN];
    char    rel_date[VERNO_DATE_STRLEN];
    char    rel_time[VERNO_TIME_STRLEN];
    char    ver_dir[VERNO_DIR_STRLEN]; 
    char    ver_dir2[VERNO_DIR_STRLEN2];   //彭军民自加
    byte    scm;                           
    byte    mob_cai_rev;                   
    byte    mob_model;                     
    word    mob_firm_rev;                  
    byte    slot_cycle_index;              
    byte    hw_maj_ver;                    
    byte    hw_min_ver;
} diag_verno_rsp_type;

typedef struct
{
    byte    cmd_code;
    byte    b_flexfile;
}diag_clearAll_req_type;        // clear user info

typedef struct
{
    byte    cmd_code;
    byte    errcode;
    //   都成功错误代码             errocode 0000 0000
    //   多媒体失败动态错误代码     errocode 0000 0001
    //   清除JAVA应用               errocode 0000 0010
    //   清除通话记录               errocode 0000 0100
    //   清除短信                   errocode 0000 1000
    //   清除电话本                 errocode 0001 0000
    //   清除记事本                 errocode 0010 0000
} diag_clearAll_rsp_type;       // clear user info

typedef struct
{
    byte    cmd_code ;                  /* 定死为75 */
    byte    sub_sys_id ;                 /* 定死为60 */
    byte    subsys_cmd_code[2];         /* .........*/  
    byte    IMEI_pri_en[128];  /* IMEI 号的已加密数据 */
}diag_sec_hand_req_type;

typedef struct
{
    byte    cmd_code ;                  /* 定死为75 */
    byte    sub_sys_id ;                 /* 定死为60 */
    byte    subsys_cmd_code[2];         /* .........*/
    boolean b_handsuccess;                      /*   是否握手成功 */
}diag_sec_hand_rsp_type;   //返回数据

// download permit verify struct start
typedef struct
{
    uint8   command_code;
    uint8   subsys_id;
    uint16  subsys_cmd_code;
} diag_download_sec_type;

typedef struct
{
    diag_download_sec_type  hdr;
    byte                    sp_dload_ver_pri_en[128];   // 私钥加密的软件下载版本
} diag_sec_dload_chk_req_type;                          // req

typedef struct
{
    diag_download_sec_type  hdr;
    int32                   errno;                      // 错误代码
} diag_sec_dload_chk_rsp_type;                          // rsp

// download permit verify struct end

typedef struct
{
    byte    len;
    int     ret[64];
}StreamWrite_ret_type;
//请求包格式的定义
typedef  struct
{
    byte    cmd_code;
} diag_esn_req_type;

typedef  struct
{
    byte    cmd_code;
} diag_status_req_type;


typedef struct
{
    byte            cmd_code;                     /* Command code */
    word            item;           /* Which item - use nv_items_enum_type  */
    nv_item_type    item_data;           /* Item itself - use nv_item_type      */
    word            nv_stat;        /* Status of operation
                                                         - use nv_stat_enum_type */
} diag_nv_read_req_type;

typedef struct
{
    byte            cmd_code;                     /* Command code */
    word            item;           /* Which item - use nv_items_enum_type  */
    nv_item_type    item_data;           /* Item itself - use nv_item_type      */
    word            nv_stat;        /* Status of operation
                                                         - use nv_stat_enum_type */
} diag_nv_write_req_type;

typedef struct
{
    byte    cmd_code                  ;   /*--------------------*/
    byte    seq_num                   ;  
    byte    more                      ;  
    byte    nam                       ;     //added by liuxiaojun,2004,6,11
    byte    num_bits[2]               ;
    byte    buf[PRL_WRITE_BLOCK_SIZE] ;
}diag_PRLWRITE_req_type;           /*--------------------------------*/

typedef struct                     /*--------------------------------------*/
{
    byte    cmd_code;
    byte    seq_num ;                     //added by liuxiaojun, 2004,6,11
    byte    nam     ;
}diag_PRLREAD_req_type;           /*---------------------------------------*/



typedef enum
{
    MODE_OFFLINE_A_F,
    MODE_OFFLINE_D_F,
    MODE_RESET_F,
    MODE_MAX_F
}    diag_mode_enum_type;

typedef  struct
{
    byte    cmd_code;
    word    mode;
} diag_control_req_type;

typedef  struct
{
    byte    cmd_code;
} diag_dload_req_type;

typedef struct
{
    byte    cmd_code;
} diag_sw_ver_req_type;

typedef struct
{
    byte    cmd_code;
    byte    mode;
} diag_sub_cmd_req_type;

typedef struct
{
    byte    cmd_code;
}diag_dloader_req_type;

/*--------------------------------------------------------*/

typedef struct
{
    byte    cmd_code;
    byte    checkpassword[DIAG_CHECKPASS_SIZE];
}diag_checkpassword_req_type;

typedef struct
{
    byte    cmd_code;
}diag_checksoftver_req_type;
/*---------------------------------------------------------*/
//2004年5月27日由刘晓军添加


typedef struct
{
    byte    cmd_code;
    byte    address[3];
    byte    length[3];
}dload_erase_req_type;

typedef struct
{
    byte    cmd_code;
    byte    segment[2];
    byte    offset[2];
}dload_go_req_type;

typedef struct
{
    byte    cmd_code;
    byte    segment[2];
    byte    offset[2];
}dload_go_rep_req_type;

typedef struct
{
    byte    cmd_code;
}dload_nop_req_type;

typedef struct
{
    byte    cmd_code;
}dload_params_req_type;

typedef struct
{
    byte    cmd_code;
}dload_reset_req_type;

typedef struct
{
    byte    cmd_code;
    byte    security_code[8];
}dload_unlock_req_type;

typedef struct
{
    byte    cmd_code;
}dload_version_req_type;

typedef struct
{
    byte    cmd_code;
}dload_powerdown_req_type;


typedef struct
{
    byte    cmd_code;
    //  byte address[3];    //扩充成4位地址
    byte    address[4];
    byte    length[4];
    byte    buf[FLASH_WRITE_BLOCK_SIZE];
}dload_write_req_type;

typedef enum
{
    UART_RATE_38400,
    UART_RATE_57600,
    UART_RATE_115200,
    UART_RATE_153600,
    UART_RATE_230400
}   uart_rate_type;

typedef struct
{
    byte    cmd_code;
    byte    rate_type;
}dload_change_rate_req_type;



typedef struct
{
    byte    cmd_code;
    byte    magicnumber[32];
    byte    version;
    byte    compatibleversion;
    byte    feature;
}sdp_hello_req_type;

typedef struct
{
    byte    cmd_code;
    byte    mode;
}sdp_open_req_type;

/*EFS req Operation start*/
typedef enum
{
    DIAG_FS_MK_DIR                                                              = 0,
    /* Create directory                */
    DIAG_FS_RM_DIR                                                              = 1,
    /* Remove Directory                */
    DIAG_FS_DISP_DIRS                                                           = 2,
    /* Display directories list        */
    DIAG_FS_DISP_FILES                                                          = 3,
    /* Display file list               */
    /* NOTE: Display ops are temporary 
    implementations                */ 
    DIAG_FS_READ_FILE                                                           = 4,
    /* Read a file in FS               */
    DIAG_FS_WRITE_FILE                                                          = 5,
    /* Write a file in FS              */  
    DIAG_FS_REMOVE_FILE                                                         = 6,
    /* Remove a file from FS           */  
    DIAG_FS_GET_ATTRIB                                                          = 7,
    /* Get file attributes             */
    DIAG_FS_SET_ATTRIB                                                          = 8,
    /* Set file attributes             */
    DIAG_FS_REMOTE_LINK                                                         = 9,
    /* Create a remote file link       */
    DIAG_FS_ITER_DIRS                                                           = 10,
    /* Iterative display directories   */
    DIAG_FS_ITER_FILES                                                          = 11,
    /* Iterative display files         */
    DIAG_FS_SPACE_AVAIL                                                         = 12,
    /* Amount of free space in EFS     */
    DIAG_FS_LAST_OP               /* Last OP.  For range checking.   */
}    diag_fs_op_enum_type;

#define DIAG_FS_MAX_FILENAME_LEN      60 /* Specified by FS              */
#define DIAG_FS_MAX_FILE_BLOCK_SIZE  256 /* Limited by req buffer of 256 */

typedef  struct
{
    byte    len;                            /* Length of filename string 
                                            including NULL terminating char   */
    char    name[DIAG_FS_MAX_FILENAME_LEN]; /* Filename string.  NULL terminated.
                                            Valid data len == filename_len+1  */
} diag_fs_filename_type;

typedef  struct
{
    word    len;                               /* Length of data block */
    byte    data[DIAG_FS_MAX_FILE_BLOCK_SIZE]; /* Data block           */
} diag_fs_data_block_type;

/* File attribute mask */
typedef enum
{
    DIAG_FS_FA_UNRESTRICTED                             = 0x00FF,
    /* No file access restrictions           */
    DIAG_FS_FA_PERMANENT                                = 0x007F,
    /* File can't be removed nor truncated   */
    DIAG_FS_FA_READONLY                                 = 0x00BF,
    /* File can't be opened for update       */
    DIAG_FS_FA_SYS_PERMANENT                            = 0x0037,
    /* File persists across system reformats */
    DIAG_FS_FA_REMOTE                                   = 0x001F   /* Remote file (resides outside file     */
    /* system address space)                 */
}    diag_fs_attribute_mask_type;     

/* File OPEN operation buffering options */
typedef enum
{
    DIAG_FS_OB_PROHIBIT,
    /* Prohibit file buffering                */
    DIAG_FS_OB_ALLOW                 /* Allow file buffering                   */
}    diag_fs_buffering_option_type;

/* File OPEN operation clean-up options */
typedef enum
{
    DIAG_FS_OC_CLOSE                                        = 0x00,
    /* Close file as-is                           */
    DIAG_FS_OC_DISCARD                                      = 0x01,
    /* Delete file and remove it from directory   */
    DIAG_FS_OC_TRUNCATE                                     = 0x02,
    /* Truncate file from specified position      */
    DIAG_FS_OC_REVERT                                       = 0x03  /* Revert to last checkpointed version        */
}    diag_fs_cleanup_option_type;

typedef  struct
{
    word                                attribute_mask;  /* Use diag_fs_attribute_mask_type   */
    byte                                buffering_option;  /* Use diag_fs_buffering_option_type */
    byte                                cleanup_option;  /* Use diag_fs_cleanup_option_type   */
} diag_fs_attributes_type;

/*-------------------------------------------------------
Definitions of data for specific operations.
-------------------------------------------------------*/
/* "Create Directory" operation */
typedef diag_fs_filename_type       diag_fs_mkdir_req_type;

/* "Remove Directory" operation */
typedef diag_fs_mkdir_req_type      diag_fs_rmdir_req_type;

/* "Display Directory List" operation */
typedef diag_fs_filename_type       diag_fs_disp_dirs_req_type;

/* "Display File List" operation */
typedef diag_fs_disp_dirs_req_type  diag_fs_disp_files_req_type;

/* "Read File" operation */
typedef  struct
{
    byte                    seq_num;                        /* Sequence number for mult blocks */

    diag_fs_filename_type   filename_info; /* File name info                  */
    /* Used only if seq_num == 0       */
} diag_fs_read_req_type;

/* "Write File" operation */
typedef enum
{
    DIAG_FS_NO_OVERWRITE        = 0,
    DIAG_FS_OVERWRITE           = 1
}    diag_fs_write_mode_enum_type;

typedef  struct
{
    byte                    mode; /* Use diag_fs_write_mode_enum_type  */
    dword                   total_length; /* Total length of this file         */
    diag_fs_attributes_type attrib; /* Attributes for this file          */
    diag_fs_filename_type   name_info; /* File name info                     */
    diag_fs_data_block_type blockdata;  /* First block data                  */
} diag_fs_write_begin_req_type;

typedef  union
{
    diag_fs_write_begin_req_type    begin; /* First block of a write           */
    diag_fs_data_block_type         subseq; /* Subsequent blocks for write      */
} diag_fs_write_block_type;

typedef  struct
{
    byte                            seq_num;                        /* Sequence number for mult blocks    */
    byte                            more;                        /* Flag if more packets are needed to
                                                                     complete write                    */
    diag_fs_write_block_type        block;  /* Block for this portion of the file */
} diag_fs_write_req_type;

/* "Get File Attributes" operation */
typedef diag_fs_filename_type   diag_fs_get_attrib_req_type;

/* "Set File Attributes" operation */
typedef  struct
{
    diag_fs_attributes_type         attribs;
    dword                           creation_date;
    diag_fs_filename_type           filename_info;
} diag_fs_set_attrib_req_type;

/* "Remove File" operation */
typedef diag_fs_filename_type   diag_fs_rmfile_req_type;

/* "Remote File Link" operation */
typedef  struct
{
    dword                   base_address;
    dword                   length;
    diag_fs_filename_type   name_info;
} diag_fs_remote_link_req_type;

/* "Iterate Directories" operation */
typedef  struct
{
    dword                               seq_num;
    diag_fs_filename_type               dir_name;
} diag_fs_iter_dirs_req_type;

/* "Iterate Files" operation */
typedef diag_fs_iter_dirs_req_type  diag_fs_iter_files_req_type;

/* Union of all possible operations.  Determined by cmd_code */
typedef  union
{
    byte                            seq_num; /* Seq number in same place for
                                                all packets that use them */
    diag_fs_mkdir_req_type          mkdir;
    diag_fs_rmdir_req_type          rmdir;
    diag_fs_read_req_type           read;
    diag_fs_write_req_type          write;
    diag_fs_disp_dirs_req_type      disp_dirs;
    diag_fs_disp_files_req_type     disp_files;
    diag_fs_get_attrib_req_type     get_attrib;
    diag_fs_set_attrib_req_type     set_attrib;
    diag_fs_rmfile_req_type         rmfile;
    diag_fs_remote_link_req_type    remote_link;
    diag_fs_iter_dirs_req_type      iter_dirs;
    diag_fs_iter_files_req_type     iter_files;
} diag_fs_req_type;

typedef  struct
{
    byte                cmd_code;                /* Command code                  */
    byte                file_op;                 /* From diag_fs_op_enum_type     */

    diag_fs_req_type    fs_req;      /* Filesystem request data union */
} diag_fs_op_req_type;

typedef struct
{
    byte    cmd_code;
    byte    start_addr[4];
    byte    len[4];
}sdp_hsdl_req_type;


/*EFS req Operation end*/
////lichenyang 20060318
typedef struct
{
    byte    cmd_code;
    byte    mode;
}sdp_security_req_type;

typedef struct
{
    byte    cmd_code;
}sdp_security_rsp_type;

typedef struct
{
    byte    cmd_code;
    byte    override;
}sdp_patition_req_type;

typedef struct
{
    byte    cmd_code;
    byte    status;
}sdp_patition_rsp_type;

typedef struct
{
    byte    cmd_code;
    byte    type;
}sdp_openmulti_req_type;

typedef struct
{
    byte    cmd_code;
    byte    status;
}sdp_openmulti_rsp_type;

typedef struct
{
    byte    cmd_code;
}sdp_eraser_req_type;
//lichenyang 20070713
typedef struct
{
    byte    cmd_code;
} diag_sw_ver_extend_req_type ;
//lichenyang 20070713

typedef struct
{
    byte    cmd_code;
}sdp_eraser_rsp_type;

typedef struct
{
    byte    cmd_code;
    byte    password[6];
}diag_spc_req_type;

////lichenyang 20060318
typedef union
{
    byte                        cmd_code;
    dload_write_req_type        write;
    dload_erase_req_type        erase;
    dload_go_req_type           go;
    dload_go_rep_req_type       gorep;
    dload_nop_req_type          nop;
    dload_params_req_type       params;
    dload_reset_req_type        reset;
    dload_unlock_req_type       unlock;
    dload_version_req_type      version;
    dload_powerdown_req_type    powerdown;
    dload_change_rate_req_type  changerate;

    diag_esn_req_type           esn;
    diag_status_req_type        stat;
    diag_nv_read_req_type       nvread;
    diag_nv_write_req_type      nvwrite;
    diag_control_req_type       cont;
    diag_dload_req_type         dload;
    diag_sw_ver_req_type        sw_ver;
    diag_checkpassword_req_type checkpassword_req;//2004年5月27日由刘晓军添加
    diag_checksoftver_req_type  checksoftver_req ;//2004年5月27日由刘晓军添加
    diag_spc_req_type           spc;
    diag_PRLWRITE_req_type      PRLwrite_req ; 
    diag_PRLREAD_req_type       PRLread_req ; 
    diag_sec_dload_chk_req_type dload_chk_req;

    sdp_hello_req_type          hello;
    sdp_open_req_type           openpacket;
    diag_fs_op_req_type         fs_op;
    diag_sub_efs2_req_type      efs2_op;    //xiezhuo add efs2

    //  diag_sec_hand_req_type      hand_req ;
    diag_clearAll_req_type      clearUserInfo;
    diag_sub_cmd_req_type       sub_cmd;
    diag_sw_ver_extend_req_type sw_ver_extend ;

    sdp_hsdl_req_type           hsdl;
    sdp_security_req_type       security;////lichenyang 20060318
    sdp_patition_req_type       patition;////lichenyang 20060318
    sdp_openmulti_req_type      openmulti;////lichenyang 20060318
    sdp_eraser_req_type         eraser;////lichenyang 20060318
} diag_req_type;

typedef  struct
{
    diag_req_type   req;           
    word            crc;           
    byte            control_char;
} diag_req_pkt_type;


//返回包格式的定义
typedef  struct
{
    byte    cmd_code;              
    dword   esn;
} diag_esn_rsp_type;


typedef enum
{
    RXC_CDMA_STATE,
    /*0 – Entering CDMA State                  */
    RXC_SC_STATE,
    /*1 – Sync Channel State                   */
    RXC_PC_STATE,
    /*2 – Paging Channel State                 */
    RXC_TCI_STATE,
    /*3 – Traffic Channel Initialization State */
    RXC_TC_STATE,
    /*4 – Traffic Channel State                */
    RXC_EXIT_STATE                  /*5 – Exit state                           */
}   rxc_state_type;

typedef enum
{
    MCC_ACQUISITION,
    /* Enter normal CDMA mode of operation */
    MCC_CONTINUE,
    /* Continue previous operation (quick reacquisition) */
    MCC_ENTER_OFFLINE,
    /* Enter offline CDMA mode of operation */
    MCC_ORIGINATION                 /* Originate call in CDMA mode of operation */
}    mcc_entry_name_type;

typedef  struct
{
    dword   esn;                
    word    cdma_rx_state;      
    word    entry_reason;           
    word    curr_chan;    
    byte    cdma_code_chan;
    word    pilot_base;             
    word    sid;                    
    word    nid;
} diag_status_rsp_type;

typedef  struct
{
    dword   subsys_id;                
    word    subsys_cmdcode;      
    word    response;
} diag_subsys_rsp_type;

/*----------------------------------------------------*/
#define NEWVER_SIZE 4
typedef struct
{
    byte    cmd_code;
    byte    length;
    byte    newsoftver[NEWVER_SIZE];
} diag_checksoftver_rsp_type;
/*----------------------------------------------------*/
//2004年5月27日由刘晓军添加
typedef struct
{
    byte    cmd_code ;
    byte    pr_list_status;
    word    nv_status ;
    byte    seq_num   ;
    byte    more      ;
    byte    num_bits[2]  ;
    byte    buf[PRL_WRITE_BLOCK_SIZE];
}diag_PRLREAD_rsp_type;

typedef struct
{
    byte    cmd_code ;
    byte    pr_list_status ;
    word    nv_status;
}diag_PRLWRITE_rsp_type;


typedef struct
{
    byte    cmd_code;
}dload_ack_rsp_type;


typedef struct
{
    char    sw_ver_str[SW_VERSION_STRLEN + 1];
    char    comp_date[COMPI_DATE_STRLEN + 1];     
    char    comp_time[COMPI_TIME_STRLEN + 1];
} diag_sw_ver_rsp_type;


typedef struct
{
    char    sw_ver_str[SW_VERSION_STRLEN + 1];
    char    comp_date[COMPI_DATE_STRLEN + 1];     
    char    comp_time[COMPI_TIME_STRLEN + 1];
} diag_ext_sw_ver_rsp_type;


typedef struct
{
    byte    cmd_code;
    word    reason;
}dload_nak_rsp_type;

typedef struct
{
    byte    flashsize;
    char    flashmfr[64];
    char    flashnam[128];
}flash_info_type;

typedef struct
{
    byte            cmd_code;
    byte            version;
    byte            min_version;
    dword           max_write_size;
    byte            model;
    byte            device_size;
    dword           device_size_inbyte;
    byte            device_type;
    flash_info_type flash_info;
}dload_params_rsp_type;


typedef struct
{
    byte    cmd_code;
    byte    length;
    char    version[VERSION_SIZE];
}dload_version_rsp_type;

typedef struct
{
    byte    cmd_code;
    byte    errorcode[4];
    byte    errortext[128];
}sdp_error_type;

typedef enum
{
    ACK,
    /* Success. Send an acknowledgement.           */
    NAK_INVALID_FCS,
    /* Failure: invalid frame check sequence.      */
    NAK_INVALID_DEST,
    /* Failure: destination address is invalid.    */
    NAK_INVALID_LEN,
    /* Failure: operation length is invalid.       */
    NAK_EARLY_END,
    /* Failure: packet was too short for this cmd. */
    NAK_TOO_LARGE,
    /* Failure: packet was too long for my buffer. */
    NAK_INVALID_CMD,
    /* Failure: packet command code was unknown.   */
    NAK_FAILED,
    /* Failure: operation did not succeed.         */
    NAK_WRONG_IID,
    /* Failure: intelligent ID code was wrong.     */
    NAK_BAD_VPP,
    /* Failure: programming voltage out of spec    */
    NAK_VERIFY_FAILED,
    /* Failure: readback verify did not match      */
    NAK_NO_SEC_CODE,
    /* Failure: not permitted without unlock       */
    NAK_BAD_SEC_CODE        /* Failure: invalid security code              */
}   dload_status_enum_type;

typedef enum
{
    PARTI_TBL_ACCEPTED,
    PARTI_TBL_DIFFERS,
    PARTI_TBL_BAD_FORMAT,
    PARTI_TBL_ERASE_FAIL,
    PARTI_TBL_UNKNOWN_ERROR,
}    parti_tbl_type;


/*EFS rsp Operation start*/
/*------------------------------------------------
Definitions of data for specific operations.
------------------------------------------------*/
/* "Display Directory List" operation */
typedef  struct
{
    word                    num_dirs;
    diag_fs_data_block_type dir_list;
} diag_fs_disp_dirs_rsp_type;

/* "Display File List" operation */
typedef  struct
{
    word                    num_files;
    diag_fs_data_block_type file_list;
} diag_fs_disp_files_rsp_type;

/* "Read File" operation */
typedef  union
{
    struct
    {
        dword                   total_length;   /* Total length of this file */
        diag_fs_data_block_type block;
    } begin;

    diag_fs_data_block_type block;
} diag_fs_read_block_type;

typedef  struct
{
    byte                    seq_num;  /* Sequence number for mult blocks    */
    byte                    more;  /* Flag if more packets are needed to
                                    complete transfer                 */
    diag_fs_read_block_type data;  /* Current portion of the file        */
} diag_fs_read_rsp_type;

/* "Write File" operation */
typedef  struct
{
    byte    seq_num;
} diag_fs_write_rsp_type;

/* "Get File Attributes" operation */
typedef  struct
{
    diag_fs_attributes_type attrib; /* Attributes         */
    dword                   creation_date; /* File creation date */
    dword                   size; /* File size          */
} diag_fs_get_attrib_rsp_type;

/* "Iterate Directories" and "Iterate Files" operation */
typedef  struct
{
    dword                   seq_num;

    diag_fs_attributes_type attrib; /* Attributes                   */
    dword                   creation_date; /* File creation date           */
    dword                   logical_size; /* Size of data                 */
    dword                   physical_size; /* Physical space on device     */
    byte                    dirname_length; /* Length of directory portion  */
    /* of file name.                */
    diag_fs_filename_type   item_name;
} diag_fs_iter_rsp_type;

/* "Space Available" operation */
typedef dword           diag_fs_space_avail_rsp_type;

typedef  union
{
    byte                            seq_num; /* Seq number in same place for 
                                                    all packets that use them */
    diag_fs_disp_dirs_rsp_type      disp_dirs;
    diag_fs_disp_files_rsp_type     disp_files;
    diag_fs_read_rsp_type           read;
    diag_fs_write_rsp_type          write;
    diag_fs_get_attrib_rsp_type     get_attrib;
    diag_fs_iter_rsp_type           iter;
    diag_fs_space_avail_rsp_type    space_avail;
} diag_fs_rsp_type;

typedef enum
{
    FS_OKAY_S,
    /* Good status                                 */
    FS_FAIL_S,
    /* Low-level operation failed                  */
    FS_BUSY_S,
    /* Operation is queued or in progress          */
    FS_FILE_NOT_OPEN_S,
    /* File needs to be opened for this operation  */
    FS_FILE_OPEN_S,
    /* File needs to be closed for this operation  */
    FS_FILE_ALREADY_EXISTS_S,
    /* File already exists                         */
    FS_NONEXISTENT_FILE_S,
    /* File does not exist                         */
    FS_DIR_ALREADY_EXISTS_S,
    /* User directory already exists               */
    FS_NONEXISTENT_DIR_S,
    /* User directory does not exist               */
    FS_DIR_NOT_EMPTY_S,
    /* User directory not empty for remove         */
    FS_UNKNOWN_OPERATION_S,
    /* Client specified an unknown operation       */
    FS_ILLEGAL_OPERATION_S,
    /* Client specified an invalid operation       */
    FS_PARAMETER_ERROR_S,
    /* Client supplied a bad parameter value       */
    FS_BAD_FILE_NAME_S,
    /* Client specified invalid file/directory name*/
    FS_BAD_FILE_HANDLE_S,
    /* Client specified invalid file handle        */
    FS_BAD_SEEK_POS_S,
    /* Client specified an invalid SEEK position   */
    FS_BAD_TRUNCATE_POS_S,
    /* Client supplied an invalid truncate position*/
    FS_FILE_IS_REMOTE_S,
    /* Operation invalid for remote files          */
    FS_INVALID_CONF_CODE_S,
    /* Invalid confirmation code specified         */
    FS_EOF_S,
    /* End of file reached                         */
    FS_MORE_S,
    /* More records exist to be processed          */
    FS_GC_IN_PROGRESS_S,
    /* Garbage Collection in progress              */
    FS_SPACE_EXHAUSTED_S,
    /* Out of file system space                    */
    FS_BLK_FULL_S,
    /* File block is full                          */
    FS_DIR_SPACE_EXHAUSTED_S,
    /* Out of Master Directory space               */
    FS_FBL_POOL_EXHAUSTED_S,
    /* Out of File Block List free pool space      */
    FS_OPEN_TABLE_FULL_S,
    /* Out of open-file table slots                */
    FS_INVALID_ITERATOR_S,
    /* Invalid enumeration iterator control struct */
    FS_ENUM_DONE_S,
    /* Enumeration operation completed             */
    FS_RMLINK_EXHAUSTED_S,
    /* Maximum number of remote links reached      */
    FS_NO_MEDIA_S,
    /* Removable media not inserted                */
    FS_MEDIA_CHANGED_S        /* Media has been changed since opened         */
}    fs_status_enum_type;

typedef  struct
{
    byte                cmd_code;                     /* Command code                     */
    byte                file_op;                      /* Operation requested              */
    byte                fs_status;                    /* Status of operation - values in  
                                                        fs_status_enum_type              */
    diag_fs_rsp_type    fs_rsp;          /* Only used if returning more than 
                                              just status                      */
} diag_fs_op_rsp_type;  
/*EFS rsp Operation end*/

//lichenyang 20070613
typedef struct
{
    byte    cmd_code;
    byte    reserved_code[3] ;
    byte    hwversion[4] ;
    byte    mobmodel[4] ;
    byte    retvalue[256] ;
}diag_sw_ver_extend_rsp_type ;
//lichenyang 20070613

typedef struct//ruiju add
{
	byte cmd_code;
	byte retType;
	byte retResult;
	//byte reserved[2];
}sdp_hdlc_rsp_type;

typedef  union
{
    byte                        cmd_code;
    diag_verno_rsp_type         verno;
    diag_esn_rsp_type           esn;
    diag_status_rsp_type        stat;
    diag_nv_read_req_type       nvread;     /* note: type is a req packet */
    diag_nv_write_req_type      nvwrite;    /* note: type is a req packet */
    diag_control_req_type       cont;  
    diag_dload_req_type         dload;      /* note: type is same as req packet */
    diag_sw_ver_rsp_type        sw_ver;
    diag_ext_sw_ver_rsp_type    sw_ext_ver;
    diag_checksoftver_rsp_type  checksoftver;//2004年5月27日由刘晓军添加
    diag_subsys_rsp_type        subsys;
    diag_sec_dload_chk_rsp_type dload_chk_rsp;
    dload_powerdown_req_type    powerdown;
    dload_ack_rsp_type          ack;
    dload_nak_rsp_type          nak;
    dload_params_rsp_type       params;
    dload_version_rsp_type      version;
    sdp_error_type              err;
    diag_fs_op_rsp_type         fs_op;
    diag_sub_efs2_rsp_type      efs2_rsp;       //xiezhuo add efs2
    diag_PRLREAD_rsp_type       PRLread_rsp ;  //added by liuxiaojun,2004,6,11
    diag_PRLWRITE_rsp_type      PRLwrite_rsp ; //added by liuxiaojun,2004,6,11
    //  diag_sec_hand_rsp_type    hand_rsp ;  
    diag_clearAll_rsp_type      clearUserInfo;
    diag_sw_ver_extend_rsp_type sw_ver_extendrsp ;

    sdp_security_rsp_type       security;////lichenyang 20060318
    sdp_patition_rsp_type       patition;////lichenyang 20060318
    sdp_openmulti_rsp_type      openmulti;////lichenyang 20060318
    sdp_eraser_rsp_type         eraser;////lichenyang 20060318
	sdp_hdlc_rsp_type           hdlcrsp;
} diag_rsp_type;

typedef struct
{
    diag_rsp_type   rsp;   
    word            crc;   
    byte            control_char;
} diag_rsp_pkt_type;

typedef struct
{
    word    length;
    byte    buf[DIAG_MAX_RX_PKT_SIZ];
}rx_pkt_buf_type;

typedef struct
{
    dword   length;
    byte    buf[DIAG_MAX_TX_PKT_SIZ];
}tx_pkt_buf_type;

typedef enum
{
    DMP         = 0,
    DLP         = 1,
    SDP         = 2,
    DDP         = 3,
}    protocol_type;

typedef struct _SDLnode //  下载Flash需要使用
{
    int     nStartPos;
    int     nBlock;
    BOOL    bFinish;
}SDLnode;
#define B_PTR( var )  ( (BYTE *) (void *) &(var) )

#endif /* DIAGPKT_H  */

