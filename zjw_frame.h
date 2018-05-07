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

private:
	//����Ƿ���ù�batchLoadObj�ӿ�
	bool batchLoad;
	bool getAllFielPath;

public:
	FrameManage();
	~FrameManage();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	//���沢�����ļ����µ����е������ļ�.�����ؼ������Ӧ��swgϵ��
	void batchLoadObj(string fileNameFormat="_cleaner.obj", string path = "./testData/football");

	//�������и�ʽ��·���ģ���С���󱣴�����·������,���õ����е�frameList
	void getAllFilesPath(string fileNameFormat = "_cleaner.obj", string path = "./testData/football");
	//��ָ����������֡����ȡobj�������ֵsgwt��ϵ����frameId ��0��ʼ
	bool loadContinuousFrames(int frameId1, int frameId2, string fileNameFormat = "_cleaner.obj", string path = "./testData/football");

};


