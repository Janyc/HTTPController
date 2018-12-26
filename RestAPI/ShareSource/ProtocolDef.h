#pragma once

#define MAX_META_COUNT_PER_FRAME 150
#define MAX_LIVE_BROADCASE_TIME 5 //��

namespace nsProtocol
{
	enum _enDevControlType  // �豸��������
	{
		enClient_Crl = 0x01,  // �ͻ��˿�����
		enDataNode_Crl = 0x02, // ���ݽڵ������
		enClient_Dt = 0x03,  // �ͻ���������
		enDataNode_Dt = 0x04, // ���ݽڵ�������
	};
	enum _enControl_Client
	{
		enClientLogin = 0x01, // �ͻ��˵�¼
		enUploadMetaReq = 0x02, // �ļ��ϴ�Ԫ��������
		enDownloadMetaReq = 0x03, // �ļ�����Ԫ��������
		enMasterProBroadcast = 0x41, // ���ڵ����Բ���
		enUploadMetaRsp = 0x42, // �ļ��ϴ�Ԫ����Ӧ��
		enDownloadMetaRsp = 0x43, // �ļ�����Ԫ����Ӧ��
	};

	enum _enControl_DataNode
	{
		enStatusBro = 0x01, // ���ݽڵ�״̬����
		enNodeMeatBro = 0x02, // �ڵ�Ԫ���ݲ���
		enFileUploadMetaBro = 0x03, // �ļ��ϴ�Ԫ���ݲ���
		enDownloadFinBro = 0x04, // �ļ�������ɲ���
		enRegistRsp = 0x41,// ע��Ӧ��
		enFileCopy = 0x42, // �ļ�����
	};

	enum _enData_Client
	{
		enFileBaseInfo = 0x01, // �ļ�������Ϣ
		enFileDataInfo = 0x02, // �ļ�������Ϣ
		enDownloadDataReq = 0x03, // ������������
		enUploadDataRsp = 0x41,// �ϴ�����Ӧ��
		enWebUrlRsp = 0x42, // ����UrlӦ��
	};

	enum _enNodeProp
	{
		enUnknow = 0,
		enReadWrite = 1,		// ��д
		enReadOnly  = 2			// ֻ��
	};
	
#pragma  pack( push, 1 )

	typedef struct _tagSingleFileMetaData
	{
		unsigned short m_usNodeId;
		unsigned long m_uiFileRecordNo;
		unsigned long m_uiFileTS;
		unsigned short m_usFileLenK;
		unsigned char  m_ui8FileType;
	}CSingleFileMetaData;

	typedef struct _tagSingleMetaData
	{
		unsigned long m_uiFileRecordNo;
		unsigned long m_uiFileTS;
		unsigned short m_usFileLenK;
		unsigned char  m_ui8FileType;
	}CSingleMetaData;

	typedef struct _tagFileWriteMetaData
	{
		unsigned long m_uiDataNodeIp;
		unsigned short m_usDataNodePort;
		unsigned long m_uiFileRecordNo;
		unsigned long m_uiFileTS;
		unsigned short m_usFileLenK;
	}CFileWriteMetaData,CFileReadMetaData;

	enum _enStatusProp
	{
		enReg = 0, // ���ݽڵ�ע���
		enAlive = 1, // ���ݽڵ������
		enMasterAlive = 2, // ���ڵ������
	};
	typedef struct _tagDataNodeRegist
	{
		unsigned short	m_ui16ListenPort;	// �����˿�
		unsigned char   m_ui8Prop;			// ���ԣ���ע�ỹ������
		unsigned long  m_uiIndexCount;		// Ԫ����������
		unsigned char   m_ui8NodeProp;		// ���ݽڵ�����
		unsigned long  m_uiDiskRate;		// ����������
		unsigned long  m_uiMemUsed;		// �ڴ�ռ��
		unsigned long  m_uiLoadValue;		// ����ֵ
	}CDataNodeStatusBro;

	typedef struct _tagDataNodeRegistRsp
	{
		unsigned short  m_usNodeId; // �ڵ�ID
	}CDataNodeRegistRsp; // ���ݽڵ�ע��Ӧ��

	enum _enMetaProp
	{
		enAddMeta = 0, // ���Ԫ����
		enDelMeta = 1, // ɾ��Ԫ����
	};
	typedef struct _tagMetaDataBroadcast
	{
		unsigned char   m_ui8Prop; // ���ͣ���/ɾ��
		unsigned long  m_uiCount; // ����
		CSingleMetaData m_ArySingleMetaDatas[MAX_META_COUNT_PER_FRAME]; // Ԫ��������
	}CMetaDataBroadcast;

	typedef struct _tagClientWriteMetaReq
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned long m_uiFileLen; // �ļ���С
	}CClientWriteMetaReq; // �ļ��ϴ�����

	typedef struct _tagIpAdressPort
	{
		unsigned long m_uiDataNodeIPAddrs;		// �ڵ�IP�б�
		unsigned short m_uiDataNodePorts;   // �ڵ�˿��б�
	}CIpAdressPort;

	typedef struct _tagMasterMetaRsp
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned long m_uiFileRecordNo; // ��¼��
		unsigned long m_uiFileTS; // ʱ���
		unsigned char  m_ui8DataNodeNum; // �ڵ���
		CIpAdressPort m_lstDataNodeAdress[8]; // �ڵ��б�
	}CMasterMetaRsp; // ���ڵ�Ԫ����Ӧ��

	typedef struct _tagFileBaseInfo
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned long m_uiFileRecordNo; // ��¼��
		unsigned long m_uiTotalFileLen; // �ļ��ܴ�С
		unsigned char  m_ui8FileType; // �ļ�����
		unsigned long m_uiFileTS; // �ļ�ʱ���
	}CFileBaseInfo; // �ļ�������Ϣ

	typedef struct _tagFileDataInfo
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned short m_usFrameNo; // ֡���
		unsigned short m_usFrameDataLen; // ��֡�ļ����ݳ���
		char   m_szFrameData[4096];
	}CFileDataInfo; // �ļ�������Ϣ
	
	typedef struct _tagFileUploadMetaBro
	{
		unsigned char  m_ui8Ret; // �ļ��ϴ����
		unsigned short m_usFileLenK; // ��KΪ��λ���ļ���С
		unsigned long m_uiFileRecordNo; // �ļ���¼��
		unsigned long m_uiFileTS; // �ļ�ʱ���
		unsigned char  m_ui8FileType; // �ļ�����
	}CFileUploadMetaBro; // �ļ��ϴ����Ԫ���ݲ���

	typedef struct _tagDownloadFinBro
	{
		unsigned short m_usFileLenK; // ��KΪ��λ���ļ���С
	}CDownloadFinBro; // �ļ�������ɽ������

	typedef struct _tagUploadDataRsp
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned char  m_ui8Ret; // �ϴ����
		unsigned char  m_ui8FileNameLen; //�ļ���
		char   m_szFileName[MAX_PATH]; 
	}CUploadDataRsp; // �ļ��ϴ����Ӧ��

	typedef struct _tagDownloadMetaReq
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned long m_uiFileRecordNo; // �ļ���¼��
		unsigned long m_uiFileTS; // �ļ�ʱ���
		unsigned short m_usFileLenK; // ��KΪ��λ���ļ���С
	}CClientReadMetaReq; // �ļ�����Ԫ��������

	typedef struct _tagFileDataDownloadReq
	{
		unsigned long m_uiSN; // ��ˮ��
		unsigned long m_uiFileTS; // ʱ���
		unsigned short m_usFileLenK; // ��KΪ�ĵ�λ���ļ���С
		unsigned char  m_ui8DownloadType; // ��������
		unsigned char  m_ui8FileProp; // �ļ����ԣ�ԭͼ/����ͼ��
		unsigned char  m_ui8FileNameLen; // �ļ���
		char   m_szFileName[MAX_PATH];
	}CFileDataDownloadReq; // �ļ�������������

	typedef struct _tagFileCopyCommand
	{
		unsigned char  m_ui8DstDataNodeNum; // Ŀ�����ݽڵ���
		CIpAdressPort m_lstAryNodeIpAddr[8];// Ŀ�����ݽڵ��ַ����
		unsigned long m_uiFileTS; // �ļ�ʱ���
		unsigned long m_uiFileRecordNo; // �ļ���¼��
		unsigned short m_usFileLenK; //��KΪ��λ���ļ���С
		unsigned char  m_ui8FileType; // �ļ�����
	}CFileCopyCommand; // �ļ�����ָ��

	const int PROTOCAL_HEAD_LEN = 2; // Э��ͷ����
	const float READ_WRITE_RATE = 5.0; // ���̶�д��������
	const int DATA_FRAMR_LEN = 1800; // ��֡�ļ����ݳ���
	const int MAX_ALIVE_TIMER = 15; // ҵ������ʱ��

#pragma  pack( pop )
}

using namespace nsProtocol;