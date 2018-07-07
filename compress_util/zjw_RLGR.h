/*
* author: Jiawei Zhou
* email: zjwking258@163.com
* time: 2018/7/4
* head file: Golomb coding��ʵ��(Golomb Rice  ��ʵ�� m��2��ָ��)
*/

#pragma once
#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <assert.h>
#include <cmath>
#include <vector>
#include "zjw_bitFile.h"

enum RunType
{
	CompleteRun,
	PartialRun
};

class RLGR
{
private:
	
	//�ж��Ƿ�run length��k
	uint64_t k;
	uint64_t u0;
	uint64_t d0;
	uint64_t u1;
	uint64_t d1;
	uint64_t kP;


	//kR��GR�е�k. kR = kRP/L;
	uint64_t kR;
	//������2��pow
	uint64_t L;
	uint64_t kRP;

	//run������
	RunType type;

public:
	std::string fileName;

	//m = pow(2, kR);
	uint64_t m;
	uint64_t p;
	
	//��Ҫ���б��������
	vector<uint64_t>* codeData;

	//��Ҫ���н���Ľ��
	vector<uint64_t> *resData;

	//��Ҫ�����symbol
	uint64_t u;

	BitReadFile  *bitReadFile;
	BitWriteFile *bitWriteFile;

public:
	RLGR(vector<uint64_t> *codeData, vector<uint64_t> *resData ,std::string fileName = "golombRicd.gr");
	~RLGR();

	void initParam();

	//RLGR���롣ѹ����֮�󣬱�����ѹ���ļ���
	void encode();

	//��ѹ���ļ��У�RLGR���롣���浽resData��
	bool decode();

	void setK(uint64_t k_);

private:

	//ÿһ��GR����֮��p�и����ˣ�Ȼ��������������p������kR��
	void updateKR();

	//����u,�����ж��Ƿ�run length��k
	void updateK();


	//����Golomb Rice �� num���б��룬����ֵ�Ǳ�������Ԫ���ȡ�
	//use it must call  open close func in class bitWriteFile .
	bool rice_golombEncode(uint64_t num);

	//encode ����m��0
	bool encodeOneZero();

	//encode ����n��0�������һ����0����,���ķ�0������
	bool encodeStringWithZeros(int zeroNum , uint64_t &u);

	//���ص���false�Լ����������ˡ�num�ǽ���������ֵ��
	//use it must call  open close func in class bitWriteFile .
	bool rice_golombDecode(uint64_t & num);

	//�ж���һ��bit�ǲ���zero.��0����true,������false
	bool nextBitIsZero();
};