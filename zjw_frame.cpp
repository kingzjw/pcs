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
#ifdef ZJW_TIMER
	ZjwTimer timer2;
	timer2.Start();
#endif
	
	Vec3 minPos(objMesh->rangeMin);
	Vec3 maxPos((objMesh->rangeMax + Epsilon));
	/*Vec3 minPos(objMesh.rangeMin);
	Vec3 maxPos((objMesh.rangeMax + Epsilon));*/

	Vec3 cellSize(0.1);
	//设置参数，并构建八叉树
	pcsOct->setParam(minPos, maxPos, cellSize);
	//pcsOct->buildPcsOctFrmPC(objMesh);
	pcsOct->buildPcsOctFrmPC(objMesh);
	//得到叶子节点的边界，并保存相关的信息
	pcsOct->getLeafboundary();

	//test
	//cout << pcsOct << endl;
	//end test
	pcsOct->getGraphMat();
	pcsOct->getMatEigenVerValue();

	//得到八叉树上顶点的信号
	pcsOct->setPointTo8Areas();
	pcsOct->getLeafSignal();
	//pcsOct->getSgwtCoeffWS();
	//拿到信号x在第一个象限0中的值
	//pcsOct->getSgwtCoeffWS(SignalType::SignalX, 0);

	VectorXd featureVec;
	pcsOct->getFeatureVector(0,&featureVec);
	//cout << featureVec << endl;


#ifdef ZJW_TIMER
	timer2.Stop();
	timer2.printTimeInMs("build oct and compute the sgwt coeff time: ");
#endif

	pcsOct->doKmeans();
	return true;
}

FrameManage::FrameManage()
{
	batchLoad = false;
	getAllFilePath = false;

	fileBatch = nullptr;
}

FrameManage::~FrameManage()
{
	delete fileBatch;
}

void FrameManage::getAllFilesPath(FileNameForMat type, string fileNameFormat, string path)
{
	//--------获取该路径下的所有文件-------------
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	getAllFilePath = true;

	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
		//拿到所有的路径
		string totalFilePath;
		stringstream ss;
		ss << f_it;

		switch (type)
		{
		case FrameManage::NUM_FRONT:
			totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat).append(".obj");
			break;
		case FrameManage::NUM_TAIL:
			totalFilePath.assign(path).append("/").append(fileNameFormat).append(ss.str()).append(".obj");
			break;
		default:
			break;
		}

		//按照需要从小到大，保存所有的文件路径
		fileBatch->files.push_back(totalFilePath);

		//拿到所有的frameList
		Frame * frame = new Frame(f_it);
		frameList.push_back(frame);
	}
}

void FrameManage::batchLoadObj(FileNameForMat type,string fileNameFormat, string path)
{
	if (fileBatch)
		delete fileBatch;
	fileBatch = new FileBatch(path, fileNameFormat);
	fileBatch->fileNum = fileBatch->getFilesNum(fileBatch->filePath);
	frameList.clear();
	batchLoad = true;

	//--------获取该路径下的所有文件-------------
	for (int f_it = 0; f_it < fileBatch->fileNum; f_it++)
	{
#ifdef ZJW_DEBUG
		cout <<endl<<endl<< "##################################################" << endl;
#endif
		string totalFilePath;
		stringstream ss;
		ss << f_it;

		switch (type)
		{
		case FrameManage::NUM_FRONT:
			totalFilePath.assign(path).append("/").append(ss.str()).append(fileNameFormat).append(".obj");
			break;
		case FrameManage::NUM_TAIL:
			totalFilePath.assign(path).append("/").append(fileNameFormat).append(ss.str()).append(".obj");
			break;
		default:
			break;
		}
 
		//按照需要从小到大，保存所有的文件路径
		fileBatch->files.push_back(totalFilePath);

		Frame * frame =new Frame(f_it);
		frameList.push_back(frame);

		//-----------对每个路径，都进行读取，和sgw操作------------
		ZjwTimer timer;
		timer.Start();
		if (!frameList[f_it]->objMesh->loadObjMeshSimply(totalFilePath))
		{
			continue;
		}
		//frameList[f_it-1].objMesh->loadObjMeshSimply(totalFilePath);
		timer.Stop();
		timer.printTimeInMs("load obj time: ");
		
		frameList[f_it]->octSgwt();
		//frameList[f_it-1].octSgwt();

#ifdef ZJW_DEBUG
		//cout << totalFilePath << endl;
#endif //zjw_debug
	}
}

bool FrameManage::loadContinuousFrames(int frameId1, int frameId2, FileNameForMat type,string fileNameFormat, string path)
{
	assert(frameId1 > -1 && frameId2 >-1);
	
	if (batchLoad)
		return true;

	if (!getAllFilePath)
	{
		getAllFilesPath(type,fileNameFormat, path);
	}

	assert( frameId1 < fileBatch->fileNum && frameId2 < fileBatch->fileNum);

	//-----------进行读取，和sgw操作------------
	ZjwTimer timer;
	timer.Start();
#ifdef ZJW_DEBUG
	cout << endl << endl << "###################################################################" <<endl<< path << endl;
#endif
	if (frameList[frameId1]->objMesh->loadObjMeshSimply(fileBatch->files[frameId1]))
	{
		timer.Stop();
		timer.printTimeInMs("load obj time: ");

		frameList[frameId1]->octSgwt();
	}

	ZjwTimer timer3;
	timer3.Start();
#ifdef ZJW_DEBUG
	cout << endl << endl << "###################################################################" <<endl<< path << endl;
#endif
	if (frameList[frameId2]->objMesh->loadObjMeshSimply(fileBatch->files[frameId2]))
	{
		timer3.Stop();
		timer3.printTimeInMs("load obj time: ");

		frameList[frameId2]->octSgwt();

	}
	return true;
}

bool FrameManage::matchNode(int frameId1, int frameId2)
{
	Frame* frame1 = frameList[frameId1];
	Frame* frame2 = frameList[frameId2];

	//遍历frame1中所有的nodelist
	for (int leaf_it = 0; leaf_it < frame1->pcsOct->ctLeaf->nodeList.size(); leaf_it++)
	{
		//frame1: 计算每个node中，点关于node相对于中点的距离成反比的权重。
		Node * node1 = &(frame1->pcsOct->ctLeaf->nodeList[leaf_it]->nodeData);
		Vec3 midPoint = (*frame1->pcsOct->ctLeaf->midVList)[leaf_it];
		vector<float> wightList;
		double totalWight = 0;
		double tempW;
		//遍历这个叶子节点上所有的点
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
			tempW = 1 / node1->pointPosList[p_it].Distance(midPoint);
			totalWight += tempW;
			wightList.push_back(tempW);
		}

		//利用fram2对应点的位置，根据权重预计出对应Node的中点位置
		Vec3 predictNodePos;
		int idx;
		for (int p_it = 0; p_it < node1->pointIdxList.size(); p_it++)
		{
			//拿到frame1节点中点的序号，在frame2中的点的值
			idx = node1->pointIdxList[p_it];
			predictNodePos += wightList[p_it] * frame2->objMesh->vertexList[idx];
		}
		predictNodePos = predictNodePos / totalWight;

		//根据预计出来的中点位置,通过在frame2上的八叉树，然后得到匹配的node结果
		//先用for 循环实现，后面改用八叉树快速判断
		int macthNodeIdx = -1;
		frame2->pcsOct->judegePointToLeafNode((Vec3*)&predictNodePos, macthNodeIdx);

		if (macthNodeIdx < 0)
		{
			cout << "frame1 node " << leaf_it << " match faied!!"<< endl;
		}
		else
		{
			//cout << "frame1 node " << leaf_it << " match frame2 node " << macthNodeIdx << endl;
			cout << leaf_it << " " << macthNodeIdx << endl;

			//cout <<"predict: "<< predictNodePos.x <<"  "<< predictNodePos.y << "  " << predictNodePos.z << endl;

			//cout << (frame2->pcsOct->ctLeaf->minVList)[macthNodeIdx].x <<"  "<<
			//	(frame2->pcsOct->ctLeaf->minVList)[macthNodeIdx].y <<"  "<<
			//	(frame2->pcsOct->ctLeaf->minVList)[macthNodeIdx].z << "  " << endl;

			//cout << (frame2->pcsOct->ctLeaf->maxVList)[macthNodeIdx].x << "  " <<
			//	(frame2->pcsOct->ctLeaf->maxVList)[macthNodeIdx].y << "  " <<
			//	(frame2->pcsOct->ctLeaf->maxVList)[macthNodeIdx].z << "  " << endl;

			//cout << "+++++++++++++++++++++++++++++" << endl;
		}
	}
	return true;
}
