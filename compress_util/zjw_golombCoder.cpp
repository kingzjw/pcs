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
	//�����ݵĸ�������GR coderѹ�����ļ��ĵ�һ�����ݡ�
	uint64_t totalNum = codeData->size();
	//����GR coder�е�k,���ǽ�ѹ��ѹ������Լ���õġ�Ҫ�ľ�ֻ��һ����ˡ�
	int temp = k;
	k = 10;
	m = pow(2, k);
	rice_golombEncode(totalNum);
	cout << "start to encode the data ( num: " << totalNum<< " )" << endl;
#endif

	//�ָ�Ĭ�ϵ�k
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
	//����GR coder�е�k,���ǽ�ѹ��ѹ������Լ���õġ�Ҫ�ľ�ֻ��һ����ˡ�
	int t = k;
	k = 10;
	m = pow(2, k);

	//�õ����滹�ж��ٸ�����(GR coder�е�decoder),�����ļ���������
	uint64_t totalNum = 0;
	rice_golombDecode(totalNum);
	cout << "start parse the coded file (num: " << totalNum << ")" << endl;
#endif
	//�ָ�Ĭ�ϵ�kֵ
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
	
	//���ƣ��൱�� q = num/ (x^k)
	uint64_t q = num >> k;
	//�൱�� r = num - q * (x^k)
	uint64_t r = num & (m - 1);

	// �������Ԫ�ĳ���
	auto len = q + 1 + k;

	//�����жϻ����Ƿ�Ϊ����ֱ��������ţ�����Ļ����浽bit buffer��
	bitWriteFile->putBit(1, (int)q);
	bitWriteFile->putBit(0);
	
	//�����Ƿ�������ġ�
	for (int i = 0; i < k; i++)
	{
		//�ж�r�����һλ�������1����ôputBit true,�����0 ,��ôputBit false;
		bitWriteFile->putBit(static_cast<bool>(r & 0x01));
		//r����һλ��
		r >>= 1;
	}
}

void GolombCoder::exp_golombEncode(uint64_t num)
{
	if (k != 0)
		num = num + pow(2, k) - 1;

	// 0��exp-golom����
	auto m = static_cast<int>(log(num + 1) / log(2)); // ǰ׺��Ҳ�������
	auto info = static_cast<int>(num + 1 - pow(2, m)); // ���ڵ�ƫ����

	auto len = 2 * m + 1; // �������Ԫ�ĳ���

						  // д��m��0
	bitWriteFile->putBit(0, m);
	// д��1
	bitWriteFile->putBit(1);

	// д��info m-1 bits
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

	//�������ȴ��λ�ģ����Խ�������Ҳ�Ǵӵ�λ��ʼ����
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

	// group id ��0�ĸ�����ֱ��1
	bitReadFile->getBit(b);
	while (!b)
	{
		groupID++;
		//b = bitStream.getBit();
		bitReadFile->getBit(b);

	}

	// �������ڵ�ƫ����
	// ��ȡgroupID-1��bit
	std::bitset<64> bits;
	bits.reset();
	for (int i = 0; i < groupID; i++)
	{
		//b = bitStream.getBit();
		bitReadFile->getBit(b);
		bits.set(i, b);
	}

	uint64_t num = pow(2, groupID) - 1 + bits.to_ulong();
	//�ж��Ƿ���0��exp���������ֱ�ӷ��ؽ��
	if (k != 0)
		num = num - pow(2, k) + 1;
	return num;
}

