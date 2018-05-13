#pragma once
#include "zjw_macro.h"
#include "zjw_pcs_octree.h"
#include "zjw_fileBatch.h"
#include "zjw_timer.h"
#include <sstream>
#include <assert.h>

//���������е�֡
class Frame
{
public:
	int frameId;
	ObjMesh *objMesh;
	PcsOctree *pcsOct;
public:
	//Frame();
	Frame(const int frameId);
	~Frame();
	bool loadObj(string path);
	bool octSgwt();
};

//���й�����
class FrameManage
{
public:
	vector<Frame*> frameList;
	FileBatch *fileBatch;
	enum FileNameForMat
	{
		NUM_FRONT,
		NUM_TAIL
	};

private:
	//����Ƿ���ù�batchLoadObj�ӿ�
	bool batchLoad;
	//����Ƿ���ù�getAllfilepath�ӿ�
	bool getAllFilePath;

public:
	FrameManage();
	~FrameManage();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	//���沢�����ļ����µ����е������ļ�.�����ؼ������Ӧ��swgϵ��
	void batchLoadObj(FileNameForMat type = NUM_FRONT, string fileNameFormat="_cleaner", string path = "./testData/football");

	//�������и�ʽ��·���ģ���С���󱣴�����·������,���õ����е�frameList
	void getAllFilesPath(FileNameForMat type = NUM_FRONT,string fileNameFormat =  "_cleaner", string path = "./testData/football");

	//��ָ����������֡����ȡobj�������ֵsgwt��ϵ����frameId ��0��ʼ
	bool loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type = NUM_FRONT, string fileNameFormat = "_cleaner", string path = "./testData/football");

};


