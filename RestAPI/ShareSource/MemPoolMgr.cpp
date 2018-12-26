// MemPoolMgr.cpp: implementation of the CMemPoolMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemPoolMgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//#define _MEMPOOL_DEBUG

CMemPoolMgr::CMemPoolMgr(int poolsize, int unitsize, unsigned int freesize)
{

	m_MaxPoolSize = poolsize;
	m_MaxUnitSize = unitsize;
	m_MaxFreeSize = freesize;
	Trace("POOLSIZE:%dM, UNITSIZE:%dKB, FREESIZE:%d\n\n", m_MaxPoolSize, m_MaxUnitSize, m_MaxFreeSize);
	CLock v_lock(&m_MemPoolListLock);
	CMemPool *v_pMemPool = new CMemPool(m_MaxPoolSize * 1024 * 1024, m_MaxUnitSize * 1024);
	m_MemPoolList.push_back((int)v_pMemPool);
}


CMemPoolMgr::~CMemPoolMgr()
{
	FreeMemory();
}

char* CMemPoolMgr::AllocateMemory(unsigned int size)
{
	char* pData = NULL;
	CLock v_lock(&m_MemPoolListLock);

	list <int> ::iterator v_iter = m_MemPoolList.begin();
	for (v_iter; v_iter != m_MemPoolList.end(); v_iter++)
	{
		pData = ((CMemPool*)(*v_iter))->AllocateMemory(size);
		if (NULL != pData)
			break;
	}

	//����ʧ��, �ӿ��ж�����ȡһ���������䡣
	if (NULL == pData)
	{
		CLock v_lock(&m_MemPoolListFreeLock);
		v_iter = m_MemPoolListFree.begin();
		for (v_iter; v_iter != m_MemPoolListFree.end(); v_iter++)
		{
			pData = ((CMemPool*)(*v_iter))->AllocateMemory(size);
			if (NULL != pData)
			{
				m_MemPoolList.push_back((int)(*v_iter));
				m_MemPoolListFree.erase(v_iter);
				break;
			}
		}
	}

	//����ʧ��, ��������һ��
	if (NULL == pData)
	{
		CMemPool *v_pMemPool = new CMemPool(m_MaxPoolSize * 1024 * 1024, m_MaxUnitSize * 1024);
		m_MemPoolList.push_back((int)v_pMemPool);
		pData = v_pMemPool->AllocateMemory(size);
	}

#ifdef _MEMPOOL_DEBUG
	Trace("AllocateMemory �ڴ��ַ��0x%0x\n", pData);
	TraceMemory();
#endif
	return pData;
}


void CMemPoolMgr::ReleaseMemory(char * pData)
{

#ifdef _MEMPOOL_DEBUG
	Trace("ReleaseMemory �ڴ��ַ��0x%0x\n", pData);
#endif

	BOOL bRelease = FALSE;
	list<int>::iterator v_iter;
	CLock v_lock(&m_MemPoolListLock);
	for (v_iter = m_MemPoolList.begin(); v_iter != m_MemPoolList.end(); v_iter++)	//����Ҫ�ͷŵ��ڴ������ڵ����ڿ�
	{
		if(TRUE == ((CMemPool*)(*v_iter))->ReleaseMemory(pData))	//�Ƿ��ҵ�
		{
			if(TRUE == ((CMemPool*)(*v_iter))->IsFree())			//����Ƿ�ձգ� ���ձշ���ձն���
			{
				CLock v_lock(&m_MemPoolListFreeLock);
				if (m_MemPoolListFree.size() < m_MaxFreeSize)
				{
					CMemPool *v_pMemPool = (CMemPool*)(*v_iter);
					m_MemPoolListFree.push_back((int)v_pMemPool);
				}
				else
				{
					delete (CMemPool*)(*v_iter);
				}
				m_MemPoolList.erase(v_iter);
			}
			bRelease = TRUE;
			break;
		}
	}

	if (!bRelease)
	{
		Trace("ReleaseMemory �ڴ��ַ��%0xû�б��ͷ�\n", pData);
	}

#ifdef _MEMPOOL_DEBUG
	TraceMemory();
#endif

}


void CMemPoolMgr::TraceMemory()
{
	Trace("����ʹ�õĶ������\n\n");
	list <int> ::iterator v_iter;
	CLock v_lock1(&m_MemPoolListLock);
	v_iter = m_MemPoolList.begin();
	for (v_iter = m_MemPoolList.begin(); v_iter != m_MemPoolList.end(); v_iter++)
	{
		((CMemPool*)(*v_iter))->TraceMemory();
	}

	Trace("�ձն������\n\n");
	CLock v_lock2(&m_MemPoolListFreeLock);
	v_iter = m_MemPoolListFree.begin();
	for (v_iter = m_MemPoolListFree.begin(); v_iter != m_MemPoolListFree.end(); v_iter++)
	{
		((CMemPool*)(*v_iter))->TraceMemory();
	}
	Trace("*******************************************************\n\n");
}



void CMemPoolMgr::FreeMemory(void)
{
	Trace("******************FreeMemory()******************\n");
	TraceMemory();

	list <int> ::iterator v_iter; 
	CLock v_lock1(&m_MemPoolListLock);
	v_iter = m_MemPoolList.begin();
	while(v_iter != m_MemPoolList.end())
	{
		delete (CMemPool*)(*v_iter);
		m_MemPoolList.erase(v_iter);
		v_iter = m_MemPoolList.begin();
	}

	CLock v_lock2(&m_MemPoolListFreeLock);
	v_iter = m_MemPoolListFree.begin();
	while(v_iter != m_MemPoolListFree.end())
	{
		delete (CMemPool*)(*v_iter);
		m_MemPoolListFree.erase(v_iter);
		v_iter = m_MemPoolListFree.begin();
	}
}

