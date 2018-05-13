/*!
 * \file zjw_obj.h
 *
 * \author zhoujiawei
 * \date ���� 2018
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
	vector<Point3> facePoslist;//�洢ÿ��face�������������
	vector<Normal> faceNormallist;//�洢ÿ��face���������������
	vector<Point3> faceTexturelist;
};

//��Ҫ�洢����obj��ʽ�е��Ӷ��󣬱���ؼ����� g��
class SubMesh
{
public:
	vector<Point3> facePoslist;//�洢ÿ��face�������������
	vector<Normal> faceNormallist;//�洢ÿ��face���������������
	vector<Point3> faceTexturelist;
	std::string gName; //g��������֣�һ��g�����ɶ��usemtl���
	std::string materialName; //usemtl���������
	Material *material;

	SubMesh();
};

//֧��mtl
class ObjMeshMtl
{
public:
	//------------------------------support material --------------------

	//obj��ÿһ�� material name ����һ���µ�mesh
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
	//�õ�vertex�����ֵ����Сֵ
	void getVertexPosMaxMin();

	//�Ѷ��������һ��rangeMin ��rangeMax֮��ķ�Χ (-0.5 �� 0.5�ķ�Χ)
	void verPosNormalize(double rangeMin = -0.5, double rangeMax = 0.5);

	//���������Ĺ�һ��
	void verNormalNormalize();

	//�Զ��㣬���㷨�򣬶����������ת
	void ratateNormal(double angleX = 0, double angleY = 0, double angleZ = 0);
	void rotateByAxle(double angleX = 0, double angleY = 0, double angleZ = 0);
	void rotateByAxleX(double angleX = 0);
	void rotateByAxleY(double angleY = 0);
	void rotateByAxleZ(double angleZ = 0);
	void updateFaceNormal();

	//��ת����2
	void getRotateMatX(double angleX = 0);
	void getRotateMatY(double angleY = 0);
	void getRotateMatZ(double angleZ = 0);
	void rotate(float** RotateMat);

	//-----------test-----------------
	void printMaxMin();
};

//��ͨ��Mesh����֧��Obj�е�g�����ȣ����ܻ���֧�ֲ���
class ObjMesh
{
public:
	//mesh ������Ƭ��Ϣ
	Mesh mesh;
	vector<Vertex> vertexList;
	vector<Color> colorList;
	vector<Normal> normalList;
	vector<Texture> texList;

	int faceNum;
	int vertexNum;
	double maxX, maxY, maxZ;
	double minX, minY, minZ;
	//��һ���ķ�Χ
	double rangeMin = 0;
	double rangeMax = 1;

	float** RotateMat;

public:
	ObjMesh();
	//����obj������ӿڱȽ�ȫ�棬��Ӧ��ȽϹ㡣֧�ָ���f���͵Ĳ���
	bool loadObjMesh(string & path);
	//loadObjMesh���ϼ򻯳����ģ���������ʽ��objmesh   f: v//vn
	bool loadObjMeshSpeedUp(string & path);
	//�ǳ����ٵ����,ֻ��v  �򵥵�����f�����
	bool loadObjMeshSimply(string & path);

	//���obj��û��color ,��ôĬ�����color��Ϣ 0.752941(obj��Ĭ�ϵĻ�ɫֵ)
	bool fillColorInfo();

	bool trianglation(int size, vector<Vec3> &trangleFaceIndx);
	//�õ�vertex�����ֵ����Сֵ
	void getVertexPosMaxMin();
	//�Ѷ��������һ��rangeMin ��rangeMax֮��ķ�Χ (-0.5 �� 0.5�ķ�Χ)
	void verPosNormalize(double rangeMin = -0.5, double rangeMax = 0.5);
	//���������Ĺ�һ��
	void verNormalNormalize();

	void printMaxMin();
};