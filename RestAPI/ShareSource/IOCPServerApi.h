//#pragma once

typedef unsigned long u_long;
typedef unsigned short u_short;

#ifdef  IOCPSERVER_EXPORTS
#define IOCP_API _declspec(dllexport)
#else
#define IOCP_API _declspec(dllimport)
#endif

//������������࣬��ΪӦ�ò�������
class IConnection;
class IConnectionFactory;

//DLL������������
DWORD IOCP_API CreateServer();

BOOL IOCP_API StartServer( DWORD v_dwSvrID, int nPort, int nMaxConnections = 5000, 
				  int nMaxFreeBuffers = 1000, int nMaxFreeContexts = 500);

void IOCP_API StopServer( DWORD v_dwSvrID );

void IOCP_API SetConnCreateFactory( DWORD v_dwSvrID, IConnectionFactory *v_pICF);

BOOL IOCP_API Send( DWORD v_dwSvrID, IConnection *v_pConnection, LPCSTR pszText, int nLen );

void IOCP_API CloseConnection( DWORD v_dwSvrID, IConnection * v_pConnection );

BOOL IOCP_API CreateConnectionToRemote(  DWORD v_dwSvrID, IConnection * v_pConnection,LPCSTR v_lpRemotIP,
									   short sRemotePort,LPCSTR v_pSendData,int nLen );

BOOL IOCP_API SetLogInterFace( DWORD v_dwSvrID, CHAR* v_pIFileLog, CHAR* v_pIWndLog );

void IOCP_API DestoryServer( DWORD v_dwSvrID );

class IOCPDLL
{
public:
	static DWORD IOCP_CreateServer()
	{
		return CreateServer();
	}

	static void IOCP_DestoryServer( DWORD v_dwSvrID )
	{
		DestoryServer( v_dwSvrID );
	}

	static BOOL IOCP_StartServer(DWORD v_dwSvrID, int nPort, int nMaxConnections = 5000, 
		int nMaxFreeBuffers = 1000, int nMaxFreeContexts = 500)
	{
		return StartServer(v_dwSvrID, nPort, nMaxConnections, 
			nMaxFreeBuffers, nMaxFreeContexts );
	}

	static void IOCP_StopServer( DWORD v_dwSvrID )
	{
		StopServer( v_dwSvrID );
	}

	static void IOCP_SetConnCreateFactory( DWORD v_dwSvrID, IConnectionFactory *v_pICF)
	{
		SetConnCreateFactory( v_dwSvrID, v_pICF );
	}

	static void IOCP_CloseConnection( DWORD v_dwSvrID, IConnection * v_pConnection)
	{
		CloseConnection( v_dwSvrID, v_pConnection );
	}
	static BOOL IOCP_Send( DWORD v_dwSvrID, IConnection *v_pConnection, char *pszText, int nLen )
	{
		return Send( v_dwSvrID, v_pConnection,pszText,nLen );
	}

	static BOOL  IOCP_CreateConnectionToRemote( DWORD v_dwSvrID, IConnection * v_pConnection,LPCSTR v_lpRemotIP,
												short sRemotePort,LPCSTR v_pSendData,int nLen )
	{
		return CreateConnectionToRemote( v_dwSvrID, v_pConnection, v_lpRemotIP, sRemotePort, v_pSendData, nLen );
	}

	static BOOL IOCP_SetLogInterFace( DWORD v_dwSvrID, CHAR* v_pIFileLog, CHAR* v_pIWndLog )
	{
		return SetLogInterFace( v_dwSvrID, v_pIFileLog, v_pIWndLog );
	}

protected:
private:
};

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* һ��Ϊ�û��ӿڻ��࣬�û���Ҫ�����Լ�������д���                     */
/*W.S.H 2008.12.05                                                      */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////

namespace LogSpace
{

	enum _enWndLog
	{
		enDevInfo	= 0x00,	// �豸��Ϣ
		enRunInfo	= 0x01,	// ������Ϣ
		enCommInfo	= 0x02,	// ͨ����Ϣ
		enErrorInfo	= 0x03	// �쳣������Ϣ
	};
	// �ļ���־�ӿ�
	class IFileLog
	{
	public:
		virtual ~IFileLog(){};
		/*
		*	��������toWriteFileLog
		*���Ρ�����v_szDescription---��־������Ϣ
		*			v_szBuf---��־����
		*			v_iLenBuf---��־����
		*	�����ܣ�����־�ļ���д����־��Ϣ
		*	����ֵ��
		*	�����ߣ��񳤷�
		*	ʱ���䣺2007-1-4 17:15
		*������ע�� 
		*/
		virtual int toWriteFileLog( const char v_szDescription[], const char v_szBuf[], int v_iLenBuf) = 0;
	};
	// ������־�ӿ�
	class IWndLog
	{
	public:
		virtual ~IWndLog(){};
		/*
		*	��������toPrintWndLog
		*���Ρ�����v_iTypeLog---��־����
		*			v_szLog---��־����
		*	�����ܣ��򴰿���д����־,����־��ʾ����
		*	����ֵ��
		*	�����ߣ��񳤷�
		*	ʱ���䣺2007-1-4 17:17
		*������ע��
		*/
		virtual int toShowWndLog(int v_iTypeLog, const char v_szLog[]) = 0;
	};
}


class IConnection
{
public:
	IConnection(void):m_iConnectID(-1),m_dwTickCount(0),m_lSerialNo(0),m_dwSvrID(-1){};
	~IConnection(void){};
public:
//�벻Ҫ�������´���
	void AttachID(int newID)
	{
		m_iConnectID = newID;
	}
	inline int GetID()
	{
		return m_iConnectID;
	}

	void AttchSvrID( DWORD v_dwSvrID )
	{
		m_dwSvrID = v_dwSvrID;
	}
//�벻Ҫ�������ϴ���

// �󶨷�����ID


	enum _enOpType
	{
		enOpNone	= 0,
		enOpAccept	= 1,
		enOpWrite	= 2, //д
		enOpRead	= 3, //��
		enOpConnect = 4
	};
public://���µĺ��������̰߳�ȫ�ġ����Ҫ����������֮�乲�����ݣ���ô�Ƿǰ�ȫ�ģ���Ҫ�Լ������߳�ͬ�����⡣
	
	//���ݵ���,������̰߳�ȫ�ģ��л����
	virtual void OnRead( char *v_pBuffRead,int nLen ) { };

	//�Է��ر�����
	//����TRUE��IOCP�������ͷ������Դ�������ϲ�Ӧ��Ӧ�����ʵ���ʱ�����CloseConnection();
	//���ͷ���ص���Դ!
	virtual BOOL OnClose( ) { return TRUE; }

	//�д�����
	virtual void OnError( DWORD dwError,DWORD dwOpType ) {}

	//��������
	virtual BOOL Send( char *v_pBuffSend,int nLen ) { return IOCPDLL::IOCP_Send(m_dwSvrID,this,v_pBuffSend,nLen);}

	//�����ر����ӻ���ָʾIOCP�ر�����
	//������յ���OnClose֪ͨ�з�����FALSE����ô���������CloseConnection()��ָʾIOCP
	//�ͷ������Դ��
	virtual void CloseConnection() { IOCPDLL::IOCP_CloseConnection( m_dwSvrID, this ); }

	//�����Ѿ�����,�����Ǳ��ص�Զ�̷����������ӣ�Ҳ������Զ�̿ͻ��˵����ص�����
	virtual void OnConnectionEstablished( BOOL bConnEsted ) { 

	};

	//д���
	virtual void OnWrite( LPCSTR lpszBuff,int nLen ) {};
public:
	DWORD m_dwTickCount; //ʱ���
	LONG  m_lSerialNo;	 //��ˮ�ţ�ÿ�����ӵ�����
	DWORD m_dwSvrID;     //IOCP�������ID
private:
	//����ID���ɵײ�IOCPģ��ָ�ɣ��ϲ�ֻ�ܶ�ȡ��Ӧ��ȥ�޸�����
	//����ͨ��IConnectionFactory::CreateConnection����Ĳ���id������Ӧ�� Attach������
	//�Ҳ�Ӧ��Attach����!
	int  m_iConnectID;
	
};

class IConnectionFactory
{
public:
	IConnectionFactory():m_dwSvrID(-1){};

public:
	//�ϲ�Ӧ�ñ������ش˺������������Լ���Ҫ��IConnection����
	virtual DWORD CreateServer()
	{
		m_dwSvrID = IOCPDLL::IOCP_CreateServer();
		return m_dwSvrID;
	}

	virtual void DestoryServer()
	{
		IOCPDLL::IOCP_DestoryServer( m_dwSvrID );
	}

	virtual BOOL StartServer(int nPort, int nMaxConnections = 5000, 
		int nMaxFreeBuffers = 1000, int nMaxFreeContexts = 500)
	{
		return IOCPDLL::IOCP_StartServer(m_dwSvrID,nPort,nMaxConnections,nMaxFreeBuffers,nMaxFreeContexts);
	}

	virtual void StopServer()
	{
		return IOCPDLL::IOCP_StopServer( m_dwSvrID );
	}

	virtual void SetConnCreateFactory( IConnectionFactory *v_pICF)
	{
		IOCPDLL::IOCP_SetConnCreateFactory(m_dwSvrID, v_pICF);
	}

	virtual BOOL SetLogInterFace( CHAR* v_pIFileLog, CHAR* v_pIWndLog )
	{
		return IOCPDLL::IOCP_SetLogInterFace(m_dwSvrID,v_pIFileLog,v_pIWndLog);
	}

	virtual IConnection* CreateConnectionFromRemote( OUT IConnection**v_ppConnection, IN DWORD v_dwSvrID,
													 IN int v_ConnID, IN char *v_pBuffRead, IN int nLen
													,u_long ulRemoteAddr,u_short sPort )
	{
		//�������뷵���´�����IConnection����ָ��
		//�����Ҫ�ڴ˺������������÷��ͺ�����
		//��ô����ͷ��Ӧ��������ʾ���룬ע�Ͳ���
		//������Բ�������´��롣
		//CMyConnection *p =  new CMyConnection;
		//*v_ppConnection = p;
		//p->AttachID( id );

		*v_ppConnection = NULL;
		return NULL;
	}

	//�ϲ�Ӧ�����ȴ��������Ӷ���IConnection�������䴫�룬
	//���ṩ��Զ��ip�Ͷ˿ںŵ�����£�IOCP���������ӹ��̡�
	//����ֵΪTRUE��ʾ�ɹ��������ӹ��̣����������IConnection::OnConnectionEstablished()�ķ�ʽ֪ͨ��
	virtual BOOL CreateConnectionToRemote( IConnection * v_pConnection,LPCSTR v_lpRemotIP,short sRemotePort,LPCSTR v_pSendData,int nLen )
	{
		if( m_dwSvrID == -1 )
			return FALSE;

		return IOCPDLL::IOCP_CreateConnectionToRemote( m_dwSvrID, v_pConnection,v_lpRemotIP,sRemotePort,v_pSendData,nLen );
	}

public:
	DWORD m_dwSvrID;

protected:
private:
};