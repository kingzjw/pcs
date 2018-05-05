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
	//设置参数，并构建八叉树
	pcsOct->setParam(minPos, maxPos, cellSize);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//得到叶子节点的边界，并保存相关的信息
	pcsOct->getLeafboundary();

	pcsOct->getGraphMat();
	pcsOct->getMatEigenVerValue();

	//得到八叉树上顶点的信号
	pcsOct->setPointTo8Areas();
	pcsOct->getLeafSignal();
	//pcsOct->getSgwtCoeffWS();
	//拿到信号x在第一个象限0中的值
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
//	//--------获取该路径下的所有文件的数量-------------
//	fileNum = getFilesNum(path);
//
//	//--------获取该路径下的所有文件-------------
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
