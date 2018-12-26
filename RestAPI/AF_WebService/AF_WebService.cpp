// AF_WebService.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AF_WebService.h"
#include "AF_WebServiceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAF_WebServiceApp

BEGIN_MESSAGE_MAP(CAF_WebServiceApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAF_WebServiceApp ����

CAF_WebServiceApp::CAF_WebServiceApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAF_WebServiceApp ����

CAF_WebServiceApp theApp;
HANDLE g_hSingleton = NULL;

// CAF_WebServiceApp ��ʼ��

BOOL CAF_WebServiceApp::InitInstance()
{
	//�ж�ʵ��Ψһ�ԣ�һ̨�����ֻ������һ��WEB����ڵ����
	DWORD dwProcessId = GetCurrentProcessId();
	if(IsExist("AF_WebService",dwProcessId))
	{
		return FALSE;
	}

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//m_YxPlatform.YxPlatform_Install("YxDFS-DataNode");

	CAF_WebServiceDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

BOOL CAF_WebServiceApp::IsExist(const CString &strExeName, const DWORD dwProcessId)
{
	HANDLE v_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(NULL == v_Snapshot)
	{
		CloseHandle(v_Snapshot);
		return	FALSE;
	}

	CString v_strExeName;
	v_strExeName.Format("%s.exe",strExeName);

	PROCESSENTRY32	processinfo;
	processinfo.dwSize = sizeof(PROCESSENTRY32);
	BOOL status = Process32First(v_Snapshot,&processinfo);
	while(status)
	{
		if( (0 == v_strExeName.CompareNoCase(processinfo.szExeFile))
			&& (processinfo.th32ProcessID != dwProcessId) )
		{
			CloseHandle(v_Snapshot);
			return TRUE;
		}

		status = Process32Next(v_Snapshot,&processinfo);
	}

	CloseHandle(v_Snapshot);

	g_hSingleton = CreateMutex(NULL, FALSE, v_strExeName);
	DWORD dwErrCode = GetLastError();
	if (dwErrCode == ERROR_ALREADY_EXISTS || dwErrCode == ERROR_INVALID_HANDLE)
	{
		return TRUE;
	}

	return FALSE;
}
