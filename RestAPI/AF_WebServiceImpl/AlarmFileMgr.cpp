#include "StdAfx.h"
#include "AlarmFileMgr.h"


CAlarmFileMgr::CAlarmFileMgr(void)
{

}

CAlarmFileMgr::~CAlarmFileMgr(void)
{
}

bool CAlarmFileMgr::GetFileList(CStringArray& v_arrCStr, const char* _szMobile, const char* _szDate, BYTE _byType)
{
	std::string strDir(_szMobile);
	strDir += "\\";
	strDir.append(_szDate);
	strDir += "\\";
	char sz[4] = {0};
	sprintf_s(sz, 16, "%d\\",_byType);
	strDir.append(sz);

	// �ļ����ͣ�00����ͼƬ��01������Ƶ��02������Ƶ��03������¼�ļ���04����������
	// ��׺����ͼƬ�ļ�Ϊjpg��png����Ƶ�ļ�Ϊwav����Ƶ�ļ�Ϊh264����¼�ļ�Ϊbin
/*
	std::string strFilter;
	switch (_byType)
	{
	case 0:
		strFilter="*.jpg";
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;		
	}*/
	return true;
}
