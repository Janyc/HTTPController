// AF_WebServiceImpl.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "AF_WebServiceImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	ע�⣡
//
//		����� DLL ��̬���ӵ� MFC
//		DLL���Ӵ� DLL ������
//		���� MFC ���κκ����ں�������ǰ��
//		��������� AFX_MANAGE_STATE �ꡣ
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CAF_WebServiceImplApp

BEGIN_MESSAGE_MAP(CAF_WebServiceImplApp, CWinApp)
END_MESSAGE_MAP()


// CAF_WebServiceImplApp ����

CAF_WebServiceImplApp::CAF_WebServiceImplApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAF_WebServiceImplApp ����

CAF_WebServiceImplApp theApp;

BOOL WINAPI DLL_InitStartup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog, WORD _wListenPort, const char* _szDir )
{
	if( theApp.m_AF_WebServiceImpl.Init_Startup( _pMyObj, _fpCB_PrintLog, _wListenPort,_szDir ) == -1 )
	{
		return FALSE;
	}

	return TRUE;
}

void WINAPI DLL_Terminate()
{
	theApp.m_AF_WebServiceImpl.Stop_Release();
}


// CAF_WebServiceImplApp ��ʼ��

BOOL CAF_WebServiceImplApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

// ��ʱ�߳�
UINT WINAPI g_OnTimes_100ms(void* v_pParam)
{
	CAF_WebServiceImpl* pInstance =(CAF_WebServiceImpl*)v_pParam;
	pInstance->OnTimer_100ms();

	::_endthreadex(0);

	return 0;
}

CAF_WebServiceImpl::CAF_WebServiceImpl(void)
{
	// ��־������
	m_fpCB_PrintLog = NULL;
	m_pMyObj = NULL;
	m_hTimerThread_100ms = NULL;				
	m_uiTimerThreadID = 0;	
	m_bExit = false;
	m_bInitSucceed = false;
	m_hExitEvent = NULL;

	m_lstLogObject.clear();

	::InitializeCriticalSection(&m_csLog);
}

CAF_WebServiceImpl::~CAF_WebServiceImpl(void)
{
	_ClearLogObject();

	::DeleteCriticalSection(&m_csLog);
}

int CAF_WebServiceImpl::Init_Startup( char *_pMyObj, CB_PRINT_LOG _fpCB_PrintLog, WORD _wListenPort, const char* _szDir )
{
	if( m_bInitSucceed )
	{
		return 0;
	}
	g_pAF_WebServiceImpl = this; 

	m_pMyObj = _pMyObj;
	m_fpCB_PrintLog = _fpCB_PrintLog;
	m_wLisenPort = _wListenPort;
	m_strDir.assign(_szDir);
	int nLen = (int)m_strDir.length();
	if ( '\\'!=m_strDir[nLen-1] )
		m_strDir.append("\\");

	// �����ȴ��˳�ʱ��
	m_hExitEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( m_hExitEvent == NULL )
	{
		return -1;
	}

	// ������־����߳�
	m_hTimerThread_100ms = (HANDLE)_beginthreadex( NULL ,0 ,g_OnTimes_100ms ,(void*)this ,0 ,&m_uiTimerThreadID);
	if( NULL == m_hTimerThread_100ms )
	{
		return -1;
	}

	//##>>��ʼ��HTTPͨ�Ų�
	if( -1 == m_HttpManager.Init( _wListenPort ) )
	{
		toPrintLog(_enError,"[CAF_WebServiceImpl::Init_Startup]: ��ʼ��ͨ�Ų�ʧ��!");
		return -1;
	}
	toPrintLog(_enComm,"[CAF_WebServiceImpl::Init_Startup]: ��ʼ��ͨ�Ų�ɹ�...");

	toPrintLog(_enComm,"[CAF_WebServiceImpl::Init_Startup]: ��ʼ���ɹ�...");

	return 0;
}

void CAF_WebServiceImpl::Stop_Release()
{
	// ��־�߳��˳�
	m_bExit = true;

	::WaitForSingleObject( m_hExitEvent, INFINITE );

	if( m_hExitEvent != NULL )
	{// �ر�ʱ����
		::CloseHandle(m_hExitEvent);
	}

	m_HttpManager.Release();

	_ClearLogObject();
}

void CAF_WebServiceImpl::OnTimer_100ms()
{
	std::list<CLogObject*> clsLstLog;
	CLogObject* pLogObj = NULL;

	static LONG lCount = 0;
	while(true)
	{
		Sleep(100);  // 100msͳһ���һ��

		if( m_bExit )
		{
			::SetEvent(m_hExitEvent);
			return;
		}

		OnTimer_LogOutPut();

		if( ::InterlockedIncrement( &lCount ) == 10 )
		{
			m_HttpManager.TimerService_1000ms();

			::InterlockedExchange(&lCount,0);
		}
	}
}

void CAF_WebServiceImpl::OnTimer_LogOutPut()
{
	std::list<CLogObject*> clsLstLog;
	CLogObject* pLogObj = NULL;
	clsLstLog.clear();

	::EnterCriticalSection(&m_csLog);
	{
		if( m_lstLogObject.size() == 0 )
		{
			::LeaveCriticalSection(&m_csLog);
		}

		for( int i=0; i<50 && !m_lstLogObject.empty(); i++ )
		{
			pLogObj = m_lstLogObject.front();

			if( pLogObj == NULL )
			{
				m_lstLogObject.pop_front();
				continue;
			}
			clsLstLog.push_back(pLogObj);
			m_lstLogObject.pop_front();
		}
	}
	::LeaveCriticalSection(&m_csLog);

	while( !clsLstLog.empty() )
	{
		pLogObj = clsLstLog.front();
		clsLstLog.pop_front();

		if( pLogObj == NULL )
		{
			continue;
		}

		if( m_fpCB_PrintLog != NULL )
		{
			m_fpCB_PrintLog( m_pMyObj, pLogObj->m_iLogType,pLogObj->m_strLogDesc.c_str(),pLogObj->m_strLogText.c_str());
		}

		if( pLogObj != NULL )
		{
			delete pLogObj;
			pLogObj = NULL;
		}
	}
	clsLstLog.clear();
}

std::string& CAF_WebServiceImpl::GetDir()
{
	return m_strDir;
}

void CAF_WebServiceImpl::toPrintLog( int _iLogType, const char _szLogText[] )
{
	CLogObject *pLogObj = NULL;
	try
	{
		pLogObj = new CLogObject();
		pLogObj->m_iLogType = _iLogType;
		pLogObj->m_strLogDesc = "AF_WebServiceImpl.dll";
		pLogObj->m_strLogText = _szLogText;

		::EnterCriticalSection(&m_csLog);
		m_lstLogObject.push_back(pLogObj);
		::LeaveCriticalSection(&m_csLog);
	}
	catch(...)
	{
		if( pLogObj != NULL )
		{
			delete pLogObj;
			pLogObj = NULL;
		}
	}
}

void CAF_WebServiceImpl::_ClearLogObject()
{
	::EnterCriticalSection(&m_csLog);
	{
		CLogObject *pLogObj = NULL;
		while( !m_lstLogObject.empty() )
		{
			pLogObj = m_lstLogObject.front();
			if( pLogObj != NULL )
			{
				delete pLogObj;
				pLogObj = NULL;
			}

			m_lstLogObject.pop_front();
		}
	}
	::LeaveCriticalSection(&m_csLog);

	m_lstLogObject.clear();
}