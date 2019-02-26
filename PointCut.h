#pragma once
#include <math.h>


//GPS������Ϣ
typedef struct _tagGPSInfo
{
	std::string           m_strTime;         //GPSʱ��
	unsigned int     m_u32Lon;          //����
	unsigned int     m_u32Lat;          //γ��
	unsigned char    m_u8Speed;         //�ٶ�
	unsigned char    m_u8Direction;     //����
	int m_iDel;

	_tagGPSInfo()
	{
		clean();
	}

	void clean()
	{
		m_strTime.clear();
		m_u32Lon = 0;
		m_u32Lat = 0;
		m_u8Speed = 0;
		m_u8Direction = 0;
		m_iDel = 0;
	}

} GPSInfo;


class CPointCut
{
public:
	CPointCut(void);
	~CPointCut(void);

public:
	//��ϡ�㷨������
	int _DoCompress(vector<GPSInfo>& v_vecPoint);
	
private:	
	//������
	void _SimplifyStep(vector<GPSInfo>& v_vecPoint,double v_dTol);
	//�Ƕȷֶη�
	void CalculateAnqle(double v_Angle, double v_tolAngle, vector<GPSInfo>& v_vecPoint);
	//������˹-�տ˷�
	void SimplifyDP(double v_tolDP, vector<GPSInfo>& v_vecPoint,int v_start,int v_end);

private:
	//����������֮��ľ���
	double _SqrNptToNpt(const GPSInfo v_px,  GPSInfo v_py);
	//����㵽ֱ�߾����ƽ��
	double SqrDistPointToLine(const GPSInfo LP0, const GPSInfo LP1, const GPSInfo P);
	
};
