// WndLogListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "WndLogListCtrl.h"


// CWndLogListCtrl

IMPLEMENT_DYNAMIC(CWndLogListCtrl, CListCtrl)
CWndLogListCtrl::CWndLogListCtrl()
{
}

CWndLogListCtrl::~CWndLogListCtrl()
{
}


BEGIN_MESSAGE_MAP(CWndLogListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CWndLogListCtrl message handlers

int CWndLogListCtrl::InitColumn()
{
	InsertColumn( 0, "", LVCFMT_LEFT, 20 );
	InsertColumn( 1, "����", LVCFMT_LEFT,  70 );
	InsertColumn( 2, "ʱ��", LVCFMT_LEFT,  60 );
	InsertColumn( 3, "�¼�", LVCFMT_LEFT, 570 );

	::SendMessage( 
		m_hWnd, 
		LVM_SETEXTENDEDLISTVIEWSTYLE, 
		0, 
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES 
		);

	return 0;
}

int CWndLogListCtrl::push_back( const char v_szDate[], const char v_szTime[], unsigned long v_iLogLen, const char v_szLog[])
{
	if( 1000 <= GetItemCount()  )
	{// ��־�б�Ի�������, ��Ҫɾ��һ����־
		DeleteItem( 999 );
	}

	InsertItem(  0, "", 2 );
	SetItemText( 0, 1, v_szDate );
	SetItemText( 0, 2, v_szTime );
	SetItemText( 0, 3, v_szLog );

	return 0;
}