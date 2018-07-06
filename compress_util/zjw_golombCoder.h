/*
* author: Jiawei Zhou
* time: 2018/7/3
* head file: Golomb coding的实现(Golomb Rice  的实现 m是2的指数)
*/

#pragma once
#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <assert.h>
#include <vector>
#include "zjw_bitFile.h"



class GolombCoder
{
private:
	string fileName;
	uint64_t m;
	uint64_t k;

public:
	BitReadFile  *bitReadFile;
	BitWriteFile *bitWriteFile;
	//需要进行编码的数据
	vector<uint64_t>* codeData;
	//需要进行解码的结果
	vector<uint64_t> *resData;

public:
	//传递K值
	GolombCoder(vector<uint64_t> *codeData, vector<uint64_t> *resData, 
		uint64_t k, string fileName="golombRicd.gr");
	~GolombCoder();

	//对num进行Golomb Rice的编码
	void encode();

	//解析出来的返回值：num,返回false表示以及解析到文件末尾。
	bool decode();

	void setK(uint64_t k_);

private:

	//利用Golomb Rice 对 num进行编码，返回值是编码后的码元长度。
	//use it must call  open close func in class bitWriteFile .
	void rice_golombEncode(uint64_t num);

	//Exp Golomb
	void exp_golombEncode(uint64_t num);

	//返回的是false以及解析结束了。num是解析出来的值。
	//use it must call  open close func in class bitWriteFile .
	bool rice_golombDecode(uint64_t & num);

	//Exp Golomb
	uint64_t exp_golombDecode();
};