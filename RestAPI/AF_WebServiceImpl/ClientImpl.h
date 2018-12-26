#pragma once

enum _enFileType // �ļ�����
{
	enJpg = 0,
};

enum _enDownloadType // ��������
{
	enMobile = 0,
	enWeb = 1,
};

enum _enFileProp // �ļ�����
{
	enOrgFile = 0, // Դ�ļ�
	enSimPic16 = 1,// ����ͼ��ֻ���ͼƬ 16*16��
	enSimPic32 = 2,// ����ͼ��ֻ���ͼƬ 32*32��
	enSimPic48 = 3,// ����ͼ��ֻ���ͼƬ 48*48��
	enSimPic64 = 4,// ����ͼ��ֻ���ͼƬ 64*64)
	enSimPic128 = 5,// ����ͼ��ֻ���ͼƬ 128*128��
	enSrcFile = 0xFF, // �������ͼ���ϴ���ԭʼͼƬ
	enSimSize = 6,
};

enum _enRetType
{
	en_Succes=1,//�ɹ�
	en_Fail=-1,//ʧ��
	en_BufferTooSmall=-2,//�ڴ治��
	en_OverTime=-3,//��ʱ
};


// �ͻ����ļ��ϴ�����ص�������ϴ��ɹ������ļ����ƣ�������ص��ļ����Ƴ���Ϊ0�����ϴ�ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
typedef int (CALLBACK* CBF_Upload)(DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[]);

// �ֻ��ն��ļ����ؽ���ص���������سɹ������ļ����ݣ�������ص��ļ�����Ϊ0��������ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
typedef int (CALLBACK* CBF_Download_Mobile)(DWORD   _dwSN,// ҵ����ˮ��
										   BYTE    _byFileType,// �ļ�����
										   DWORD   _dwFileLen, // �ļ���С
										   char    *_pFileBuff) ;// �ļ�����);

// ��ҳ�ն��ļ����ؽ���ص���������سɹ������ļ�Url��������ص�Url����Ϊ0��������ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
typedef int (CALLBACK* CBF_Download_Web)(DWORD   _dwSN, // ҵ����ˮ��
										BYTE    _byWebUrlLen, // �ļ�Web���ʵ�ַ���ȣ�ֻ���ͼ���ļ���
										char    _szWebUrl[MAX_PATH]); // Web���ʵ�ַ);

typedef DWORD (WINAPI* FP_Create)();

typedef BOOL (WINAPI* FP_Init_Startup)( DWORD _dwObjId, int v_iNodeCnt, char* v_szIp[], WORD v_wPort[] );

typedef BOOL (WINAPI* FP_Terminate)( DWORD _dwObjId );

typedef BOOL (WINAPI* FP_ToUpload)( DWORD _dwObjId , DWORD   _dwSN,BYTE    _byFileType,DWORD   _dwFileLen,char    *_pFileBuff);

typedef BOOL (WINAPI* FP_ToDownload)( DWORD _dwObjId ,
									  DWORD   _dwSN,
									  BYTE    _byFileNameLen,
									  char    _szFileName[],
									  const BYTE v_byDownloadType,
									  const BYTE v_byFileProp );


// �ͻ���ͬ���ļ�����
typedef int (WINAPI* FP_ToDownload_Sync)( DWORD _dwObjId, 
										  DWORD   _dwSN,// ҵ����ˮ��
										  BYTE    _byFileNameLen,// �ļ�������
										  char    _szFileName[], // �ļ��������ļ��������ļ����ݣ�
										  const BYTE v_byDownloadType, //��������(�ֻ�or��ҳ)
										  const BYTE v_byFileProp,
										  char* _szFileContent,
										  int&	 _iLen
										 );//�ļ�����

typedef void (WINAPI* FP_SetICallback_Dfs_Client_Impl)( DWORD _dwObjId,
														CBF_Upload		   _cb_Upload,
														CBF_Download_Mobile  _cb_Download_Mobile,
														CBF_Download_Web     _cb_Download_Web );

class CClientImpl
{
public:	
	CClientImpl();
	~CClientImpl();

public:
	BOOL Load_Impl();
	void UnLoad_Impl();
	BOOL InitStartup( int v_iNodeCnt, char* v_szIp[], WORD v_wPort[] );
	BOOL Terminate();
	BOOL ToUpload(DWORD _dwSN,BYTE _byFileType,DWORD _dwFileLen,char *_pFileBuff);
	BOOL ToDownload( DWORD _dwSN, BYTE _byFileNameLen, char _szFileName[], const BYTE v_byDownloadType, const BYTE v_byFileProp );
	int  ToDownload_Sync( DWORD   _dwSN,// ҵ����ˮ��
						 BYTE    _byFileNameLen,// �ļ�������
						 char    _szFileName[], // �ļ��������ļ��������ļ����ݣ�
						 const BYTE v_byDownloadType, //��������(�ֻ�or��ҳ)
						 const BYTE v_byFileProp,
						 char* _szFileContent,
						 int&	 _iLen
						);//�ļ�����

	void SetICallback_Dfs_Client_Impl( CBF_Upload		   _cb_Upload,
									   CBF_Download_Mobile  _cb_Download_Mobile,
									   CBF_Download_Web     _cb_Download_Web );

private:
	DWORD Create();

private:
	DWORD m_dwObjId;
private:
	HINSTANCE m_hInstance; 
	FP_Create	m_Create;
	FP_Init_Startup m_InitStartup;
	FP_Terminate m_Stop_Release;
	FP_ToUpload		m_ToUpload;
	FP_ToDownload   m_ToDownLoad;
	FP_ToDownload_Sync	m_ToDownLoad_Sync;
	FP_SetICallback_Dfs_Client_Impl	m_SetICallback_Dfs_Client_Impl;
};