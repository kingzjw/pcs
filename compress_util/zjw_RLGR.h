/*
* author: Jiawei Zhou
* email: zjwking258@163.com
* time: 2018/7/4
* head file: Golomb coding的实现(Golomb Rice  的实现 m是2的指数)
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
	
	//判断是否run length的k
	uint64_t k;
	uint64_t u0;
	uint64_t d0;
	uint64_t u1;
	uint64_t d1;
	uint64_t kP;


	//kR是GR中的k. kR = kRP/L;
	uint64_t kR;
	//必须是2的pow
	uint64_t L;
	uint64_t kRP;

	//run的类型
	RunType type;

public:
	std::string fileName;

	//m = pow(2, kR);
	uint64_t m;
	uint64_t p;
	
	//需要进行编码的数据
	vector<uint64_t>* codeData;

	//需要进行解码的结果
	vector<uint64_t> *resData;

	//需要编码的symbol
	uint64_t u;

	BitReadFile  *bitReadFile;
	BitWriteFile *bitWriteFile;

public:
	RLGR(vector<uint64_t> *codeData, vector<uint64_t> *resData ,std::string fileName = "golombRicd.gr");
	~RLGR();

	void initParam();

	//RLGR编码。压缩包之后，保存在压缩文件中
	void encode();

	//从压缩文件中，RLGR解码。保存到resData中
	bool decode();

	void setK(uint64_t k_);

private:

	//每一次GR编码之后，p有更新了，然后更新这个。根据p来更新kR。
	void updateKR();

	//根据u,更新判断是否run length的k
	void updateK();


	//利用Golomb Rice 对 num进行编码，返回值是编码后的码元长度。
	//use it must call  open close func in class bitWriteFile .
	bool rice_golombEncode(uint64_t num);

	//encode 连续m个0
	bool encodeOneZero();

	//encode 连续n个0，最后是一个非0数字,最后的非0数字是
	bool encodeStringWithZeros(int zeroNum , uint64_t &u);

	//返回的是false以及解析结束了。num是解析出来的值。
	//use it must call  open close func in class bitWriteFile .
	bool rice_golombDecode(uint64_t & num);

	//判断下一个bit是不是zero.是0返回true,否则是false
	bool nextBitIsZero();
};