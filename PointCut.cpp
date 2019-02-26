#include "StdAfx.h"
#include "PointCut.h"


CPointCut::CPointCut(void)
{
}

CPointCut::~CPointCut(void)
{
}

int CPointCut::_DoCompress(vector<GPSInfo>& v_vecPoint)
{
	int iStart = 0;
	int iEnd = v_vecPoint.size() - 1;

	double Lenvtoll = 80.0;
	double llenToll_step = Lenvtoll;//������������ֵ����ʾ������Ϊ60*60/100��ƽ��ֵ��,����ͬ��ʹ�ó��ȵ�ƽ��

	double langleToll_angle = 10.0;//�Ƕȷ������Ƕ���ֵ����ʾ������Ϊ20��
	double llenToll_angle = Lenvtoll*Lenvtoll/25;//�Ƕȷ��������߶γ�����ֵ����ʾ������Ϊ60*60/25(ƽ��ֵ)

	//double ldisToll_angle = Lenvtoll*Lenvtoll*2.13/5;//�Ƕȷ�����������ֵ,��ʾ������Ϊ60*60*2.13/5��ƽ��ֵ��
	double ldistToll_DP = Lenvtoll*Lenvtoll*3/5;//������˹�տ˷�,������ֵ����ʾ������Ϊ60*60*3/5��ƽ��ֵ��

	//���ò��������Ƕȷ��������Լ� Douglas-Peucker�㷨���м�
	//ʹ�ò�������ϡ 
	_SimplifyStep(v_vecPoint,llenToll_step); 
	
	//ʹ�ýǶȷ�������ϡ
	CalculateAnqle(langleToll_angle,llenToll_angle,v_vecPoint);

	//ʹ�õ�����˹�տ˷���ϡ
	SimplifyDP(ldistToll_DP, v_vecPoint,iStart,iEnd);

	return 0;
}

//������
void CPointCut::_SimplifyStep(vector<GPSInfo>& v_vecPoint,double v_dTol)
{
	//��ֵ��ƽ��
	double dTol = v_dTol*v_dTol;

	int iHigh = v_vecPoint.size() -1;
	int iLow = 0;

	//��㡢�յ���뱣����
	v_vecPoint[iLow].m_iDel = 1;
	v_vecPoint[iHigh].m_iDel = 1;

	//��һ����С����ֵ���߶Σ����������߶�С����ֵ���򽫺�һ�ڵ��ʶΪ��ɾ����
	int iStart = iLow;
	for (int iEnd = iLow +1; iEnd < iHigh; iEnd++)
	{
		if (1 == v_vecPoint[iEnd].m_iDel)
		{//�����Ϊ1����Ϊ�����㣬�޸�����λ��
			iStart = iEnd;
		}
		else if (v_vecPoint[iEnd].m_iDel != -1)
		{//����Ƿ���ɾ���㣬�����ж�
			if (_SqrNptToNpt(v_vecPoint[iStart],v_vecPoint[iEnd]) < dTol)
			{//�����߶μ��߶�С����ֵ���򽫺�һ�ڵ���Ϊ��ɾ����
				v_vecPoint[iEnd].m_iDel = -1;
			}
			else
			{
				iStart = iEnd;
			}
		}
	}

	return;
}

//�Ƕȷֶη�
void CPointCut::CalculateAnqle(double v_Angle, double v_tolAngle, vector<GPSInfo>& v_vecPoint)
{
	if (v_vecPoint.size() < 2) // ����Ҫ���������ж����߶εļн�
	{
		return;
	}

	double dDist;
	double a, b, c;
	int k;
	int firstPoint,middlePoint,secondPoint;
	double a1, b1, a2, b2;
	double k1, k2, L1, L2;
	double angle;

	firstPoint = 0;
	for (int i = 1; i < (int)v_vecPoint.size(); i++)//�ӵڸ��㿪ʼ�ж�
	{
		angle = 0;

		if (v_vecPoint[i].m_iDel == 1)//�õ�Ϊ������
		{
			firstPoint = i;
			continue; 
		}
		else if (v_vecPoint[i].m_iDel == -1)
		{
			continue; //�˵��Ѿ���ɾ����
		}
		else
		{
			middlePoint = i;
		}

		for (k = i + 1; k <= (int)v_vecPoint.size()-1; k++)
		{
			if (v_vecPoint[k].m_iDel == -1)
			{
				k++; //�˵��Ѿ���ɾ����
			}
			else
			{
				secondPoint = k;
				break;
			}
		}

		if (k >(int)v_vecPoint.size()-1)
		{
			break;
		}

		a1 = (double)v_vecPoint[firstPoint].m_u32Lat - (double)v_vecPoint[middlePoint].m_u32Lat;
		b1 = (double)v_vecPoint[middlePoint].m_u32Lon - (double)v_vecPoint[firstPoint].m_u32Lon;		

		if ((double)v_vecPoint[middlePoint].m_u32Lon == (double)v_vecPoint[firstPoint].m_u32Lon)
		{
			k1 = -a1;//��ֱ����б�ʣ�����������ֵƫ��
		}
		else
		{
			k1 = -a1 / b1;//б��
		}

		a2 = (double)v_vecPoint[middlePoint].m_u32Lat - (double)v_vecPoint[secondPoint].m_u32Lat;
		b2 = (double)v_vecPoint[secondPoint].m_u32Lon - (double)v_vecPoint[middlePoint].m_u32Lon;

		if ((double)v_vecPoint[secondPoint].m_u32Lon == (double)v_vecPoint[middlePoint].m_u32Lon)
		{
			k2 = -a2;//��ֱ����б�ʣ�����������ֵƫ��
		}
		else
		{
			k2 = -a2 / b2;//б��
		}

		double tangent = (k2 - k1) / (1 + k1 * k2);//�нǹ�ʽ
		double radian = atan(abs(tangent));//�������������ֵ
		angle = radian * 180.0 / Pi;//����ת���ɽǶ�

		L1 = sqrt(a1 * a1 + b1 *b1);//�߶γ��ȵĿ�ƽ��//sqrt(a1 * a1 + b1 *b1);
		L2 = sqrt(a2 * a2 + b2 * b2); //�߶γ��ȵĿ�ƽ��//sqrt(a2 * a2 + b2 * b2);

		//������ӣ����k1��б��(Ҳ���Ǹ��߶κ���ֵΪ)�����Ƕȡ��߶γ��ȶ�С����ֵ���ж�����ֵ
		//v_pPoints[middlePoint].y < v_pPoints[secondPoint].y,��õ㲻ɾ����ͬ��k2

		if ((angle < v_Angle))//���߶μ�ļн�
		{
			if (L1 < v_tolAngle || L2 < v_tolAngle)//���߶�֮һ�볤����ֵ���
			{
				v_vecPoint[middlePoint].m_iDel = -1;
				continue;
			}
			else
			{
				//������ֵ��
				a = (double)v_vecPoint[firstPoint].m_u32Lat - (double)v_vecPoint[secondPoint].m_u32Lat;//��β�������򳤶�:a=ys-ye
				b = (double)v_vecPoint[secondPoint].m_u32Lon - (double)v_vecPoint[firstPoint].m_u32Lon;//��β������򳤶�:b=xe-xs		 

				c = ((double)v_vecPoint[firstPoint].m_u32Lon * (double)v_vecPoint[secondPoint].m_u32Lat) //ֱ�߷���y=kx+c1=(-a/b)*x+c1
					- ((double)v_vecPoint[firstPoint].m_u32Lat * (double)v_vecPoint[secondPoint].m_u32Lon);//c1=(xs*ye-ys*xe)/(xs-xe)=c/(-b)

				//��(x0,y0)���߶�Ax+By+c1=0�ľ��빫ʽ��d=(|A*x0+B*y0+c1|)/[sqrt(A*A+B*B)] 
				dDist = ((double)v_vecPoint[middlePoint].m_u32Lon * a) + ((double)v_vecPoint[middlePoint].m_u32Lat * b) + c;//�㵽�߶ξ��빫ʽ�ķ��Ӳ���
				if (dDist < 0.0)
				{
					dDist = -dDist;
				}

				double RdDist = dDist / sqrt(a * a + b * b);//�����ĵ㵽�߶ξ����ƽ��
				if (RdDist < v_tolAngle)
				{
					v_vecPoint[middlePoint].m_iDel = -1;
				}
			}
		}
		else
		{
			firstPoint = i;
		}	
	}	

}

//������˹-�տ˷�
void CPointCut::SimplifyDP(double v_tolDP, vector<GPSInfo>& v_vecPoint,int v_start,int v_end)
{
	if (v_end <= v_start + 1) // �����㰤��һ��û��Ҫ��
	{
		return;
	} 

	int iMaxIdx = v_start;  	// ׼����¼�����߶���Զ���������

	double dMaxDist = 0.0;  	 // ׼����¼��Զ�����ƽ��

	bool bFlag; // ��β��ͬ�ı�־
	double dDist;
	double a, b, c;		

	if (((double)v_vecPoint[v_start].m_u32Lon == (double)v_vecPoint[v_end].m_u32Lon) &&
		((double)v_vecPoint[v_start].m_u32Lat == (double)v_vecPoint[v_end].m_u32Lat))
	{
		bFlag = true;	//��һ���������һ�����ص�
	}
	else
	{
		bFlag = false; //Ϊ��ͬ��������
		a = (double)v_vecPoint[v_start].m_u32Lat - (double)v_vecPoint[v_end].m_u32Lat;//��β�������򳤶�:a=ys-ye
		b = (double)v_vecPoint[v_end].m_u32Lon - (double)v_vecPoint[v_start].m_u32Lon;//��β������򳤶�:b=xe-xs
		c = ((double)v_vecPoint[v_start].m_u32Lon * (double)v_vecPoint[v_end].m_u32Lat) //ֱ�߷���y=kx+c1=(-a/b)*x+c1
			- ((double)v_vecPoint[v_start].m_u32Lat * (double)v_vecPoint[v_end].m_u32Lon);//c1=(xs*ye-ys*xe)/(xs-xe)=c/(-b)
	}

	for (int i = v_start + 1; i < v_end; i++)//�ӵڸ��㿪ʼ�ж�
	{
		if (v_vecPoint[i].m_iDel == -1)
		{
			continue; //�˵��Ѿ���ɾ����
		}

		//dDist = SqrDistPointToLine(v_pPoints[v_start], v_pPoints[v_end], v_pPoints[i]);

		if (bFlag)
		{
			////��һ���������һ�����ص����
			a = (double)v_vecPoint[i].m_u32Lon - (double)v_vecPoint[v_start].m_u32Lon;
			b = (double)v_vecPoint[i].m_u32Lat - (double)v_vecPoint[v_start].m_u32Lat;
			dDist = a * a + b * b;	//����֮��ľ���
		}
		else
		{
			//��(x0,y0)���߶�Ax+By+c1=0�ľ��빫ʽ��d=(|A*x0+B*y0+c1|)/[sqrt(A*A+B*B)] 
			dDist = ((double)v_vecPoint[i].m_u32Lon * a) + ((double)v_vecPoint[i].m_u32Lat * b) + c;//�㵽�߶ξ��빫ʽ�ķ��Ӳ���
			if (dDist < 0.0)
			{
				dDist = -dDist;
			}
		}		

		if (dDist > dMaxDist)//dMaxDist: ��¼��Զ�����ƽ��
		{
			dMaxDist = dDist;
			iMaxIdx = i;			
		}
	} 

	if (!bFlag)
	{
		dMaxDist *= dMaxDist / (a * a + b * b);//�����ĵ㵽�߶ξ����ƽ��
	}

	//����tol 
	if (dMaxDist > v_tolDP )      // �����Զ���㵽�߶�S�ľ��������ֵ
	{
		//v_pPoints[iMaxIdx].iDel = 1;    //��¼maxi����������˶��㽫��������

		//�ݹ���ô˳���

		SimplifyDP(v_tolDP, v_vecPoint, v_start, iMaxIdx); // ��һ�����߶�  
		SimplifyDP(v_tolDP, v_vecPoint, iMaxIdx, v_end); // �ڶ������߶�

		if (v_vecPoint[iMaxIdx].m_iDel == 1) //�����õ��Ѿ�Ԥ����
		{
			return;
		}

		//��ǰ�ұ����ĵ�
		int iLeft = -1;
		int iRight = -1;
		int j;
		for (j = iMaxIdx - 1; j > v_start; j--)
		{
			if (v_vecPoint[j].m_iDel == 1)
			{
				iLeft = j;
				break;
			}
		}

		if (iLeft == -1)
		{
			v_vecPoint[iMaxIdx].m_iDel = 1;
			return;
		}

		for (j = iMaxIdx + 1; j < v_end; j++)
		{
			if (v_vecPoint[j].m_iDel == 1)
			{
				iRight = j;
				break;
			}
		}

		if (iRight == -1)
		{
			v_vecPoint[iMaxIdx].m_iDel = 1;
			return;
		}

		dDist = SqrDistPointToLine(v_vecPoint[iLeft], v_vecPoint[iRight], v_vecPoint[iMaxIdx]);

		v_vecPoint[iMaxIdx].m_iDel = (dDist < v_tolDP ? -1 : 1);
	}

}

//����������֮������ƽ��
double CPointCut::_SqrNptToNpt(const GPSInfo v_p, const GPSInfo v_q)
{
	double dx, dy;
	dx = (double)v_p.m_u32Lon - (double)v_q.m_u32Lon;
	dy = (double)v_p.m_u32Lat - (double)v_q.m_u32Lat;
	return dx * dx + dy * dy;
}

//����㵽ֱ�߾����ƽ��
double CPointCut::SqrDistPointToLine(const GPSInfo LP0, const GPSInfo LP1, const GPSInfo P)
{
	double A, B, C;
	double tmp;
	double dist;

	if ((LP0.m_u32Lon == LP1.m_u32Lon) && (LP0.m_u32Lat == LP1.m_u32Lat))
	{ 
		A = (double)P.m_u32Lon - (double)LP0.m_u32Lon;
		B = (double)P.m_u32Lat - (double)LP1.m_u32Lat;
		dist = A * A + B * B;		
	}
	else
	{
		A = (double)LP1.m_u32Lat - (double)LP0.m_u32Lat;
		B = (double)LP0.m_u32Lon - (double)LP1.m_u32Lon;
		C = -((double)LP0.m_u32Lat * B + (double)LP0.m_u32Lon * A); 		

		tmp = (A* (double)P.m_u32Lon + B* (double)P.m_u32Lat + C);
		dist = tmp* tmp / (A* A + B* B);
	}

	return dist;
}