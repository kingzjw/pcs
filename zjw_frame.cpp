#include "zjw_frame.h"

Frame::Frame()
{
	objMesh = new ObjMesh();
	pcsOct = new PcsOctree();
}

Frame::Frame(const int frameId)
{
	objMesh = new ObjMesh();
	pcsOct = new PcsOctree();
}

Frame::~Frame()
{
	if (objMesh)
		delete objMesh;
	if (pcsOct)
		delete pcsOct;
}

bool Frame::loadObj(string path)
{
	return objMesh->loadObjMesh(path);
}

bool Frame::octSgwt()
{
	Vec3 minPos(objMesh->rangeMin);
	Vec3 maxPos((objMesh->rangeMax + Epsilon));
	Vec3 cellSize(0.42);
	//���ò������������˲���
	pcsOct->setParam(minPos, maxPos, cellSize);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//�õ�Ҷ�ӽڵ�ı߽磬��������ص���Ϣ
	pcsOct->getLeafboundary();

	pcsOct->getGraphMat();
	pcsOct->getMatEigenVerValue();

	//�õ��˲����϶�����ź�
	pcsOct->setPointTo8Areas();
	pcsOct->getLeafSignal();
	//pcsOct->getSgwtCoeffWS();
	//�õ��ź�x�ڵ�һ������0�е�ֵ
	pcsOct->getSgwtCoeffWS(SignalType::SignalX, 0);
	return true;
}

FrameManage::FrameManage(string path, string file)
{
	fileNum = 0;
	filePath = path;
	fileName = file;
}

FrameManage::FrameManage()
{
}

FrameManage::~FrameManage()
{
}

void FrameManage::batchLoadObj(string fileNameFormat, string path)
{

//#ifdef ZJW_DEBUG
//	vector<string> files;
//	getFiles(path, files);
//
//	cout << "############################" << endl;
//	cout << "file num : "<<fileNum << endl;
//	for (int i = 0; i < files.size(); i++)
//	{
//		cout << files[i] << endl;
//	}
//	cout << "############################" << endl;
//#endif
//
//	//--------��ȡ��·���µ������ļ�������-------------
//	fileNum = getFilesNum(path);
//
//	//--------��ȡ��·���µ������ļ�-------------
//	
//	//for (int f_it = 0; f_it < fileNum; f_it++)
//	for (int f_it = 0; f_it < 1; f_it++)
//	{
//		string totalFilePath = path + string(1,f_it)+fileNameFormat;
//#ifdef ZJW_DEBUG
//		cout << totalFilePath << endl;
//#endif 
//		frameList.push_back(Frame(f_it));
//		frameList[f_it].objMesh->loadObjMesh(totalFilePath);
//		frameList[f_it].octSgwt();
//	}
}
