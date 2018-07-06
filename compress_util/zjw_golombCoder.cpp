#include "zjw_golombCoder.h"

GolombCoder::GolombCoder(vector<uint64_t> *codeData, vector<uint64_t> *resData, uint64_t k, string fileName)
{
	this->codeData = codeData;
	this->resData = resData;

	this->k = k;
	//Golomb Rice 
	m = pow(2, k);

	this->fileName = fileName;
	bitReadFile = new BitReadFile(fileName);
	bitWriteFile = new BitWriteFile(fileName);

}

GolombCoder::~GolombCoder()
{
	if (bitReadFile)
		delete bitReadFile;
	if(bitWriteFile)
		delete bitWriteFile;
}

void  GolombCoder::encode()
{
	bitWriteFile->open();

#define COMPRESS_NUM_TO_FILE
#ifdef COMPRESS_NUM_TO_FILE
	//把数据的个数利用GR coder压缩到文件的第一个数据。
	uint64_t totalNum = codeData->size();
	//设置GR coder中的k,这是解压和压缩两边约定好的。要改就只能一起改了。
	int temp = k;
	k = 10;
	m = pow(2, k);
	rice_golombEncode(totalNum);
	cout << "start to encode the data ( num: " << totalNum<< " )" << endl;
#endif

	//恢复默认的k
	k = temp;
	m = pow(2, k);
	for (int i = 0; i < codeData->size(); i++)
	{
		rice_golombEncode((*codeData)[i]);
	}

	cout << "finish  encode the data !!!" << endl;
	bitWriteFile->close();
}

bool GolombCoder::decode()
{
	resData->clear();
	bitReadFile->open();

#define COMPRESS_NUM_TO_FILE
#ifdef COMPRESS_NUM_TO_FILE
	//设置GR coder中的k,这是解压和压缩两边约定好的。要改就只能一起改了。
	int t = k;
	k = 10;
	m = pow(2, k);

	//得到后面还有多少个数据(GR coder中的decoder),包括文件结束符。
	uint64_t totalNum = 0;
	rice_golombDecode(totalNum);
	cout << "start parse the coded file (num: " << totalNum << ")" << endl;
#endif
	//恢复默认的k值
	k = t;
	m = pow(2, k);

	uint64_t temp;
	for (int i = 0; i < totalNum; i++)
	{
		rice_golombDecode(temp);
		resData->push_back(temp);
	}
	cout << "finish  encode the data !!!" << endl;
	bitReadFile->close();
	return true;
}

void GolombCoder::setK(uint64_t k_)
{
	k = k_;
	m = pow(2, k);
}

void GolombCoder::rice_golombEncode(uint64_t num)
{
	
	//右移，相当于 q = num/ (x^k)
	uint64_t q = num >> k;
	//相当于 r = num - q * (x^k)
	uint64_t r = num & (m - 1);

	// 编码后码元的长度
	auto len = q + 1 + k;

	//不会判断缓存是否为满，直接向里面放，不足的话缓存到bit buffer中
	bitWriteFile->putBit(1, (int)q);
	bitWriteFile->putBit(0);
	
	//余数是反着来存的。
	for (int i = 0; i < k; i++)
	{
		//判断r的最低一位，如果是1，那么putBit true,如果是0 ,那么putBit false;
		bitWriteFile->putBit(static_cast<bool>(r & 0x01));
		//r右移一位。
		r >>= 1;
	}
}

void GolombCoder::exp_golombEncode(uint64_t num)
{
	if (k != 0)
		num = num + pow(2, k) - 1;

	// 0阶exp-golom编码
	auto m = static_cast<int>(log(num + 1) / log(2)); // 前缀，也即是组号
	auto info = static_cast<int>(num + 1 - pow(2, m)); // 组内的偏移量

	auto len = 2 * m + 1; // 编码后码元的长度

						  // 写入m个0
	bitWriteFile->putBit(0, m);
	// 写入1
	bitWriteFile->putBit(1);

	// 写入info m-1 bits
	for (int i = 0; i < m; i++)
	{
		bitWriteFile->putBit(static_cast<bool>(info & 0x01));
		info >>= 1;
	}
}

 bool GolombCoder::rice_golombDecode(uint64_t & num)
{
	/*if (bitReadFile->checkTerminator())
		return false;*/

	bool b;
	uint64_t unary = 0;
	bitReadFile->getBit(b);
	while (b)
	{
		unary++;
		bitReadFile->getBit(b);
	}

	std::bitset<64> bits;
	bits.reset();

	//余数是先存低位的，所以解析出来也是从低位开始解析
	for (int i = 0; i < k; i++)
	{
		bitReadFile->getBit(b);
		bits.set(i, b);
	}

	num = unary * m + bits.to_ulong();

	return true;
}

uint64_t GolombCoder::exp_golombDecode()
{
	bool b;
	uint64_t groupID = 0;

	// group id 是0的个数，直到1
	bitReadFile->getBit(b);
	while (!b)
	{
		groupID++;
		//b = bitStream.getBit();
		bitReadFile->getBit(b);

	}

	// 解码组内的偏移量
	// 读取groupID-1个bit
	std::bitset<64> bits;
	bits.reset();
	for (int i = 0; i < groupID; i++)
	{
		//b = bitStream.getBit();
		bitReadFile->getBit(b);
		bits.set(i, b);
	}

	uint64_t num = pow(2, groupID) - 1 + bits.to_ulong();
	//判断是否是0阶exp，如果是则直接返回结果
	if (k != 0)
		num = num - pow(2, k) + 1;
	return num;
}

