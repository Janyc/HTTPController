// AF_WebService.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������
//#include "..\ShareSource\YxPlatform\YxPlatformInterfaceLib.h"

// CAF_WebServiceApp:
// �йش����ʵ�֣������ AF_WebService.cpp
//

class CAF_WebServiceApp : public CWinApp
{
public:
	CAF_WebServiceApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��
public:
	//CYxPlatformInterfaceLib	m_YxPlatform;

	DECLARE_MESSAGE_MAP()

private:
	BOOL IsExist(const CString &strExeName, const DWORD dwProcessId);
};

extern CAF_WebServiceApp theApp;
