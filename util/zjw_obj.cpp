#include "zjw_obj.h"

ObjMeshMtl::ObjMeshMtl()
{
	//materialList = nullptr;

	faceNum = 0;
	vertexNum = 0;
	maxX = (numeric_limits<int>::min)();
	maxY = (numeric_limits<int>::min)();
	maxZ = (numeric_limits<int>::min)();

	minX = (numeric_limits<double>::max)();
	minY = (numeric_limits<double>::max)();
	minZ = (numeric_limits<double>::max)();

	RotateMat = new float*[3];
	for (int i = 0; i < 3; ++i)
	{
		RotateMat[i] = new float[3];
		for (int j = 0; j < 3; ++j)
		{
			RotateMat[i][j] = 0.0f;
		}
	}
}

bool ObjMeshMtl::loadObjMeshAndMtl(string & path)
{
	// --------------------check--------------------------
	if (path.substr(path.size() - 4, 4) != ".obj")
		return false;

	std::ifstream file(path);

	if (!file.is_open())
	{
		cout << "obj file open failed!" << endl;
		return false;
	}

#ifdef ZJW_DEBUG
	cout << "starting load the obj: " << path << endl;
#endif // ZJW_DEUG

	//-----------------------init --------------------
	subMeshList.clear();
	vertexList.clear();
	normalList.clear();
	texList.clear();
	//materialList = new MaterialFile;

	// submesh material name
	std::vector<std::string> meshMatNames;
	//obj中 g后面的名字  g
	std::string gName;
	//obj每个mesh的名字(usemtl)
	std::string preMtlName;
	std::string nowMtlName;
	//标记第一次遇到 usemtl标记
	bool firstUsemtl = true;
	SubMesh tempMesh;
	bool subMeshFlag = false;

	std::string curline;
	while (std::getline(file, curline))
	{
		//---------------------load material----------------------------
		if (algorithm::firstToken(curline) == "mtllib")
		{
			// Generate LoadedMaterial
			// Generate a path to the material file
			std::vector<std::string> temp;
			algorithm::split(path, temp, "/");

			std::string pathMaterial = "";

			if (temp.size() != 1)
			{
				for (int i = 0; i < temp.size() - 1; i++)
				{
					pathMaterial += temp[i] + "/";
				}
			}

			pathMaterial += algorithm::tail(curline);

			// Load Materials
			materialFile.loadMaterial(pathMaterial);
		}
		// Get Mesh Material Name
		if (algorithm::firstToken(curline) == "usemtl")
		{
			if (firstUsemtl)
			{
				nowMtlName = algorithm::tail(curline);
				preMtlName = nowMtlName;
			}
			else
			{
				nowMtlName = algorithm::tail(curline);
			}

			// Create new Mesh, if Material changes within a group
			if (!tempMesh.facePoslist.empty())
			{
				// Create pre Mesh  , prepare next mesh
				tempMesh.materialName = preMtlName;
				//确定 material 的参数--最后统一处理

				// Insert Mesh
				subMeshList.push_back(tempMesh);
				preMtlName = nowMtlName;

				// Cleanup
				tempMesh.facePoslist.clear();
				tempMesh.faceNormallist.clear();
				tempMesh.faceTexturelist.clear();
				tempMesh.materialName.clear();
				tempMesh.material = nullptr;
			}
			else {
				if (firstUsemtl)
				{
					firstUsemtl = false;
				}
				else
				{
#ifdef ZJW_DEBUG
					cout << preMtlName << "  : this matiral do not has any face!!" << endl;
#endif // ZJW_DEUG
				}
			}
		}
		// Generate a Mesh Object or Prepare for an object to be created
		if (algorithm::firstToken(curline) == "o" || algorithm::firstToken(curline) == "g" || curline[0] == 'g')
		{
			//need to solve the gname
		}
		// Generate a Vertex Position
		if (algorithm::firstToken(curline) == "v")
		{
			std::vector<std::string> spos;
			Point3 vpos;
			algorithm::split(algorithm::tail(curline), spos, " ");

			vpos.x = std::stof(spos[0]);
			vpos.y = std::stof(spos[1]);
			vpos.z = std::stof(spos[2]);

			vertexList.push_back(vpos);
		}
		// Generate a Vertex Texture Coordinate
		if (algorithm::firstToken(curline) == "vt")
		{
			std::vector<std::string> stex;
			Texture vtex;
			algorithm::split(algorithm::tail(curline), stex, " ");

			vtex.x = std::stof(stex[0]);
			vtex.y = std::stof(stex[1]);

			texList.push_back(vtex);
		}
		// Generate a Vertex Normal;
		if (algorithm::firstToken(curline) == "vn")
		{
			std::vector<std::string> snor;
			Normal vnor;
			algorithm::split(algorithm::tail(curline), snor, " ");

			vnor.x = std::stof(snor[0]);
			vnor.y = std::stof(snor[1]);
			vnor.z = std::stof(snor[2]);

			normalList.push_back(vnor);
		}
		// Generate a Face (vertices & indices)
		if (algorithm::firstToken(curline) == "f")
		{
			//因为这里的face不一定是三角形面片，先把顶点，纹理，法向量索引分开
			std::vector<int> verIndexList;
			std::vector<int> texIndexList;
			std::vector<int> normalIndexList;

			std::vector<std::string> sface, svert;
			algorithm::split(algorithm::tail(curline), sface, " ");

			bool noNormal = false;

			// For every given vertex do this
			for (int i = 0; i < int(sface.size()); i++)
			{
				// See What type the vertex is.
				int vtype;

				algorithm::split(sface[i], svert, "/");

				// Check for just position - v1
				if (svert.size() == 1)
				{
					// Only position
					vtype = 1;
				}

				// Check for position & texture - v1/vt1
				if (svert.size() == 2)
				{
					// Position & Texture
					vtype = 2;
				}

				// Check for Position, Texture and Normal - v1/vt1/vn1
				// or if Position and Normal - v1//vn1
				//v1//vn 根据 /分成了三个部分，第二个部分为空
				if (svert.size() == 3)
				{
					if (svert[1] != "")
					{
						// Position, Texture, and Normal
						vtype = 4;
					}
					else
					{
						// Position & Normal
						vtype = 3;
					}
				}

				// Calculate and store the vertex
				switch (vtype)
				{
				case 1: // P
				{
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					break;
				}
				case 2: // P/T
				{
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					texIndexList.push_back(std::stoi(svert[1]) - 1);
					break;
				}
				case 3: // P//N
				{
					//序号统一需要减一处理，因为在数组中是从0开始的
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					normalIndexList.push_back(std::stoi(svert[2]) - 1);
					break;
				}
				case 4: // P/T/N
				{
					//序号统一需要减一处理，因为在数组中是从0开始的
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					texIndexList.push_back(std::stoi(svert[1]) - 1);
					normalIndexList.push_back(std::stoi(svert[2]) - 1);
					break;
				}
				default:
				{
					break;
				}
				}
			}

			//如果不是三角面片，那么采用三角化的方法
			Point3 tempPIndx;
			Point3 tempTextureIndx;
			Normal tempNIndx;

			if (verIndexList.size() == 3)
			{
				//保存三角面片信息到mesh中
				if (!verIndexList.empty())
				{
					tempPIndx.x = verIndexList[0];
					tempPIndx.y = verIndexList[1];
					tempPIndx.z = verIndexList[2];

					tempMesh.facePoslist.push_back(tempPIndx);
				}

				if (!texIndexList.empty())
				{
					tempTextureIndx.x = texIndexList[0];
					tempTextureIndx.y = texIndexList[1];
					tempTextureIndx.z = texIndexList[2];
					tempMesh.faceTexturelist.push_back(tempTextureIndx);
				}

				if (!normalIndexList.empty())
				{
					tempNIndx.x = normalIndexList[0];
					tempNIndx.y = normalIndexList[1];
					tempNIndx.z = normalIndexList[2];
					tempMesh.faceNormallist.push_back(tempNIndx);
				}
			}
			else if (verIndexList.size() < 3)
			{
				cout << "the f read error!!" << endl;
			}
			else {
				//进行三角化

				//每一个Point 存储序号
				vector<Point3> indexList;
				trianglation(verIndexList.size(), indexList);

				for (int i = 0; i < indexList.size(); i++)
				{
					if (!verIndexList.empty())
					{
						//保存信息到mesh中
						tempPIndx.x = verIndexList[indexList[i].x];
						tempPIndx.y = verIndexList[indexList[i].y];
						tempPIndx.z = verIndexList[indexList[i].z];
						tempMesh.facePoslist.push_back(tempPIndx);
					}

					if (!texIndexList.empty())
					{
						tempTextureIndx.x = texIndexList[indexList[i].x];
						tempTextureIndx.y = texIndexList[indexList[i].y];
						tempTextureIndx.z = texIndexList[indexList[i].z];
						tempMesh.faceTexturelist.push_back(tempTextureIndx);
					}

					if (!normalIndexList.empty())
					{
						tempNIndx.x = normalIndexList[indexList[i].x];
						tempNIndx.y = normalIndexList[indexList[i].y];
						tempNIndx.z = normalIndexList[indexList[i].z];
						tempMesh.faceNormallist.push_back(tempNIndx);
					}
				}
			}
		}
	}
	// Deal with last mesh
	if (!tempMesh.facePoslist.empty())
	{
		// Create Mesh
		tempMesh.materialName = preMtlName;
		// Insert Mesh
		subMeshList.push_back(tempMesh);
	}

	file.close();

	// Set Materials for each Mesh
	for (int i = 0; i < subMeshList.size(); i++)
	{
		std::string matname = subMeshList[i].materialName;

		// Find corresponding material name in loaded materials
		// when found copy material variables into mesh material
		for (int j = 0; j < materialFile.materialList.size(); j++)
		{
			if (materialFile.materialList[j].name == matname)
			{
				subMeshList[i].material = &(materialFile.materialList[j]);
				break;
			}
		}
	}

	if (subMeshList.empty() && vertexList.empty() && normalList.empty())
	{
		return false;
	}
	else
	{
#ifdef ZJW_DEBUG
		cout << "finish load the obj!" << endl;
#endif // ZJW_DEUG

		//==================================
		/*getVertexPosMaxMin();
		printMaxMin();
		verPosNormalize();
		verNormalNormalize();
		printMaxMin();*/
		return true;
	}
}

bool ObjMeshMtl::trianglation(int size, vector<Vec3> &triangleFaceList)
{
	if (size < 3)
		return false;
	else
	{
		for (int i = 1; i < size - 1; i++)
		{
			triangleFaceList.push_back(Vec3(0, i, i + 1));
		}
		return true;
	}
}

void ObjMeshMtl::verPosNormalize(double rangeMin, double rangeMax)
{
	//拿到最大的max len,
	double lenX = maxX - minX;
	double lenY = maxY - minY;
	double lenZ = maxZ - minZ;

	double lenMax = lenX;
	if (lenMax < lenY)
		lenMax = lenY;
	if (lenMax < lenZ)
		lenMax = lenZ;

	//get scale
	double scale = lenMax / (rangeMax - rangeMin);

	//normalize
#pragma omp parallel for
	for (int v_it = 0; v_it < vertexList.size(); v_it++)
	{
		//先转换到0到（rangeMax - rangeMin）之间，然后加上偏移量
		vertexList[v_it].x = (vertexList[v_it].x - minX) / scale + rangeMin;
		vertexList[v_it].y = (vertexList[v_it].y - minY) / scale + rangeMin;
		vertexList[v_it].z = (vertexList[v_it].z - minZ) / scale + rangeMin;
	}

	//对最大值也同样进行归一化
	maxX = (maxX - minX) / scale + rangeMin;
	maxY = (maxY - minY) / scale + rangeMin;
	maxZ = (maxZ - minZ) / scale + rangeMin;
	minX = (minX - minX) / scale + rangeMin;
	minY = (minY - minY) / scale + rangeMin;
	minZ = (minZ - minZ) / scale + rangeMin;
}

void ObjMeshMtl::verNormalNormalize()
{
#pragma omp parallel for
	for (int n_it = 0; n_it < normalList.size(); n_it++)
	{
		//先转换到0到（rangeMax - rangeMin）之间，然后加上偏移量
		normalList[n_it].normalize();
	}
}

void ObjMeshMtl::getVertexPosMaxMin()
{
	maxX = (numeric_limits<int>::min)();
	maxY = (numeric_limits<int>::min)();
	maxZ = (numeric_limits<int>::min)();

	minX = (numeric_limits<double>::max)();
	minY = (numeric_limits<double>::max)();
	minZ = (numeric_limits<double>::max)();

	for (int i = 0; i < vertexList.size(); i++)
	{
		Vertex  vt = vertexList[i];
		if (vt.x > maxX)
			maxX = vt.x;
		if (vt.y > maxY)
			maxY = vt.y;
		if (vt.z > maxZ)
			maxZ = vt.z;

		if (vt.x < minX)
			minX = vt.x;
		if (vt.y < minY)
			minY = vt.y;
		if (vt.z < minZ)
			minZ = vt.z;
	}
}

void ObjMeshMtl::ratateNormal(double angleX, double angleY, double angleZ)
{
	//cout << "更新顶点的normal 法向量--没有实现" << endl;

	if (angleY == 0)
		return;
	//遍历所有的顶点的坐标，更新坐标
#pragma omp parallel for
	for (int i = 0; i < normalList.size(); i++)
	{
		double x, z;
		x = normalList[i].x * cos(angleY) + normalList[i].z * sin(angleY);
		z = -normalList[i].x * sin(angleY) + normalList[i].z * cos(angleY);
		normalList[i].x = x;
		normalList[i].z = z;
		normalList[i] = normalList[i] * (1.0f / sqrt(normalList[i].x * normalList[i].x + normalList[i].y * normalList[i].y + normalList[i].z * normalList[i].z));
	}
}

void ObjMeshMtl::rotateByAxle(double angleX, double angleY, double angleZ)
{
	//way1
	//rotateByAxleX(angleX);
	//rotateByAxleY(angleY);
	//rotateByAxleZ(angleZ);

	//way2
	getRotateMatX(angleX);
	rotate(RotateMat);

	getRotateMatY(angleY);
	rotate(RotateMat);

	getVertexPosMaxMin();
	verPosNormalize(rangeMin, rangeMax);
	//printMaxMin();
}

void ObjMeshMtl::rotateByAxleX(double angleX)
{
	if (angleX == 0)
		return;
	//遍历所有的顶点的坐标，更新坐标
#pragma omp parallel for
	for (int i = 0; i < vertexList.size(); i++)
	{
		double y, z;
		y = vertexList[i].y * cos(angleX) - vertexList[i].z * sin(angleX);
		z = vertexList[i].y * sin(angleX) + vertexList[i].z * cos(angleX);
		vertexList[i].y = y;
		vertexList[i].z = z;
	}
}

void ObjMeshMtl::rotateByAxleY(double angleY)
{
	if (angleY == 0)
		return;
	//遍历所有的顶点的坐标，更新坐标
#pragma omp parallel for
	for (int i = 0; i < vertexList.size(); i++)
	{
		double x, z;
		x = vertexList[i].x * cos(angleY) + vertexList[i].z * sin(angleY);
		z = -vertexList[i].x * sin(angleY) + vertexList[i].z * cos(angleY);
		vertexList[i].x = x;
		vertexList[i].z = z;
	}
}

void ObjMeshMtl::rotateByAxleZ(double angleZ)
{
	if (angleZ == 0)
		return;
	//遍历所有的顶点的坐标，更新坐标
#pragma omp parallel for
	for (int i = 0; i < vertexList.size(); i++)
	{
		double x, y;
		x = vertexList[i].x * cos(angleZ) - vertexList[i].y * sin(angleZ);
		y = vertexList[i].x * sin(angleZ) + vertexList[i].y * cos(angleZ);
		vertexList[i].x = x;
		vertexList[i].y = y;
	}
}

void ObjMeshMtl::updateFaceNormal()
{
	//#pragma omp parallel for
	//for (int i = 0; i < faceList.size(); i++)
	//{
	//	Face  face = faceList[i];
	//	if (face.vertexIndex.size() > 2) {
	//		//计算face的normal:  向量积
	//		Vertex& a = vertexList[face.vertexIndex[0]];
	//		Vertex& b = vertexList[face.vertexIndex[1]];
	//		Vertex& c = vertexList[face.vertexIndex[2]];
	//		//向量积计算法向量
	//		Normal tempNor(0, 0, 0);
	//		tempNor.x = (b - a).y * (c - b).z - (b - a).z * (c - b).y;
	//		tempNor.y = (b - a).z * (c - b).x - (b - a).x * (c - b).z;
	//		tempNor.z = (b - a).x * (c - b).y - (b - a).y * (c - b).x;
	//		//向量归一化
	//		Normal normal = tempNor * (1.0f / sqrt(tempNor.x * tempNor.x + tempNor.y *tempNor.y + tempNor.z * tempNor.z));
	//		face.normalForFace = normal;
	//	}
	//}
}

void ObjMeshMtl::getRotateMatX(double angleX)
{
	float cosine = cos(angleX), sine = sin(angleX);

	RotateMat[0][0] = 1;
	RotateMat[0][1] = 0;
	RotateMat[0][2] = 0;

	RotateMat[1][0] = 0;
	//RotateMat[1][1] = n2*n2*(1 - n2*n2)*cosine;
	RotateMat[1][1] = cosine;
	RotateMat[1][2] = -sine;

	RotateMat[2][0] = 0;
	RotateMat[2][1] = sine;
	RotateMat[2][2] = cosine;
}

void ObjMeshMtl::getRotateMatY(double angleY)
{
	float cosine = cos(angleY), sine = sin(angleY);

	RotateMat[0][0] = cosine;
	RotateMat[0][1] = 0;
	RotateMat[0][2] = sine;

	RotateMat[1][0] = 0;
	RotateMat[1][1] = 1;
	RotateMat[1][2] = 0;

	RotateMat[2][0] = -sine;
	RotateMat[2][1] = 0;
	RotateMat[2][2] = cosine;
}

void ObjMeshMtl::getRotateMatZ(double angleZ)
{
	float cosine = cos(angleZ), sine = sin(angleZ);

	RotateMat[0][0] = cosine;
	RotateMat[0][1] = -sine;
	RotateMat[0][2] = 0;

	RotateMat[1][0] = sine;
	//RotateMat[1][1] = n2*n2*(1 - n2*n2)*cosine;
	RotateMat[1][1] = cosine;
	RotateMat[1][2] = 0;

	RotateMat[2][0] = 0;
	RotateMat[2][1] = 0;
	RotateMat[2][2] = 1;
}

void ObjMeshMtl::rotate(float ** RotateMat)
{
	//	int vertex_num = vertexList.size();
	//#pragma omp parallel for
	//	for (int i = 0; i < vertex_num; ++i)
	//	{
	//		//Point3f* point = &vertexes[i].point;
	//		Point3 point;
	//		Point3 tmp_point = vertexList[i];
	//		//Point tmp_point = vertexList[i] - center_point;
	//		vertexList[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
	//		vertexList[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
	//		vertexList[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	//		//vertexList[i] += center_point;
	//	}
	//
	//	int normal_num = normalList.size();
	//#pragma omp parallel for
	//	for (int i = 0; i < normal_num; ++i)
	//	{
	//		Point3 tmp_point = normalList[i];
	//		normalList[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
	//		normalList[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
	//		normalList[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	//
	//	}
	//
	//	int face_num = faceList.size();
	//#pragma omp parallel for
	//	for (int i = 0; i < face_num; ++i)
	//	{
	//		Point3 tmp_point = faceList[i].normalForFace;
	//		faceList[i].normalForFace.x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
	//		faceList[i].normalForFace.y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
	//		faceList[i].normalForFace.z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	//	}
}

void ObjMeshMtl::printMaxMin()
{
	cout << "------------------------------" << endl;
	cout << "max xyz: " << maxX << " " << maxY << " " << maxZ << endl;
	cout << "min xyz: " << minX << " " << minY << " " << minZ << endl;
	cout << "------------------------------" << endl;
}

SubMesh::SubMesh()
{
	facePoslist.clear();
	materialName.clear();
	material = nullptr;
}

//==================================================================

ObjMesh::ObjMesh()
{
}

bool ObjMesh::loadObjMesh(string & path)
{
	// --------------------check--------------------------
	if (path.substr(path.size() - 4, 4) != ".obj")
		return false;

	std::ifstream file(path);

	if (!file.is_open())
	{
		cout << "obj file open failed!" << endl;
		return false;
	}

#ifdef ZJW_DEBUG
	cout << "starting load the obj: " << path << endl;
#endif // ZJW_DEUG

	//-----------------------init --------------------
	if (mesh.facePoslist.size())
		mesh.facePoslist.clear();
	if (mesh.faceNormallist.size())
		mesh.faceNormallist.clear();
	if (mesh.faceTexturelist.size())
		mesh.faceTexturelist.clear();

	if (vertexList.size())
		vertexList.clear();
	if (normalList.size())
		normalList.clear();
	if (texList.size())
		texList.clear();

	std::string curline;
	while (std::getline(file, curline))
	{
		// Generate a Vertex Position
		if (algorithm::firstToken(curline) == "v")
		{
			std::vector<std::string> spos;
			Point3 vpos;
			algorithm::split(algorithm::tail(curline), spos, " ");

			vpos.x = std::stof(spos[0]);
			vpos.y = std::stof(spos[1]);
			vpos.z = std::stof(spos[2]);
			vertexList.push_back(vpos);
		}
		// Generate a Vertex Texture Coordinate
		if (algorithm::firstToken(curline) == "vt")
		{
			std::vector<std::string> stex;
			Texture vtex;
			algorithm::split(algorithm::tail(curline), stex, " ");

			vtex.x = std::stof(stex[0]);
			vtex.y = std::stof(stex[1]);
			texList.push_back(vtex);
		}
		// Generate a Vertex Normal;
		if (algorithm::firstToken(curline) == "vn")
		{
			std::vector<std::string> snor;
			Normal vnor;
			algorithm::split(algorithm::tail(curline), snor, " ");

			vnor.x = std::stof(snor[0]);
			vnor.y = std::stof(snor[1]);
			vnor.z = std::stof(snor[2]);

			normalList.push_back(vnor);
		}
		// Generate a Face (vertices & indices)
		if (algorithm::firstToken(curline) == "f")
		{
			//因为这里的face不一定是三角形面片，先把顶点，纹理，法向量索引分开
			std::vector<int> verIndexList;
			std::vector<int> texIndexList;
			std::vector<int> normalIndexList;

			std::vector<std::string> sface, svert;
			algorithm::split(algorithm::tail(curline), sface, " ");

			bool noNormal = false;

			// For every given vertex do this
			for (int i = 0; i < int(sface.size()); i++)
			{
				// See What type the vertex is.
				int vtype;

				algorithm::split(sface[i], svert, "/");

				// Check for just position - v1
				if (svert.size() == 1)
				{
					// Only position
					vtype = 1;
				}

				// Check for position & texture - v1/vt1
				if (svert.size() == 2)
				{
					// Position & Texture
					vtype = 2;
				}

				// Check for Position, Texture and Normal - v1/vt1/vn1
				// or if Position and Normal - v1//vn1
				//v1//vn 根据 /分成了三个部分，第二个部分为空
				if (svert.size() == 3)
				{
					if (svert[1] != "")
					{
						// Position, Texture, and Normal
						vtype = 4;
					}
					else
					{
						// P//N

						// Position & Normal
						vtype = 3;
					}
				}

				// Calculate and store the vertex
				switch (vtype)
				{
				case 1: // P
				{
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					break;
				}
				case 2: // P/T
				{
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					texIndexList.push_back(std::stoi(svert[1]) - 1);
					break;
				}
				case 3: // P//N
				{
					//序号统一需要减一处理，因为在数组中是从0开始的
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					normalIndexList.push_back(std::stoi(svert[2]) - 1);
					break;
				}
				case 4: // P/T/N
				{
					//序号统一需要减一处理，因为在数组中是从0开始的
					verIndexList.push_back(std::stoi(svert[0]) - 1);
					texIndexList.push_back(std::stoi(svert[1]) - 1);
					normalIndexList.push_back(std::stoi(svert[2]) - 1);
					break;
				}
				default:
				{
					break;
				}
				}
			}

			//如果不是三角面片，那么采用三角化的方法
			Point3 tempPIndx;
			Point3 tempTextureIndx;
			Normal tempNIndx;

			if (verIndexList.size() == 3)
			{
				//保存三角面片信息到mesh中
				if (!verIndexList.empty())
				{
					tempPIndx.x = verIndexList[0];
					tempPIndx.y = verIndexList[1];
					tempPIndx.z = verIndexList[2];

					mesh.facePoslist.push_back(tempPIndx);
				}

				if (!texIndexList.empty())
				{
					tempTextureIndx.x = texIndexList[0];
					tempTextureIndx.y = texIndexList[1];
					tempTextureIndx.z = texIndexList[2];
					mesh.faceTexturelist.push_back(tempTextureIndx);
				}

				if (!normalIndexList.empty())
				{
					tempNIndx.x = normalIndexList[0];
					tempNIndx.y = normalIndexList[1];
					tempNIndx.z = normalIndexList[2];
					mesh.faceNormallist.push_back(tempNIndx);
				}
			}
			else if (verIndexList.size() < 3)
			{
				cout << "the f read error!!" << endl;
			}
			else {
				//进行三角化

				//每一个Point 存储序号
				vector<Point3> indexList;
				trianglation(verIndexList.size(), indexList);

				for (int i = 0; i < indexList.size(); i++)
				{
					if (!verIndexList.empty())
					{
						//保存信息到mesh中
						tempPIndx.x = verIndexList[indexList[i].x];
						tempPIndx.y = verIndexList[indexList[i].y];
						tempPIndx.z = verIndexList[indexList[i].z];

						mesh.facePoslist.push_back(tempPIndx);
					}

					if (!texIndexList.empty())
					{
						tempTextureIndx.x = texIndexList[indexList[i].x];
						tempTextureIndx.y = texIndexList[indexList[i].y];
						tempTextureIndx.z = texIndexList[indexList[i].z];
						mesh.faceTexturelist.push_back(tempTextureIndx);
					}

					if (!normalIndexList.empty())
					{
						tempNIndx.x = normalIndexList[indexList[i].x];
						tempNIndx.y = normalIndexList[indexList[i].y];
						tempNIndx.z = normalIndexList[indexList[i].z];

						mesh.faceNormallist.push_back(tempNIndx);
					}
				}
			}
		}
	}

#ifdef ZJW_DEBUG
	cout << "finish load the obj!" << endl;
#endif // ZJW_DEUG

	//==================================
	//获取其他信息
	getVertexPosMaxMin();
	printMaxMin();
	verPosNormalize(rangeMin, rangeMax);
	verNormalNormalize();
	printMaxMin();

	return true;
}
//针对特殊格式的objmesh   f: v//vn
bool ObjMesh::loadObjMeshSpeedUp(string & path)
{
	// --------------------check--------------------------
	if (path.substr(path.size() - 4, 4) != ".obj")
		return false;

	std::ifstream file(path);

	if (!file.is_open())
	{
		cout << "obj file open failed!" << endl;
		return false;
	}

#ifdef ZJW_DEBUG
	cout << "starting load the obj: " << path << endl;
#endif // ZJW_DEUG

	//-----------------------init --------------------
	mesh.facePoslist.clear();
	mesh.faceNormallist.clear();
	mesh.faceTexturelist.clear();

	vertexList.clear();
	normalList.clear();
	texList.clear();

	Point3 tempPIndx;
	Point3 tempTextureIndx;
	Normal tempNIndx;

	std::string curline;
	while (std::getline(file, curline))
	{
		// Generate a Vertex Position
		if (algorithm::firstToken(curline) == "v")
		{
			std::vector<std::string> spos;
			Point3 vpos;
			algorithm::split(algorithm::tail(curline), spos, " ");

			vpos.x = std::stof(spos[0]);
			vpos.y = std::stof(spos[1]);
			vpos.z = std::stof(spos[2]);

			vertexList.push_back(vpos);
		}
		// Generate a Vertex Normal;
		if (algorithm::firstToken(curline) == "vn")
		{
			std::vector<std::string> snor;
			Normal vnor;
			algorithm::split(algorithm::tail(curline), snor, " ");

			vnor.x = std::stof(snor[0]);
			vnor.y = std::stof(snor[1]);
			vnor.z = std::stof(snor[2]);

			normalList.push_back(vnor);
		}
		// Generate a Face (vertices & indices)
		if (algorithm::firstToken(curline) == "f")
		{
			//因为这里的face不一定是三角形面片，先把顶点，纹理，法向量索引分开
			std::vector<int> verIndexList;
			std::vector<int> texIndexList;
			std::vector<int> normalIndexList;

			std::vector<std::string> sface, svert;
			algorithm::split(algorithm::tail(curline), sface, " ");

			bool noNormal = false;

			// For every given vertex do this
			for (int i = 0; i < int(sface.size()); i++)
			{
				// See What type the vertex is.
				int vtype;

				algorithm::split(sface[i], svert, "/");

				//序号统一需要减一处理，因为在数组中是从0开始的
				verIndexList.push_back(std::stoi(svert[0]) - 1);
				normalIndexList.push_back(std::stoi(svert[2]) - 1);
			}

			tempPIndx.x = verIndexList[0];
			tempPIndx.y = verIndexList[1];
			tempPIndx.z = verIndexList[2];
			mesh.facePoslist.push_back(tempPIndx);

			tempNIndx.x = normalIndexList[0];
			tempNIndx.y = normalIndexList[1];
			tempNIndx.z = normalIndexList[2];
			mesh.faceNormallist.push_back(tempNIndx);
		}
	}

#ifdef ZJW_DEBUG
	cout << "finish load the obj!" << endl;
#endif // ZJW_DEUG

	//==================================
	//获取其他信息
	getVertexPosMaxMin();
	printMaxMin();
	verPosNormalize(rangeMin, rangeMax);
	verNormalNormalize();
	printMaxMin();
#ifdef ZJW_DEBUG
	cout << "finish load the obj!" << endl;
	cout << "==========================" << endl;
#endif // ZJW_DEUG

	return true;
}

bool ObjMesh::loadObjMeshSimply(string & path)
{
	fstream objFile;
	objFile.open(path, fstream::in | fstream::out | fstream::app);

	if (!objFile.is_open()) {
		cout << "open file failed!" << endl;
		return false;
	}

#ifdef ZJW_DEBUG
	cout << "starting load the obj: " << path << endl;
#endif // ZJW_DEUG

	//-----------------------init --------------------
	if (mesh.facePoslist.size())
		mesh.facePoslist.clear();
	if (mesh.faceNormallist.size())
		mesh.faceNormallist.clear();
	if (mesh.faceTexturelist.size())
		mesh.faceTexturelist.clear();

	if (vertexList.size())
		vertexList.clear();
	if (normalList.size())
		normalList.clear();
	if (texList.size())
		texList.clear();

	string type;
	while (objFile >> type)
	{
		if (type == "v")
		{
			Vertex v;
			objFile >> v.x;
			objFile >> v.y;
			objFile >> v.z;
			vertexList.push_back(v);
		}
		else if (type == "f")
		{
			Point3 faceVerIdx;
			objFile >> faceVerIdx.x;
			objFile >> faceVerIdx.y;
			objFile >> faceVerIdx.z;
			Point3 verIdx = faceVerIdx - 1.0;
			mesh.facePoslist.push_back(verIdx);

			////保存face的三个顶点的序号
			//Point3 faceVerIdx;
			//Point3 textureIndex;
			//Point3 normalIndex;
			//while (true)
			//{
			//	char ch = objFile.get();
			//	if (ch == ' ')
			//		continue;
			//	else if (ch == '\n' || ch == EOF)
			//		break;
			//	else
			//		objFile.putback(ch);
			//	//--------------------get 第一组-----------------
			//	objFile >> faceVerIdx.x;
			//	char splitter = objFile.get();
			//	normalIndex.x = 0;
			//	//如果是双斜杠，说明只有 vertex normal,如果是单斜杠
			//	//说明是 vertex texture normal.如果是空格或者其他只有vertex
			//	if (splitter == '/')
			//	{
			//		splitter = objFile.get();
			//		if (splitter == '/')
			//		{
			//			objFile >> normalIndex.x;
			//		}
			//		else
			//		{
			//			objFile.putback(splitter);
			//			objFile >> textureIndex.x;
			//			splitter = objFile.get();
			//			if (splitter == '/')
			//			{
			//				objFile >> normalIndex.x;
			//			}
			//			else
			//				objFile.putback(splitter);
			//		}
			//	}
			//	else
			//		objFile.putback(splitter);
			//	//--------------------get 第二组-----------------
			//	objFile >> faceVerIdx.y;
			//	splitter = objFile.get();
			//	//如果是双斜杠，说明只有 vertex normal,如果是单斜杠
			//	//说明是 vertex texture normal.如果是空格或者其他只有vertex
			//	if (splitter == '/')
			//	{
			//		splitter = objFile.get();
			//		if (splitter == '/')
			//		{
			//			objFile >> normalIndex.y;
			//		}
			//		else
			//		{
			//			objFile.putback(splitter);
			//			objFile >> textureIndex.y;
			//			splitter = objFile.get();
			//			if (splitter == '/')
			//			{
			//				objFile >> normalIndex.y;
			//			}
			//			else
			//				objFile.putback(splitter);
			//		}
			//	}
			//	else
			//		objFile.putback(splitter);
			//	//--------------------get 第三组-----------------
			//	objFile >> faceVerIdx.z;
			//		splitter = objFile.get();
			//		//如果是双斜杠，说明只有 vertex normal,如果是单斜杠
			//		//说明是 vertex texture normal.如果是空格或者其他只有vertex
			//		if (splitter == '/')
			//		{
			//			splitter = objFile.get();
			//			if (splitter == '/')
			//			{
			//				objFile >> normalIndex.z;
			//			}
			//			else
			//			{
			//				objFile.putback(splitter);
			//				objFile >> textureIndex.z;
			//				splitter = objFile.get();
			//				if (splitter == '/')
			//				{
			//					objFile >> normalIndex.x;
			//				}
			//				else
			//					objFile.putback(splitter);
			//			}
			//		}
			//		else
			//			objFile.putback(splitter);
			//	}
			//	Point3 verIdx = faceVerIdx - 1.0;
			//	mesh.facePoslist.push_back(verIdx);
			//	//if (face.vertexIndex.size() > 2) {
			//	//	//计算face的normal:  向量积
			//	//	Vertex& a = vertexList[face.vertexIndex[0]];
			//	//	Vertex& b = vertexList[face.vertexIndex[1]];
			//	//	Vertex& c = vertexList[face.vertexIndex[2]];
			//	//	//向量积计算法向量
			//	//	Normal tempNor(0, 0, 0);
			//	//	tempNor.x = (b - a).y * (c - b).z - (b - a).z * (c - b).y;
			//	//	tempNor.y = (b - a).z * (c - b).x - (b - a).x * (c - b).z;
			//	//	tempNor.z = (b - a).x * (c - b).y - (b - a).y * (c - b).x;
			//	//	//向量归一化
			//	//	Normal normal = tempNor * (1.0f / sqrt(tempNor.x * tempNor.x + tempNor.y *tempNor.y + tempNor.z * tempNor.z));
			//	//	face.normalForFace = normal;
			//	//	faceList.push_back(face);
			//}
		}
		else if (type == "vn")
		{
			Normal vn;
			objFile >> vn.x >> vn.y >> vn.z;
			normalList.push_back(vn);
		}
		else if (type == "vt")
		{
			Vertex vt;
			objFile >> vt.x;
			objFile >> vt.y;
			objFile >> vt.z;
			texList.push_back(vt);
		}
		
	}
	objFile.close();
#ifdef ZJW_DEBUG
	cout << "finish load the obj!" << endl;
#endif // ZJW_DEUG

	//==================================
	//获取其他信息
	getVertexPosMaxMin();
	printMaxMin();
	verPosNormalize(rangeMin, rangeMax);
	verNormalNormalize();
	printMaxMin();
	return true;
}

bool ObjMesh::trianglation(int size, vector<Vec3> &triangleFaceList)
{
	if (size < 3)
		return false;
	else
	{
		for (int i = 1; i < size - 1; i++)
		{
			triangleFaceList.push_back(Vec3(0, i, i + 1));
		}
		return true;
	}
}

void ObjMesh::getVertexPosMaxMin()
{
	maxX = (numeric_limits<int>::min)();
	maxY = (numeric_limits<int>::min)();
	maxZ = (numeric_limits<int>::min)();

	minX = (numeric_limits<double>::max)();
	minY = (numeric_limits<double>::max)();
	minZ = (numeric_limits<double>::max)();

	for (int i = 0; i < vertexList.size(); i++)
	{
		Vertex  vt = vertexList[i];
		if (vt.x > maxX)
			maxX = vt.x;
		if (vt.y > maxY)
			maxY = vt.y;
		if (vt.z > maxZ)
			maxZ = vt.z;

		if (vt.x < minX)
			minX = vt.x;
		if (vt.y < minY)
			minY = vt.y;
		if (vt.z < minZ)
			minZ = vt.z;
	}
}

void ObjMesh::verPosNormalize(double rangeMin, double rangeMax)
{
	//拿到最大的max len,
	double lenX = maxX - minX;
	double lenY = maxY - minY;
	double lenZ = maxZ - minZ;

	double lenMax = lenX;
	if (lenMax < lenY)
		lenMax = lenY;
	if (lenMax < lenZ)
		lenMax = lenZ;

	//get scale
	double scale = lenMax / (rangeMax - rangeMin);

	//normalize
#pragma omp parallel for
	for (int v_it = 0; v_it < vertexList.size(); v_it++)
	{
		//先转换到0到（rangeMax - rangeMin）之间，然后加上偏移量
		vertexList[v_it].x = (vertexList[v_it].x - minX) / scale + rangeMin;
		vertexList[v_it].y = (vertexList[v_it].y - minY) / scale + rangeMin;
		vertexList[v_it].z = (vertexList[v_it].z - minZ) / scale + rangeMin;
	}

	//对最大值也同样进行归一化
	maxX = (maxX - minX) / scale + rangeMin;
	maxY = (maxY - minY) / scale + rangeMin;
	maxZ = (maxZ - minZ) / scale + rangeMin;
	minX = (minX - minX) / scale + rangeMin;
	minY = (minY - minY) / scale + rangeMin;
	minZ = (minZ - minZ) / scale + rangeMin;
}

void ObjMesh::verNormalNormalize()
{
#pragma omp parallel for
	for (int n_it = 0; n_it < normalList.size(); n_it++)
	{
		//先转换到0到（rangeMax - rangeMin）之间，然后加上偏移量
		normalList[n_it].normalize();
	}
}

void ObjMesh::printMaxMin()
{
	cout << "------------------------------" << endl;
	cout << "max xyz: " << maxX << " " << maxY << " " << maxZ << endl;
	cout << "min xyz: " << minX << " " << minY << " " << minZ << endl;
	cout << "------------------------------" << endl;
}