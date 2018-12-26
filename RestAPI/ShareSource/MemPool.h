// MemPool.h: interface for the CMemPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMPOOL_H__290602C2_97B4_4484_A9EE_07E3C6D14368__INCLUDED_)
#define AFX_MEMPOOL_H__290602C2_97B4_4484_A9EE_07E3C6D14368__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <time.h>



typedef struct MemoryBlock
{
	char*        pData;			// Pointer to the actual Data
	unsigned int DataSize;	    // Size of the Data Block
}
MemoryBlock, *PMemoryBlock;


class CMemPool  
{
public:

	CMemPool(unsigned int poolsize = 1024 * 1024 * 8, unsigned int unitsize = 1024 * 8 ); // Constructor
	virtual ~CMemPool(); // Destructor
	
	// Attribute
public:
	
	char*        GetMemPoolEntry() const;
	unsigned int GetMemPoolSize() const;
	unsigned int GetCurUsedSize() const;
	BOOL         IsFree() const; 
	
	// Operation
public:
	
	virtual char*  AllocateMemory(unsigned int size);		// Get Memory from the Memory Pool
	virtual BOOL   ReleaseMemory(char * pData);				// Free Memory to Memory Pool
	virtual void   TraceMemory();
	
protected:
	
	virtual void			InitMemPool(); // Init the Memory Pool
	virtual void			ClearMemPool();
	virtual void			FreeMemory(void);
	virtual PMemoryBlock	RequestBlock(unsigned int blocksize);
	virtual unsigned int	CalcBlockSize(unsigned int blocksize);

private:
	PMemoryBlock	m_MemoryUnitList;		//�ڴ��������ָ��
	char*			m_pPoolEntry;			// The Pool Entry���ڴ��ܿռ����ָ��
	
	unsigned int	m_nBlockUnitSize;		// ÿ���ڴ��޶��Ĵ�С
	unsigned int	m_nPoolSize;			// �ڴ�ؿ����ܿռ��С
	unsigned int	m_blocksize;			// �ܹ����ֵ��ڴ������

};

void Trace(char *szFormat, ...);

#endif // !defined(AFX_MEMPOOL_H__290602C2_97B4_4484_A9EE_07E3C6D14368__INCLUDED_)
