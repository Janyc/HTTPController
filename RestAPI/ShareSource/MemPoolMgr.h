// MemPoolMgr.h: interface for the CMemPoolMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMPOOLMGR_H__8BEC6041_4552_44A6_B9DA_F4E00613953E__INCLUDED_)
#define AFX_MEMPOOLMGR_H__8BEC6041_4552_44A6_B9DA_F4E00613953E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <list>
using namespace std;
#include "MemPool.h"

#include "Lock.h"


const int MAX_DEFAULT_POOL_SIZE = 10;  
const int MAX_DEFAULT_UNIT_SIZE = 8;
const int MAX_DEFAULT_FREE_SIZE = 5;


class CMemPoolMgr  
{
public:

	virtual ~CMemPoolMgr();
	/*
	 * ��ʼ���ڴ��
	 * @poolsize ��λ��MB���ڴ���ܴ�С
	 * @unitsize ��λ��KB����Ԫ���С
	 * @freesize ��λ������Ԥ�������ڴ�ظ���
	*/
	CMemPoolMgr(int poolsize = MAX_DEFAULT_POOL_SIZE, int unitsize = MAX_DEFAULT_UNIT_SIZE,unsigned int freesize = MAX_DEFAULT_FREE_SIZE);
	
	char*	AllocateMemory(unsigned int size);
	void	ReleaseMemory(char * pData);
	void	TraceMemory();

private:
	void	FreeMemory(void);

private:
	int						m_MaxFreeSize;
	int						m_MaxPoolSize;
	int						m_MaxUnitSize;

	list <int>				m_MemPoolList;
	CriticalSections		m_MemPoolListLock;

	list <int>				m_MemPoolListFree;
	CriticalSections		m_MemPoolListFreeLock;
};

#endif // !defined(AFX_MEMPOOLMGR_H__8BEC6041_4552_44A6_B9DA_F4E00613953E__INCLUDED_)
