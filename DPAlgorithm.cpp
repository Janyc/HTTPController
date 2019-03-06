#include "stdafx.h"

#include <math.h>
#include "DPAlgorithm.h"


namespace nsDP
{
	//��������֮������ƽ��

	inline double SqrDistPointToPoint(const Vertex_dp& p, const Vertex_dp& q)
	{
		double dx, dy;
		dx = p.x - q.x;
		dy = p.y - q.y;
		return dx * dx + dy * dy;
	}



	//����㵽ֱ�ߵľ����ƽ��
	// ����: LP0, LP1ֱ�ߵ������˵㣻P���������ĵ�

	inline double SqrDistPointToLine(const Vertex_dp &LP0, const Vertex_dp &LP1, const Vertex_dp &P)
	{
		double A, B, C;
		double tmp;
		double dist;

		if ((LP0.x == LP1.x) && (LP0.y == LP1.y))
		{ 
			A = P.x - LP0.x;
			B = P.y - LP1.y;
			dist = A * A + B * B;		
		}
		else
		{
			A = LP1.y - LP0.y;
			B = LP0.x - LP1.x;
			C = -(LP0.y * B + LP0.x * A); 		

			tmp = (A* P.x + B* P.y + C);
			dist = tmp* tmp / (A* A + B* B);
		}

		return dist;
	}



	/*********************************************************************
	* ������ : SimplifyDP
	* ��  �� : ������˹ѹ���㷨
	* ��  �� :
	* v_tol -- ��ֵ
	* ����ֵ : 
	* ��  �� : Cheng.X.R 
	* ��  �� : 2008-07-03 
	*********************************************************************/


	void SimplifyDP(double v_tol, Vertex_dp* v_pPoints, int v_start, int v_end)
	{
		//assert(v_pPoints);
		//assert(v_start < v_end);

		//while ((v_pPoints[v_start].x == v_pPoints[v_end].x) &&
		//	(v_pPoints[v_start].y == v_pPoints[v_end].y))
		//{
		//	if (v_start < v_end - 1)
		//	{
		//		//SimplifyDP(v_tol, v_pPoints, v_start, v_end - 1);
		//		--v_end;			
		//	} 
		//	else
		//	{
		//		return;
		//	}		
		//}

		if (v_end <= v_start + 1) // �����㰤��һ��û��Ҫ��
		{
			return;
		}      


		double dTol2 = v_tol * v_tol;

		int iMaxIdx = v_start;  	// ׼����¼�����߶ε���Զ���������

		double dMaxDist = 0.0;  	 // ׼����¼��Զ�����ƽ��

		bool bFlag; // ��β��ͬ�ı�־
		double dDist;
		double a, b, c;	

		if ((v_pPoints[v_start].x == v_pPoints[v_end].x) &&
			(v_pPoints[v_start].y == v_pPoints[v_end].y))
		{
			bFlag = true;
		}
		else
		{
			bFlag = false;
			a = v_pPoints[v_start].y - v_pPoints[v_end].y;
			b = v_pPoints[v_end].x - v_pPoints[v_start].x;
			c = ((double)v_pPoints[v_start].x * (double)v_pPoints[v_end].y) 
				- ((double)v_pPoints[v_start].y * (double)v_pPoints[v_end].x);
		}



		for (int i = v_start + 1; i < v_end; i++)
		{
			if (v_pPoints[i].state == -1)
			{
				continue; //�˵��Ѿ���ɾ����
			}

			//dDist = SqrDistPointToLine(v_pPoints[v_start], v_pPoints[v_end], v_pPoints[i]);

			if (bFlag)
			{
				a = v_pPoints[i].x - v_pPoints[v_start].x;
				b = v_pPoints[i].y - v_pPoints[v_start].y;
				dDist = a * a + b * b;	
			}
			else
			{
				dDist = (v_pPoints[i].x * a) + (v_pPoints[i].y * b) + c;
				if (dDist < 0.0)
				{
					dDist = -dDist;
				}
			}		

			if (dDist > dMaxDist)
			{
				dMaxDist = dDist;
				iMaxIdx = i;			
			}
		} 

		if (!bFlag)
		{
			dMaxDist *= dMaxDist / (a * a + b * b);
		}

		//����tol 
		if (dMaxDist > dTol2)      // �����Զ���㵽�߶�S�ľ��������ֵ
		{
			//v_pPoints[iMaxIdx].state = 1;    //��¼maxi��������� �˶��㽫��������

			//�ݹ���ô˳���

			SimplifyDP(v_tol, v_pPoints, v_start, iMaxIdx); // ��һ�����߶�   
			SimplifyDP(v_tol, v_pPoints, iMaxIdx, v_end); // �ڶ������߶�

			if (v_pPoints[iMaxIdx].state == 1) //�����õ��Ѿ�Ԥ����
			{
				return;
			}

			//��ǰ�ұ����ĵ�
			int iLeft = -1;
			int iRight = -1;
			int j;
			for (j = iMaxIdx - 1; j > v_start; j--)
			{
				if (v_pPoints[j].state == 1)
				{
					iLeft = j;
					break;
				}
			}

			if (iLeft == -1)
			{
				v_pPoints[iMaxIdx].state = 1;
				return;
			}

			for (j = iMaxIdx + 1; j < v_end; j++)
			{
				if (v_pPoints[j].state == 1)
				{
					iRight = j;
					break;
				}
			}

			if (iRight == -1)
			{
				v_pPoints[iMaxIdx].state = 1;
				return;
			}

			dDist = SqrDistPointToLine(v_pPoints[iLeft], v_pPoints[iRight],
				v_pPoints[iMaxIdx]);

			v_pPoints[iMaxIdx].state = (dDist < dTol2 ? -1 : 1); 	
			//////////////////////////////////////////
		}
	}



	/*********************************************************************
	* ������ : DataCompress_Poly
	* ��  �� : ʸ��ͼ��ѹ��
	* ��  �� :
	* v_tol -- ��ֵ
	* v_pPoly -- ��������ϸ�������飬 ����Ǵ�Ŷ���Σ�����뱣֤���һ���㲻�����һ����ͬ
	* v_len -- �����ϵĵ�ĸ���
	* ����ֵ : 
	* v_lpPt�е�byDel��������Ϊ1����ʾ�����ĵ㣻����ֵ����0��-1���ǿ�ɾ���ĵ�
	* ��  �� : Cheng.X.R 
	* ��  �� : 2008-07-03 
	*********************************************************************/
	void DataCompress_Poly(double v_tol, Vertex_dp* v_pPoly, int v_iNumPts)
	{
		//assert(v_lpPt);
		//assert(v_iNumPts >= 2);


		int iLow = v_iNumPts - 1;
		int iHigh = 0;
		double dTol2 = v_tol * v_tol;	 //��ֵ��ƽ��



		//��һ����ͨ������֮��ľ����жϣ��Ƿ�ɾ��ĳЩ���㣬��ɾ���ĵ�delFlag���Ϊ-1
		for (int i = 0; i < v_iNumPts; i++)
		{
			if (v_pPoly[i].state != -1)
			{
				iLow = i;
				break;
			}
		}	

		for (int i = v_iNumPts - 1; i >= 0; i--)
		{
			if (v_pPoly[i].state != -1)
			{
				iHigh= i;
				break;
			}
		}	

		/////  
		int s = iLow;
		for (int e = iLow + 1; e < iHigh; e++) //�������ÿ������ѭ������
		{
			if (v_pPoly[e].state == 1)
			{
				s = e;
			}
			else if (v_pPoly[e].state != -1)
			{
				if (SqrDistPointToPoint(v_pPoly[s], v_pPoly[e]) < dTol2)
				{
					v_pPoly[e].state = -1;
				}
				else
				{
					s = e;
				}
			}
		}
		///// 


		//�ڶ��������� Douglas-Peucker�㷨���м�  
		v_pPoly[iLow].state = v_pPoly[iHigh].state = 1;	 // ����һ�������һ��������Ϊ1������

		SimplifyDP(v_tol, v_pPoly, iLow, iHigh);
	}

}

