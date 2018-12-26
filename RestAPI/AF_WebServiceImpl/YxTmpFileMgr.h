#pragma once

typedef BOOL (WINAPI* FP_Init_StartUp)( const int v_iTmpSize );

typedef void (WINAPI* FP_Stop_Release)();

typedef int (WINAPI* FP_Opt_Add)( const char *v_szKey, const char* v_pData, const int v_iDataLen );

typedef char* (WINAPI* FP_Opt_Get)( const char *v_szKey, int &v_iDataLen );

typedef void (WINAPI* FP_Opt_Free)( const char *v_pData );

class CYxTmpFileMgr
{
public:
	CYxTmpFileMgr();
	~CYxTmpFileMgr();

public:
	// ���ػ�����Դ��RsCommIO
	BOOL LoadRes();
	// �ͷŻ�����Դ��RsCommIO
	void FreeRes();

public:
	// ��ʼ���ӿ�
	BOOL Init_StartUp( const int v_iTmpSize );
	// ��ֹ�ӿ�
	void Stop_Release();
	// ���滺������
	int Opt_Add( const char *v_szKey, const char* v_pData, const int v_iDataLen );
	// ��ȡ��������
	char* Opt_Get( const char *v_szKey, int &v_iDataLen );
	// �ͷ��ڴ�ռ�
	void Opt_Free( const char* v_pData );

private:
	HINSTANCE			m_hInstance;

private:
	FP_Init_StartUp		m_fpInit_Startup;
	FP_Stop_Release		m_fpStopRelease;
	FP_Opt_Add			m_fpOpt_Add;
	FP_Opt_Get			m_fpOpt_Get;
	FP_Opt_Free			m_fpOpt_Free;
};