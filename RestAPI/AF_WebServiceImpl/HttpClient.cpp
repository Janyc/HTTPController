#pragma once 

#include "StdAfx.h"
#include ".\HttpClient.h"
#include <fstream>


std::locale loc("english");

namespace nsHttp
{
	CHttpClient::CHttpClient(void)
	{
		memset( m_szClientIp, 0x00, sizeof(m_szClientIp));
		m_ui16Port = 0;
		m_TcpConnStatus = TCPCONN_IDLE;
		m_reqStatus = REQ_REQUEST;
		m_ulLiveTime = MAX_CONN_TIME;
		m_pHttpManager = NULL;

		::InitializeCriticalSection(&m_csCS);
	}

	CHttpClient::~CHttpClient(void)
	{
		// ���ӻ��⴦��
		CCS CS(&m_csCS);

		//�ȹر��������ͷ���Դ
		this->CloseConnection();
		this->ToClose();
		this->Reset();

		::DeleteCriticalSection(&m_csCS);
	}

	void CHttpClient::OnRead( char *v_pBuffRead, int nLen )
	{
		// ���ӻ��⴦��
		CCS CS(&m_csCS);
		this->OnReceive(v_pBuffRead, nLen);
	}

	void CHttpClient::OnConnectionEstablished( BOOL bConnEsted )
	{
	}

	//�Է��ر�����
	BOOL CHttpClient::OnClose( )
	{
		// ���ӻ��⴦��
		CCS CS(&m_csCS);

		this->ToClose();

		char szLog[256];
		sprintf_s(szLog,sizeof(szLog),"==>���ͻ��˶�������%s:%d��...",m_szClientIp,m_ui16Port);

		return true;  //ֱ�Ӳ���·
	}

	//�д�����
	void CHttpClient::OnError( DWORD dwError,DWORD dwOpType )
	{	
		// ���ӻ��⴦��
		CCS CS(&m_csCS);

		char szLog[256];
		switch(dwOpType)
		{
		case enOpRead:
			{
				m_TcpConnStatus = TCPCONN_ERROR;

				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d�����տͻ������ݳ���[������:%d]",m_szClientIp,m_ui16Port,dwError);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
			}
			return;

		case enOpWrite:
			{
				m_TcpConnStatus = TCPCONN_ERROR;

				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d�����Ϳͻ������ݳ���[������:%d]",m_szClientIp,m_ui16Port,dwError);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
			}
			return;

		default:
			return;
		}
	}

	//д���
	void CHttpClient::OnWrite( LPCSTR lpszBuff,int nLen )
	{
		// �ݲ�����
	}

	void CHttpClient::ToClose(void)
	{
		m_TcpConnStatus = TCPCONN_CLOSED;
	}

	void CHttpClient::Reset(void)
	{
		m_reqStatus = REQ_REQUEST;
		m_TcpConnStatus = TCPCONN_IDLE;
		m_ssLine.str("");
		m_ssLine.clear();
		::InterlockedExchange( (LONG*)&m_ulLiveTime,MAX_CONN_TIME);
		m_Request.Reset();
	}

	void CHttpClient::OnReceive(char* v_szRecvBuf, int v_iRecvLen)
	{
		// �����־
		char szLog[256];

		this->m_TcpConnStatus = TCPCONN_ALIVE;
		::InterlockedExchange( (LONG*)&m_ulLiveTime,MAX_CONN_TIME);

		sprintf_s(szLog,sizeof(szLog),"<==�����ա���%s:%d��...",m_szClientIp,m_ui16Port);
		m_pHttpManager->toShowWndLog(LogSpace::enRunInfo,szLog);

		int ndx = 0;

		switch ( m_reqStatus )
		{
		case REQ_REQUEST:
		case REQ_HEADER:
			//���ջس����з�\r\n�ָ���ȡ
			while( this->GetLine( v_szRecvBuf, v_iRecvLen, ndx )  )
			{
				if ( m_ssLine.str().empty() )
				{//	��⵽����������\r\n��˵�������и�������
					m_Request.m_mbBody.Clear();	
					m_reqStatus = REQ_BODY;
					break;
				}
				else
				{
					ProcessLine();
				}
			}

			if ( m_reqStatus != REQ_BODY )
				break;

			if ( ! this->BodySent() )
			{ // ��������
				m_reqStatus = REQ_DONE;
				break;
			}

		case REQ_BODY:
			this->AddToBody( (unsigned char*)v_szRecvBuf, v_iRecvLen, ndx );
			break;
		}

		if( m_reqStatus == REQ_DONE )
		{
			this->DoProcess();
			this->Reset();
		}
	}

	bool CHttpClient::GetLine(char* v_szRecvBuf, int v_iRecvLen, int& v_ndx)
	{
		bool bLine = false;
		while ( !bLine && v_ndx < v_iRecvLen )
		{
			char ch = (char)v_szRecvBuf[v_ndx];
			switch( ch )
			{
			case '\r':
				break;
			case '\n':
				bLine = true;
				break;
			default:
				m_ssLine << ch;
				break;
			}
			++v_ndx;
		}

		return bLine;
	}

	void CHttpClient::ProcessLine(void)
	{
		std::locale loc("chs");

		std::stringstream  ssTemp;
		std::stringstream  ssSpace;

		switch( m_reqStatus )
		{
		case REQ_REQUEST:
/*
GET /REST/?Method=Yaxon.AF.DownLoad&filename=161533490_1_0_858.jpg&filetype=0 HTTP/1.1
Host: 127.0.0.1:10110
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36
..
*/
			// ��λ����ʼλ������ַ���ȡ���ж�
			m_ssLine.seekg( 0 );
			while( !m_ssLine.eof() )
			{
				char ch;
				m_ssLine.get(ch);
				if( m_ssLine.eof() ) break;
				// ���ո�ָ�
				if( ' ' == ch )
				{
					break;
				}
				ssTemp << ch;
			}

			if( !m_ssLine.eof() )
			{
				// ��ȡ������ GET �� POST
				m_Request.m_strMethod = ssTemp.str();	// ��������ʽ
				ssTemp.str(""); 
				ssTemp.clear();

				// ������ǰ��ȡ�ַ�
				while( !m_ssLine.eof() )
				{
					char ch;
					m_ssLine.get(ch);
					if( m_ssLine.eof() ) break;
					// ���������ַ��ڵ�ǰ��C���ػ������Ƿ��ǿհ��ַ�,��Ĭ�ϵı��ػ�����
					// �հ��ַ��ǿո�0x20����ҳ��0x0c�����У�0x0a���س���0x0d��ˮƽ�Ʊ����0x09���ʹ�ֱ�Ʊ����0x0b��
					if( !std::isspace(ch, loc))
					{
						ssTemp << ch;
						break;
					}
				}

				// ������ǰ
				while( !m_ssLine.eof() )
				{
					char ch;
					m_ssLine.get(ch);
					if( m_ssLine.eof() ) break;
					if( ' ' == ch )
					{
						break;
					}
					if( !std::isspace(ch, loc) )
					{
						if( !ssSpace.str().empty() )
						{
							ssTemp << ssSpace.str();
							ssSpace.str("");
							ssSpace.clear();
						}
						ssTemp << ch;
					}else
					{
						ssSpace << ch;
					}
				}

				if( m_ssLine.eof() )
				{// û�а汾�ַ���
					m_Request.m_strUrl = ssTemp.str(); // ����Url��ַ
					ssTemp.str("");
					ssTemp.clear();
					m_reqStatus = REQ_SIMPLE;
				}else
				{
					// �õ�method url
					// /REST/?Method=Yaxon.AF.DownLoad&filename=161533490_1_0_858.jpg&filetype=0
					m_Request.m_strUrl = ssTemp.str();
					ssTemp.str("");
					ssTemp.clear();
					// ������ǰ
					while( !m_ssLine.eof() )
					{
						char ch;
						m_ssLine.get(ch);
						if( m_ssLine.eof() ) break;
						if( !std::isspace(ch, loc))
						{
							ssTemp << ch;
							break;
						}
					}
					while( !m_ssLine.eof() )
					{
						char ch;
						m_ssLine.get(ch);
						if( m_ssLine.eof() ) break;
						ssTemp << ch;
					}
					// �õ�Э��汾�� HTTP/1.1
					m_Request.m_strVersion = ssTemp.str();	// �����汾
					ssTemp.str("");
					ssTemp.clear();
				}

				// ��url��ַ���б���Ĳ��ֽ��д���
				CStringCoder::UrlUTF8Decode(m_Request.m_strUrl);
				transform(m_Request.m_strUrl.begin(),m_Request.m_strUrl.end(),m_Request.m_strUrl.begin(),tolower); // ת��ΪСд
				std::stringstream   ssUrl;
				ssUrl.str(m_Request.m_strUrl);
				ssUrl.seekg(0);
				while( !ssUrl.eof() )
				{
					char ch;
					ssUrl.get(ch);
					if( ssUrl.eof() ) break;
					// ���ַ�'?'���ֿ�
					if( '?' == ch )
					{
						break;
					}
					ssTemp << tolower(ch,loc);//ch;
				}
				// �õ� /REST/
				m_Request.m_strUrl = ssTemp.str();
				ssTemp.str("");
				ssTemp.clear();
				while( !ssUrl.eof() )
				{
					char ch;
					ssUrl.get(ch);
					if( ssUrl.eof() ) break;
					ssTemp << ch;//ch;
				}
				// �õ� Method=Yaxon.AF.DownLoad&filename=161533490_1_0_858.jpg&filetype=0
				m_Request.m_strArgs = ssTemp.str();
				ssTemp.str("");
				ssTemp.clear();
				m_Request.m_dwExecute = CRequest::APP_EXECUTE;
			}
			m_reqStatus = REQ_HEADER;
			break;
		case REQ_HEADER:
			m_ssLine.seekg( 0 );

			while( !m_ssLine.eof() )
			{
				char ch;
				m_ssLine.get(ch);
				if( m_ssLine.eof() ) break;
				if( ':' == ch )
				{
					break;
				}
				ssTemp << ch;
			}
			if( !m_ssLine.eof() )
			{
				std::string strName = ssTemp.str();
				ssTemp.str(""); 
				ssTemp.clear();

				while( !m_ssLine.eof() )
				{
					char ch;
					m_ssLine.get(ch);
					if( m_ssLine.eof() ) break;
					if( !std::isspace(ch, loc))
					{
						ssTemp << ch;
						break;
					}
				}
				while( !m_ssLine.eof() )
				{
					char ch;
					m_ssLine.get(ch);
					if( m_ssLine.eof() ) break;
					ssTemp << ch;
				}
				std::string strValue = ssTemp.str();
				ssTemp.str(""); 
				ssTemp.clear();

				std::transform(strName.begin(),strName.end(),strName.begin(),std::tolower);
				if( 0 == ::strcmp( strName.c_str(), "content-length" ) )
				{
					if( ! strValue.empty() )
					{
						m_Request.m_cbBody = atoi( strValue.c_str() );
					}
				}
			}
			break;
		}
		m_ssLine.str("");
		m_ssLine.clear();
	}

	void CHttpClient::DoProcess()
	{
		char szLog[256];

		if( m_Request.GetUrl() != "/rest/" )
		{
			std::string strUrl = m_Request.GetUrl();
			sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d�������URL�����ʽ_%s!",m_szClientIp,m_ui16Port,strUrl.c_str());
			m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
			return;
		}

		std::string strMethod = m_Request.GetQueryArgs("method");
		if( strMethod == "" )
		{
			sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[method]�������ȱʧ!",m_szClientIp,m_ui16Port);
			m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
		}
		else if( strMethod == "yaxon.af.getfilelist" )
		{
			DWORD dwSN = m_pHttpManager->GetSN();
			std::string strVal = "";
			std::string strMobile = "";
			std::string strDate ="";
			BYTE byType=0;

			// �ֻ���
			strVal = m_Request.GetQueryArgs("mobile");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[mobile]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			strMobile = strVal;

			// ��ѯ����
			strVal = m_Request.GetQueryArgs("dt");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[dt]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			strDate = strVal;

			// �ļ�����
			strVal = m_Request.GetQueryArgs("type");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[type]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			//byType = (BYTE)::atoi(strVal.c_str());
			
			CString cstrDir;
			cstrDir.Format("%s%s\\%s\\%s",g_pAF_WebServiceImpl->GetDir().c_str(),strMobile.c_str(),strDate.c_str(),strVal.c_str());

			bool bSucc = false;
			CStringArray arrFile;			
			if ( GetFileListFromDir( arrFile, cstrDir, "*.*" ) )
			{
				bSucc = true;
/*
//����
				CString cstrFiles;
				/// �ļ�����
				int nTotla = (int)arrFile.GetSize();
				/// ���õ�һ���ļ�
				cstrFiles = arrFile.GetAt(0);
				/// ��ȡ�����ļ�
				for ( int n=1; n<nTotla; n++ )
				{
					cstrFiles += ",";
					cstrFiles += arrFile.GetAt(n);
				}*/
			}	
			else
			{
				bSucc = false;
			}
			std::stringstream ssData;
			BulidGetFilesResp( arrFile, ssData, bSucc );
			std::string strData = ssData.str();
			char *pData = (char*)strData.c_str();
			int iDataLen = (int)(strData.length());
			ToResponseGeneral("text/xml", pData, iDataLen);

			m_pHttpManager->BindSnToConnId(dwSN,(DWORD)GetID());
		}
		else if( strMethod == "yaxon.af.getfile" )
		{
			DWORD dwSN = m_pHttpManager->GetSN();
			std::string strVal = "";
			std::string strMobile = "";
			std::string strDate = "";
			std::string strType = "";
			std::string strFile = "";
			BYTE byType=0;

			// �ֻ���
			strVal = m_Request.GetQueryArgs("mobile");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[mobile]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			strMobile = strVal;

			// ��ѯ����
			strVal = m_Request.GetQueryArgs("dt");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[dt]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			strDate = strVal;

			// �ļ�����
			strVal = m_Request.GetQueryArgs("type");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[type]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			strType = strVal;
			byType = (BYTE)::atoi(strVal.c_str());	
			
			// �ļ�����
			strVal = m_Request.GetQueryArgs("filename");
			if( strVal == "" )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d��[filename]���ز���ȱʧ!",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseDownLoad("image/jpeg",NULL,0,false);
				return;
			}
			strFile = strVal;

			char szDir[MAX_PATH];
			//CString cstrDir;
			sprintf_s(szDir, MAX_PATH, "%s%s\\%s\\%s\\%s"
				, g_pAF_WebServiceImpl->GetDir().c_str(),strMobile.c_str(),strDate.c_str(),strType.c_str(),strFile.c_str());

			std::fstream file;
			try 
			{
				file.open(szDir, ios::in|ios::binary);
			} catch(...) {
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s�����ļ�����!",szDir);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseGeneral("text/xml", NULL, 0);
				return;
			}
			if ( !file.is_open() )
			{
				sprintf_s(szLog,sizeof(szLog),"[����]:��%s�����ļ�ʧ��!",szDir);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				ToResponseGeneral("text/xml", NULL, 0);
				return;
			}
			// �õ��ļ���С
			file.seekg(0,ios::end);
			int nDataLen = file.tellg();
			file.seekg(0, ios::beg);

			// �����ڴ�ռ�
			char* pContent = g_MemPoolMgr.AllocateMemory(nDataLen+1024);

			// �����ļ�
			file.read(pContent,nDataLen);
			file.close();

			char szMetaType[64];
			switch (byType)
			{
			case 0:
				break;
				sprintf_s(szMetaType,64,"%s","image/jpeg");
			case 1:
				sprintf_s(szMetaType,64,"%s","audio/wav");
				break;
			default:
				sprintf_s(szMetaType,64,"%s","application/octet-stream");			
			}

			ToResponseGeneral(szMetaType, pContent, nDataLen);
			g_MemPoolMgr.ReleaseMemory(pContent);

			m_pHttpManager->BindSnToConnId(dwSN,(DWORD)GetID());
		}
		else
		{
			sprintf_s(szLog,sizeof(szLog),"[����]:��%s:%d�������[method]�������!",m_szClientIp,m_ui16Port);
			m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
		}
	}

	BOOL CHttpClient::GetFileListFromDir(CStringArray &v_arrCStr, LPCSTR pszDir, LPCSTR pszFilter /*= "*.*"*/)
	{
		if ( NULL == pszDir )
		{
			return FALSE;
		}

		CString strFilter;
		strFilter.Format("%s\\%s", pszDir, pszFilter);

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
				//ֻҪ�ļ�����
				//strFilePath.Format("%s\\%s", pszDir, finddata.cFileName);
				strFilePath.Format("%s", finddata.cFileName);
				v_arrCStr.Add(strFilePath);
			}

			bIsFind = ::FindNextFile( hFindFile, &finddata );
		}

		::FindClose(hFindFile);

		return ( v_arrCStr.GetSize() > 0 );
	}

	void CHttpClient::BulidGetFilesResp(CStringArray &v_arrFile, std::stringstream &v_ssContent, bool v_bSucceed)
	{
		v_ssContent.str("");
		v_ssContent.clear();
		v_ssContent<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		if( v_bSucceed )
		{
			v_ssContent<<"<result>";
			v_ssContent<<"<ack>succeed</ack>";
			v_ssContent<<"<files>";
			/// �ļ�����
			int nTotla = (int)v_arrFile.GetSize();
			CString cstrFile;
			for ( int n=0; n<nTotla; n++ )
			{
				CString cstrFile;
				cstrFile = v_arrFile.GetAt(n);
				v_ssContent<<"<file>"<<cstrFile.GetBuffer(0)<<"</file>";
				cstrFile.ReleaseBuffer();				
			}
			v_ssContent<<"</files>";
			v_ssContent<<"</result>";
		}
		else
		{
			v_ssContent<<"<result>";
			v_ssContent<<"<ack>failed</ack>";
			v_ssContent<<"</result>";
		}
	}

	void CHttpClient::BulidGetFileResp(CString &_strFile, std::stringstream &v_ssContent, bool v_bSucceed)
	{

	}

	bool CHttpClient::IsTimeOut()
	{
		// ���ӻ��⴦��
		CCS CS(&m_csCS);

		if(  ::InterlockedDecrement((LONG*)&m_ulLiveTime) == 0 )
		{
			return true;
		}

		return false;
	}
	
	void CHttpClient::CB_Upload( DWORD  _dwSN, BYTE _byFileNameLen,char _szFileName[] )
	{
		if( _byFileNameLen == 0 )
		{
			ToResponseUpLoad("text/xml",NULL,0,false);
		}
		else
		{
			ToResponseUpLoad("text/xml",(char*)_szFileName,_byFileNameLen,true);
		}
	}

	void CHttpClient::CB_Download_Mobile( DWORD _dwSN, BYTE _byFileType, DWORD   _dwFileLen, char    *_pFileBuff )
	{
		if( _dwFileLen == 0 )
		{
			ToResponseDownLoad("image/jpeg",NULL,0,false);
		}
		else
		{
			ToResponseDownLoad("image/jpeg",_pFileBuff,_dwFileLen,true);
		}
	}

	void CHttpClient::CB_Download_Web( DWORD _dwSN, BYTE    _byWebUrlLen, char    _szWebUrl[MAX_PATH] )
	{// ��ʱ�ò���
	}

	bool CHttpClient::BodySent(void)
	{
		if( 0 < m_Request.m_cbBody )
		{
			return true;
		}
		return false;
	}

	void CHttpClient::AddToBody(unsigned char* v_szRecvBuf, int v_nBytes, int v_ndx)
	{

		int iLeftBody = m_Request.m_cbBody - m_Request.m_mbBody.data_size();
		int iAppend = min( iLeftBody, v_nBytes - v_ndx );

		m_Request.m_mbBody.Append( v_szRecvBuf + v_ndx, iAppend );
		if( m_Request.m_mbBody.data_size() >= m_Request.m_cbBody )
		{//	���ﲻ����һ��Tcp�����ϴ������http����Ŀ���
			m_reqStatus = REQ_DONE;
		}
	}
	
	void CHttpClient::ToResponseUpLoad( char *v_pContentType, char* v_pFileName, int v_iFileNameLen, bool v_bSucceed )
	{
		char szLog[256];
		// �����־
		sprintf_s(szLog,sizeof(szLog),"==>�����͡���%s:%d��...",m_szClientIp,m_ui16Port);
		m_pHttpManager->toShowWndLog(LogSpace::enRunInfo,szLog);

		int iDataLen = 0;
		std::stringstream    ssLog;
		ssLog.str("");
		ssLog.clear();
		ssLog<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		if( v_bSucceed )
		{
			ssLog<<"<result>";
			ssLog<<"<ack>Succeed</ack>";
			ssLog<<"<picid>"<<v_pFileName<<"</picid>";
			ssLog<<"</result>";
		}
		else
		{
			ssLog<<"<result>";
			ssLog<<"<ack>Failed</ack>";
			ssLog<<"</result>";
		}
		iDataLen = (int)(ssLog.str().length());
		
		char szHttpHeader[1024];
		int  iHttpHeaderLen = 0;
		::sprintf_s( szHttpHeader,sizeof(szLog), 
			"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
			"Content-Type: %s\r\n"
			"\r\n",
			iDataLen,
			v_pContentType
			);
		iHttpHeaderLen = (int)strlen(szHttpHeader);

		if( ! (this->Send(reinterpret_cast<char*>(szHttpHeader), iHttpHeaderLen) ) )
		{
			// �����־
			sprintf_s(szLog,sizeof(szLog),"[����]:[%s:%d]ͼƬ�ϴ�Э��ͷ���ݷ���ʧ�ܣ�����",m_szClientIp,m_ui16Port);
			m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
			return;
		}

		// ����Э��������
		int index = 0;
		int iLeftLen = iDataLen;
		int iSendLen = 0;
		std::string strResult = ssLog.str();
		char *pData = (char*)strResult.c_str();
		while( iLeftLen > 0 )
		{
			if( iLeftLen >= 2048 )
			{
				iSendLen = 2048;
			}
			else
			{
				iSendLen = iLeftLen;
			}

			if( ! (this->Send(reinterpret_cast<char*>(pData+index), iSendLen) ) )
			{
				// �����־
				sprintf_s(szLog,sizeof(szLog),"[����]:[%s:%d]ͼƬ�ϴ�Ӧ��XML���ݷ���ʧ�ܣ�����",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);

				return;
			}

			index += iSendLen;
			iLeftLen -= iSendLen;
		}
	}

	void CHttpClient::ToResponseGeneral(char *v_pContentType, char* v_pData, int v_iDataLen)
	{
		// �����־
		char szLog[256];
		sprintf_s(szLog,sizeof(szLog),"==>�����͡���%s:%d��...",m_szClientIp,m_ui16Port);
		m_pHttpManager->toShowWndLog(LogSpace::enRunInfo,szLog);

		char szHttpHeader[1024];
		int  iHttpHeaderLen = 0;
		::sprintf_s( szHttpHeader,sizeof(szHttpHeader), 
			"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n"
			"Content-Type: %s\r\n"
			"\r\n",
			v_iDataLen,
			v_pContentType
			);
		iHttpHeaderLen = (int)strlen(szHttpHeader);

		///++
		// �ȷ�Ӧ��ͷ
		if( ! (this->Send(reinterpret_cast<char*>(szHttpHeader), iHttpHeaderLen) ) )
		{
			// �����־
			sprintf_s(szLog,sizeof(szLog),"[����]:[%s:%d]HTTPӦ��ͷ����ʧ�ܣ�����",m_szClientIp,m_ui16Port);
			m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
			return;
		}

		int index = 0;
		int iLeftLen = v_iDataLen;
		int iSendLen = 0;
		while( iLeftLen > 0 )
		{
			if( iLeftLen >= 2048 )
			{
				iSendLen = 2048;
			}
			else
			{
				iSendLen = iLeftLen;
			}

			// �ٷ�����
			if( ! (this->Send(reinterpret_cast<char*>(v_pData+index), iSendLen) ) )
			{
				// �����־
				sprintf_s(szLog,sizeof(szLog),"[����]:[%s:%d]HTTPӦ�����ݷ���ʧ�ܣ�����",m_szClientIp,m_ui16Port);
				m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
				return;
			}

			index += iSendLen;
			iLeftLen -= iSendLen;
		}
	}

	void CHttpClient::ToResponseDownLoad( char *v_pContentType, char* v_pData, int v_iDataLen, bool v_bSucceed )
	{
		// �����־
		char szLog[256];
		sprintf_s(szLog,sizeof(szLog),"==>�����͡���%s:%d��...",m_szClientIp,m_ui16Port);
		m_pHttpManager->toShowWndLog(LogSpace::enRunInfo,szLog);

		char szHttpHeader[1024];
		int  iHttpHeaderLen = 0;
		::sprintf_s( szHttpHeader,sizeof(szHttpHeader), 
			"HTTP/1.1 200 OK\r\nContent-Length: %d;charset=utf-8\r\n"
			"Content-Type: %s\r\n"
			"\r\n",
			v_iDataLen,
			v_pContentType
			);
		iHttpHeaderLen = (int)strlen(szHttpHeader);

		///++
		// �ȷ�Ӧ��ͷ
		if( ! (this->Send(reinterpret_cast<char*>(szHttpHeader), iHttpHeaderLen) ) )
		{
			// �����־
			sprintf_s(szLog,sizeof(szLog),"[����]:[%s:%d]ͼƬ����Э��ͷ���ݷ���ʧ�ܣ�����",m_szClientIp,m_ui16Port);
			m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);
			return;
		}

		if( v_bSucceed )
		{
			int index = 0;
			int iLeftLen = v_iDataLen;
			int iSendLen = 0;
			while( iLeftLen > 0 )
			{
				if( iLeftLen >= 2048 )
				{
					iSendLen = 2048;
				}
				else
				{
					iSendLen = iLeftLen;
				}

				// �ٷ�����
				if( ! (this->Send(reinterpret_cast<char*>(v_pData+index), iSendLen) ) )
				{
					// �����־
					sprintf_s(szLog,sizeof(szLog),"[����]:[%s:%d]ͼƬ����Ӧ�����ݷ���ʧ�ܣ�����",m_szClientIp,m_ui16Port);
					m_pHttpManager->toShowWndLog(LogSpace::enErrorInfo,szLog);

					return;
				}

				index += iSendLen;
				iLeftLen -= iSendLen;
			}
		}
	}
}