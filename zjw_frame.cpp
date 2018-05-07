#include "zjw_frame.h"

//Frame::Frame()
//{
//	objMesh = new ObjMesh();
//	pcsOct = new PcsOctree();
//}

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
	//return objMesh.loadObjMesh(path);
	return objMesh->loadObjMesh(path);
}

bool Frame::octSgwt()
{
	Vec3 minPos(objMesh->rangeMin);
	Vec3 maxPos((objMesh->rangeMax + Epsilon));
	/*Vec3 minPos(objMesh.rangeMin);
	Vec3 maxPos((objMesh.rangeMax + Epsilon));*/

	Vec3 cellSize(0.42);
	//���ò������������˲���
	pcsOct->setParam(minPos, maxPos, cellSize);
	//pcsOct->buildPcsOctFrmPC(objMesh);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//�õ�Ҷ�ӽڵ�ı߽磬��������ص���Ϣ
	pcsOct->getLeafboundary();

	//test
	//cout << pcsOct << endl;
	//end test
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

FrameManage::FrameManage()
{
	batchLoad = false;
	getAllFielPath = false;

	fileBatch = nullptr;
}

FrameManage::~FrameManage()
{
	delete fileBatch;
}

void FrameManage::getAllFilesPath(string fileNameFormat, string path)
{
	//--------��ȡ��·���µ������ļ�-------------
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	getAllFielPath = true;

	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
		//�õ����е�·��
		string totalFilePath;
		stringstream ss;
		ss << f_it;
		totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat);

		//������Ҫ��С���󣬱������е��ļ�·��
		fileBatch->files.push_back(totalFilePath);

		//�õ����е�frameList
		Frame * frame = new Frame(f_it);
		frameList.push_back(frame);
	}
}

void FrameManage::batchLoadObj(string fileNameFormat, string path)
{
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	batchLoad = true;

	//--------��ȡ��·���µ������ļ�-------------
	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
#ifdef ZJW_DEBUG
		cout <<endl<<endl<< "##################################################" << endl;
#endif
		string totalFilePath;
		stringstream ss;
		ss << f_it;
		totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat);

		//������Ҫ��С���󣬱������е��ļ�·��
		fileBatch->files.push_back(totalFilePath);

		Frame * frame =new Frame(f_it);
		frameList.push_back(frame);

		//-----------��ÿ��·���������ж�ȡ����sgw����------------
		ZjwTimer timer;
		timer.Start();
		if (!frameList[f_it]->objMesh->loadObjMeshSimply(totalFilePath))
		{
			continue;
		}
		//frameList[f_it-1].objMesh->loadObjMeshSimply(totalFilePath);
		timer.Stop();
		timer.printTimeInMs("load obj time: ");
		
		ZjwTimer timer2;
		timer2.Start();
		frameList[f_it]->octSgwt();
		//frameList[f_it-1].octSgwt();

		timer2.Stop();
		timer2.printTimeInMs("build oct and compute the sgwt coeff time: ");

#ifdef ZJW_DEBUG
		//cout << totalFilePath << endl;
#endif //zjw_debug
	}
}

bool FrameManage::loadContinuousFrames(int frameId1, int frameId2, string fileNameFormat, string path)
{
	assert(frameId1 > -1 && frameId2 >-1);
	
	if (batchLoad)
		return true;
	if (!getAllFielPath)
	{
		getAllFilesPath(fileNameFormat, path);
	}

	assert( frameId1 < fileBatch->fileNum && frameId2 < fileBatch->fileNum);

	//-----------���ж�ȡ����sgw����------------
	ZjwTimer timer;
	timer.Start();
	if (frameList[frameId1]->objMesh->loadObjMeshSimply(fileBatch->files[frameId1]))
	{
		timer.Stop();
		timer.printTimeInMs("load obj time: ");

		ZjwTimer timer2;
		timer2.Start();
		frameList[frameId1]->octSgwt();
		timer2.Stop();
		timer2.printTimeInMs("build oct and compute the sgwt coeff time: ");
	}

	ZjwTimer timer3;
	timer3.Start();
	if (frameList[frameId2]->objMesh->loadObjMeshSimply(fileBatch->files[frameId2]))
	{
		timer3.Stop();
		timer3.printTimeInMs("load obj time: ");

		ZjwTimer timer4;
		timer4.Start();
		frameList[frameId2]->octSgwt();

		timer4.Stop();
		timer4.printTimeInMs("build oct and compute the sgwt coeff time: ");
	}
}
