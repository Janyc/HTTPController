#pragma once

#define MAX_BUF 4096
#define MAX_BUFF_FRAME_COUNT 10000

namespace nsTcpConn
{
	#pragma  pack( push, 1 )
	typedef struct _tagFrameBuffer
	{
		BYTE  m_ui8ServiceType;
		BYTE  m_ui8DataType;
		char   m_szFrameBuf[MAX_BUF];
		int    m_iFrameLen;
	}CFrameBuffer;
	#pragma  pack( pop )

	class CTcpConn : public IConnection
	{
	public:
		enum _enConnStatus
		{
			enDisConn = 0,  // δ���ӳɹ�
			enConning = 1,  // ����������
			enConnOK  = 2,  // ���ӳɹ�
			enConnFail = 3, // ����ʧ��
			enConnClose = 4,// ���ӹر�
			enConnError = 5,// ���Ӵ���
		};

	public:
		CTcpConn(void);
		virtual ~CTcpConn(void);

	public:
		virtual void OnRead( char *v_pBuffRead, int nLen );
		virtual void OnConnectionEstablished( BOOL bConnEsted );
		//�Է��ر�����
		virtual BOOL OnClose( );
		//�д�����
		virtual void OnError( DWORD dwError,DWORD dwOpType );
		//д���
		virtual void OnWrite( LPCSTR lpszBuff,int nLen );

	public:
		void Reset(void);
		void OnReceive(unsigned char* v_szRecvBuf, int v_iRecvLen);
		int TcpSend(const CFrameBuffer* const v_pFrameBuffer);
		int AddFsynToBuf(const unsigned char* v_szOldBuf, unsigned char* v_szNewBuf, const int v_iBufLen);
		virtual int OneFrameChecked(void) = 0;

	public:
		char    m_szIp[32];
		unsigned long          m_uiIpAddr;
		unsigned short  m_usPort;

		unsigned long m_uiTotalSend; // �������ֽ���ͳ��
		unsigned long m_uiTotalRecv; // �������ֽ���ͳ��
		int    m_iFrameCheckCount; // ��֡��֡��ͳ��
		int    m_iFrameSendCount;  // ������֡��ͳ��

	protected:
		//��֡�ṹ
		unsigned char*	m_pBufLeave;
		int		m_iBufPos;
		int	    m_Flag7e;
		int	    m_Flag7d;
	
	public:
		//��·״̬
		unsigned char  m_ui8ConnStatus;
	};
}

using namespace nsTcpConn;
