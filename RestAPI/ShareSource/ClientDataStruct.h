#pragma once

#include <eh.h>
class CSE
{
public:

	static void MapSEtoCE()
	{ 
		_set_se_translator(TranslateSEtoCE); 
	}

	operator DWORD() 
	{ 
		return (m_er.ExceptionCode); 
	}
	PVOID GetExpAddress()
	{
		return (m_er.ExceptionAddress);
	}	
private:

	CSE(PEXCEPTION_POINTERS pep)
	{
		m_er      = *pep->ExceptionRecord;
		m_context = *pep->ContextRecord;
	}

	static void _cdecl TranslateSEtoCE(UINT dwEC,
		PEXCEPTION_POINTERS pep)
	{
		throw CSE(pep);
	}

private:
	EXCEPTION_RECORD m_er;      // CPU independent exception information
	CONTEXT          m_context; // CPU dependent exception information
};

namespace nsDfs
{
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

	#pragma  pack( push, 1 )
	typedef struct _tagFileDataBuffer // �ļ�������Ϣ
	{
		DWORD   m_dwSN; // ҵ����ˮ��
		BYTE    m_byFileType; // �ļ�����
		DWORD   m_dwFileLen;  // �ļ���С
		char    *m_pFileBuff; // �ļ�����
	}CFileDataBuffer;

	typedef struct _tagFileIndexBuffer // �ļ�������Ϣ
	{
		DWORD   m_dwSN; // ҵ����ˮ��
		BYTE    m_byFileNameLen; // �ļ�������
		char    m_szFileName[MAX_PATH]; // �ļ��������ļ��������ļ����ݣ�
	}CFileIndexBuffer;

	typedef struct _tagFileWebUrl // �ļ�Web��ַ
	{
		DWORD   m_dwSN; // ҵ����ˮ��
		BYTE    m_byWebUrlLen; // �ļ�Web���ʵ�ַ���ȣ�ֻ���ͼ���ļ���
		char    m_szWebUrl[MAX_PATH]; // Web���ʵ�ַ
	}CFileWebUrl;
	#pragma  pack( pop )


	// �ͻ����ļ��ϴ�����ص�������ϴ��ɹ������ļ����ƣ�������ص��ļ����Ƴ���Ϊ0�����ϴ�ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
	typedef int (CALLBACK* CBF_Upload)(DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[]);

	// �ֻ��ն��ļ����ؽ���ص���������سɹ������ļ����ݣ�������ص��ļ�����Ϊ0��������ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
	typedef int (CALLBACK*CBF_Download_Mobile)(DWORD   _dwSN,// ҵ����ˮ��
		BYTE    _byFileType,// �ļ�����
		DWORD   _dwFileLen, // �ļ���С
		char    *_pFileBuff) ;// �ļ�����);

	// ��ҳ�ն��ļ����ؽ���ص���������سɹ������ļ�Url��������ص�Url����Ϊ0��������ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
	typedef int (CALLBACK*CBF_Download_Web)(DWORD   _dwSN, // ҵ����ˮ��
		BYTE    _byWebUrlLen, // �ļ�Web���ʵ�ַ���ȣ�ֻ���ͼ���ļ���
		char    _szWebUrl[MAX_PATH]); // Web���ʵ�ַ);

	class C_Dfs_Client_CB// Dfs�ͻ���ҵ��ص��ӿ�
	{
	public:
		CBF_Upload	m_cbUpload;
		CBF_Download_Mobile m_cbDownload;
		CBF_Download_Web    m_cbDownload_Web;
	public:
		C_Dfs_Client_CB(void):m_cbUpload(NULL),m_cbDownload(NULL),m_cbDownload_Web(NULL)
		{

		}

		virtual ~C_Dfs_Client_CB(void){};

	public:

		// �ͻ����ļ��ϴ�����ص�������ϴ��ɹ������ļ����ƣ�������ص��ļ����Ƴ���Ϊ0�����ϴ�ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
		int CB_Upload(DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[])
		{
			if (m_cbUpload!=NULL)
			{

				return m_cbUpload(_dwSN,_byFileNameLen,_szFileName);
			}
			return 0;
		}

		// �ֻ��ն��ļ����ؽ���ص���������سɹ������ļ����ݣ�������ص��ļ�����Ϊ0��������ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
		int CB_Download_Mobile(DWORD   _dwSN,// ҵ����ˮ��
			BYTE    _byFileType,// �ļ�����
			DWORD   _dwFileLen, // �ļ���С
			char    *_pFileBuff) // �ļ�����);
		{
			if (m_cbDownload!=NULL)
			{
				return m_cbDownload(_dwSN,_byFileType,_dwFileLen,_pFileBuff);
			}
			return 0;
		}

		// ��ҳ�ն��ļ����ؽ���ص���������سɹ������ļ�Url��������ص�Url����Ϊ0��������ʧ�ܣ���������Ҫ����ʧ�������ʧ�����ͣ�
		int CB_Download_Web(DWORD   _dwSN, // ҵ����ˮ��
			BYTE    _byWebUrlLen, // �ļ�Web���ʵ�ַ���ȣ�ֻ���ͼ���ļ���
			char    _szWebUrl[MAX_PATH])
		{
			if (m_cbDownload_Web!=NULL)
			{
				m_cbDownload_Web(_dwSN,_byWebUrlLen,_szWebUrl);
			}
			return 0;
		}

	};
}

using namespace nsDfs;

