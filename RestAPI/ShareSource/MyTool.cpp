// MyTool.cpp: implementation of the CMyTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyTool::CMyTool()
{

}

CMyTool::~CMyTool()
{

}

BOOL CMyTool::SetFileToCurrentTime(HANDLE hFile)
{
	FILETIME ft;
	SYSTEMTIME st;
	BOOL bRet;
	
	GetSystemTime(&st);              // gets current time
	SystemTimeToFileTime(&st, &ft);  // converts to file time format
	
	bRet = SetFileTime( hFile, NULL, NULL, &ft);// sets last-write time for file
	
	return bRet;
}

BOOL CMyTool::IsProcessExist(LPCSTR lpcstrExeName, const DWORD dwProcessId)
{
	HANDLE v_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(NULL == v_Snapshot)
	{
		CloseHandle(v_Snapshot);
		return	FALSE;
	}
	
	CString v_strExeName;
	v_strExeName.Format("%s.exe",lpcstrExeName);
	
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
	return FALSE;
}

void CMyTool::GetCurrDateTime(char *v_pszCurrTiem, int nLen, int nDateType/*=0*/)
{
	if (NULL==v_pszCurrTiem || nLen < 20 ) 
	{
		return;
	}
	
	SYSTEMTIME st;
	::GetLocalTime(&st);
	
	switch(nDateType)
	{
	case 1:		///< ��-��-��
		sprintf(v_pszCurrTiem ,"%04d-%02d-%02d", st.wYear ,st.wMonth ,st.wDay);
		break;
		
	case 2:		///< ʱ:��:��
		sprintf(v_pszCurrTiem ,"%02d:%02d:%02d", st.wHour ,st.wMinute ,st.wSecond);
		break;	
		
	default:	///< ��-��-�� ʱ:��:��
		sprintf(v_pszCurrTiem ,"%04d-%02d-%02d %02d:%02d:%02d", st.wYear ,st.wMonth ,st.wDay ,st.wHour ,st.wMinute ,st.wSecond);	
		break;
	}
}

BOOL CMyTool::IsFileOrDirExists(const char *v_pszPath)
{
	if ( NULL == v_pszPath )
	{
		return FALSE;
	}
	
	if (_access(v_pszPath, 0x00) != 0)
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL CMyTool::MakeMultiDir(const char v_szPath[])
{
	char szPath[512] = {0};
	char szTmpPath[512] = {0};
	memset(szPath, 0, sizeof(szPath));
	strcpy(szPath, v_szPath);
	size_t iLen = strlen(szPath);
	if (iLen > 0)
	{
		if (szPath[iLen - 1] != '\\')
		{
			szPath[iLen] = '\\';
			iLen += 1;
		}
	}
	else
	{
		return FALSE;
	}
	
	char* p = strchr(szPath, '\\');
	if (*(p - 1) == ':')
	{
		p = strchr(p + 1, '\\');
	}
	else if (p == szPath)
	{
		memset(szTmpPath, 0, sizeof(szTmpPath));
		strcpy(szTmpPath, szPath);
		sprintf(szPath, ".%s", szTmpPath);
		p = strchr(szPath + 2, '\\');
	} 
	
	while (p)
	{
		if ((*(p - 1) != '.'))
		{
			memset(szTmpPath, 0, sizeof(szTmpPath));
			strncpy(szTmpPath, szPath, p - szPath);
			if (_access(szTmpPath, 0) == -1)
			{
				if (_mkdir(szTmpPath) == -1)
				{
					if (errno != EEXIST)
					{
						return FALSE;
					}
				}
			}
		}
		
		p = strchr(p + 1, '\\');
	}
	
	return TRUE;
}	


BOOL CMyTool::GetFileListFromDir(CStringArray &v_arrCStr, LPCSTR pszDir, LPCSTR pszFilter /*= "*.*"*/)
{
	if ( NULL == pszDir )
	{
		return FALSE;
	}
	
	CString strFilter;
	if ( '\\' != CString(pszDir).Right(1) )
	{
		strFilter.Format("%s\\%s", pszDir, pszFilter);
	}
	else
	{
		strFilter.Format("%s%s", pszDir, pszFilter);
	}
	
	HANDLE	hFindFile = NULL;
    WIN32_FIND_DATA	finddata;
	
    hFindFile = ::FindFirstFile( strFilter, &finddata );
	if ( NULL == hFindFile || INVALID_HANDLE_VALUE == hFindFile )
	{
		::FindClose(hFindFile);
		return FALSE;
	}
	
    CString	strFilePath;
	
	/// ����ʱ�������ҵ� ��.�� �� ��..������Ŀ¼������������������
    BOOL bIsFind=TRUE;
    while ( bIsFind )
    {
		if ( 0==strcmp(".", finddata.cFileName) || 0==strcmp("..", finddata.cFileName) )
		{
			bIsFind = ::FindNextFile( hFindFile, &finddata );
			continue;
		}
		
		/// �����Ŀ¼�������ݹ鴦��
		if ( (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            strFilePath.Format("%s\\%s", pszDir, finddata.cFileName);
			GetFileListFromDir(v_arrCStr, strFilePath, pszFilter);
        }
		/// ������ļ�
		else if ( (finddata.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE)
		{
			strFilePath.Format("%s\\%s", pszDir, finddata.cFileName);
			v_arrCStr.Add(strFilePath);
		}
		
        bIsFind = ::FindNextFile( hFindFile, &finddata );
    }
	
    ::FindClose(hFindFile);

	return ( v_arrCStr.GetSize() > 0 );
}

void CMyTool::DeleteDirAndFile(LPCSTR pszDir, LPCSTR pszFilter /*= "*.*"*/, BOOL bIsDelDir /*= FALSE*/)
{
	if ( NULL == pszDir )
	{
		return;
	}
	
	CString strFilter;
	strFilter.Format("%s\\%s", pszDir, pszFilter);
	
	HANDLE	hFindFile = NULL;
    WIN32_FIND_DATA	finddata;
	
    hFindFile = ::FindFirstFile( strFilter, &finddata );
	if ( NULL == hFindFile || INVALID_HANDLE_VALUE == hFindFile )
	{
		::FindClose(hFindFile);
		return;
	}
		
    CString	strFilePath;
	
	/// ����ʱ�������ҵ� ��.�� �� ��..������Ŀ¼������������������
    BOOL bIsFind=TRUE;
    while ( bIsFind )
    {
		if ( 0==strcmp(".", finddata.cFileName) || 0==strcmp("..", finddata.cFileName) )
		{
			bIsFind = ::FindNextFile( hFindFile, &finddata );
			continue;
		}
		
		/// �����Ŀ¼������ݹ鴦��
		if ( (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            strFilePath.Format("%s\\%s", pszDir, finddata.cFileName);
			DeleteDirAndFile(strFilePath, pszFilter, bIsDelDir);
        }
		/// ������ļ�����ɾ��
		else if ( (finddata.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE)
		{
			strFilePath.Format("%s\\%s", pszDir, finddata.cFileName);
	        remove(strFilePath);
		}

        bIsFind = ::FindNextFile( hFindFile, &finddata );
    }

	/// Ŀ¼�µ��ļ�ɾ����Ϻ��ж��Ƿ����ɾ��Ŀ¼
	if (bIsDelDir)
	{
		::RemoveDirectory(pszDir);
	}

    ::FindClose(hFindFile);	
}

void CMyTool::GetCurrModulePath(char* v_pModuleDir, int _nDirLen, char* v_pModuleName, int _nNameLen)
{
	//char   szModuleDir[MAX_PATH];
	::GetModuleFileName( NULL, v_pModuleDir, _nDirLen );
	
	char   szDriver[MAX_PATH];
	char   szDir[MAX_PATH];
	char   szFName[MAX_PATH];
	char   szExt[MAX_PATH];
	
	/// ���ģ��ȫ·��
	::_splitpath( v_pModuleDir, szDriver, szDir, szFName, szExt );
	
	/// �������ģ��·��
	::_makepath( v_pModuleDir, szDriver, szDir, NULL, NULL );
	
	/// ���ģ�����ƣ���չ��szExt�����Ѿ�����.
	sprintf( v_pModuleName, "%s%s", szFName, szExt );
}

void CMyTool::ExtractFilePath(LPCSTR _pszFilePath, LPSTR v_pszFileDir, int _nDirSize, LPSTR v_pszFileName, int _nNameSize, BOOL _bIncExt /*= FALSE*/)
{
	char   szDriver[MAX_PATH];
	char   szDir[MAX_PATH];
	char   szFName[MAX_PATH];
	char   szExt[MAX_PATH]; 
	
	/// ����ļ�ȫ·��
	::_splitpath( _pszFilePath, szDriver, szDir, szFName, szExt );
	
	/// �������·��
	::_makepath( v_pszFileDir, szDriver, szDir, NULL, NULL );
	
	/// �ļ�����
	if ( _bIncExt )
	{
		sprintf( v_pszFileName, "%s%s", szFName, szExt );	
	}
	else
	{
		sprintf( v_pszFileName, "%s", szFName );
	}

}

BOOL CMyTool::ExtractSubString(const char* v_szSrc, char* v_szDest, int v_iMaxLen, int v_iSubStr, char v_cDelim)
{
	memset(v_szDest, 0, v_iMaxLen);

	if (!v_szSrc)
	{
		return FALSE;
	}
	
	const char* p1 = v_szSrc;
	const char* p2 = v_szSrc;
	int iCount = 0;
	
	while (*p2 != '\0')		///< ����ַ�ƥ��
	{
		if (*p2 == v_cDelim)  ///< ƥ�䵽ָ���ķָ���
		{
			if ((*p1 != '\"') || ((p1+1 < p2) && (*(p2-1) == '\"')))
			{
				++iCount;
				if (iCount == v_iSubStr + 1)
				{
					if ((int) (p2 - p1) <= (v_iMaxLen - 1))
					{
						memcpy(v_szDest, p1, p2 - p1);
						v_szDest[p2 - p1] = '\0';
						return true;
					}
					else
					{
						memcpy(v_szDest, p1, v_iMaxLen - 1);
						v_szDest[v_iMaxLen - 1] = '\0';
						return false;
					}
				}
				else
				{
					p1 = p2 + 1;
				}
			}
		}
		++p2;
	}
	
	if ((iCount > 0) && 
		(iCount == v_iSubStr) &&
		((*p1 != '\"') || ((p1+1 < p2) && (*(p2-1) == '\"')))
		)
	{
		if ((int) (p2 - p1) <= (v_iMaxLen - 1))
		{
			memcpy(v_szDest, p1, p2 - p1);
			v_szDest[p2 - p1] = '\0';
			return true;
		}
		else
		{
			memcpy(v_szDest, p1, v_iMaxLen - 1);
			v_szDest[v_iMaxLen - 1] = '\0';
			return false;
		}
	}
	
	return false;
}

int CMyTool::SplitCString(LPCSTR _lpcstrSrc, LPCSTR _lpcstrSplit, CStringArray& v_arrCString)
{	
	v_arrCString.RemoveAll();  

	CString strTemp = _lpcstrSrc;  
	int iIndex = 0;  
	while (1)  
	{  
		iIndex = strTemp.Find(_lpcstrSplit);  //�Ҳ�������-1
		if(iIndex >= 0)  
		{  
			v_arrCString.Add(strTemp.Left(iIndex));   ///< �󿽱���������index���ָ���

			strTemp = strTemp.Right(strTemp.GetLength()-iIndex-strlen(_lpcstrSplit));  ///< �ҿ����������ָ���
			//strTemp = strTemp.Mid(iIndex+strlen(_lpcstrSplit));
		}  
		else  
		{  
			break;  
		}  
	}  

	v_arrCString.Add(strTemp); 
	
	return v_arrCString.GetSize();  
}  


void CMyTool::StrDateToChar(LPCSTR _lpcstrDate, char v_szDate[])
{
	int nYear = 0, nMon =0, nDay = 0;   
	sscanf( _lpcstrDate, "%d-%d-%d %d:%d:%d", &nYear, &nMon, &nDay);   
	
	v_szDate[0] = (BYTE)(nYear-2000);
	v_szDate[1] = (BYTE)nMon;
	v_szDate[2] = (BYTE)nDay;
}

void CMyTool::StrDatetimeToChar(LPCSTR _lpcstrDatetime, char v_szDatetime[])
{	
	int nYear = 0, nMon =0, nDay = 0, nHour = 0, nMin = 0, nSec = 0;   
	sscanf( _lpcstrDatetime, "%d-%d-%d %d:%d:%d", &nYear, &nMon, &nDay, &nHour, &nMin, &nSec);   
	
	v_szDatetime[0] = (BYTE)(nYear-2000);
	v_szDatetime[1] = (BYTE)nMon;
	v_szDatetime[2] = (BYTE)nDay;
	v_szDatetime[3] = (BYTE)nHour;
	v_szDatetime[4] = (BYTE)nMin;
	v_szDatetime[5] = (BYTE)nSec;
/*

	/// ȡ��������
	char szYMD[12] = {0};
	ExtractSubString( _lpcstrDatetime, szYMD, sizeof(szYMD), 0, ' ' );
	
	/// ȡ��ʱ����
	char szHMS[12] = {0};
	ExtractSubString( _lpcstrDatetime, szHMS, sizeof(szHMS) ,1, ' ' );


	//-------------------------------------------------------------------------
	/// ���������н���
	/// ��
	char szYear[6] = {0};
	ExtractSubString( szYMD,szYear, sizeof(szYear), 0, '-' );
	int iYear = (int)strtoul( szYear, NULL, 10 );
	
	/// ��
	char szMonth[4] ={0};
	ExtractSubString( szYMD, szMonth, sizeof(szMonth), 1, '-' );
	int iMonth = (int)strtoul( szMonth, NULL, 10 );
	
	/// ��
	char szDay[4] = {0};
	ExtractSubString( szYMD, szDay, sizeof(szDay), 2, '-' );
	int iDay = (int)strtoul( szDay, NULL, 10 );

	//-------------------------------------------------------------------------
	/// ��ʱ�����н���
	/// ʱ
	char szHour[4] = {0};
	ExtractSubString( szHMS, szHour, sizeof(szHour), 0, ':' );
	int iHour = (int)strtoul( szHour, NULL, 10 );
	
	/// ��
	char szMin[4] ={0};
	ExtractSubString( szHMS, szMin, sizeof(szMin), 1, ':' );
	int iMin = (int)strtoul( szMin, NULL, 10 );
	
	/// ��
	char szSecond[4] ={0};
	ExtractSubString( szHMS, szSecond, sizeof(szSecond), 2, ':' );
	int iSec = (int)strtoul( szSecond, NULL, 10 );
	
	v_szDatetime[0] = (BYTE)(iYear - 2000);
	v_szDatetime[1] = (BYTE)iMonth;
	v_szDatetime[2] = (BYTE)iDay;
	v_szDatetime[3] = (BYTE)iHour;
	v_szDatetime[4] = (BYTE)iMin;
	v_szDatetime[5] = (BYTE)iSec;*/	
}

time_t CMyTool::ConstructDatetime(int _nY, int _nMon, int _nD, int _H, int _nMin, int _nS)
{
	/// 1970��1��1��0ʱ0��0�룬2038��1��18��19ʱ14��07��

	//int tm_sec;     /* �� �C ȡֵ����Ϊ[0,59] */
	//int tm_min;     /* �� - ȡֵ����Ϊ[0,59] */
	//int tm_hour;    /* ʱ - ȡֵ����Ϊ[0,23] */
	//int tm_mday;    /* һ�����е����� - ȡֵ����Ϊ[1,31] */
	//int tm_mon;     /* �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11] */
	//int tm_year;    /* ��ݣ���ֵ����ʵ����ݼ�ȥ1900 */
	//int tm_wday;    /* ���� �C ȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ����� */
	//int tm_yday;    /* ��ÿ���1��1�տ�ʼ������ �C ȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ����� */
    //int tm_isdst;   /* ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����*/
	
	tm dt;
	dt.tm_year	= _nY - 1900;
	dt.tm_mon	= _nMon - 1 ;
	dt.tm_mday	= _nD;
	dt.tm_hour	= _H;
	dt.tm_min	= _nMin;
	dt.tm_sec	= _nS;

	return mktime( &dt );
}

int CMyTool::Add7EforData(char _szOldBuf[], char _szNewBuf[], int _iOldLen)
{
	int		v_iPos=1,i;
	char	v_szTemp[3000]={0};
	
	for( i=0; i<_iOldLen; i++ )
	{
		switch( _szOldBuf[i] )
		{
		case 0x7e:
			v_szTemp[v_iPos++] = 0x7d;
			v_szTemp[v_iPos++] = 0x02;
			break;
		case 0x7d:
			v_szTemp[v_iPos++] = 0x7d;
			v_szTemp[v_iPos++] = 0x01;
			break;
		default:
			v_szTemp[v_iPos++] = _szOldBuf[i];
			break;
		}
	}
	
	v_szTemp[0]=0x7e;
	v_szTemp[v_iPos++]=0x7e;	
	
	memcpy(_szNewBuf,v_szTemp,v_iPos);
	return v_iPos;
}

BYTE CMyTool::GetXorCheckSum(const BYTE *_szBuf, const int _iLen)
{	
	BYTE	v_byCheckSum = 0;
	for(int i=0; i<_iLen; i++)
	{		
		v_byCheckSum=v_byCheckSum^_szBuf[i];  //<���
	}
	return v_byCheckSum;
}

void CMyTool::FillBlackToMobile(char _szMobile[], int _iLen)
{
	int	i=0,j=0;
	
	for(i=0;i<_iLen;i++)
	{
		if(_szMobile[i]==0)
		{
			for(j=i;j<_iLen;j++)
			{
				_szMobile[j]=0x20;
			}
			break;
		}
	}
}	


BOOL CMyTool::CheckTimeLimit(char* pcCurTime, char*pcTimeBegin, char*pcTimeEnd)
{
	
	
	CTime ctTimeBegin = 0;
	CTime ctCurTime = 0;
	CTime ctTimeEnd = 0;
	
	BOOL m1 = ByteTimeToCTime(pcTimeBegin, ctTimeBegin);
	BOOL m2 = ByteTimeToCTime(pcCurTime, ctCurTime);
	BOOL m3 = ByteTimeToCTime(pcTimeEnd, ctTimeEnd);
	
	if ((m1 == FALSE) && (m2 == FALSE) && (m3 == FALSE))
	{
		return FALSE;
	}
	
	if(ctTimeBegin <= ctCurTime && ctCurTime <= ctTimeEnd)
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CMyTool::GetLocalHostIP(CString& strIPAddr)
{
	char szHostName[256] = {0};
	
	if(gethostname(szHostName, sizeof(szHostName)))
	{
		return FALSE;
	}
	
	PHOSTENT hostinfo = NULL;
	hostinfo = gethostbyname(szHostName);
	if( NULL == hostinfo)
	{
		return FALSE;
	}
	
	LPCSTR ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
	
	strIPAddr = ip;
	
	return TRUE;
}

DWORD CMyTool::GetProcessIdByName(LPCTSTR pszExeFile)
{
	DWORD dwProcessId(0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		BOOL bMore = ::Process32First(hProcessSnap,&pe32);
		while(bMore)
		{
			//printf("�������ƣ�%s\n",pe32.szExeFile);
			//printf("����ID��%u\n\n",pe32.th32ProcessID);
			if (lstrcmpi(pszExeFile, pe32.szExeFile) == 0)
			{
				dwProcessId = pe32.th32ProcessID;
				break;
			}
			bMore = ::Process32Next(hProcessSnap,&pe32);
		}
		
		CloseHandle(hProcessSnap);
	}
	return dwProcessId;
}

BOOL CMyTool::GetProcessNameById(DWORD dwProcessId, CString& lpstrProcName)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, dwProcessId);
	
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		lpstrProcName.Format("%s",pe32.szExeFile);
		//strcpy_s(lpstrName, nMaxLen, pe32.szExeFile);
		
		CloseHandle(hProcessSnap);

		return TRUE;
	}

	return FALSE;
}

BOOL CMyTool::StartProcess(LPCTSTR lpcstrProcessName, LPTSTR lpstrProcessPath)
{
	//ShellExecute(NULL ,"open", lpcstrProcessName, NULL, lpcstrProcessPath , SW_SHOWNORMAL);
	
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	
	// Start the child process. 
	if ( !CreateProcess( NULL,   // No module name (use command line). 
		lpstrProcessPath, // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		)
	{
		//����ʧ��
		return FALSE;
	}
	else{
		//������ɺ��ͷŹ�ϵ
		
		// Wait until child process exits.
		//WaitForSingleObject( pi.hProcess, INFINITE );
		
		// Close child process and main thread handles. 
		CloseHandle( pi.hThread );
		CloseHandle( pi.hProcess );
	}
	
	return TRUE;
}


BOOL CMyTool::CloseProcess(DWORD dwProcessId)
{
	HANDLE hProcess=OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	
	if(NULL==hProcess){ return false; }
	
	if( !TerminateProcess(hProcess,0) )
	{
		//���ٽ���ʧ��
		return FALSE;
	}
	
	//�����ѱ�����
	return TRUE;
}


BOOL CMyTool::IsRegOcx(LPCSTR lpcstrCLSID)
{
	/// mscomm32.ocx��Clsid={648A5600-2C6E-101B-82B6-000000000014}
	/// Teechart8.ocx��Clsid={BDEB0088-66F9-4A55-ABD2-0BF8DEEC1196}
	/// Teechart5.ocx��Clsid={B6C10489-FB89-11D4-93C9-006008A7EED4}	
	/// ע�������
    HKEY    hKeyClsid = NULL; 
	BOOL    bRet = FALSE;
	int     nRet = ERROR_SUCCESS;
	CString strSubKey;
	strSubKey.Format("CLSID\\%s\\InprocServer32",lpcstrCLSID);
    //char   *szsubkey   =   "CLSID\\{1FDB3FEB-A12B-4A8F-9BA7-CC342949D9AF}\\InprocServer32";

	nRet = ::RegOpenKeyEx( HKEY_CLASSES_ROOT, strSubKey, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, &hKeyClsid );
    if( nRet != ERROR_SUCCESS )
    {
        bRet = FALSE;
    }
    else
    {
        char pFile[MAX_PATH];
        memset(pFile, 0, MAX_PATH);
        DWORD  datalen = MAX_PATH;
		DWORD  datatype = REG_SZ;
		
		/// �õ�Ocx�ļ�·��
        nRet = ::RegQueryValueEx(hKeyClsid, "", NULL, &datatype,(unsigned char*)pFile, &datalen);
		
        if( NULL != hKeyClsid )
		{
            ::RegCloseKey( hKeyClsid );
		}

		if ( nRet != ERROR_SUCCESS )
		{
			bRet = FALSE;
		}
		else
		{
			/// �ж�Ocx�ļ��Ƿ����
			CFileFind ff;
			bRet = ff.FindFile(pFile); 
			ff.Close();
		}
    }

	return bRet;		
}

BOOL CMyTool::RegOcx(LPCSTR lpcstrOcxPath)
{
	if ( NULL == lpcstrOcxPath )  return  FALSE;

	BOOL  bRet = FALSE;

	/// �ж�Ocx�ļ��Ƿ����
	CFileFind ff;
	bRet = ff.FindFile(lpcstrOcxPath);
	ff.Close();
	if ( !bRet )  return  FALSE;
	
	/// ���ؿؼ�
	HINSTANCE hLib = NULL;
	hLib = LoadLibrary(lpcstrOcxPath);
	if ( hLib < (HINSTANCE)HINSTANCE_ERROR )  return  FALSE;

	/// ��ȡע�ắ��DllRegisterServer��ַ
	FARPROC  lpDllEntryPoint = NULL;
	lpDllEntryPoint = GetProcAddress(hLib, "DllRegisterServer"); 
	if( lpDllEntryPoint==NULL )
	{
		FreeLibrary( hLib );
		return FALSE;
	}
	
	if( FAILED( (*lpDllEntryPoint)() ) )
	{
		FreeLibrary(hLib);
		return FALSE;
	}

    FreeLibrary(hLib);	

	return TRUE;
}

BOOL CMyTool::UnRegOcx(LPCSTR lpcstrOcxPath)
{
	if ( NULL == lpcstrOcxPath )  return FALSE;
	
	BOOL  bRet = FALSE;
	
	/// �ж�Ocx�ļ��Ƿ����
	CFileFind ff;
	bRet = ff.FindFile(lpcstrOcxPath);
	ff.Close();
	if ( !bRet )  return FALSE;
	
	/// ���ؿؼ�
	HINSTANCE hLib = NULL;
	hLib = LoadLibrary(lpcstrOcxPath);
	if ( hLib < (HINSTANCE)HINSTANCE_ERROR )  return  FALSE;
	
	/// ��ȡע�ắ��DllUnregisterServer��ַ
	FARPROC  lpDllEntryPoint = NULL;
	lpDllEntryPoint = GetProcAddress(hLib, "DllUnregisterServer"); 
	if( lpDllEntryPoint==NULL )
	{
		FreeLibrary( hLib );
		return FALSE;
	}
	
	if( FAILED( (*lpDllEntryPoint)() ) )
	{
		FreeLibrary(hLib);
		return FALSE;
	}
	
    FreeLibrary(hLib);	
	
	return TRUE;
}

BOOL CMyTool::ByteTimeToCTime(const char szDatetime[], CTime &ctTime)
{
	try
	{
		int nYear = 2000+szDatetime[0];		
		int nMonth = szDatetime[1];
		int nDay = szDatetime[2];
		int nHour = szDatetime[3];
		int nMinute = szDatetime[4];
		int nSecond = szDatetime[5];

		if (nMonth<1 || nMonth>12 )
		{
			return FALSE;
		}
		if (nDay<1 || nDay>31 )
		{
			return FALSE;
		}
		if (nHour<0 || nHour>23 )
		{
			return FALSE;
		}
		if (nMinute<0 || nMinute>59 )
		{
			return FALSE;
		}
		if (nSecond<0 || nSecond>59 )
		{
			return FALSE;
		}

		CTime ctTimeBegin(nYear,nMonth,nDay,nHour,nMinute,nSecond);	
		ctTime = ctTimeBegin;

		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}
}