// Packet.cpp: implementation of the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Packet.h"

#define WRITE_TIMER 5000
#define ERASE_TIMER 2500
#define CHECK_TIMER 2000
#define QUERY_TIMER 300

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacket::CPacket()
{
    EmptyVaiables();
    waitsign = 0;//lichenyang 20061109
    m_bOverridePtl = 0;
    m_pEFS2DirectoryPoint = 0;
}

CPacket::~CPacket()
{
}

dword CPacket::CombinePacket()
{
    //根据m_ReqPacket的命令需求，组装数据包，组装的结果放在m_TxPacket,包长度作为返回值
    dword   i   = 0;

    if (m_Protocol == DLP)
    {
        m_TxPacket.length = 0;
        CombinePacketDLP();
    }
    if (m_Protocol == DMP)
    {
        m_TxPacket.length = 0;
        CombinePacketDMP();
    }
    if (m_Protocol == SDP)
    {
        m_TxPacket.length = 0;
        CombinePacketSDP();
    }

    m_TxPacket.length = PacketCRCMake(m_TxPacket.buf,m_TxPacket.length);//增加循环冗余校验

    byte   *buffer      = new byte[m_TxPacket.length * 2];
    dword   ValidLen    = 0;
    for (i = 0; i < m_TxPacket.length; i++)//特殊字替换
    {
        if ((m_TxPacket.buf[i] == ASYNC_HDLC_FLAG) || (m_TxPacket.buf[i] == ASYNC_HDLC_ESC))
        {
            buffer[ValidLen++] = ASYNC_HDLC_ESC;
            buffer[ValidLen++] = m_TxPacket.buf[i] ^ (byte) ASYNC_HDLC_ESC_MASK;
        }
        else
        {
            buffer[ValidLen++] = m_TxPacket.buf[i];
        }
    }
    m_TxPacket.length = 0;
    if ((m_Protocol == DLP) || (m_Protocol == SDP))//下载协议增加协议头“7E”
    {
        m_TxPacket.buf[m_TxPacket.length++] = ASYNC_HDLC_FLAG;
    }
    for (i = 0; i < ValidLen; i++)
        m_TxPacket.buf[m_TxPacket.length++] = buffer[i];
    m_TxPacket.buf[m_TxPacket.length++] = ASYNC_HDLC_FLAG;
    delete buffer;

    return (dword) m_TxPacket.length;
}
BOOL CPacket::CombinePacketDLP()
{
    int i   = 0;
    int len = 0;
    switch (m_ReqPacket.req.cmd_code)
    {
        case DLOAD_WRITE_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.cmd_code;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[2];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[1];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[0];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.length[1];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.length[0];

        B_PTR(len)[0] = m_ReqPacket.req.write.length[0];
        B_PTR(len)[1] = m_ReqPacket.req.write.length[1];
        memcpy(&m_TxPacket.buf[m_TxPacket.length],m_ReqPacket.req.write.buf,len);
        m_TxPacket.length += len;

        break;
        case DLOAD_WRITE32_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.cmd_code;
        //DLP协议的地址从高位到低位。按一个字来计算
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[3];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[2];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[1];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[0];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.length[1];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.length[0];

        B_PTR(len)[0] = m_ReqPacket.req.write.length[0];
        B_PTR(len)[1] = m_ReqPacket.req.write.length[1];
        memcpy(&m_TxPacket.buf[m_TxPacket.length],m_ReqPacket.req.write.buf,len);
        m_TxPacket.length += len;
        break;
        case DLOAD_ERASE_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.erase.cmd_code;
        for (i = 0; i < 3; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.erase.address[2 - i];
        for (i = 0; i < 3; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.erase.length[2 - i];

        break;

        case DLOAD_GO_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.go.cmd_code;
        for (i = 0; i < 2; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.go.segment[1 - i];
        for (i = 0; i < 2; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.go.offset[1 - i];
        break;

        case DLOAD_GO_REP_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.gorep.cmd_code;
        for (i = 0; i < 2; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.gorep.segment[1 - i];
        for (i = 0; i < 2; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.gorep.offset[1 - i];
        break;

        case DLOAD_UNLOCK_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.unlock.cmd_code;
        for (i = 0; i < 8; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.unlock.security_code[i];
        break;

        case DLOAD_NOP_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.nop.cmd_code;
        break;

        case DLOAD_PREQ_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.params.cmd_code;
        break;

        case DLOAD_RESET_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.reset.cmd_code;
        break;

        case DLOAD_VERREQ_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.version.cmd_code;
        break;

        case DLOAD_PWROFF_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.powerdown.cmd_code;
        break;

        case DLOAD_CHANGE_RATE_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.changerate.cmd_code;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.changerate.rate_type;
        break;

        case DLOAD_CHECKPASS_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.checkpassword_req.cmd_code ;
        for (i = 0 ; i < DIAG_CHECKPASS_SIZE ; i++)
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.checkpassword_req.checkpassword[i] ;
        break ;
        case DLOAD_CHECKVER_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.checksoftver_req.cmd_code ;
        break;
        case DLOAD_BOOT_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.powerdown.cmd_code;
        break;
        default:
        break;
    }
    return TRUE;
}
BOOL CPacket::CombinePacketDMP()
{
    int     i   = 0;
    byte   *pPacket;
    switch (m_ReqPacket.req.cmd_code)
    {
        case DIAG_FS_OP_F:
        m_TxPacket.length = CombineFSPacket();
        break;
        case DIAG_SUBSYS_CMD_F:
        m_TxPacket.length = CombineSubsysPacket();
        break;
        case DIAG_ESN_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.esn.cmd_code;
        break;

        case DIAG_STATUS_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.stat.cmd_code;
        break;

        case DIAG_CONTROL_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.cont.cmd_code;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.cont.mode,2);
        m_TxPacket.length += 2;
        break;

        case DIAG_DLOAD_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.dload.cmd_code;
        break;

        case DIAG_NV_READ_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.nvread.cmd_code;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.nvread.item,sizeof(word));
        m_TxPacket.length += sizeof(word);
        pPacket = (byte *) &m_ReqPacket.req.nvread.item_data;
        for (i = 0; i < DIAG_NV_ITEM_SIZE; i++)
            m_TxPacket.buf[m_TxPacket.length++] = pPacket[i];
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.nvread.nv_stat,sizeof(word));
        m_TxPacket.length += sizeof(word);

        break;

        case DIAG_NV_WRITE_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.nvwrite.cmd_code;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.nvwrite.item,sizeof(word));
        m_TxPacket.length += sizeof(word);
        pPacket = (byte *) &m_ReqPacket.req.nvwrite.item_data;
        for (i = 0; i < DIAG_NV_ITEM_SIZE; i++)
            m_TxPacket.buf[m_TxPacket.length++] = pPacket[i];
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.nvwrite.nv_stat,sizeof(word));
        m_TxPacket.length += sizeof(word);

        break;
        case DIAG_COMMCONFIG_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.cmd_code;
        m_TxPacket.buf[m_TxPacket.length++] = 0x01;
        //设置速率为115200，00 C2 01 00
        m_TxPacket.buf[m_TxPacket.length++] = 0x00;
        m_TxPacket.buf[m_TxPacket.length++] = 0xC2;
        m_TxPacket.buf[m_TxPacket.length++] = 0x01;
        m_TxPacket.buf[m_TxPacket.length++] = 0x00;
        break;
        case DIAG_SPC_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.cmd_code;
        for (i = 0; i < 6; i++)
        {
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.spc.password[i];
        }
        break;
        case DIAG_SW_VERSION_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.sw_ver.cmd_code;
        break;
        case DIAG_EXT_SW_VERSION_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.sw_ver.cmd_code;
        break;
        case DIAG_DEL_USER_INFOR_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.clearUserInfo.cmd_code;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.clearUserInfo.b_flexfile;
        break;

        case DIAG_SW_VERSION_EXTEND_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.sw_ver_extend.cmd_code;
        break;

        case DIAG_PSW_NV_READ_F:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.nvread.cmd_code;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.nvread.item,sizeof(word));
        m_TxPacket.length += sizeof(word);
        pPacket = (byte *) &m_ReqPacket.req.nvread.item_data;
        for (i = 0; i < DIAG_NV_ITEM_SIZE; i++)
            m_TxPacket.buf[m_TxPacket.length++] = pPacket[i];
        memcpy(&m_TxPacket.buf[m_TxPacket.length],&m_ReqPacket.req.nvread.nv_stat,sizeof(word));
        m_TxPacket.length += sizeof(word);

        break;

        case DIAG_PRL_READ_F:
        {
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLread_req.cmd_code ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLread_req.seq_num ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLread_req.nam ;
        }
        break ;

        case DIAG_PRL_WRITE_F:
        {
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLwrite_req.cmd_code ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLwrite_req.seq_num ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLwrite_req.more ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLwrite_req.nam ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLwrite_req.num_bits[0] ;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.PRLwrite_req.num_bits[1] ;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],m_ReqPacket.req.PRLwrite_req.buf,PRL_WRITE_BLOCK_SIZE) ;
            m_TxPacket.length += PRL_WRITE_BLOCK_SIZE ;
        }
        break ;

        default:
        break;
    }
    return TRUE;
}
BOOL CPacket::CombinePacketSDP()
{
    int i   = 0;
    int len = 0;
    switch (m_ReqPacket.req.cmd_code)
    {
	case SDP_HSDL_F_NEW:
		m_TxPacket.buf[m_TxPacket.length++] = SDP_HSDL_F_NEW ;
		
		//address ;
		m_TxPacket.buf[m_TxPacket.length++] = 0x00 ;
		m_TxPacket.buf[m_TxPacket.length++] = 0x90 ;
		m_TxPacket.buf[m_TxPacket.length++] = 0x00 ;
		m_TxPacket.buf[m_TxPacket.length++] = 0x00 ;		
		//data length:		
		m_TxPacket.buf[m_TxPacket.length++] = plen[0] ;
		m_TxPacket.buf[m_TxPacket.length++] = plen[1] ;
		m_TxPacket.buf[m_TxPacket.length++] = plen[2] ;
		m_TxPacket.buf[m_TxPacket.length++] = plen[3] ;
		//CRC
		// m_TxPacket.buf[m_TxPacket.length++] = CRC[0] ;
		// m_TxPacket.buf[m_TxPacket.length++] = CRC[1] ;
		m_TxPacket.buf[m_TxPacket.length++] = 1;
		break ;
        case SDP_HSDL_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_HSDL_F ;

        //address ;
        m_TxPacket.buf[m_TxPacket.length++] = 0x00 ;
        m_TxPacket.buf[m_TxPacket.length++] = 0x90 ;
        m_TxPacket.buf[m_TxPacket.length++] = 0x00 ;
        m_TxPacket.buf[m_TxPacket.length++] = 0x00 ;        
        //data length:      
        m_TxPacket.buf[m_TxPacket.length++] = plen[0] ;
        m_TxPacket.buf[m_TxPacket.length++] = plen[1] ;
        m_TxPacket.buf[m_TxPacket.length++] = plen[2] ;
        m_TxPacket.buf[m_TxPacket.length++] = plen[3] ;
        //CRC
        m_TxPacket.buf[m_TxPacket.length++] = CRC[0] ;
        m_TxPacket.buf[m_TxPacket.length++] = CRC[1] ;
        break ;
        case SDP_HELLO_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_HELLO_F;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],"QCOM fast download protocol host",32);
        m_TxPacket.length += 32;
        m_TxPacket.buf[m_TxPacket.length++] = 0x02;
        m_TxPacket.buf[m_TxPacket.length++] = 0x02;
        m_TxPacket.buf[m_TxPacket.length++] = 0x03;
        break;
        case SDP_OPEN_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_OPEN_F;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.openpacket.mode;
        break;
        case SDP_CLOSE_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_CLOSE_F;
        break;
        case SDP_SWRITE_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_SWRITE_F;
        //SDP协议包的地址顺序是从低位到高位的，与DLP不同
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[0];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[1];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[2];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[3];

        B_PTR(len)[0] = m_ReqPacket.req.write.length[0];
        B_PTR(len)[1] = m_ReqPacket.req.write.length[1];
        B_PTR(len)[2] = m_ReqPacket.req.write.length[2];
        B_PTR(len)[3] = m_ReqPacket.req.write.length[3];

        for (i = 0; i < len; i++)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x00;
        }
        break;
        case SDP_WRITE_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_WRITE_F;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[0];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[1];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[2];
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.write.address[3];
        B_PTR(len)[0] = m_ReqPacket.req.write.length[0];
        B_PTR(len)[1] = m_ReqPacket.req.write.length[1];
        B_PTR(len)[2] = m_ReqPacket.req.write.length[2];
        B_PTR(len)[3] = m_ReqPacket.req.write.length[3];
        memcpy(&m_TxPacket.buf[m_TxPacket.length],m_ReqPacket.req.write.buf,len);
        m_TxPacket.length += len;
        break;
        case SDP_NOP_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_NOP_F;
        m_TxPacket.buf[m_TxPacket.length++] = 0x11;//发一个标志为给手机，手机也要返回相同的值
        break;
        case SDP_RESET_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_RESET_F;
        break;
        //lichenyang 20060318
        case SDP_SECURITY_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_SECURITY_F;
        //m_TxPacket.buf[m_TxPacket.length++]=0x00;
        m_TxPacket.buf[m_TxPacket.length++] = 0x01;
        break;

        case SDP_PARTITION_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_PARTITION_F;
        //m_TxPacket.buf[m_TxPacket.length++]=0x01;//lichenyang 20060831
        m_TxPacket.buf[m_TxPacket.length++] = m_bOverridePtl;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],Buf,nLen);
        m_TxPacket.length = m_TxPacket.length + nLen;
        break;

        case SDP_OPENMULTI_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_OPENMULTI_F;
        if (m_ReqPacket.req.openmulti.type == 0x01)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x01;
            break;
        }
        if (m_ReqPacket.req.openmulti.type == 0x02)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x02;
            break;
        }
        if (m_ReqPacket.req.openmulti.type == 0x03)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x03;
            break;
        }
        if (m_ReqPacket.req.openmulti.type == 0x04)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x04;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],Buf,nLen);
            m_TxPacket.length = m_TxPacket.length + nLen;
            break;
        }
        if (m_ReqPacket.req.openmulti.type == 0x05)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x05;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],Buf,nLen);
            m_TxPacket.length = m_TxPacket.length + nLen;
            break;
        }
        //      break;
        if (m_ReqPacket.req.openmulti.type == 0x09)
        {
            m_TxPacket.buf[m_TxPacket.length++] = 0x09;
            break;
        }
        break;

        case SDP_ERASER_F:
        m_TxPacket.buf[m_TxPacket.length++] = SDP_ERASER_F;
        break;      
        //lichenyang 20060318
        default:
        break;
    }
    return TRUE;
}

BOOL CPacket::DividePacket()
{
    //根据发送的请求信息，分解收到的数据包，结果放在m_RspPacket中
    byte   *tmpbuf;
    word    ValidLen = 0, i = 0;

    //下载协议有协议头7E，从第二位开始计算
    if ((m_Protocol == DLP) || (m_Protocol == SDP))
    {
        if (m_RxPacket.buf[0] != ASYNC_HDLC_FLAG)  //有时协议头会丢失，需要补上。有隐患
        {
            for (int j = 0; j < m_RxPacket.length; j++)
            {
                m_RxPacket.buf[m_RxPacket.length - j] = m_RxPacket.buf[m_RxPacket.length - j - 1];
            }
            m_RxPacket.buf[0] = ASYNC_HDLC_FLAG;
            m_RxPacket.length += 1;

            //          return FALSE;
        }
        i = 1;
    }
    //特殊字符转换回来
    tmpbuf = new byte[m_RxPacket.length];
    for (ValidLen = 0; i < m_RxPacket.length; i++)
    {
        if (m_RxPacket.buf[i] == ASYNC_HDLC_ESC)
        {
            tmpbuf[ValidLen++] = m_RxPacket.buf[++i] ^ ASYNC_HDLC_ESC_MASK;
        }
        else
        {
            tmpbuf[ValidLen++] = m_RxPacket.buf[i];
        }
    }
    if (tmpbuf[ValidLen - 1] != ASYNC_HDLC_FLAG)
    {
        m_strErrInfo.Format("the Received Data is Wrong without End Flag(0x7e)");
        delete tmpbuf;
        return FALSE;
    }
    m_RxPacket.length = 0;
    for (i = 0; i < ValidLen - 1; i++)
        m_RxPacket.buf[m_RxPacket.length++] = tmpbuf[i];
    delete tmpbuf;
    //验证校验码
    if (!PacketCRCCheck(m_RxPacket.buf,m_RxPacket.length))
    {
        m_strErrInfo.Format("the Received Data is Wrong with Wrong CRC Checksum");
        return FALSE;
    }

    m_RxPacket.length = 0;
    m_RspPacket.rsp.cmd_code = m_RxPacket.buf[m_RxPacket.length];

    switch (m_Protocol)
    {
        case DLP:
        if (!DividePacketDLP())
        {
            return FALSE;
        }
        break;
        case DMP:
        if (!DividePacketDMP())
        {
            return FALSE;
        }
        break;
        case SDP:
        if (!DividePacketSDP())
        {
            return FALSE;
        }
        break;
        default:
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::DividePacketDLP()
{
    int i;
    switch (m_RspPacket.rsp.cmd_code)
    {
        case DLOAD_ACK_F:
        m_RspPacket.rsp.ack.cmd_code = m_RxPacket.buf[m_RxPacket.length];
        break;

        case DLOAD_NAK_F:
        m_RspPacket.rsp.nak.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.nak.reason = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.nak.reason = (m_RspPacket.rsp.nak.reason << 8) + m_RxPacket.buf[m_RxPacket.length++];
        m_strErrInfo = MapDloadError(m_RspPacket.rsp.nak.reason);

        return FALSE;

        case DLOAD_PARAMS_F:
        m_RspPacket.rsp.params.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.params.version = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.params.min_version = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.params.max_write_size,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(m_RspPacket.rsp.params.max_write_size));
        m_RxPacket.length += sizeof(m_RspPacket.rsp.params.max_write_size);
        m_RspPacket.rsp.params.model = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.params.device_size = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.params.flash_info.flashsize = m_RspPacket.rsp.params.device_size;
        if (m_RspPacket.rsp.params.flash_info.flashsize >= TOTAL_FLASH_SIZE_TYPES)
        {
            m_RspPacket.rsp.params.flash_info.flashsize = 0x00;
        }
        switch (m_RspPacket.rsp.params.flash_info.flashsize)
        {
            case FLASH_SIZE_4M:
            m_RspPacket.rsp.params.device_size_inbyte = 0x80000;
            break;
            case FLASH_SIZE_8M:
            case FLASH_SIZE_16TO8M:
            m_RspPacket.rsp.params.device_size_inbyte = 0x100000;
            break;
            case FLASH_SIZE_16M:
            m_RspPacket.rsp.params.device_size_inbyte = 0x200000;
            break;
            case FLASH_SIZE_32M:
            m_RspPacket.rsp.params.device_size_inbyte = 0x400000;
            break;
            case FLASH_SIZE_64M:
            m_RspPacket.rsp.params.device_size_inbyte = 0x800000;
            break;

            case FLASH_SIZE_INVALID:
            default:
            m_RspPacket.rsp.params.device_size_inbyte = 0x0;
            break;
        }
        m_RspPacket.rsp.params.device_type = m_RxPacket.buf[m_RxPacket.length++];
        if (m_RspPacket.rsp.params.device_type >= TOTAL_FLASH_TYPES)
        {
            m_RspPacket.rsp.params.device_type = 0;
        }
        //              strcpy(m_RspPacket.rsp.params.flash_info.flashmfr, flash_mfr_name[m_RspPacket.rsp.params.device_type].flashmfr);
        //              strcpy(m_RspPacket.rsp.params.flash_info.flashnam, flash_mfr_name[m_RspPacket.rsp.params.device_type].flashnam);
        //Flash类型，暂未使用，赋值为Unknown
        strcpy(m_RspPacket.rsp.params.flash_info.flashmfr,"Unknown");
        strcpy(m_RspPacket.rsp.params.flash_info.flashnam,"Unknown");
        break;

        case DLOAD_VERRSP_F:
        m_RspPacket.rsp.version.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.version.length = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(m_RspPacket.rsp.version.version,
               &m_RxPacket.buf[m_RxPacket.length],
               m_RspPacket.rsp.version.length);
        break;
        case DLOAD_CHECKVER_RET_F:
        m_RspPacket.rsp.checksoftver.cmd_code = m_RxPacket.buf[m_RxPacket.length++] ;
        m_RspPacket.rsp.checksoftver.length = m_RxPacket.buf[m_RxPacket.length++] ;
        for (i = 0 ; i < NEWVER_SIZE ; i++)
            m_RspPacket.rsp.checksoftver.newsoftver[i] = m_RxPacket.buf[m_RxPacket.length++] ;
        break ;
        case DLOAD_BOOT_F:
        //add by lujuan 060424 for lock_8K
        m_RspPacket.rsp.powerdown.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        break;  
        default:
        break;
    }
    return TRUE;
}
BOOL CPacket::DividePacketDMP()
{
    int i;
    switch (m_RspPacket.rsp.cmd_code)
    {
        case DIAG_FS_OP_F:
        if (!DivideFSPacket())
        {
            return FALSE;
        }
        break;
        case DIAG_SUBSYS_CMD_F:
        if (!DivideSubsysPacket())
        {
            return FALSE;
        }
        break;
        case DIAG_ESN_F:
        m_RspPacket.rsp.esn.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.esn.esn,&m_RxPacket.buf[m_RxPacket.length],4);          
        break;

        case DIAG_STATUS_F:
        m_RxPacket.length += 4;
        memcpy(&m_RspPacket.rsp.stat.esn,&m_RxPacket.buf[m_RxPacket.length],sizeof(dword));
        m_RxPacket.length += sizeof(dword);
        m_RxPacket.length += 15;
        memcpy(&m_RspPacket.rsp.stat.cdma_rx_state,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += 9;
        memcpy(&m_RspPacket.rsp.stat.entry_reason,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += 2;
        memcpy(&m_RspPacket.rsp.stat.curr_chan,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        m_RspPacket.rsp.stat.cdma_code_chan = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.stat.pilot_base,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.stat.sid,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.stat.nid,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        break;

        case DIAG_CONTROL_F:
        m_RspPacket.rsp.cont.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.cont.mode,&m_RxPacket.buf[m_RxPacket.length],2);
        break;

        case DIAG_NV_READ_F:
        m_RspPacket.rsp.nvread.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.nvread.item,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.nvread.item_data,&m_RxPacket.buf[m_RxPacket.length],DIAG_NV_ITEM_SIZE);
        m_RxPacket.length += DIAG_NV_ITEM_SIZE;
        memcpy(&m_RspPacket.rsp.nvread.nv_stat,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        break;

        case DIAG_NV_WRITE_F:
        m_RspPacket.rsp.nvwrite.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.nvwrite.item,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.nvwrite.item_data,&m_RxPacket.buf[m_RxPacket.length],DIAG_NV_ITEM_SIZE);
        m_RxPacket.length += DIAG_NV_ITEM_SIZE;
        memcpy(&m_RspPacket.rsp.nvwrite.nv_stat,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        break;

        case DIAG_SW_VERSION_F:
        m_RxPacket.length = 1;
        for (i = 0; i < SW_VERSION_STRLEN; i++)
            m_RspPacket.rsp.sw_ver.sw_ver_str[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.sw_ver.sw_ver_str[i] = '\0';
        for (i = 0; i < COMPI_DATE_STRLEN; i++)
            m_RspPacket.rsp.sw_ver.comp_date[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.sw_ver.comp_date[i] = '\0';
        for (i = 0; i < COMPI_TIME_STRLEN; i++)
            m_RspPacket.rsp.sw_ver.comp_time[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.sw_ver.comp_time[i] = '\0';
        break;
        case DIAG_EXT_SW_VERSION_F:
        m_RxPacket.length = 1;
        for (i = 0; i < SW_VERSION_STRLEN; i++)
            m_RspPacket.rsp.sw_ver.sw_ver_str[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.sw_ver.sw_ver_str[i] = '\0';
        for (i = 0; i < COMPI_DATE_STRLEN; i++)
            m_RspPacket.rsp.sw_ver.comp_date[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.sw_ver.comp_date[i] = '\0';
        for (i = 0; i < COMPI_TIME_STRLEN; i++)
            m_RspPacket.rsp.sw_ver.comp_time[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.sw_ver.comp_time[i] = '\0';
        break;
        case DIAG_DEL_USER_INFOR_F:
        m_RspPacket.rsp.clearUserInfo.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.clearUserInfo.errcode = m_RxPacket.buf[m_RxPacket.length++];
        break;

        case DIAG_SW_VERSION_EXTEND_F:
        m_RspPacket.rsp.sw_ver_extendrsp.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(m_RspPacket.rsp.sw_ver_extendrsp.reserved_code,&m_RxPacket.buf[m_RxPacket.length],3) ;
        m_RxPacket.length += 3 ;
        memcpy(m_RspPacket.rsp.sw_ver_extendrsp.hwversion,&m_RxPacket.buf[m_RxPacket.length],4) ;
        m_RxPacket.length += 4 ;
        memcpy(m_RspPacket.rsp.sw_ver_extendrsp.mobmodel,&m_RxPacket.buf[m_RxPacket.length],4) ;
        m_RxPacket.length += 4 ;
        memset(m_RspPacket.rsp.sw_ver_extendrsp.retvalue,0,256) ;
        memcpy(m_RspPacket.rsp.sw_ver_extendrsp.retvalue,&m_RxPacket.buf[m_RxPacket.length],256) ;
        m_RxPacket.length += 256 ;
        break ;
        case DIAG_PRL_READ_F:
        {
            m_RspPacket.rsp.PRLread_rsp.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
            m_RspPacket.rsp.PRLread_rsp.pr_list_status = m_RxPacket.buf[m_RxPacket.length++];
            memcpy(&m_RspPacket.rsp.PRLread_rsp.nv_status,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
            m_RxPacket.length += sizeof(word);
            m_RspPacket.rsp.PRLread_rsp.seq_num = m_RxPacket.buf[m_RxPacket.length++];
            m_RspPacket.rsp.PRLread_rsp.more = m_RxPacket.buf[m_RxPacket.length++];
            m_RspPacket.rsp.PRLread_rsp.num_bits[0] = m_RxPacket.buf[m_RxPacket.length++];
            m_RspPacket.rsp.PRLread_rsp.num_bits[1] = m_RxPacket.buf[m_RxPacket.length++];
            memcpy(&m_RspPacket.rsp.PRLread_rsp.buf,&m_RxPacket.buf[m_RxPacket.length],120) ;
            m_RxPacket.length += 120 ;
        }
        break;

        case DIAG_PRL_WRITE_F:
        {
            m_RspPacket.rsp.PRLwrite_rsp.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
            m_RspPacket.rsp.PRLwrite_rsp.pr_list_status = m_RxPacket.buf[m_RxPacket.length++] ;
            memcpy(&m_RspPacket.rsp.PRLwrite_rsp.nv_status,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
            m_RxPacket.length += sizeof(word);
        }
        break ;
        case DIAG_PSW_NV_READ_F:
        m_RspPacket.rsp.nvread.cmd_code = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.nvread.item,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.nvread.item_data,&m_RxPacket.buf[m_RxPacket.length],DIAG_NV_ITEM_SIZE);
        m_RxPacket.length += DIAG_NV_ITEM_SIZE;
        memcpy(&m_RspPacket.rsp.nvread.nv_stat,&m_RxPacket.buf[m_RxPacket.length],sizeof(word));
        break;

        default:
        break;
    }
    return TRUE;
}

BOOL CPacket::DividePacketSDP()
{
    int i;
    switch (m_RspPacket.rsp.cmd_code)
    {
        case SDP_HELLORSP_F:
        break;
        case SDP_NOPRSP_F:
        break;
        case SDP_OPENRSP_F:
        break;
        case SDP_CLOSERSP_F:
        break;
        case SDP_PATITIONRSP_F:
        m_RspPacket.rsp.patition.status = m_RxPacket.buf[1];
        break;      
        case SDP_ERROR_F:
        for (i = 0; i < 4; i++)
            m_RspPacket.rsp.err.errorcode[i] = m_RxPacket.buf[1 + i]; 
        return FALSE;
        break;
        case SDP_RESETRSP_F:
        case SDP_SECURITY_F:
        default:
        break;
    }

    return TRUE;
}

BOOL CPacket::RxTxPacket(byte retry, dword wait_ms)
{
    //组装数据包并向串口写入数据，然后读出串口的返回信息存放在m_RxPacket中
    BOOL    status      = TRUE;
    BOOL    bGotData    = FALSE;
    dword   tBegin = 0, tEnd = 0;
    dword   tmpLength   = 0;
    dword   TxPacketLen;

    SleepMe();

    TxPacketLen = CombinePacket();  //组装数据包

    for (int i = 0; i < retry; i++)
    {
        //写LOG
        //LogMe(m_TxPacket.buf,m_TxPacket.length,true);
        //往串口写数据
        TxPacketLen = WriteCommBytes((char *) m_TxPacket.buf,m_TxPacket.length);
        if (TxPacketLen != m_TxPacket.length)
        {
            m_strErrInfo.Format("Port Cannot Send desired data to the Phone");
            status = FALSE;
            continue;
        }
        //监视是否有正确的返回数据
        tBegin = GetTickCount();
        do
        {
            m_RxPacket.length = ReadDataWaiting();
            if (m_Protocol == DMP
             && (m_TxPacket.buf[0] == DIAG_NV_READ_F || m_TxPacket.buf[0] == DIAG_NV_WRITE_F))
            {
                ::Sleep(30);
            }
            else
            {
                ::Sleep(5);
            }
            tmpLength = ReadDataWaiting();
            if (m_RxPacket.length >= 1 && tmpLength == m_RxPacket.length)
            {
                break;
            }
            tEnd = GetTickCount();
        }
        while ((tEnd - tBegin) <= wait_ms);

        if (m_RxPacket.length < 1)
        {
            m_RxPacket.length = ReadDataWaiting();
            m_strErrInfo.Format("Port Cannot Receive Desired Data from the Phone");
            status = FALSE;
            continue;
        }
        else
        {
            if (m_RxPacket.length > DIAG_MAX_RX_PKT_SIZ)
            {
                m_strErrInfo.Format("The Received Data from the Phone is Too Large");
                status = FALSE;
                continue;
            }
        }
        //读取手机返回的数据
        m_RxPacket.length = ReadData(m_RxPacket.buf,m_RxPacket.length); 
        //写LOG
        //LogMe(m_RxPacket.buf,m_RxPacket.length,false);

        //根据不同协议分解数据包
        if (m_Protocol == DMP)
        {
            //DMP的返回值总是和发送的命令一样，先做简单判断
            if ((m_RxPacket.buf[0] == 75) || (m_RxPacket.buf[0] == m_ReqPacket.req.cmd_code))
            {
                status = DividePacket();
                if (status || m_ReqPacket.req.cmd_code == DIAG_FS_OP_F)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (m_RxPacket.buf[0] == 0x13)
                {
                    m_strErrInfo.Format("Bad command");
                }
                else if (m_RxPacket.buf[0] == 0x14)
                {
                    m_strErrInfo.Format("The Request packet has invalid or inconsistent parameters");
                }
                else if (m_RxPacket.buf[0] == 0x15)
                {
                    m_strErrInfo.Format("The Request packet has an invalid length");
                }
                else if (m_RxPacket.buf[0] == 0x18)
                {
                    m_strErrInfo.Format("the DMSS isn't in a particular mode");
                }
                else if (m_RxPacket.buf[0] == 0x42)
                {
                    m_strErrInfo.Format("the correct SPC has not yet been entered to unlock service programming");
                }
                else if (m_RxPacket.buf[0] == 0x47)
                {
                    m_strErrInfo.Format("the correct Security Password has not yet been entered to unlock the phone");
                }
                else
                {
                    m_strErrInfo.Format("unknow reason");
                }
                status = FALSE;
                continue;
            }
        }
        else if (m_Protocol == DLP || m_Protocol == SDP)
        {
            status = DividePacket();
            if (status)
            {
                break;
            }
            else
            {
                continue;
            }
        }
    }

    return status;
}

dword CPacket::PacketCRCMake(byte *buffer, dword PacketLen)
{
    word    CRC = CRC_SEED;

    for (dword i = 0; i < PacketLen; i++)
        CRC = CRC_COMPUTE(CRC,*buffer++);

    CRC ^= 0xFFFF;
    *buffer++ = (byte) CRC;
    *buffer = (byte) (CRC >> 8);

    return (PacketLen + 2);
}



/*----------------------------------------------------------------------------*/
void CPacket::CheckSendPassWord()  //对应向手机发口令
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.checkpassword_req.cmd_code = DLOAD_CHECKPASS_F;
    m_ReqPacket.req.checkpassword_req.checkpassword[0] = 0x08;
    m_ReqPacket.req.checkpassword_req.checkpassword[1] = 0x08;
    m_ReqPacket.req.checkpassword_req.checkpassword[2] = 0x00;
    m_ReqPacket.req.checkpassword_req.checkpassword[3] = 0x00;

    RxTxPacket(1,CHECK_TIMER);
}

BOOL CPacket::CheckSoftVer()     //对应从手机中读取软件是否锁网标识
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.checksoftver_req.cmd_code = DLOAD_CHECKVER_F;
    if (!RxTxPacket(1,WRITE_TIMER))
    {
        m_strTempInfo.Format("Cannot Get the checksoftver of the Phone, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
/*---------------------------------------------------------------------------*/
//2004年5月27日由刘晓军添加



BOOL CPacket::PacketCRCCheck(byte *buffer, word PacketLen)
{
    word    CRC = CRC_SEED;

    for (int i = 0; i < PacketLen; i++)
        CRC = CRC_COMPUTE(CRC,buffer[i]);

    if (CRC != CRC_END)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::ResetDMP()
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.cont.cmd_code = DIAG_CONTROL_F;
    m_ReqPacket.req.cont.mode = MODE_RESET_F;

    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Cannot Reset the Phone, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::EnterOfflineMode()
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.cont.cmd_code = DIAG_CONTROL_F;
    m_ReqPacket.req.cont.mode = MODE_OFFLINE_D_F;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Cannot Put the Phone Offline, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::EnterDMSS()
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.cont.cmd_code = DIAG_CONTROL_F;
    m_ReqPacket.req.cont.mode = 4;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Cannot Put the Phone Offline, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::GetMobileStatus(diag_status_rsp_type &status)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.stat.cmd_code = DIAG_STATUS_F;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Cannot get mobile status from the mobile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        if (m_RxPacket.length < 24)
        {
            return FALSE;
        }
        status = m_RspPacket.rsp.stat;
        return TRUE;
    }
}

BOOL CPacket::GetEsnNum(dword &esn)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.esn.cmd_code = DIAG_ESN_F;
    if (!RxTxPacket(1,QUERY_TIMER))
    {
        m_strTempInfo.Format("Cannot Get the ESN Number of the Phone, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        esn = 0xffffffff;
        return FALSE;
    }
    else
    {
        esn = m_RspPacket.rsp.esn.esn;
        return TRUE;
    }
}

BOOL CPacket::GetSoftwareVersion(diag_sw_ver_rsp_type &version)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.sw_ver.cmd_code = DIAG_SW_VERSION_F;
    if (!RxTxPacket(1,QUERY_TIMER))
    {
        m_strTempInfo.Format("Cannot get software version from the mobile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        memset(&version,0,sizeof(diag_sw_ver_rsp_type));
        return FALSE;
    }
    else
    {
        strcpy(version.sw_ver_str,m_RspPacket.rsp.sw_ver.sw_ver_str);
        return TRUE;
    }
}

BOOL CPacket::GetExtSoftwareVersion(diag_sw_ver_rsp_type &version)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.sw_ver.cmd_code = DIAG_EXT_SW_VERSION_F;
    if (!RxTxPacket(1,QUERY_TIMER))
    {
        m_strTempInfo.Format("Cannot get software version from the mobile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        memset(&version,0,sizeof(diag_sw_ver_rsp_type));
        return FALSE;
    }
    else
    {
        strcpy(version.sw_ver_str,m_RspPacket.rsp.sw_ext_ver.sw_ver_str);
        return TRUE;
    }
}

BOOL CPacket::AT_QCDMG()
{
    char    at[9]               =
    {
        0x41, 0x54, 0x24, 0x51, 0x43, 0x44, 0x4D, 0x47, 0x0D
    };
    char    at_desired_ret[15]  =
    {
        0x41, 0x54, 0x24, 0x51, 0x43, 0x44, 0x4D, 0x47, 0x0D, 0x0D, 0x0A, 0x4F, 0x4B, 0x0D, 0x0A
    };
    char    at_ret[15];
    WriteCommBytes(at,9);
    ::Sleep(100);
    int len = ReadDataWaiting();
    if (len > 0)
    {
        ReadData(at_ret,15);
    }
    if (_strnicmp(at_ret,at_desired_ret,15) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CPacket::CommunicateWithMobile()
{
    dword   esn = 0;
    if (GetEsnNum(esn))
    {
        return TRUE;
    }
    else
    {
        if (AT_QCDMG())
        {
            return TRUE;
        }
        else
        {
            if (!GetEsnNum(esn))
            {
                m_strTempInfo.Format("Cannot communicate with the mobile, Since %s",m_strErrInfo);
                m_strErrInfo = m_strTempInfo;
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
}

BOOL CPacket::AT_DLOAD()
{
    char    at[9]               =
    {
        0x41, 0x54, 0x24, 0x44, 0x4C, 0x4F, 0x41, 0x44, 0x0D
    };
    char    at_desired_ret[9]   =
    {
        0x41, 0x54, 0x24, 0x44, 0x4C, 0x4F, 0x41, 0x44, 0x0D
    };
    char    at_ret[9];
    WriteCommBytes(at,9);
    ::Sleep(100);
    int len = ReadDataWaiting();
    if (len > 0)
    {
        ReadData(at_ret,9);
    }
    if (_strnicmp(at_ret,at_desired_ret,9) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CPacket::NvItemWrite(word item, nv_item_type item_data, word &status)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.nvwrite.cmd_code = DIAG_NV_WRITE_F;
    m_ReqPacket.req.nvwrite.item = item;
    status = NV_DONE_S;
    m_ReqPacket.req.nvwrite.nv_stat = status;
    m_ReqPacket.req.nvwrite.item_data = item_data;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot write #%d NVItem, Since %s",item,m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        status = NV_RWOPFAILED_S;
        return FALSE;
    }
    else
    {
        if (m_RspPacket.rsp.nvwrite.nv_stat == NV_DONE_S)
        {
            status = NV_DONE_S;
            return TRUE;
        }
        else
        {
            m_strErrInfo.Format("Cannot Write #%d NVItem, Since %s",
                                item,
                                MapNVOperateError((nv_stat_enum_type) m_RspPacket.rsp.nvwrite.nv_stat));
            status = m_RspPacket.rsp.nvwrite.nv_stat;
            return FALSE;
        }
    }
}

BOOL CPacket::NvItemRead(word item, nv_item_type &item_data, word &status)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.nvread.cmd_code = DIAG_NV_READ_F;
    m_ReqPacket.req.nvread.item = item;
    m_ReqPacket.req.nvread.item_data = item_data;
    m_ReqPacket.req.nvread.nv_stat = NV_DONE_S;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("cannot read #%d NVItem, since %s",item,m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        status = NV_RWOPFAILED_S;
        memset(&item_data,0,sizeof(item_data));
        return FALSE;
    }
    else
    {
        if (m_RspPacket.rsp.nvread.nv_stat == NV_DONE_S)
        {
            status = NV_DONE_S;
            item_data = m_RspPacket.rsp.nvread.item_data;
            return TRUE;
        }
        else
        {
            m_strErrInfo.Format("cannot read #%d NV Item, since %s",
                                item,
                                MapNVOperateError((nv_stat_enum_type) m_RspPacket.rsp.nvread.nv_stat));
            status = m_RspPacket.rsp.nvread.nv_stat;
            memset(&item_data,0,sizeof(item_data));
            return FALSE;
        }
    }
}

BOOL CPacket::DoNoop()
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.powerdown.cmd_code = DLOAD_NOP_F;
    if ((!RxTxPacket(5)) || (m_RspPacket.rsp.cmd_code != DLOAD_ACK_F))
    {
        m_strTempInfo.Format("Cannot Do No Operation, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::SwitchDownloader()
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.dload.cmd_code = DIAG_DLOAD_F;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot Switch to Downloader, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::Unlock(const byte *seccode)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.unlock.cmd_code = DLOAD_UNLOCK_F;
    memcpy(m_ReqPacket.req.unlock.security_code,seccode,sizeof(m_ReqPacket.req.unlock.security_code));
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot Unlock Access to Secured Operations, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::PowerDown()
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.powerdown.cmd_code = DLOAD_PWROFF_F;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Cannot Turn Phone Power Off, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::ResetDLP()
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.reset.cmd_code = DLOAD_RESET_F;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot Reset the Phone, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::ParamRequest(dload_params_rsp_type &params)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.params.cmd_code = DLOAD_PREQ_F;
    if (!RxTxPacket(3,3000))
    {
        m_strTempInfo.Format("Cannot Get flash Information, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        memset(&params,0,sizeof(dload_params_rsp_type));
        return FALSE;
    }
    else
    {
        params = m_RspPacket.rsp.params;
        return TRUE;
    }
}

BOOL CPacket::Go(word segment, word offset)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.go.cmd_code = DLOAD_GO_F;
    int     i   = 0;
    byte   *seg = (byte *) &segment;
    byte   *off = (byte *) &offset;
    for (i = 0; i < 2; i++)
    {
        m_ReqPacket.req.go.segment[i] = seg[i];
        m_ReqPacket.req.go.offset[i] = off[i];
    }
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot begin execution at an address, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::Go_Rep(word segment, word offset)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.gorep.cmd_code = DLOAD_GO_REP_F;
    int     i   = 0;
    byte   *seg = (byte *) &segment;
    byte   *off = (byte *) &offset;
    for (i = 0; i < 2; i++)
    {
        m_ReqPacket.req.gorep.segment[i] = seg[i];
        m_ReqPacket.req.gorep.offset[i] = off[i];
    }
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot Begin Execution at an Address for Repair, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::Erase(dword address, dword length)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.erase.cmd_code = DLOAD_ERASE_F;
    byte   *add = (byte *) &address;
    byte   *len = (byte *) &length;
    for (int i = 0; i < 3; i++)
    {
        m_ReqPacket.req.erase.address[i] = add[i];
        m_ReqPacket.req.erase.length[i] = len[i];
    }
    if (!RxTxPacket(3,ERASE_TIMER))
    {
        m_strTempInfo.Format("Erase a Block of Memory Failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::Write(const byte *blockbuf, dword address, dword length)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.write.cmd_code = DLOAD_WRITE_F;
    byte   *add = (byte *) &address;
    byte   *len = (byte *) &length;
    for (int i = 0; i < 4; i++)
    {
        m_ReqPacket.req.write.address[i] = add[i];
    }
    for (int j = 0; j < 2; j++)
    {
        m_ReqPacket.req.write.length[j] = len[j];
    }
    memcpy(m_ReqPacket.req.write.buf,blockbuf,length);
    if (!RxTxPacket(1,WRITE_TIMER))
    {
        m_strTempInfo.Format("Write a Block of Data to Memory Failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

CString CPacket::MapDloadError(word reason)
{
    CString strDloadError   = _T("");
    switch (reason)
    {
        case NAK_INVALID_FCS:
        strDloadError = _T("invalid frame check sequence");
        break;
        case NAK_INVALID_DEST:
        strDloadError = _T("destination address is invalid");
        break;
        case NAK_INVALID_LEN:
        strDloadError = _T("operation length is invalid");
        break;
        case NAK_EARLY_END:
        strDloadError = _T("packet was too short for this cmd");
        break;
        case NAK_TOO_LARGE:
        strDloadError = _T("packet was too long for phone's buffer");
        break;
        case NAK_INVALID_CMD:
        strDloadError = _T("packet command code was unknown");
        break;
        case NAK_FAILED:
        strDloadError = _T("operation did not succeed");
        break;
        case NAK_WRONG_IID:
        strDloadError = _T("intelligent ID code was wrong");
        break;
        case NAK_BAD_VPP:
        strDloadError = _T("programming voltage out of spec");
        break;
        case NAK_VERIFY_FAILED:
        strDloadError = _T("readback verify did not match");
        break;
        case NAK_NO_SEC_CODE:
        strDloadError = _T("not permitted without unlock");
        break;
        case NAK_BAD_SEC_CODE:
        strDloadError = _T("invalid security code");
        break;
        default:
        break;
    }
    return strDloadError;
}

CString CPacket::MapNVOperateError(nv_stat_enum_type stat)
{
    CString strError    = _T("");
    switch (stat)
    {
        case NV_BUSY_S:
        strError = _T("Request is queued");
        break;
        case NV_BADCMD_S:
        strError = _T("Unrecognizable command field");
        break;
        case NV_FULL_S:
        strError = _T("The NVM is full");
        break;
        case NV_FAIL_S:
        strError = _T("Command failed, reason other than NVM was full");
        break;
        case NV_NOTACTIVE_S:
        strError = _T("Variable was not active");
        break;
        case NV_BADPARM_S:
        strError = _T("Bad parameter in command block");
        break;
        case NV_READONLY_S:
        strError = _T("Parameter is write-protected and thus read only");
        break;
        case NV_BADTG_S:
        strError = _T("Item not valid for Target");
        break;
        case NV_NOMEM_S:
        strError = _T("free memory exhausted");
        break;
        case NV_NOTALLOC_S:
        strError = _T("address is not a valid allocation ");
        break;
        default:
        break;
    }

    return strError;
}

BOOL CPacket::ChangeUartRate(int baudrate)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.changerate.cmd_code = DLOAD_CHANGE_RATE_F;
    switch (baudrate)
    {
        case 38400:
        m_ReqPacket.req.changerate.rate_type = UART_RATE_38400;
        break;
        case 57600:
        m_ReqPacket.req.changerate.rate_type = UART_RATE_57600;
        break;
        case 153600:
        m_ReqPacket.req.changerate.rate_type = UART_RATE_153600;
        break;
        case 230400:
        m_ReqPacket.req.changerate.rate_type = UART_RATE_230400;
        break;
        case 115200:
        default:
        m_ReqPacket.req.changerate.rate_type = UART_RATE_115200;
        break;
    }
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Change UART baud rate failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::CommConfig()
{
    //Communications Configuration Request Message
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.cmd_code = DIAG_COMMCONFIG_F;
    if (!RxTxPacket(10,2500))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CPacket::SendSpc(CString spc)
{
    //Communications Configuration Request Message
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.spc.cmd_code = DIAG_SPC_F;
    for (int i = 0; i < 6; i++)
    {
        m_ReqPacket.req.spc.password[i] = spc.GetAt(i);
    }

    if (!RxTxPacket(1))
    {
        return FALSE;
    }
    if (m_RxPacket.buf[1] != 1)
    {
        return FALSE;
    }
    return TRUE;
}

void CPacket::EmptyVaiables()
{
    m_strErrInfo = "";
    m_strTempInfo = "";
    //  memset(&m_RxPacket, 0, sizeof(rx_pkt_buf_type));
    memset(&m_TxPacket,0,sizeof(tx_pkt_buf_type));
}

BOOL CPacket::Write32(const byte *blockbuf, int address, int length)
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.write.cmd_code = DLOAD_WRITE32_F;
    byte   *add = (byte *) &address;
    byte   *len = (byte *) &length;
    for (int i = 0; i < 4; i++)
    {
        m_ReqPacket.req.write.address[i] = add[i];
    }
    for (int j = 0; j < 2; j++)
    {
        m_ReqPacket.req.write.length[j] = len[j];
    }
    memcpy(m_ReqPacket.req.write.buf,blockbuf,length);
    if (!RxTxPacket(1,WRITE_TIMER))
    {
        m_strTempInfo.Format("Write a Block of Data to Memory Failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::Hello()
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.cmd_code = SDP_HELLO_F;

    //if(!RxTxPacket(3, WRITE_TIMER))
    if (!RxTxPacket(5,3000))
    {
        return FALSE;
    }
    if (m_RxPacket.buf[0] != SDP_HELLORSP_F)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::OpenPacket(int mode)
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.cmd_code = SDP_OPEN_F;
    m_ReqPacket.req.openpacket.mode = mode; 
    if (!RxTxPacket(1,WRITE_TIMER))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CPacket::ClosePacket()
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.cmd_code = SDP_CLOSE_F;
    if (!RxTxPacket(3,10000))
    {
        return FALSE;
    }
    return TRUE;
}
BOOL CPacket::SimpleWrite(int address, int length)
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.write.cmd_code = SDP_SWRITE_F;
    byte   *add = (byte *) &address;
    byte   *len = (byte *) &length;
    for (int i = 0; i < 4; i++)
    {
        m_ReqPacket.req.write.address[i] = add[i];
    }
    for (int j = 0; j < 4; j++)
    {
        m_ReqPacket.req.write.length[j] = len[j];
    }

    if (!RxTxPacket(3,WRITE_TIMER))
    {
        m_strTempInfo.Format("Write a Block of Data to Memory Failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::StreamWrite(const byte *blockbuf, int address, int length)
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.write.cmd_code = SDP_WRITE_F;
    byte   *add = (byte *) &address;
    byte   *len = (byte *) &length;
    for (int i = 0; i < 4; i++)
    {
        m_ReqPacket.req.write.address[i] = add[i];
    }
    for (int j = 0; j < 4; j++)
    {
        m_ReqPacket.req.write.length[j] = len[j];
    }
    memcpy(m_ReqPacket.req.write.buf,blockbuf,length);

    //  if(!RxTxPacket(3, WRITE_TIMER))

    //if(!DLRxTxPacket(1000)) old 
    if (!RxTxPacket(3,40000)) // add  time for erase efs
    {
        m_strTempInfo.Format("Write a Block of Data to Memory Failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }

    if (address == 0xffffffffL)     // erase efs judge if success
    {
        if (m_RxPacket.buf[0] != 0x7E)
        {
            int ntempcnt    = 0;
            do
            {
                DLRxPacket();
                if (m_RxPacket.buf[0] == 0x7e && m_RxPacket.buf[1] == 0x08)
                {
                    break;
                }

                ::Sleep(500);

                ntempcnt++;
            }
            while (ntempcnt < 20);
        }
        else
        {
            if (m_RxPacket.buf[1] == 0x08)
            {
                return TRUE;
            }
            //          m_RxPacket.buf[0] = m_RxPacket.buf[1];
            //          m_RxPacket.buf[1] = m_RxPacket.buf[2];
            //          m_RxPacket.buf[2] = m_RxPacket.buf[3];
        }
        //根据不同协议分解数据包
        //      if(m_RxPacket.buf[0] != 0x08 || m_RxPacket.buf[1] != 0x1F || m_RxPacket.buf[2] != 0xF1)
        //          return FALSE;
    }


    return TRUE;
}

BOOL CPacket::Nop()
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.cmd_code = SDP_NOP_F;
    if (!RxTxPacket(1,WRITE_TIMER))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::ResetSDP()
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.cmd_code = SDP_RESET_F;
    if (!RxTxPacket(1,WRITE_TIMER))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::LogMe(const byte *buf, int len, bool bWrite)
{
    //需要注意多线程同时写造成访问冲突的问题
    // get current directory

    char    szPath[128];
    ::GetCurrentDirectory(128,szPath);

    CStdioFile  file;
    CString     str, str2;
    CString     FileName;
    CString     MyInfo;
    CTime       T;
    CString     strTime;

    FileName.Format("%s\\log%d.txt",szPath,m_nPort);

    file.Open(FileName,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText,NULL);
    file.SeekToEnd();

    str.Format("Len %d: ",len); 

    //  if(len > 10) 
    //      len = 10;

    for (int i = 0; i < len; i++)
    {
        str2.Format("%X",buf[i]);
        if (str2.GetLength() == 1)
        {
            str = str + "0" + str2 + " ";
        }
        else
        {
            str = str + str2 + " ";
        }
    }
    if (bWrite)
    {
        str = "W: " + str + '\n';
    }
    else
    {
        str = "R: " + str + '\n';
    }
    file.Write(str,str.GetLength());
    file.Close();

    return TRUE;
}   

void CPacket::LogCls()
{
    // get current directory
    char    szPath[128];
    ::GetCurrentDirectory(128,szPath);

    CStdioFile  file;
    CString     FileName;

    FileName.Format("%s\\log%d.txt",szPath,m_nPort);

    file.Open(FileName,CFile::modeCreate | CFile::typeText,NULL);
    file.Close();
}

BOOL CPacket::DLTxPacket()
{
    dword   TxPacketLen;
    //写LOG
    //LogMe(m_TxPacket.buf,m_TxPacket.length,true);
    //往串口写数据
    //  SleepMe();
    TxPacketLen = DLWriteCommBytes((char *) m_TxPacket.buf,m_TxPacket.length);
    if (TxPacketLen != m_TxPacket.length)
    {
        m_strErrInfo.Format("Port Cannot Send desired data to the Phone");
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::DLRxPacket()
{
    //读取手机返回的数据
    m_RxPacket.length = ReadData(m_RxPacket.buf,1000);  
    if (m_RxPacket.length <= 0)
    {
        m_retPacket.len = 0 ;
        return TRUE;
    }
    //写LOG
    //LogMe(m_RxPacket.buf,m_RxPacket.length,false);
    //分解数据包
    if (!DLDividePacket())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::DLDividePacket()
{
    //根据发送的请求信息，分解收到的数据包，结果放在m_RspPacket中
    word    i   = 0;
    word    start, end;
    int     cnt;

    m_retPacket.len = 0;

    //下载协议有协议头7E，从第二位开始计算
    if (m_RxPacket.buf[0] != ASYNC_HDLC_FLAG)  //有时协议头会丢失，需要补上。有隐患
    {
        for (int j = 0; j < m_RxPacket.length; j++)
        {
            m_RxPacket.buf[m_RxPacket.length - j] = m_RxPacket.buf[m_RxPacket.length - j - 1];
        }
        m_RxPacket.buf[0] = ASYNC_HDLC_FLAG;
        m_RxPacket.length += 1;
    }

    /*分解接收包，如果是正确返回取出地址*/
    cnt = 0;   //cnt指示当前是否已经读出过一个协议头了
    for (i = 0 ; i < m_RxPacket.length ; i++)
    {
        if (m_RxPacket.buf[i] == ASYNC_HDLC_FLAG)
        {
            if (cnt)
            {
                end = i;
                cnt = 0;
                if (!CheckPacketVal(start,end))
                {
                    return FALSE;
                }
            }
            else
            {
                start = i;
                cnt = 1;
            }
        }
    }//for
    return TRUE;
}

BOOL CPacket::CheckPacketVal(word start, word end)
{
    byte   *tmpbuf;
    word    ValidLen, i, len;
    BOOL    bret    = TRUE;

    len = end - start - 1;  
    tmpbuf = new byte[len];
    ValidLen = 0;

    for (i = start + 1 ; i < end ; i++)
    {
        if (m_RxPacket.buf[i] == ASYNC_HDLC_ESC)
        {
            tmpbuf[ValidLen++] = m_RxPacket.buf[++i] ^ ASYNC_HDLC_ESC_MASK;
        }
        else
        {
            tmpbuf[ValidLen++] = m_RxPacket.buf[i];
        }
    }

    //验证校验码
    if (!PacketCRCCheck(tmpbuf,ValidLen))
    {
        m_strErrInfo = "the Received Data is Wrong with CRC Checksum";
        delete tmpbuf;
        return FALSE;
    }

    byte    add[4];
    byte    err[4];
    dword   errcode;

    switch (tmpbuf[0])
    {
        case SDP_HELLORSP_F:
        break;

        case SDP_READRSP_F:
        break;

        case SDP_SWRITERSP_F:
        break;

        case SDP_WRITERSP_F:
        for (i = 0 ; i < 4 ; i++)
            add[i] = tmpbuf[1 + i];
        m_retPacket.ret[m_retPacket.len++] = *(int *) add;

        break;

        case SDP_NOPRSP_F:
        break;

        case SDP_RESETRSP_F:
        break;

        case SDP_ERROR_F:
        for (i = 0 ; i < 4 ; i++)
            err[i] = tmpbuf[1 + i]; 
        memcpy(&errcode,err,4);
        if (errcode == 2)
        {
            m_strErrInfo = "Invalid destination address";
        }
        else if (errcode == 3)
        {
            m_strErrInfo = "Invalid length";
        }
        else if (errcode == 4)
        {
            m_strErrInfo = "Unexpected end of packet";
        }
        else if (errcode == 5)
        {
            m_strErrInfo = "Invalid Command";
        }
        else if (errcode == 7)
        {
            m_strErrInfo = "Operation failed";
        }
        else if (errcode == 8)
        {
            m_strErrInfo = "Wrong Flash Intelligent ID";
        }
        else if (errcode == 9)
        {
            m_strErrInfo = "Bad programming voltage";
        }
        else if (errcode == 10)
        {
            m_strErrInfo = "Write-verify failed";
        }
        else if (errcode == 12)
        {
            m_strErrInfo = "Incorrect security code";
        }
        else if (errcode == 13)
        {
            m_strErrInfo = "Cannot power down phone";
        }
        else if (errcode == 14)
        {
            m_strErrInfo = "NAND flash programming not supported";
        }
        else if (errcode == 15)
        {
            m_strErrInfo = "Command out of sequence";
        }
        else if (errcode == 16)
        {
            m_strErrInfo = "Close did not succeed";
        }
        else if (errcode == 17)
        {
            m_strErrInfo = "Incompatible feature bits";
        }
        else if (errcode == 18)
        {
            m_strErrInfo = "Out of space";
        }
        else
        {
            m_strErrInfo = "Illegal reason";
        }

        bret = FALSE;

        break;

        case SDP_LOG_F:
        break;

        case SDP_UNLOCKRSP_F:
        break;

        case SDP_POWERRSP_F:
        break;

        case SDP_OPENRSP_F:
        break;

        case SDP_CLOSERSP_F:
        break;
		case SDP_HSDL_RSP_F_NEW://ruiju add
			m_RspPacket.rsp.hdlcrsp.cmd_code = tmpbuf[0];
			m_RspPacket.rsp.hdlcrsp.retType = tmpbuf[1];
			m_RspPacket.rsp.hdlcrsp.retResult = tmpbuf[2];
			//m_RspPacket.rsp.hdlcrsp.reserved[0] = tmpbuf[3];
			//m_RspPacket.rsp.hdlcrsp.reserved[1] = tmpbuf[4];
		break;
        default:
        break;
    }


    delete tmpbuf;
    return bret;
}

BOOL CPacket::StreamWriteAsy(const byte *blockbuf, int address, int length)
{
    /* 清空发送和接收的数据区（可以考虑省略） */
    EmptyVaiables();
    //封装数据包
    DLCombinePacket(blockbuf,address,length);
    /* 发送到串口 */
    if (DLTxPacket())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CPacket::DLRxTxPacket(dword wait_ms)
{
    //组装数据包并向串口写入数据，然后读出串口的返回信息存放在m_RxPacket中
    BOOL    bReceive    = FALSE;
    dword   tBegin = 0, tEnd = 0, dtime;
    dword   tmpLength   = 0;
    dword   TxPacketLen;
    SleepMe();
    TxPacketLen = CombinePacket();  //组装数据包

    for (int i = 0; i < 4; i++)  //20061025
    {
        //写LOG
        //LogMe(m_TxPacket.buf,m_TxPacket.length,true);
        //往串口写数据
        TxPacketLen = DLWriteCommBytes((char *) m_TxPacket.buf,m_TxPacket.length);
        if (TxPacketLen != m_TxPacket.length)
        {
            m_strErrInfo = "Port Cannot Send desired data to the Phone";
            return FALSE;
        }
        //监视是否有正确的返回数据
        tBegin = GetTickCount();
        do
        {
            ::Sleep(20);

            m_RxPacket.length = ReadDataWaiting();

            ::Sleep(10);

            tmpLength = ReadDataWaiting();

            if (m_RxPacket.length >= 1 && tmpLength == m_RxPacket.length)
            {
                bReceive = TRUE;
                break;
            }

            tEnd = GetTickCount();
        }
        while ((tEnd - tBegin) <= wait_ms);

        /* for test time on using this operation */
        tEnd = GetTickCount();
        dtime = tEnd - tBegin;
        //  m_strErrInfo.Format("time is %d(ms)",dtime);

        //lichenyang 20061109
        if (m_TxPacket.length < 1000 && waitsign == 1)
        {
            waitsign = 0;
            //Sleep(5000);
            return TRUE;
        }

        //lichenyang 20061109

        //读取手机返回的数据
        if (bReceive)
        {
            m_RxPacket.length = ReadData(m_RxPacket.buf,m_RxPacket.length); 
            //写LOG
            //LogMe(m_RxPacket.buf,m_RxPacket.length,false);
            //根据不同协议分解数据包
            if (!DLDividePacket())
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void CPacket::DLCombinePacket(const byte *blockbuf, int address, int len)
{
    byte   *add = (byte *) &address;

    /* 计算包头的命令结构 */
    byte    Headbuf[5];
    Headbuf[0] = SDP_WRITE_F;
    for (int i = 0 ; i < 4 ; i++)
    {
        Headbuf[i + 1] = add[i];
    }

    /* 计算CRC校验码 */
    byte    Crcbuf[2];
    word    CRC = CRC_SEED;
    for (i = 0 ; i < 5 ; i++)
        CRC = CRC_COMPUTE(CRC,Headbuf[i]);

    for (i = 0; i < len; i++)
        CRC = CRC_COMPUTE(CRC,blockbuf[i]);

    CRC ^= 0xFFFF;
    Crcbuf[0] = (byte) CRC;
    Crcbuf[1] = (byte) (CRC >> 8);

    /* 组装完整的发送包 */
    dword   ValidLen    = 1;
    /* 检查包头是否含有特别字节并传送到发送缓冲区 */
    for (i = 0 ; i < 5 ; i++)
    {
        if ((Headbuf[i] == ASYNC_HDLC_FLAG) || (Headbuf[i] == ASYNC_HDLC_ESC))
        {
            m_TxPacket.buf[ValidLen++] = ASYNC_HDLC_ESC;
            m_TxPacket.buf[ValidLen++] = Headbuf[i] ^ (byte) ASYNC_HDLC_ESC_MASK;
        }
        else
        {
            m_TxPacket.buf[ValidLen++] = Headbuf[i];
        }
    }
    /* 检查包的数据部分是否含有特别字节并传送到发送缓冲区 */
    for (i = 0 ; i < len ; i++)
    {
        if ((blockbuf[i] == ASYNC_HDLC_FLAG) || (blockbuf[i] == ASYNC_HDLC_ESC))
        {
            m_TxPacket.buf[ValidLen++] = ASYNC_HDLC_ESC;
            m_TxPacket.buf[ValidLen++] = blockbuf[i] ^ (byte) ASYNC_HDLC_ESC_MASK;
        }
        else
        {
            m_TxPacket.buf[ValidLen++] = blockbuf[i];
        }
    }
    /* 检查包的CRC校验部分是否含有特别字节并传送到发送缓冲区 */
    for (i = 0 ; i < 2 ; i++)
    {
        if ((Crcbuf[i] == ASYNC_HDLC_FLAG) || (Crcbuf[i] == ASYNC_HDLC_ESC))
        {
            m_TxPacket.buf[ValidLen++] = ASYNC_HDLC_ESC;
            m_TxPacket.buf[ValidLen++] = Crcbuf[i] ^ (byte) ASYNC_HDLC_ESC_MASK;
        }
        else
        {
            m_TxPacket.buf[ValidLen++] = Crcbuf[i];
        }
    }

    /* 增加协议头和尾0x7E */
    m_TxPacket.buf[0] = ASYNC_HDLC_FLAG;
    m_TxPacket.buf[ValidLen++] = ASYNC_HDLC_FLAG;

    m_TxPacket.length = ValidLen;

    return;
}

void CPacket::SleepMe()
{
    ::Sleep(5);
}

/*EFS Operation*/
BOOL CPacket::DisplayDirList(diag_fs_filename_type filename, diag_fs_disp_dirs_rsp_type &fs_disp_dirs)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_DISP_DIRS;
    m_ReqPacket.req.fs_op.fs_req.disp_dirs = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Display directory list operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_disp_dirs = m_RspPacket.rsp.fs_op.fs_rsp.disp_dirs;
        return TRUE;
    }
}

BOOL CPacket::DisplayFileList(diag_fs_filename_type filename, diag_fs_disp_files_rsp_type &fs_disp_files)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_DISP_FILES;
    m_ReqPacket.req.fs_op.fs_req.disp_files = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Display file list operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_disp_files = m_RspPacket.rsp.fs_op.fs_rsp.disp_files;
        return TRUE;
    }
}

BOOL CPacket::IterativeDirList(int seqnum,
                               diag_fs_filename_type filename,
                               diag_fs_iter_rsp_type &fs_iter,
                               BOOL &bDone)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_ITER_DIRS;
    m_ReqPacket.req.fs_op.fs_req.iter_dirs.seq_num = seqnum;
    m_ReqPacket.req.fs_op.fs_req.iter_dirs.dir_name = filename;
    bDone = FALSE;
    if (!RxTxPacket())
    {
        if (m_RspPacket.rsp.fs_op.fs_status == FS_ENUM_DONE_S)
        {
            bDone = TRUE;
        }
        m_strTempInfo.Format("Iterative directory list operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_iter = m_RspPacket.rsp.fs_op.fs_rsp.iter;
        return TRUE;
    }
}

BOOL CPacket::IterativeFileList(int seqnum,
                                diag_fs_filename_type filename,
                                diag_fs_iter_rsp_type &fs_iter,
                                BOOL &bDone)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_ITER_FILES;
    m_ReqPacket.req.fs_op.fs_req.iter_files.seq_num = seqnum;
    m_ReqPacket.req.fs_op.fs_req.iter_files.dir_name = filename;
    bDone = FALSE;
    if (!RxTxPacket())
    {
        if (m_RspPacket.rsp.fs_op.fs_status == FS_ENUM_DONE_S)
        {
            bDone = TRUE;
        }
        m_strTempInfo.Format("Iterative file list operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_iter = m_RspPacket.rsp.fs_op.fs_rsp.iter;
        return TRUE;
    }
}

BOOL CPacket::RemoveFile(diag_fs_filename_type filename)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_REMOVE_FILE;
    m_ReqPacket.req.fs_op.fs_req.rmfile = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Remove file operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::CreateDirectory(diag_fs_filename_type filename)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_MK_DIR;
    m_ReqPacket.req.fs_op.fs_req.mkdir = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("create directory operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::RemoveDir(diag_fs_filename_type filename)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_RM_DIR;
    m_ReqPacket.req.fs_op.fs_req.rmdir = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Remove directory operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::GetFileAttributes(diag_fs_filename_type filename, diag_fs_get_attrib_rsp_type &fs_get_attrib)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_GET_ATTRIB;
    m_ReqPacket.req.fs_op.fs_req.get_attrib = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Get file attributes operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_get_attrib = m_RspPacket.rsp.fs_op.fs_rsp.get_attrib;
        return TRUE;
    }
}

BOOL CPacket::SetFileAttributes(diag_fs_filename_type filename, diag_fs_attributes_type attr, dword cdate)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_SET_ATTRIB;
    m_ReqPacket.req.fs_op.fs_req.set_attrib.filename_info = filename;
    m_ReqPacket.req.fs_op.fs_req.set_attrib.attribs.attribute_mask = attr.attribute_mask;
    m_ReqPacket.req.fs_op.fs_req.set_attrib.attribs.buffering_option = attr.buffering_option;
    m_ReqPacket.req.fs_op.fs_req.set_attrib.attribs.cleanup_option = attr.cleanup_option;
    m_ReqPacket.req.fs_op.fs_req.set_attrib.creation_date = cdate;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Set file attributes operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::RemoteFileLink(dword address, dword size, diag_fs_filename_type filename)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_REMOTE_LINK;
    m_ReqPacket.req.fs_op.fs_req.remote_link.base_address = address;
    m_ReqPacket.req.fs_op.fs_req.remote_link.length = size;
    m_ReqPacket.req.fs_op.fs_req.remote_link.name_info = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Remote file link operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL CPacket::ReadSpaceAvailiable(dword &space)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_SPACE_AVAIL;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Space available operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        space = m_RspPacket.rsp.fs_op.fs_rsp.space_avail;
        return TRUE;
    }
}

BOOL CPacket::ReadFileFirstBlock(diag_fs_filename_type filename, diag_fs_read_rsp_type &fs_read)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_READ_FILE;
    m_ReqPacket.req.fs_op.fs_req.read.seq_num = 0;
    m_ReqPacket.req.fs_op.fs_req.read.filename_info = filename;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Read file first block operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_read = m_RspPacket.rsp.fs_op.fs_rsp.read;
        return TRUE;
    }
}

BOOL CPacket::ReadFileSequenceBlock(byte seqnum, diag_fs_read_rsp_type &fs_read)
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_READ_FILE;
    m_ReqPacket.req.fs_op.fs_req.read.seq_num = seqnum;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Read file sequence block operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        fs_read = m_RspPacket.rsp.fs_op.fs_rsp.read;
        return TRUE;
    }
}

BOOL CPacket::WriteFileFirstBlock(byte more,
                                  diag_fs_write_begin_req_type beginblock,
                                  diag_fs_write_rsp_type &fs_write,
                                  byte &status)
{
    EmptyVaiables();
    m_Protocol = DMP;
    status = 0;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_WRITE_FILE;
    m_ReqPacket.req.fs_op.fs_req.write.seq_num = 0;
    m_ReqPacket.req.fs_op.fs_req.write.more = more;
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.attrib.attribute_mask = beginblock.attrib.attribute_mask;
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.attrib.buffering_option = beginblock.attrib.buffering_option;
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.attrib.cleanup_option = beginblock.attrib.cleanup_option;
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.total_length = beginblock.total_length;
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.mode = beginblock.mode;
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.name_info.len = beginblock.name_info.len;
    strncpy(m_ReqPacket.req.fs_op.fs_req.write.block.begin.name_info.name,
            beginblock.name_info.name,
            beginblock.name_info.len);
    m_ReqPacket.req.fs_op.fs_req.write.block.begin.blockdata.len = beginblock.blockdata.len;
    memcpy(m_ReqPacket.req.fs_op.fs_req.write.block.begin.blockdata.data,
           beginblock.blockdata.data,
           beginblock.blockdata.len);
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Write file first block operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        status = m_RspPacket.rsp.fs_op.fs_status;
        return FALSE;
    }
    else
    {
        fs_write = m_RspPacket.rsp.fs_op.fs_rsp.write;
        status = m_RspPacket.rsp.fs_op.fs_status;
        return TRUE;
    }
}

BOOL CPacket::WriteFileSequenceBlock(byte seqnum,
                                     byte more,
                                     diag_fs_data_block_type seqblock,
                                     diag_fs_write_rsp_type &fs_write,
                                     byte &status)
{
    EmptyVaiables();
    m_Protocol = DMP;
    status = 0;
    m_ReqPacket.req.fs_op.cmd_code = DIAG_FS_OP_F;
    m_ReqPacket.req.fs_op.file_op = DIAG_FS_WRITE_FILE;
    m_ReqPacket.req.fs_op.fs_req.write.seq_num = seqnum;
    m_ReqPacket.req.fs_op.fs_req.write.more = more;
    m_ReqPacket.req.fs_op.fs_req.write.block.subseq.len = seqblock.len;
    memcpy(m_ReqPacket.req.fs_op.fs_req.write.block.subseq.data,seqblock.data,seqblock.len);
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Write file sequence block operation failed, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        status = m_RspPacket.rsp.fs_op.fs_status;
        return FALSE;
    }
    else
    {
        fs_write = m_RspPacket.rsp.fs_op.fs_rsp.write;
        status = m_RspPacket.rsp.fs_op.fs_status;
        return TRUE;
    }
}

word CPacket::CombineFSPacket()
{
    m_TxPacket.length = 0;
    m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.cmd_code;
    m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.file_op;

    switch (m_ReqPacket.req.fs_op.file_op)
    {
        case DIAG_FS_MK_DIR:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.mkdir.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.mkdir.name,
               m_ReqPacket.req.fs_op.fs_req.mkdir.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.mkdir.len;
        break;

        case DIAG_FS_RM_DIR:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.rmdir.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.rmdir.name,
               m_ReqPacket.req.fs_op.fs_req.rmdir.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.rmdir.len;
        break;

        case DIAG_FS_DISP_DIRS:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.disp_dirs.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.disp_dirs.name,
               m_ReqPacket.req.fs_op.fs_req.disp_dirs.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.disp_dirs.len;
        break;

        case DIAG_FS_DISP_FILES:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.disp_files.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.disp_files.name,
               m_ReqPacket.req.fs_op.fs_req.disp_files.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.disp_files.len;
        break;

        case DIAG_FS_READ_FILE:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.read.seq_num;
        if (m_ReqPacket.req.fs_op.fs_req.read.seq_num == 0)
        {
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.read.filename_info.len;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   m_ReqPacket.req.fs_op.fs_req.read.filename_info.name,
                   m_ReqPacket.req.fs_op.fs_req.read.filename_info.len);
            m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.read.filename_info.len;
        }
        break;

        case DIAG_FS_WRITE_FILE:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.write.seq_num;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.write.more;
        if (m_ReqPacket.req.fs_op.fs_req.write.seq_num == 0)
        {
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.write.block.begin.mode;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.fs_op.fs_req.write.block.begin.total_length,
                   sizeof(dword));
            m_TxPacket.length += sizeof(dword);
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.fs_op.fs_req.write.block.begin.attrib.attribute_mask,
                   sizeof(word));
            m_TxPacket.length += sizeof(word);
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.write.block.begin.attrib.buffering_option;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.write.block.begin.attrib.cleanup_option;
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.write.block.begin.name_info.len;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   m_ReqPacket.req.fs_op.fs_req.write.block.begin.name_info.name,
                   m_ReqPacket.req.fs_op.fs_req.write.block.begin.name_info.len);
            m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.write.block.begin.name_info.len;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.fs_op.fs_req.write.block.begin.blockdata.len,
                   sizeof(word));
            m_TxPacket.length += sizeof(word);
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   m_ReqPacket.req.fs_op.fs_req.write.block.begin.blockdata.data,
                   m_ReqPacket.req.fs_op.fs_req.write.block.begin.blockdata.len);
            m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.write.block.begin.blockdata.len;
        }
        else
        {
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.fs_op.fs_req.write.block.subseq.len,
                   sizeof(word));
            m_TxPacket.length += sizeof(word);
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   m_ReqPacket.req.fs_op.fs_req.write.block.subseq.data,
                   m_ReqPacket.req.fs_op.fs_req.write.block.subseq.len);
            m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.write.block.subseq.len;
        }
        break;

        case DIAG_FS_REMOVE_FILE:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.rmfile.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.rmfile.name,
               m_ReqPacket.req.fs_op.fs_req.rmfile.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.rmfile.len;
        break;

        case DIAG_FS_GET_ATTRIB:
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.get_attrib.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.get_attrib.name,
               m_ReqPacket.req.fs_op.fs_req.get_attrib.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.get_attrib.len;
        break;

        case DIAG_FS_SET_ATTRIB:
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               &m_ReqPacket.req.fs_op.fs_req.set_attrib.attribs.attribute_mask,
               sizeof(word));
        m_TxPacket.length += sizeof(word);
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.set_attrib.attribs.buffering_option;
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.set_attrib.attribs.cleanup_option;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               &m_ReqPacket.req.fs_op.fs_req.set_attrib.creation_date,
               sizeof(dword));
        m_TxPacket.length += sizeof(dword);
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.set_attrib.filename_info.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.set_attrib.filename_info.name,
               m_ReqPacket.req.fs_op.fs_req.set_attrib.filename_info.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.set_attrib.filename_info.len;
        break;

        case DIAG_FS_REMOTE_LINK:
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               &m_ReqPacket.req.fs_op.fs_req.remote_link.base_address,
               sizeof(dword));
        m_TxPacket.length += sizeof(dword);
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               &m_ReqPacket.req.fs_op.fs_req.remote_link.length,
               sizeof(dword));
        m_TxPacket.length += sizeof(dword);
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.remote_link.name_info.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.remote_link.name_info.name,
               m_ReqPacket.req.fs_op.fs_req.remote_link.name_info.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.remote_link.name_info.len;
        break;

        case DIAG_FS_ITER_DIRS:
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               &m_ReqPacket.req.fs_op.fs_req.iter_dirs.seq_num,
               sizeof(dword));
        m_TxPacket.length += sizeof(dword);
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.iter_dirs.dir_name.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.iter_dirs.dir_name.name,
               m_ReqPacket.req.fs_op.fs_req.iter_dirs.dir_name.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.iter_dirs.dir_name.len;
        break;

        case DIAG_FS_ITER_FILES:
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               &m_ReqPacket.req.fs_op.fs_req.iter_files.seq_num,
               sizeof(dword));
        m_TxPacket.length += sizeof(dword);
        m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.fs_op.fs_req.iter_files.dir_name.len;
        memcpy(&m_TxPacket.buf[m_TxPacket.length],
               m_ReqPacket.req.fs_op.fs_req.iter_files.dir_name.name,
               m_ReqPacket.req.fs_op.fs_req.iter_files.dir_name.len);
        m_TxPacket.length += m_ReqPacket.req.fs_op.fs_req.iter_files.dir_name.len;
        break;

        case DIAG_FS_SPACE_AVAIL:
        default:
        break;
    }
    return (word) m_TxPacket.length;
}

BOOL CPacket::DivideFSPacket()
{
    word    i   = 0;
    m_RspPacket.rsp.fs_op.cmd_code = m_RxPacket.buf[m_RxPacket.length++];   
    m_RspPacket.rsp.fs_op.file_op = m_RxPacket.buf[m_RxPacket.length++];
    m_RspPacket.rsp.fs_op.fs_status = m_RxPacket.buf[m_RxPacket.length++];
    if (m_RspPacket.rsp.fs_op.fs_status != FS_OKAY_S)
    {
        m_strErrInfo = EFSOperError(m_RspPacket.rsp.fs_op.fs_status);
        return FALSE;
    }
    switch (m_RspPacket.rsp.fs_op.file_op)
    {
        case DIAG_FS_DISP_DIRS:
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.disp_dirs.num_dirs,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(word));           
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.disp_dirs.dir_list.len,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(word));
        m_RxPacket.length += sizeof(word);
        for (i = 0; i < m_RspPacket.rsp.fs_op.fs_rsp.disp_dirs.dir_list.len; i++)
            m_RspPacket.rsp.fs_op.fs_rsp.disp_dirs.dir_list.data[i] = m_RxPacket.buf[m_RxPacket.length++];
        break;

        case DIAG_FS_DISP_FILES:
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.disp_files.num_files,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(word));
        m_RxPacket.length += sizeof(word);
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.disp_files.file_list.len,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(word));
        m_RxPacket.length += sizeof(word);
        for (i = 0; i < m_RspPacket.rsp.fs_op.fs_rsp.disp_files.file_list.len; i++)
            m_RspPacket.rsp.fs_op.fs_rsp.disp_files.file_list.data[i] = m_RxPacket.buf[m_RxPacket.length++];
        break;

        case DIAG_FS_READ_FILE:
        m_RspPacket.rsp.fs_op.fs_rsp.read.seq_num = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.fs_op.fs_rsp.read.more = m_RxPacket.buf[m_RxPacket.length++];
        if (m_RspPacket.rsp.fs_op.fs_rsp.read.seq_num == 0)
        {
            memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.read.data.begin.total_length,
                   &m_RxPacket.buf[m_RxPacket.length],
                   sizeof(dword));
            m_RxPacket.length += sizeof(dword);
            memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.read.data.begin.block.len,
                   &m_RxPacket.buf[m_RxPacket.length],
                   sizeof(word));
            m_RxPacket.length += sizeof(word);
            for (i = 0; i < m_RspPacket.rsp.fs_op.fs_rsp.read.data.begin.block.len; i++)
                m_RspPacket.rsp.fs_op.fs_rsp.read.data.begin.block.data[i] = m_RxPacket.buf[m_RxPacket.length++];
        }
        else
        {
            memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.read.data.block.len,
                   &m_RxPacket.buf[m_RxPacket.length],
                   sizeof(word));
            m_RxPacket.length += sizeof(word);
            for (i = 0; i < m_RspPacket.rsp.fs_op.fs_rsp.read.data.block.len; i++)
                m_RspPacket.rsp.fs_op.fs_rsp.read.data.block.data[i] = m_RxPacket.buf[m_RxPacket.length++];
        }
        break;

        case DIAG_FS_WRITE_FILE:
        m_RspPacket.rsp.fs_op.fs_rsp.write.seq_num = m_RxPacket.buf[m_RxPacket.length++];
        break;

        case DIAG_FS_GET_ATTRIB:
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.get_attrib.attrib.attribute_mask,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(word));
        m_RxPacket.length += sizeof(word);
        m_RspPacket.rsp.fs_op.fs_rsp.get_attrib.attrib.buffering_option = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.fs_op.fs_rsp.get_attrib.attrib.cleanup_option = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.get_attrib.creation_date,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(dword));
        m_RxPacket.length += sizeof(dword);
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.get_attrib.size,&m_RxPacket.buf[m_RxPacket.length],sizeof(dword));
        break;

        case DIAG_FS_ITER_DIRS:
        case DIAG_FS_ITER_FILES:
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.iter.seq_num,&m_RxPacket.buf[m_RxPacket.length],sizeof(dword));
        m_RxPacket.length += sizeof(dword);
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.iter.attrib.attribute_mask,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(word));
        m_RxPacket.length += sizeof(word);
        m_RspPacket.rsp.fs_op.fs_rsp.iter.attrib.buffering_option = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.fs_op.fs_rsp.iter.attrib.cleanup_option = m_RxPacket.buf[m_RxPacket.length++];
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.iter.creation_date,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(dword));
        m_RxPacket.length += sizeof(dword);
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.iter.logical_size,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(dword));
        m_RxPacket.length += sizeof(dword);
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.iter.physical_size,
               &m_RxPacket.buf[m_RxPacket.length],
               sizeof(dword));
        m_RxPacket.length += sizeof(dword);
        m_RspPacket.rsp.fs_op.fs_rsp.iter.dirname_length = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.fs_op.fs_rsp.iter.item_name.len = m_RxPacket.buf[m_RxPacket.length++];
        for (i = 0; i < m_RspPacket.rsp.fs_op.fs_rsp.iter.item_name.len; i++)
            m_RspPacket.rsp.fs_op.fs_rsp.iter.item_name.name[i] = m_RxPacket.buf[m_RxPacket.length++];
        m_RspPacket.rsp.fs_op.fs_rsp.iter.item_name.name[i] = '\0';
        break;

        case DIAG_FS_SPACE_AVAIL:
        memcpy(&m_RspPacket.rsp.fs_op.fs_rsp.space_avail,&m_RxPacket.buf[m_RxPacket.length],sizeof(dword));
        break;

        default:
        break;
    }
    return TRUE;
}

CString CPacket::EFSOperError(byte status)
{
    CString m_ErrInfo   = _T("");
    switch (status)
    {
        case FS_FAIL_S:
        m_ErrInfo = _T("Low-level operation failed!");
        break;
        case FS_BUSY_S:
        m_ErrInfo = _T("Operation is queued or in progress!");
        break;
        case FS_FILE_NOT_OPEN_S:
        m_ErrInfo = _T("File needs to be opened for this operation!");
        break;
        case FS_FILE_OPEN_S:
        m_ErrInfo = _T("File needs to be closed for this operation!");
        break;
        case FS_FILE_ALREADY_EXISTS_S:
        m_ErrInfo = _T("File already exists!");
        break;
        case FS_NONEXISTENT_FILE_S:
        m_ErrInfo = _T("File does not exist!");
        break;
        case FS_DIR_ALREADY_EXISTS_S:
        m_ErrInfo = _T("User directory already exists!");
        break;
        case FS_NONEXISTENT_DIR_S:
        m_ErrInfo = _T("User directory does not exist!");
        break;
        case FS_DIR_NOT_EMPTY_S:
        m_ErrInfo = _T("User directory not empty for remove!");
        break;
        case FS_UNKNOWN_OPERATION_S:
        m_ErrInfo = _T("Client specified an unknown operation!");
        break;
        case FS_ILLEGAL_OPERATION_S:
        m_ErrInfo = _T("Client specified an invalid operation!");
        break;
        case FS_PARAMETER_ERROR_S:
        m_ErrInfo = _T("Client supplied a bad parameter value!");
        break;
        case FS_BAD_FILE_NAME_S:
        m_ErrInfo = _T("Client specified invalid file/directory name!");
        break;
        case FS_BAD_FILE_HANDLE_S:
        m_ErrInfo = _T("Client specified invalid file handle");
        break;
        case FS_BAD_SEEK_POS_S:
        m_ErrInfo = _T("Client specified an invalid SEEK position");
        break;
        case FS_BAD_TRUNCATE_POS_S:
        m_ErrInfo = _T("Client supplied an invalid truncate position");
        break;
        case FS_FILE_IS_REMOTE_S:
        m_ErrInfo = _T("Operation invalid for remote files!");
        break;
        case FS_INVALID_CONF_CODE_S:
        m_ErrInfo = _T("Invalid confirmation code specified!");
        break;
        case FS_EOF_S:
        m_ErrInfo = _T("End of file reached!");
        break;
        case FS_MORE_S:
        m_ErrInfo = _T("More records exist to be processed!");
        break;
        case FS_GC_IN_PROGRESS_S:
        m_ErrInfo = _T("Garbage Collection in progress!");
        break;
        case FS_SPACE_EXHAUSTED_S:
        m_ErrInfo = _T("Out of file system space!");
        break;
        case FS_BLK_FULL_S:
        m_ErrInfo = _T("File block is full!");
        break;
        case FS_DIR_SPACE_EXHAUSTED_S:
        m_ErrInfo = _T("Out of Master Directory space!");
        break;
        case FS_FBL_POOL_EXHAUSTED_S:
        m_ErrInfo = _T("Out of File Block List free pool space!");
        break;
        case FS_OPEN_TABLE_FULL_S:
        m_ErrInfo = _T("Out of open-file table slots!");
        break;
        case FS_INVALID_ITERATOR_S:
        m_ErrInfo = _T("Invalid enumeration iterator control struct!");
        break;
        case FS_ENUM_DONE_S:
        m_ErrInfo = _T("Enumeration operation completed!");
        break;
        case FS_RMLINK_EXHAUSTED_S:
        m_ErrInfo = _T("Maximum number of remote links reached!");
        break;
        case FS_NO_MEDIA_S:
        m_ErrInfo = _T("Removable media not inserted!");
        break;
        case FS_MEDIA_CHANGED_S:
        m_ErrInfo = _T("Media has been changed since opened!");
        break;
        default:
        m_ErrInfo = _T("Unknown reason!");
        break;
    }

    return m_ErrInfo;
}

BOOL CPacket::Verify_Download_Security(byte *input)
{
    EmptyVaiables();

    m_Protocol = DDP;   // 直接传递

    m_TxPacket.length = 0;

    m_TxPacket.buf[m_TxPacket.length++] = 75;       // cmd_code
    m_TxPacket.buf[m_TxPacket.length++] = 60;       // sub_sys_id
    m_TxPacket.buf[m_TxPacket.length++] = 5;        // sub_cmd_code
    m_TxPacket.buf[m_TxPacket.length++] = 0;        

    memcpy(&m_TxPacket.buf[m_TxPacket.length],input,128);
    m_TxPacket.length += 128;

    if (!RxTxPacket(2,2300))
    {
        m_strTempInfo.Format("Cannot pass the download check, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }

    int32   retcode = 0;
    if (m_RxPacket.buf[0] == 75 && m_RxPacket.buf[1] == 60 && m_RxPacket.buf[2] == 5 && m_RxPacket.buf[3] == 0)
    {
        memcpy(&retcode,&m_RxPacket.buf[4],4);
        if (retcode == 0)
        {
            return TRUE;
        }
        else if (retcode == 1)
        {
            m_strErrInfo = "Cannot pass the download check, Since public key error";
        }
        else if (retcode == 2)
        {
            m_strErrInfo = "Cannot pass the download check, Since main ver error";
        }
        else if (retcode == 3)
        {
            m_strErrInfo = "Cannot pass the download check, Since compare ver error";
        }
    }
    return FALSE;
}

BOOL CPacket::ReadIMEI(byte ImeiNVbyte[15])
{
    EmptyVaiables();
    m_Protocol = DMP;

    int             cnt     = 0;
    nv_item_type    item_data;
    word            status  = 0;
    memset(&item_data,0,sizeof(nv_item_type));
    byte    linshidata[9] ;
    memset(linshidata,0,9) ;

    if (NvItemRead(NV_UE_IMEI_I,item_data,status))
    {
        memcpy(linshidata,item_data.nv_ue_item.ue_imei,NV_UE_IMEI_SIZE) ;
        ImeiNVbyte[0] = linshidata[1] >> 4;
        int i = 1, j = 2;
        for (j = 2 ; j < 9 ; j++)
        {
            ImeiNVbyte[i++] = linshidata[j] & 0x0f ;
            ImeiNVbyte[i++] = (linshidata[j] & 0xf0) >> 4;
        }

        return TRUE;
    }
    else
    {
        memset(ImeiNVbyte,0,NV_UE_IMEI_SIZE);
        return FALSE;
    }
}

BOOL CPacket::HandWithMobile(byte Imeibypri[128])
{
    EmptyVaiables();

    m_Protocol = DDP;   // 直接传递

    m_TxPacket.length = 0;

    m_TxPacket.buf[m_TxPacket.length++] = 75;       // cmd_code
    m_TxPacket.buf[m_TxPacket.length++] = 60;       // sub_sys_id
    m_TxPacket.buf[m_TxPacket.length++] = 0;        // sub_cmd_code
    m_TxPacket.buf[m_TxPacket.length++] = 0;        

    memcpy(&m_TxPacket.buf[m_TxPacket.length],Imeibypri,128);
    m_TxPacket.length += 128;

    if (!RxTxPacket(2,5100))
    {
        m_strTempInfo.Format("Cannot get value from the mobile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }

    if (m_RxPacket.buf[0] == 75 && m_RxPacket.buf[1] == 60 && m_RxPacket.buf[2] == 0 && m_RxPacket.buf[3] == 0)
    {
        if (m_RxPacket.buf[4] == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CPacket::IniFlexfile()
{
    byte    retcode = 0;
    m_Protocol = DMP;
    EmptyVaiables();
    m_ReqPacket.req.clearUserInfo.cmd_code = DIAG_DEL_USER_INFOR_F;
    m_ReqPacket.req.clearUserInfo.b_flexfile = 1;
    if (!RxTxPacket())
    {
        m_strTempInfo.Format("Cannot active flexfile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        retcode = m_RspPacket.rsp.clearUserInfo.errcode;
        if (retcode == 0)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}

BOOL CPacket::Security()
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.security.cmd_code = SDP_SECURITY_F;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot go into security, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::partition(byte *writeBuf, int nTotalLen)
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.patition.cmd_code = SDP_PARTITION_F;

    //  Buf=writeBuf;
    memcpy(Buf,writeBuf,nTotalLen);
    nLen = nTotalLen;
    if (!RxTxPacket(3,20000))
    {
        m_strTempInfo.Format("Cannot download partition, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }

    m_bOverridePtl = 0;
    switch (m_RspPacket.rsp.patition.status)
    {
        case PARTI_TBL_ACCEPTED:
        return TRUE;
        case PARTI_TBL_DIFFERS:
        m_strErrInfo = "Partition table differs,override will be accepted!";
        break;
        case PARTI_TBL_BAD_FORMAT:
        m_strErrInfo = "partition table format not recognized,will not accept override.";
        case PARTI_TBL_ERASE_FAIL:
        m_strErrInfo = "Erase operaion failed.";
        case PARTI_TBL_UNKNOWN_ERROR:
        m_strErrInfo = "Unknow error.";
        default:
        break;
    }

    return FALSE;
}

BOOL CPacket::OpenMulti(byte type, byte *writeBuf, int nTotalLen)
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.openmulti.cmd_code = SDP_OPENMULTI_F;
    m_ReqPacket.req.openmulti.type = type;

    //lichenyang 20061107
    if (type == 1 || type == 3)
    {
        //lichenyang 2007
        waitsign = 1;
    }
    //lichenyang 20061107

    memcpy(Buf,writeBuf,nTotalLen);
    nLen = nTotalLen;

    if (!RxTxPacket(3,20000))
    {
        m_strTempInfo.Format("Cannot open multi-image, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::Eraser()
{
    EmptyVaiables();
    m_Protocol = SDP;
    m_ReqPacket.req.openmulti.cmd_code = SDP_ERASER_F;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("Cannot erase, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::GetSWExtend(char retSW[])
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.sw_ver_extend.cmd_code = DIAG_SW_VERSION_EXTEND_F ;
    if (!RxTxPacket(1,QUERY_TIMER))
    {
        m_strTempInfo.Format("Cannot get software version from the mobile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        //lichenyang 20080228
        //memset(retSW, 0, 256);
        memset(retSW,0,32);
        return FALSE;
    }
    else
    {
        //lichenyang 20080228
        //memcpy(retSW, m_RspPacket.rsp.sw_ver_extendrsp.retvalue, 256) ;
        memcpy(retSW,m_RspPacket.rsp.sw_ver_extendrsp.retvalue,32) ;

        /*  char showstr[256] ;
            memset(showstr,0,256) ;
            memcpy(showstr,retSW, 256) ;
            CString strshow ;
            strshow = showstr ;
            AfxMessageBox(strshow) ;*/

        return TRUE ;
    }
}
//chenjie 20071127 begin
int CPacket::DDP0x7C()
{
    EmptyVaiables();

    m_Protocol = DDP;   // 直接传递

    m_TxPacket.length = 0;

    m_TxPacket.buf[m_TxPacket.length++] = 0x7c;     // cmd_code

    if (!RxTxPacket(1,5000))
    {
        m_strTempInfo.Format("Cannot get value from the mobile, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return -1;
    }

    if (m_RxPacket.buf[0] == 0x7e
     && m_RxPacket.buf[1] == 0x03
     && m_RxPacket.buf[2] == 0x00
     && m_RxPacket.buf[3] == 0x06)
    {
        return 1;
    }

    return 0 ;
}

BOOL CPacket::DmssCommand(byte dmss_ID)  //彭军民增加
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.stat.cmd_code = dmss_ID;
    if (!RxTxPacket(5,800))
    {
        m_strTempInfo.Format("Cannot Get Current DMSS Status Information, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        if (dmss_ID == DIAG_STATUS_F)
        {
            if (m_RxPacket.length < 10)
            {
                return FALSE;
            }
        }
        return TRUE;
    }
}

/************************************************************************
* 函数名称： // GetSoftVersion()                                        *
* 功能描述： // 发送Diag命令: 0x00...读取手机的软件版本信息             *
* 输入参数： // 无                                                      *
* 返 回 值： // 成功运行返回TRUE                                        *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-28                                              *
* 修改内容： //                                                         *
************************************************************************/
BOOL CPacket::GetSoftVersion()
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.cmd_code = DIAG_VERNO_F;
    if (!RxTxPacket(3,QUERY_TIMER))
    {
        m_strTempInfo.Format("Cannot Get Software Version Information and Model Number, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
    return FALSE;
}

/************************************************************************
* 函数名称： // GetConnectState()                                       *
* 功能描述： // 调用此函数得到手机的连接状态,函数中调用了GetSoftVersion *
* 输入参数： // 无                                                      *
* 返 回 值： // 如果与手机通信成功返回TRUE                              *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-28                                              *
* 修改内容： //                                                         *
************************************************************************/
BOOL CPacket::GetConnectState()
{
    if (GetSoftVersion())
    {
        return TRUE;
    }
    else
    {
        if (AT_QCDMG())
        {
            return TRUE;
        }
        else
        {
            return GetSoftVersion();
        }
    }
}

BOOL CPacket::Down()
{
    EmptyVaiables();
    m_Protocol = DLP;
    m_ReqPacket.req.powerdown.cmd_code = DLOAD_BOOT_F;

    if ((!RxTxPacket(10,100)))//lujuan from 500 to 200 060211
    {
        m_strTempInfo.Format("Cannot Enter Download Mode, Since %s",m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        return FALSE;
    }
    else
    {
        if (m_RspPacket.rsp.powerdown.cmd_code == DLOAD_ACK_F)
        {
            return TRUE;
        }
        return FALSE;
    }
}

//////////////////////////////////////////////////////////////////////////

BOOL CPacket::PRLwrite(LPCTSTR lpszFileName, BOOL &NAM)
{
    byte    writeBuf[PRL_WRITE_BLOCK_SIZE] ;
    dword   LenOfReadFromFile   = PRL_WRITE_BLOCK_SIZE ;
    dword   LenTemp ;
    int     TotalLength ;
    byte    WriteIndex          = 0 ;
    byte   *temp ;
    BOOL    bMore               = TRUE ;
    CFile   hReadFile ;
    if (!hReadFile.Open(lpszFileName,CFile::modeRead))
    {
        AfxMessageBox("Can't open *.prl") ;
        return FALSE ;
    }

    TotalLength = hReadFile.GetLength() ;

    while ((bMore) && (TotalLength > 0))
    {
        if (TotalLength > PRL_WRITE_BLOCK_SIZE)
        {
            LenOfReadFromFile = PRL_WRITE_BLOCK_SIZE ;
            bMore = TRUE ;
        }
        else
        {
            LenOfReadFromFile = TotalLength ;
            bMore = FALSE ;
        }
        hReadFile.ReadHuge(writeBuf,LenOfReadFromFile) ; 
        EmptyVaiables();
        m_ReqPacket.req.PRLwrite_req.cmd_code = DIAG_PRL_WRITE_F ;
        m_ReqPacket.req.PRLwrite_req.seq_num = WriteIndex ;
        if (bMore)
        {
            m_ReqPacket.req.PRLwrite_req.more = 1 ;
        }
        else
        {
            m_ReqPacket.req.PRLwrite_req.more = 0 ;
        }
        m_ReqPacket.req.PRLwrite_req.nam = 0x00 ;
        if (NAM)
        {
            m_ReqPacket.req.PRLwrite_req.nam = 0x01 ;
        }
        LenTemp = LenOfReadFromFile * 8 ;
        temp = (byte *) &LenTemp ;
        m_ReqPacket.req.PRLwrite_req.num_bits[0] = temp[0] ;
        m_ReqPacket.req.PRLwrite_req.num_bits[1] = temp[1] ;
        memcpy(m_ReqPacket.req.PRLwrite_req.buf,writeBuf,PRL_WRITE_BLOCK_SIZE);

        if (!RxTxPacket(1,1000))
        {
            m_strErrInfo.Format("Fail to write PRL to phone") ;
            AfxMessageBox(m_strErrInfo) ;
            hReadFile.Close() ;
            return FALSE ;
        }
        else
        {
            if (m_RspPacket.rsp.PRLwrite_rsp.pr_list_status != 0)
            {
                if (m_RspPacket.rsp.PRLwrite_rsp.pr_list_status == 1)
                {
                    if (m_RspPacket.rsp.PRLwrite_rsp.nv_status != NV_DONE_S)
                    {
                        m_strErrInfo.Format("Fail to write PRL, since %s",
                                            MapNVOperateError((nv_stat_enum_type)
                                                              m_RspPacket.rsp.PRLwrite_rsp.nv_status));
                        AfxMessageBox(m_strErrInfo);
                        hReadFile.Close() ;
                        return FALSE;
                    }
                }
                else if (m_RspPacket.rsp.PRLwrite_rsp.pr_list_status == 2)
                {
                    AfxMessageBox("out of seq");
                    hReadFile.Close() ;
                    return FALSE;
                }
                else
                {
                    AfxMessageBox("Overflowed Maximum PRL_Size");
                    hReadFile.Close() ;
                    return FALSE;
                }
            }
        }

        TotalLength = TotalLength - LenOfReadFromFile ;
        WriteIndex++ ;
    }

    hReadFile.Close() ;
    return TRUE ;
}


word CPacket::CombineSubsysPacket()
{
    int     i   = 0;
    byte    nsub;
    byte    cmd = 0;
    m_TxPacket.length = 0;
    memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,4);
    nsub = m_TxPacket.buf[1];
    cmd = m_TxPacket.buf[2];

    switch (nsub)
    {
        case DIAG_SUBSYS_FS:
        switch (cmd)
        {
            case EFS2_DIAG_HELLO:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,sizeof(efs2_diag_hello_req_type));
            m_TxPacket.length = sizeof(efs2_diag_hello_req_type);
            break;
            case EFS2_DIAG_QUERY:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,sizeof(efs2_diag_query_req_type));
            m_TxPacket.length = sizeof(efs2_diag_query_req_type);   
            break;
            case EFS2_DIAG_OPEN:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,4);
            m_TxPacket.length = 4;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.efs2_op.file_open_req.oflag,
                   sizeof(int32));
            m_TxPacket.length += sizeof(int32);
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.efs2_op.file_open_req.mode,
                   sizeof(int32));
            m_TxPacket.length += sizeof(int32);
            for (i = 0; m_ReqPacket.req.efs2_op.file_open_req.path[i]; i++)
            {
                m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.file_open_req.path[i];
            }
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.file_open_req.path[i];                    
            break;
            case EFS2_DIAG_CLOSE:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,sizeof(efs2_diag_close_req_type));
            m_TxPacket.length = sizeof(efs2_diag_close_req_type);
            break;
            case EFS2_DIAG_READ:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,sizeof(efs2_diag_read_req_type));
            m_TxPacket.length = sizeof(efs2_diag_read_req_type);
            break;
            case EFS2_DIAG_WRITE:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op.file_write_req,12);
            m_TxPacket.length = 12;
            memcpy(&m_TxPacket.buf[m_TxPacket.length],
                   &m_ReqPacket.req.efs2_op.file_write_req.data,
                   m_ReqPacket.req.efs2_op.file_write_req.nlen);
            m_TxPacket.length += (word) m_ReqPacket.req.efs2_op.file_write_req.nlen;
            break;
            case EFS2_DIAG_MKDIR:
            memcpy(&m_TxPacket.buf,&m_ReqPacket.req.efs2_op.mkdir_req,6);
            m_TxPacket.length = 6;
            for (i = 0; m_ReqPacket.req.efs2_op.mkdir_req.path[i]; i++)
            {
                m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.mkdir_req.path[i];
            }
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.mkdir_req.path[i];
            break;
            case EFS2_DIAG_RMDIR:
            memcpy(&m_TxPacket.buf,&m_ReqPacket.req.efs2_op.rmdir_req,4);
            m_TxPacket.length = 4;
            for (i = 0; m_ReqPacket.req.efs2_op.rmdir_req.path[i]; i++)
            {
                m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.rmdir_req.path[i];
            }
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.rmdir_req.path[i];
            break;      
            case EFS2_DIAG_OPENDIR:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op.open_dir_req,4);
            m_TxPacket.length = 4;
            for (i = 0; m_ReqPacket.req.efs2_op.open_dir_req.path[i]; i++)
            {
                m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.open_dir_req.path[i];
            }
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.open_dir_req.path[i];                 
            break;
            case EFS2_DIAG_READDIR:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op.read_dir_req,sizeof(efs2_diag_readdir_req_type));
            m_TxPacket.length = sizeof(efs2_diag_readdir_req_type);
            break;
            case EFS2_DIAG_CLOSEDIR:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op,sizeof(efs2_diag_closedir_req_type));
            m_TxPacket.length = sizeof(efs2_diag_closedir_req_type);
            break;
            case EFS2_DIAG_RENAME:
            memcpy(m_TxPacket.buf,&m_ReqPacket.req.efs2_op.rename_req,4);
            m_TxPacket.length = 4;
            for (i = 0; m_ReqPacket.req.efs2_op.rename_req.oldpath[i]; i++)
            {
                m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.rename_req.oldpath[i];
            }
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.rename_req.oldpath[i];                    
            for (i = 0; m_ReqPacket.req.efs2_op.rename_req.newpath[i]; i++)
            {
                m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.rename_req.newpath[i];
            }
            m_TxPacket.buf[m_TxPacket.length++] = m_ReqPacket.req.efs2_op.open_dir_req.path[i];
            break;
        }
        break;
        default:
        break;
    }
    return (word) m_TxPacket.length;
}
BOOL CPacket::DivideSubsysPacket()
{
    word    i   = 0;
    byte    nsub;
    word    cmd;
    nsub = m_RxPacket.buf[1];
    memcpy(&cmd,m_RxPacket.buf + 2,2);
    switch (nsub)
    {
        case DIAG_SUBSYS_FS:
        switch (cmd)
        {
            case EFS2_DIAG_HELLO:
            memcpy(&m_RspPacket.rsp.efs2_rsp,m_RxPacket.buf,sizeof(efs2_diag_hello_rsp_type));
            break;
            case EFS2_DIAG_QUERY:
            memcpy(&m_RspPacket.rsp.efs2_rsp,m_RxPacket.buf,sizeof(efs2_diag_query_rsp_type));
            break;
            case EFS2_DIAG_OPEN:
            memcpy(&m_RspPacket.rsp.efs2_rsp,m_RxPacket.buf,sizeof(efs2_diag_open_rsp_type));   
            break;
            case EFS2_DIAG_CLOSE:
            memcpy(&m_RspPacket.rsp.efs2_rsp,m_RxPacket.buf,sizeof(efs2_diag_close_rsp_type));              
            break;
            case EFS2_DIAG_READ:
            memcpy(&m_RspPacket.rsp.efs2_rsp.file_read_rsp,m_RxPacket.buf,20);
            memcpy(m_RspPacket.rsp.efs2_rsp.file_read_rsp.data,
                   &m_RxPacket.buf[20],
                   m_RspPacket.rsp.efs2_rsp.file_read_rsp.bytes_read);
            break;
            case EFS2_DIAG_WRITE:
            memcpy(&m_RspPacket.rsp.efs2_rsp.file_write_rsp,m_RxPacket.buf,sizeof(efs2_diag_write_rsp_type));
            break;
            case EFS2_DIAG_MKDIR:
            memcpy(&m_RspPacket.rsp.efs2_rsp.mkdir_rsp,m_RxPacket.buf,sizeof(efs2_diag_mkdir_rsp_type));
            break;
            case EFS2_DIAG_RMDIR:
            memcpy(&m_RspPacket.rsp.efs2_rsp.rmdir_rsp,m_RxPacket.buf,sizeof(efs2_diag_rmdir_rsp_type));
            break;
            case EFS2_DIAG_OPENDIR:
            memcpy(&m_RspPacket.rsp.efs2_rsp.open_dir_rsp,m_RxPacket.buf,sizeof(efs2_diag_opendir_rsp_type));
            break;
            case EFS2_DIAG_READDIR:
            memcpy(&m_RspPacket.rsp.efs2_rsp.read_dir_rsp,m_RxPacket.buf,40);
            m_RxPacket.length = 40;
            for (i = 0; m_RxPacket.buf[m_RxPacket.length]; i++)
            {
                m_RspPacket.rsp.efs2_rsp.read_dir_rsp.entry_name[i] = m_RxPacket.buf[m_RxPacket.length++];
            }
            m_RspPacket.rsp.efs2_rsp.read_dir_rsp.entry_name[i] = m_RxPacket.buf[m_RxPacket.length];            
            break;
            case EFS2_DIAG_CLOSEDIR:
            memcpy(&m_RspPacket.rsp.efs2_rsp.close_dir_rsp,m_RxPacket.buf,sizeof(efs2_diag_closedir_rsp_type));         
            break;
            case EFS2_DIAG_RENAME:
            memcpy(&m_RspPacket.rsp.efs2_rsp.rename_rsp,m_RxPacket.buf,sizeof(efs2_diag_rename_rsp_type));
            break;
            default:
            break;
        }
        break;
        default:
        break;
    }
    return TRUE;
}

BOOL CPacket::EFS2Hello(diag_sub_efs2_req_type helloType)
{
    EmptyVaiables();
    m_Protocol = DMP;
    helloType.hello_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    helloType.hello_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    helloType.hello_req.hdr.opaque_header[2] = EFS2_DIAG_HELLO;
    helloType.hello_req.hdr.opaque_header[3] = 0;
    memcpy(&m_ReqPacket.req.efs2_op,&helloType,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::EFS2FileOpen(CString strPath, int openMode)
{
    diag_sub_efs2_req_type  fileOpen;
    int                     n   = sizeof(diag_sub_efs2_req_type);

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&fileOpen,0,sizeof(diag_sub_efs2_req_type));
    fileOpen.file_open_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    fileOpen.file_open_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    fileOpen.file_open_req.hdr.opaque_header[2] = EFS2_DIAG_OPEN;
    fileOpen.file_open_req.hdr.opaque_header[3] = 0;
    fileOpen.file_open_req.mode = 0;
    fileOpen.file_open_req.oflag = openMode;
    for (int i = 0; i < strPath.GetLength(); i++)
    {
        fileOpen.file_open_req.path[i] = strPath.GetAt(i);
    }
    memcpy(&m_ReqPacket.req.efs2_op,&fileOpen,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.file_open_rsp.fd == -1)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CPacket::EFS2FileRead(byte *buf, int &nLen, int nOffset)
{
    diag_sub_efs2_req_type  fileRead;
    EmptyVaiables();
    m_Protocol = DMP;
    fileRead.file_read_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    fileRead.file_read_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    fileRead.file_read_req.hdr.opaque_header[2] = EFS2_DIAG_READ;
    fileRead.file_read_req.hdr.opaque_header[3] = 0;

    fileRead.file_read_req.nbyte = nLen;
    fileRead.file_read_req.fd = 0;
    fileRead.file_read_req.offset = nOffset;
    memcpy(&m_ReqPacket.req.efs2_op,&fileRead,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    nLen = m_RspPacket.rsp.efs2_rsp.file_read_rsp.bytes_read;
    memcpy(buf,m_RspPacket.rsp.efs2_rsp.file_read_rsp.data,nLen);

    return TRUE;
}

BOOL CPacket::EFS2FileWrite(byte *buf, int nLen, int nOffset)
{
    diag_sub_efs2_req_type  fileWrite;
    EmptyVaiables();
    m_Protocol = DMP;
    fileWrite.file_write_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    fileWrite.file_write_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    fileWrite.file_write_req.hdr.opaque_header[2] = EFS2_DIAG_WRITE;
    fileWrite.file_write_req.hdr.opaque_header[3] = 0;

    fileWrite.file_write_req.nlen = nLen;
    fileWrite.file_write_req.fd = 0;
    fileWrite.file_write_req.offset = nOffset;
    memcpy(fileWrite.file_write_req.data,buf,nLen);
    memcpy(&m_ReqPacket.req.efs2_op,&fileWrite,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::EFS2FileClose()
{
    diag_sub_efs2_req_type  fileClose;
    EmptyVaiables();
    m_Protocol = DMP;
    fileClose.file_close_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    fileClose.file_close_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    fileClose.file_close_req.hdr.opaque_header[2] = EFS2_DIAG_CLOSE;
    fileClose.file_close_req.hdr.opaque_header[3] = 0;

    fileClose.file_close_req.fd = 0;
    memcpy(&m_ReqPacket.req.efs2_op,&fileClose,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::EFS2Query(diag_sub_efs2_rsp_type &queryType)
{
    diag_sub_efs2_req_type  fileQuery;
    EmptyVaiables();
    m_Protocol = DMP;
    fileQuery.query_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    fileQuery.query_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    fileQuery.query_req.hdr.opaque_header[2] = EFS2_DIAG_QUERY;
    fileQuery.query_req.hdr.opaque_header[3] = 0;

    memcpy(&m_ReqPacket.req.efs2_op,&fileQuery,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    memcpy(&queryType,&m_RspPacket.rsp.efs2_rsp,sizeof(diag_sub_efs2_rsp_type));
    return TRUE;
}

BOOL CPacket::EFS2MkDir(CString strPath)
{
    diag_sub_efs2_req_type  dirMake;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&dirMake,0,sizeof(diag_sub_efs2_req_type));
    dirMake.mkdir_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    dirMake.mkdir_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    dirMake.mkdir_req.hdr.opaque_header[2] = EFS2_DIAG_MKDIR;
    dirMake.mkdir_req.hdr.opaque_header[3] = 0;

    dirMake.mkdir_req.mode = 0x1ff;
    for (int i = 0; i < strPath.GetLength(); i++)
    {
        dirMake.mkdir_req.path[i] = strPath.GetAt(i);
    }
    memcpy(&m_ReqPacket.req.efs2_op,&dirMake,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.mkdir_rsp.err_no)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::EFS2RmDir(CString strPath)
{
    diag_sub_efs2_req_type  dirRm;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&dirRm,0,sizeof(diag_sub_efs2_req_type));
    dirRm.rmdir_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    dirRm.rmdir_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    dirRm.rmdir_req.hdr.opaque_header[2] = EFS2_DIAG_RMDIR;
    dirRm.rmdir_req.hdr.opaque_header[3] = 0;

    for (int i = 0; i < strPath.GetLength(); i++)
    {
        dirRm.rmdir_req.path[i] = strPath.GetAt(i);
    }
    memcpy(&m_ReqPacket.req.efs2_op,&dirRm,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.rmdir_rsp.err_no)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::EFS2OpenDir(CString strDir)
{
    diag_sub_efs2_req_type  OpenDir;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&OpenDir,0,sizeof(diag_sub_efs2_req_type));

    OpenDir.open_dir_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    OpenDir.open_dir_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    OpenDir.open_dir_req.hdr.opaque_header[2] = EFS2_DIAG_OPENDIR;
    OpenDir.open_dir_req.hdr.opaque_header[3] = 0;

    for (int i = 0; i < strDir.GetLength(); i++)
    {
        OpenDir.open_dir_req.path[i] = strDir.GetAt(i);
    }
    memcpy(&m_ReqPacket.req.efs2_op,&OpenDir,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.open_dir_rsp.err_no)
    {
        return FALSE;
    }
    m_pEFS2DirectoryPoint = m_RspPacket.rsp.efs2_rsp.open_dir_rsp.dirp;

    return TRUE;
}

BOOL CPacket::EFS2ReadDir(char *buf, int nSeqNo, efs2_diag_readdir_rsp_type &rData)
{
    diag_sub_efs2_req_type  ReadDir;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&ReadDir,0,sizeof(diag_sub_efs2_req_type));

    ReadDir.read_dir_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    ReadDir.read_dir_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    ReadDir.read_dir_req.hdr.opaque_header[2] = EFS2_DIAG_READDIR;
    ReadDir.read_dir_req.hdr.opaque_header[3] = 0;
    ReadDir.read_dir_req.dirp = m_pEFS2DirectoryPoint;
    ReadDir.read_dir_req.seqno = nSeqNo;

    memcpy(&m_ReqPacket.req.efs2_op,&ReadDir,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.read_dir_rsp.err_no)
    {
        return FALSE;
    }
    for (int i = 0; m_RspPacket.rsp.efs2_rsp.read_dir_rsp.entry_name[i]; i++)
    {
        buf[i] = m_RspPacket.rsp.efs2_rsp.read_dir_rsp.entry_name[i];
    }
    buf[i] = m_RspPacket.rsp.efs2_rsp.read_dir_rsp.entry_name[i];
    rData = m_RspPacket.rsp.efs2_rsp.read_dir_rsp;
    if (m_RspPacket.rsp.efs2_rsp.read_dir_rsp.entry_name[0] == 0)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::EFS2CloseDir(int32 nDir)
{
    diag_sub_efs2_req_type  CloseDir;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&CloseDir,0,sizeof(diag_sub_efs2_req_type));

    CloseDir.close_dir_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    CloseDir.close_dir_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    CloseDir.close_dir_req.hdr.opaque_header[2] = EFS2_DIAG_CLOSEDIR;
    CloseDir.close_dir_req.hdr.opaque_header[3] = 0;

    CloseDir.close_dir_req.dirp = nDir;
    memcpy(&m_ReqPacket.req.efs2_op,&CloseDir,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.close_dir_rsp.err_no)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CPacket::EFS2CloseDir()
{
    diag_sub_efs2_req_type  CloseDir;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&CloseDir,0,sizeof(diag_sub_efs2_req_type));

    CloseDir.close_dir_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    CloseDir.close_dir_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    CloseDir.close_dir_req.hdr.opaque_header[2] = EFS2_DIAG_CLOSEDIR;
    CloseDir.close_dir_req.hdr.opaque_header[3] = 0;
    CloseDir.close_dir_req.dirp = m_pEFS2DirectoryPoint;
    memcpy(&m_ReqPacket.req.efs2_op,&CloseDir,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.close_dir_rsp.err_no)
    {
        return FALSE;
    }
    m_pEFS2DirectoryPoint = 0;
    return TRUE;
}

BOOL CPacket::EFS2Rename(CString strOld, CString strNew)
{
    diag_sub_efs2_req_type  renameReq;

    EmptyVaiables();
    m_Protocol = DMP;
    memset(&renameReq,0,sizeof(diag_sub_efs2_req_type));

    renameReq.rename_req.hdr.opaque_header[0] = DIAG_SUBSYS_CMD_F;
    renameReq.rename_req.hdr.opaque_header[1] = DIAG_SUBSYS_FS;
    renameReq.rename_req.hdr.opaque_header[2] = EFS2_DIAG_CLOSEDIR;
    renameReq.rename_req.hdr.opaque_header[3] = 0;

    for (int i = 0; i < strOld.GetLength(); i++)
    {
        renameReq.rename_req.oldpath[i] = strOld.GetAt(i);
    }
    renameReq.rename_req.oldpath[i] = 0;

    for (i = 0; i < strNew.GetLength(); i++)
    {
        renameReq.rename_req.newpath[i] = strNew.GetAt(i);
    }
    renameReq.rename_req.newpath[i] = 0;    
    memcpy(&m_ReqPacket.req.efs2_op,&renameReq,sizeof(diag_sub_efs2_req_type));
    if (!RxTxPacket(3,2000))
    {
        return FALSE;
    }
    if (m_RspPacket.rsp.efs2_rsp.rename_rsp.err_no)
    {
        return FALSE;
    }

    return TRUE;
}


BOOL CPacket::NvItemReadPSW(word item, nv_item_type &item_data, word &status)//ruiju add 091120
{
    EmptyVaiables();
    m_Protocol = DMP;
    m_ReqPacket.req.nvread.cmd_code = DIAG_PSW_NV_READ_F;
    m_ReqPacket.req.nvread.item = item;
    m_ReqPacket.req.nvread.item_data = item_data;
    m_ReqPacket.req.nvread.nv_stat = NV_DONE_S;
    if (!RxTxPacket(3))
    {
        m_strTempInfo.Format("cannot read #%d NVItem, since %s",item,m_strErrInfo);
        m_strErrInfo = m_strTempInfo;
        status = NV_RWOPFAILED_S;
        memset(&item_data,0,sizeof(nv_item_type));
        return FALSE;
    }
    else
    {
        if (m_RxPacket.buf[0] == 0x13)
        {
            CString strtemp (m_strErrInfo);
            m_strErrInfo.Format("cannot read #%d NV Item, since ",item);
            m_strErrInfo += strtemp;
            status = NV_BADCMD_S;
            return FALSE;
        }
        else if (m_RspPacket.rsp.nvread.nv_stat == NV_DONE_S)
        {
            status = NV_DONE_S;
            item_data = m_RspPacket.rsp.nvread.item_data;
            return TRUE;
        }
        else
        {
            m_strErrInfo.Format("cannot read #%d NV Item, since %s",
                                item,
                                MapNVOperateError((nv_stat_enum_type) m_RspPacket.rsp.nvread.nv_stat));
            status = m_RspPacket.rsp.nvread.nv_stat;
            memset(&item_data,0,sizeof(nv_item_type));
            return FALSE;
        }
    }
}



BOOL CPacket::TransHSDLstartCommand(int length, byte CRC1, byte CRC2)
{
    m_Protocol = SDP ;
    EmptyVaiables();
    m_ReqPacket.req.cmd_code = SDP_HSDL_F;

    //data length:
    byte   *myplen  = (byte *) &length ;
    for (int i = 0 ; i < 4 ; i++)
        plen[3 - i] = myplen[i] ;
    //CRC
    CRC[0] = CRC1;
    CRC[1] = CRC2;
    dword   TxPacketLen;
    TxPacketLen = CombinePacket();  //组装数据包
    TxPacketLen = WriteCommBytes((char *) m_TxPacket.buf,m_TxPacket.length);
    if (TxPacketLen != m_TxPacket.length)
    {
        m_strErrInfo.Format("Port Cannot Send desired data to the Phone");
        return FALSE;
    }
    return TRUE ;
}


BOOL CPacket::MakeCRC2bytes(char FilePath[], byte &CRC1, byte &CRC2)
{
    CFile   hReadFile ;
    word    CRC = ~CRC_SEED;

    if (!hReadFile.Open(FilePath,CFile::modeRead | CFile::shareDenyWrite))
    {
        AfxMessageBox("open file fail!");
        return FALSE ;
    }

    int     nTotalLen   = hReadFile.GetLength();    
    int     nBlockLen   = 0;    
    int     nCRCLen     = 0;
    int     i           = 0 ;
    byte    CRCBuf[HS_TRANSMIT_BLOCK_SIZE]; //按块校验，块的大小
    BOOL    bMore       = FALSE;

    do
    {
        nBlockLen = nTotalLen - nCRCLen;
        if (nBlockLen > HS_TRANSMIT_BLOCK_SIZE)
        {
            nBlockLen = HS_TRANSMIT_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }

        hReadFile.Read(CRCBuf,nBlockLen);

        for (i = 0; i < nBlockLen ; i++)
        {
            CRC = CRC_COMPUTE(CRC,CRCBuf[i]);
        }
        nCRCLen += nBlockLen ;
    }
    while (bMore) ;

    hReadFile.Close() ;
    CRC ^= 0xFFFF;
    CRC1 = (byte) CRC ;
    CRC2 = (byte) (CRC >> 8) ;
    return TRUE;
}

BOOL CPacket::TransHSDLstartCommandNew(int length/*,byte CRC1,byte CRC2*/)
{
	m_Protocol = SDP ;
	EmptyVaiables();
	m_ReqPacket.req.cmd_code=SDP_HSDL_F_NEW;
	
	//data length:
	byte* myplen = (byte*)&length ;
	for ( int i = 0 ; i < 4 ; i++ )
		plen[3-i] = myplen[i] ;
	////ruiju del
	// 	//CRC
	// 	CRC[0]=CRC1;
	// 	CRC[1]=CRC2;
	dword   TxPacketLen;
	TxPacketLen= CombinePacket();  //组装数据包
	TxPacketLen = WriteCommBytes((char *)m_TxPacket.buf,m_TxPacket.length);
	if(TxPacketLen != m_TxPacket.length)
	{
		m_strErrInfo.Format("Port Cannot Send desired data to the Phone");
		return FALSE;
	}
	return TRUE ;
}

int CPacket::MakeCheckSum(char FilePath[], dword offset, dword length)
{
	CFile hReadFile ;
	int CheckSum=0;
	int temp;
	
	if(!hReadFile.Open(FilePath, CFile::modeRead | CFile::shareDenyWrite))
	{
		AfxMessageBox("open file fail!");
		return FALSE ;
	}
	
	DWORD  nTotalLen = hReadFile.GetLength();
	nTotalLen=nTotalLen>length?length:nTotalLen;
	
	byte CheckSumBuf[HS_DEVICE_MEMORY_SIZE];	//按块校验，块的大小
	DWORD * dataSource;
	
	hReadFile.Seek(offset,0);	
	hReadFile.Read(CheckSumBuf,nTotalLen);
	dataSource = (DWORD *)CheckSumBuf;
	
	for(temp=0;temp<(nTotalLen/4);temp++,dataSource++)
	{
		CheckSum+=*dataSource;
	}
	if(nTotalLen%4)
	{
		temp=8*(4-length%4);
		CheckSum+=((*dataSource<<temp)>>temp);
	}
	
	hReadFile.Close() ;
	return CheckSum;
}



