// MyTool.h: interface for the CMyTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTOOL_H__6B7665CE_BD59_46DB_B704_4566D6D51866__INCLUDED_)
#define AFX_MYTOOL_H__6B7665CE_BD59_46DB_B704_4566D6D51866__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <windows.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <stdarg.h>

#include <direct.h>
#include <errno.h>
#include <io.h>
#include "tlhelp32.h"
#include "winsock2.h"
#include "afxsock.h"

class CMyTool  
{
public:
	CMyTool();
	virtual ~CMyTool();
	
	// SetFileToCurrentTime - sets last write time to current system time
	// Return value - TRUE if successful, FALSE otherwise
	// hFile  - must be a valid file handle
	static BOOL SetFileToCurrentTime(HANDLE hFile);

	/*
	* ���Ӧ�ó�������Ƿ��Ѿ�����
	* ����Ӧ�ó�������m_pszExeName�ͽ���ID GetCurrentProcessId()
	*/
	static BOOL IsProcessExist(LPCSTR lpcstrExeName, const DWORD dwProcessId);
	
	/// �ж��ļ���·���Ƿ����
	static BOOL IsFileOrDirExists(const char *v_pszPath);

	/// ����·����������Ŀ¼
	static BOOL MakeMultiDir(const char v_szPath[]);


	/** 
	 * @brief ��ȡָ��·���µ��ļ��б�
	 * @param[out] v_arrCStr �����ļ��б�
	 * @param[in] pszDir ָ��·��
	 * @param[in] pszFilter ָ���ļ���չ����*.*��ʾ�����ļ���*.txt��ʾ�ı��ļ�
	 *
	 * @return void
	 */
	static BOOL GetFileListFromDir(CStringArray &v_arrCStr, LPCSTR pszDir, LPCSTR pszFilter = "*.*");

	/** 
	 * @brief ɾ��ָ�������ļ���Ŀ¼
	 * @param[in] pszDir ָ��Ŀ¼
	 * @param[in] pszFilter ָ���ļ���չ����*.*��ʾ�����ļ���*.txt��ʾ�ı��ļ�
	 * @param[in] bIsDelDir ָ���Ƿ�ɾ��Ŀ¼��TRUE-ɾ����FALSE-��ɾ��
	 *
	 * @return void
	 */
	static void DeleteDirAndFile(LPCSTR pszDir, LPCSTR pszFilter = "*.*", BOOL bIsDelDir = FALSE);

	/** 
	 * @brief ��ȡģ�鵱ǰ·����ģ������
	 * @param[out] v_pModuleDir �����ǰģ��·��
	 * @param[in] _nDirLen ָ�����·������������
	 * @param[out] v_pModuleName �����ǰģ�����ƣ�������չ��
	 * @param[in] _nNameLen ָ��������ƻ���������
	 *
	 * @return void
	 */
	static void GetCurrModulePath( char* v_pModuleDir, int _nDirLen, char* v_pModuleName, int _nNameLen );

	/** 
	 * @brief ��ָ���ļ�ȫ·������ȡ·�����ļ�����
	 * @param[in] _pszFilePath ָ����ǰ�ļ�ȫ·��
	 * @param[out] v_pszFileDir �����·��
	 * @param[in] _nDirSize ָ�����·������������
	 * @param[out] v_pszFileName ����ļ����ƣ�������չ��
	 * @param[in] _nNameSize ָ��������ƻ���������
	 * @param[in] _bIncExt �ļ����Ƿ������չ��
	 *
	 * @return void
	 */
	static void ExtractFilePath(
		LPCSTR _pszFilePath, 
		LPSTR v_pszFileDir, int _nDirSize, 
		LPSTR v_pszFileName, int _nNameSize,
		BOOL _bIncExt = FALSE);


	/** 
	 * @brief ����ָ���ķָ�����ȡ�Ӵ�
	 * @param[in] v_szSrc ԭ�ַ���
	 * @param[out] v_szDest Ŀ�껺������������ȡ���ַ���
	 * @param[in] v_iMaxLen �޶�Ŀ�껺������С
	 * @param[in] v_iSubStr ָ����ȡ�ָ�����ַ�������
	 * @param[in] v_cDelim  ָ���ָ���
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */
	static BOOL ExtractSubString(
		const char* v_szSrc, 
		char* v_szDest, 
		int v_iMaxLen,
		int v_iSubStr, 
		char v_cDelim);

	static BOOL StrTokenEx();

	/// ��ָ���ķָ�������ַ������ָ���֧���Ӵ�
	static int SplitCString(LPCSTR _lpcstrSrc, LPCSTR _lpcstrSplit, CStringArray& v_arrCString);

	/*
	* ��ȡ��ǰϵͳ����ʱ��
	*
	*/
	static void GetCurrDateTime(char *v_pszCurrTiem, int nLen, int nDateType/*=0*/);

	/*
	* ����ָ����ʱ��
	*
	*/
	static time_t ConstructDatetime(int _nY, int _nMon, int _nD, int _H, int _nMin, int _nS);
	

	/// ����׼���ڴ�2017-06-14ת��Ϊ3�ֽ�����
	static void StrDateToChar(LPCSTR _lpcstrDate, char v_szDate[]);

	/// ����׼ʱ�䴮2017-06-14 12:30:56ת��Ϊ6�ֽ�����
	static void StrDatetimeToChar(LPCSTR _lpcstrDatetime, char v_szDatetime[]);


	//static void CharToStrDate(LPCSTR _lpcstrDate, PCHAR v_pszDate);
	

	/*
	 * @brief ���7Eͷβ��ת��
	 *
	 * @param[in] _szOldBuf ����ǰ������
	 * @param[in] _iOldLen ����ǰ�����ݳ���
	 * @param[out] _szNewBuf ����������
	 *
	 * @return ���������ݳ���
	 * @see ת��˵��
			7E-->7D+02
			7D-->7D+01
	*/
	static int Add7EforData(char _szOldBuf[], char _szNewBuf[], int _iOldLen);

	/** 
	 * @brief �������У��� \n
	 * �ӵ�һ���ֽڿ�ʼ������ֽڽ������ռ��1���ֽ�
	 * @param[in] _szBuf �����������
	 * @param[in] _iLen ���ݳ���
	 *
	 * @return У���
	 *
	 */
	static BYTE GetXorCheckSum(const BYTE *_szBuf, const int _iLen);

	//�ֻ��Ų���15ʱ���ո�0x20
	static void FillBlackToMobile(char _szMobile[], int _iLen);

	static BOOL ByteTimeToCTime(const char szDatetime[], CTime &ctTime);
    static BOOL CheckTimeLimit(char* pcCurTime, char*pcTimeBegin, char*pcTimeEnd);

	/** 
	 * @brief ��ȡ������ַ
	 *
	 * @param[out] strIPAddr ����IP��ַ�ַ���
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 */
	static BOOL GetLocalHostIP(CString& strIPAddr);

	/** 
	 * @brief ���ݽ������ƻ�ȡ����ID
	 *
	 * @param[in] ��������
	 *
	 * @return ����IDֵ
	 *
	 */
	static DWORD GetProcessIdByName( LPCTSTR pszExeFile );

	/** 
	 * @brief ���ݻ�ȡ����ID��������
	 *
	 * @param[in] ����ID
	 * @param[out] ��������
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */	
	static BOOL GetProcessNameById( DWORD dwProcessId, CString& lpstrProcName );

	/** 
	 * @brief ��������
	 *
	 * @param[in] ����ID
	 * @param[out] ��������
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */		
	static BOOL StartProcess( LPCTSTR lpcstrProcessName, LPTSTR lpstrProcessPath );

	/** 
	 * @brief �رս���
	 *
	 * @param[in] ����ID
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */		
	static BOOL CloseProcess( DWORD dwProcessId );


	
	/** 
	 * @brief �ж�Ocx�Ƿ���ע��
	 *
	 * @param[in] Ocx��ClsId�����磺{BDEB0088-66F9-4A55-ABD2-0BF8DEEC1196}
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */		
	static BOOL IsRegOcx( LPCSTR lpcstrCLSID );

	/** 
	 * @brief ע��Ocx
	 *
	 * @param[in] Ocx�ļ�������·�������磺C:\TeeChart\Teechart8.ocx
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */
	static BOOL RegOcx( LPCSTR lpcstrOcxPath );

	/** 
	 * @brief ��ע��Ocx
	 *
	 * @param[in] Ocx�ļ�������·�������磺C:\TeeChart\Teechart8.ocx
	 *
	 * @return 
	 *   -<em>false</em> fail
	 *   -<em>true</em> succeed
	 *
	 */
	static BOOL UnRegOcx( LPCSTR lpcstrOcxPath );

	
};

#endif // !defined(AFX_MYTOOL_H__6B7665CE_BD59_46DB_B704_4566D6D51866__INCLUDED_)
