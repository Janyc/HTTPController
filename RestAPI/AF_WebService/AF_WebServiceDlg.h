// AF_WebServiceDlg.h : ͷ�ļ�
//

#pragma once
#include "WndLogTabCtrl.h"
#include "WebServiceImpl.h"
#include "../ShareSource/IniFile.h"

// CAF_WebServiceDlg �Ի���
class CAF_WebServiceDlg: public CDialog
{
// ����
public:
	CAF_WebServiceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AF_WEBSERVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CWndLogTabCtrl   m_wndWndLogTabCtrl;
	

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	CStatusBar    m_status;

private:
	static void CALLBACK CB_PrintLog(  char *_pMyObj, const int _iLogType, const char _szLogDesc[], const char _szLogText[] );
	void PrintLog(  const int _iLogType, const char _szLogDesc[], const char _szLogText[] );
	CWebServiceImpl m_WebServiceImpl;
	afx_msg void OnDestroy();
};
