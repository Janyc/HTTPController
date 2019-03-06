#ifndef __dp_algorithm_h_
#define __dp_algorithm_h_


namespace nsDP
{
	typedef struct _tagVertex_dp
	{
		long x;
		long y;		
		int state; // ���״̬��
		           // 0 -- ����̬���õ��ڳ�ϡ�Ĺ����и������������ɾ��
		           // -1 -- ɾ��̬���õ��ڳ�ϡ�Ĺ����б�Ȼ��ɾ��
		           // 1 -- ����̬���õ��ڳ�ϡ�Ĺ����б�Ȼ����

		_tagVertex_dp(long v_x = 0, long v_y = 0, int v_state = 0)
			: x(v_x), y(v_y), state(v_state)
		{}
		
	}Vertex_dp;

	extern void DataCompress_Poly(double v_tol, Vertex_dp* v_lpPt, int v_iPtNum);

}

#endif
