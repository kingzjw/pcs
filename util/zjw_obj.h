/*!
 * \file zjw_obj.h
 *
 * \author zhoujiawei
 * \date 三月 2018
 * create by myself for reuse
 *
 */
#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <assert.h>
#include <limits>
#include "zjw_math.h"
#include "zjw_material.h"
#include "zjw_macro.h"

using namespace std;

typedef Vec3 Point3;
typedef Vec3 Vertex;
typedef Vec3 Normal;
typedef Vec2 Texture;
typedef Vec3 Color;

class Mesh
{
public:
	vector<Point3> facePoslist;//存储每个face的三个顶点序号
	vector<Normal> faceNormallist;//存储每个face的三个法向量序号
	vector<Point3> faceTexturelist;
};

//主要存储的是obj格式中的子对象，比如关键词是 g的
class SubMesh
{
public:
	vector<Point3> facePoslist;//存储每个face的三个顶点序号
	vector<Normal> faceNormallist;//存储每个face的三个法向量序号
	vector<Point3> faceTexturelist;
	std::string gName; //g后面的名字，一个g本质由多个usemtl组成
	std::string materialName; //usemtl后面的名字
	Material *material;

	SubMesh();
};

//支持mtl
class ObjMeshMtl
{
public:
	//------------------------------support material --------------------

	//obj中每一个 material name 就是一个新的mesh
	vector<SubMesh> subMeshList;
	vector<Vertex> vertexList;
	vector<Normal> normalList;
	vector<Texture> texList;
	MaterialFile  materialFile;

	//--------------------------end--------------------------

	int faceNum;
	int vertexNum;
	//vector<Face> faceList;
	double maxX, maxY, maxZ;
	double minX, minY, minZ;
	double rangeMin = 0;
	double rangeMax = 1;

	float** RotateMat;

public:
	ObjMeshMtl();

	//------------------------------support material --------------------
	bool loadObjMeshAndMtl(string & path);
	bool trianglation(int size, vector<Vec3> &trangleFaceIndx);

	//-------------------- end ------------------
	//拿到vertex中最大值和最小值
	void getVertexPosMaxMin();

	//把顶点坐标归一化rangeMin 到rangeMax之间的范围 (-0.5 到 0.5的范围)
	void verPosNormalize(double rangeMin = -0.5, double rangeMax = 0.5);

	//顶点向量的归一化
	void verNormalNormalize();

	//对顶点，顶点法向，顶点面进行旋转
	void ratateNormal(double angleX = 0, double angleY = 0, double angleZ = 0);
	void rotateByAxle(double angleX = 0, double angleY = 0, double angleZ = 0);
	void rotateByAxleX(double angleX = 0);
	void rotateByAxleY(double angleY = 0);
	void rotateByAxleZ(double angleZ = 0);
	void updateFaceNormal();

	//旋转方法2
	void getRotateMatX(double angleX = 0);
	void getRotateMatY(double angleY = 0);
	void getRotateMatZ(double angleZ = 0);
	void rotate(float** RotateMat);

	//-----------test-----------------
	void printMaxMin();
};

//普通的Mesh，不支持Obj中的g参数等，可能还不支持材质
class ObjMesh
{
public:
	//mesh 保存面片信息
	Mesh mesh;
	vector<Vertex> vertexList;
	vector<Color> colorList;
	vector<Normal> normalList;
	vector<Texture> texList;

	int faceNum;
	int vertexNum;
	double maxX, maxY, maxZ;
	double minX, minY, minZ;
	//归一化的范围
	double rangeMin = 0;
	double rangeMax = 1;

	float** RotateMat;

public:
	ObjMesh();
	//读入obj，这个接口比较全面，适应面比较广。支持各种f类型的参数
	bool loadObjMesh(string & path);
	//loadObjMesh，上简化出来的，针对特殊格式的objmesh   f: v//vn
	bool loadObjMeshSpeedUp(string & path);
	//非常快速的针对,只有v  简单的三种f的情况
	bool loadObjMeshSimply(string & path);

	//如果obj中没有color ,那么默认填充color信息 0.752941(obj中默认的灰色值)
	bool fillColorInfo();

	bool trianglation(int size, vector<Vec3> &trangleFaceIndx);
	//拿到vertex中最大值和最小值
	void getVertexPosMaxMin();
	//把顶点坐标归一化rangeMin 到rangeMax之间的范围 (-0.5 到 0.5的范围)
	void verPosNormalize(double rangeMin = -0.5, double rangeMax = 0.5);
	//顶点向量的归一化
	void verNormalNormalize();

	void printMaxMin();
};