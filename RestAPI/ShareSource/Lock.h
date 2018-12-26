// Lock.h: interface for the CLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCK_H__9BAC41F6_40C1_4F29_A996_63B87F605D72__INCLUDED_)
#define AFX_LOCK_H__9BAC41F6_40C1_4F29_A996_63B87F605D72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class ILockableObject
{
public:
    ILockableObject() {};
    virtual ~ILockableObject() {};
    virtual void Lock() = 0;
    virtual void UnLock() = 0;
};


class CriticalSections : public ILockableObject
{
private:
    CRITICAL_SECTION m_CrisSec;
public:
   CriticalSections()
    {
        Initial();    // ��ʼ��
    }

    virtual ~CriticalSections()
    {
        Delete();    // ɾ��
    }

    void Initial()
    {
        InitializeCriticalSection( &m_CrisSec );
    }

    void Delete()
    {
        DeleteCriticalSection( &m_CrisSec );
    }

    virtual void Lock()
    {
        ::EnterCriticalSection( &m_CrisSec );			// ����
    }
    virtual void UnLock()
    {
        ::LeaveCriticalSection( &m_CrisSec );			// ����
    }
};


class CLock  
{
private:
    ILockableObject * m_pLock;

public:
	CLock(ILockableObject * pLockable );
	virtual ~CLock();
};

#endif // !defined(AFX_LOCK_H__9BAC41F6_40C1_4F29_A996_63B87F605D72__INCLUDED_)
