#include "zjw_RLGR.h"


RLGR::RLGR(vector<uint64_t> *codeData, vector<uint64_t> * resData,std::string fileName)
{

	this->codeData = codeData;
	this->resData = resData;
	
	initParam();

	this->type = type;
	this->fileName = fileName;
	bitReadFile = new BitReadFile(fileName);
	bitWriteFile = new BitWriteFile(fileName);
}

RLGR::~RLGR()
{
	if (bitReadFile)
		delete bitReadFile;
	if (bitWriteFile)
		delete bitWriteFile;
}

void RLGR::initParam()
{
	resData->clear();

	//�ж��Ƿ�run length��k
	k = 0;

	//default, Lһ����2��pow
	L = 4;
	kRP = 16;

	//kR = kRP/L;
	kR = kRP >> ((int)log2(L));
	//Golomb Rice 
	m = pow(2, kR);

	//�ĸ�����
	u0 = 3;
	d0 = 1;
	u1 = 2;
	d1 = 1;
	kP = 16;
}

void RLGR::encode()
{
	//open
	bitWriteFile->open();

	//��������ĩβ����һ����0�ķ��ţ����ڴ����ļ�ĩβ�Ķ��0�������
	codeData->push_back(10);
	
#define COMPRESS_NUM_TO_FILE
#ifdef COMPRESS_NUM_TO_FILE
	//�����ݵĸ�������GR coderѹ�����ļ��ĵ�һ�����ݡ�
	uint64_t totalNum = codeData->size();
	//����GR coder�е�k,���ǽ�ѹ��ѹ������Լ���õġ�Ҫ�ľ�ֻ��һ����ˡ�
	kR = 10;
	rice_golombEncode(totalNum);
	cout << "start to encode the data ( num: " << totalNum - 1 << " )" << endl;
#endif

	//kR��ֵ���ʼ��
	initParam();

	//��ÿһ�����ֽ��б���
	for (int num_it = 0; num_it < codeData->size(); num_it++)
	{
		//test
		if (num_it == 302)
			int asdfa = 1234;

		cout << "num: " << num_it << endl;
		//end test

		u = (*codeData)[num_it];
		if (k == 0)
		{
			//no run model  GR�ķ�ʽcode.
			rice_golombEncode(u);
			updateKR();
			updateK();
		}
		else if (k > 0)
		{
			//bool fileEnd = false;
			//run model 
			if (u == 0)// && ++num_it < codeData->size())
			{
				//u����0�����滹������

				//��ԭnum_it
				/*num_it--;

				if (++num_it >= codeData->size())
				{
					fileEnd = true;
					num_it--;
				}
				*/


				u = (*codeData)[++num_it];
				int uIsZero = 1;
				while (u == 0 && uIsZero < m)
				{
					uIsZero++;
					u = (*codeData)[++num_it];
				}

				if (uIsZero == m)
				{
					//���ֵ�����Ϊm��0000000������Ϊ0
					//�Ѷ�ȡ���Ǹ������˻�ȥ
					num_it--;
					//encode Ϊ0
					encodeOneZero();

					//����K��ʱ��ͬģʽ
					type = CompleteRun;

					//update
					updateKR();
					updateK();

					continue;
				}

				//else if (u == 0 && ++num_it > codeData->size())
				//{
				//	//u����0�������Ѿ�û�������ˡ��ﵽ�ļ���ĩβ����ô����Partion run�����д���
				//	uIsZero = 0;
				//	//���ֵ�����ΪuIsZero��0000000 ����һ��u������Ϊ0
				//	encodeStringWithZeros(uIsZero, u);
				//	//����K��ʱ��ͬģʽ
				//	type = PartialRun;
				//	updateKR();
				//	updateK();
				//}

				else if (u != 0)
				{
					//���ֵ�����ΪuIsZero��0000000 ����һ��u������Ϊ0
					encodeStringWithZeros(uIsZero, u);

					//����K��ʱ��ͬģʽ
					type = PartialRun;

					updateKR();
					updateK();
				}
				else
				{
					//u= 0����������ǿ��ܳ�������󼸸������ϵģ�0�ĸ��������������
					assert(0);
					//��Ҫ�����������
					//add something
				}
			}
			else if (u > 0)
			{
				// run model ��������������0�����֣�ǰ��û��0��
				//rice_golombEncode(u);

				//���� ǰ����0��0��
				encodeStringWithZeros(0, u);

				//����K��ʱ��ͬģʽ
				type = PartialRun;

				updateKR();
				updateK();
			}
		}
		else
		{
			assert(0);
		}
	}
	//ɾ���ļ���������
	codeData->erase(codeData->begin() + codeData->size() - 1);
	//�ر�
	cout << "finish  encode the data !!!"<< endl;
	bitWriteFile->close();


}

bool RLGR::decode()
{

	bitReadFile->open();

#define COMPRESS_NUM_TO_FILE
#ifdef COMPRESS_NUM_TO_FILE
	//����GR coder�е�k,���ǽ�ѹ��ѹ������Լ���õġ�Ҫ�ľ�ֻ��һ����ˡ�
	kR = 10;
	//�õ����滹�ж��ٸ�����(GR coder�е�decoder),�����ļ���������
	uint64_t totalNum =0;
	rice_golombDecode(totalNum);
	cout << "start parse the coded file (num: " << totalNum-1 << ")" << endl;
#endif

	//RLGR�Ĳ�������ʼ
	initParam();
	while (resData->size() < totalNum)
	{
		if (k == 0)
		{
			//�ú���������������p
			rice_golombDecode(u);
			//saveData
			resData->push_back(u);

			//�������decodeNumҲҪ����ԭ���Ǹ� kR��k��update
			updateKR();
			//u = decodeNum;
			updateK();
		}
		else if(k>0)
		{
			//add something
			//�õ���һ��bit�����жϣ������0��ô������m��0���������000000u����ʽ
			if (nextBitIsZero())
			{
				u = 0;
				//������һ�θ��º��m���ָ��� m��0
				for (int i = 0; i < m; i++)
				{
					resData->push_back(u);
				}
				
				//����K��ʱ��ͬģʽ
				type = CompleteRun;

				//�������decodeNumҲҪ����ԭ���Ǹ� kR��k��update
				updateKR();
				//u = decodeNum;
				updateK();
			}
			else
			{
				//���ִ� 000000u  ������ û��0��ֻ�з�0��u����ʽ

				//�õ�0�ĸ���,������
				uint64_t zeroNum = 0;
				bitset<64> bits;
				bits.reset();

				bool b;
				for (int i = 0; i < k; i++)
				{
					bitReadFile->getBit(b);
					bits.set(i, b);
				}
				zeroNum = bits.to_ulong();
				u = 0;
				for (int i = 0; i < zeroNum; i++)
				{
					resData->push_back(u);
				}

				//��������Ǹ�����0������
				rice_golombDecode(u);
				u++;
				resData->push_back(u);

				//����K��ʱ��ͬģʽ
				type = PartialRun;

				//�������decodeNumҲҪ����ԭ���Ǹ� kR��k��update
				updateKR();
				//u = decodeNum;
				updateK();
			}
		}
		else
		{
			assert(0);
		}
	}

	resData->erase(resData->begin() + resData->size() - 1);
	cout << "finish  decode the data !!!" << endl;
	bitReadFile->close();
	return true;
}

void RLGR::setK(uint64_t k_)
{
	k = k_;
	m = pow(2, k);
}

bool RLGR::nextBitIsZero()
{
	bool  b_out;
	if (!bitReadFile->getBit(b_out))
	{
		cout << "file ��û��������" << endl;
	}
	return !b_out;
}

void RLGR::updateKR()
{
	if (p == 0)
	{
		//assert(kRP >1 );
		if (kRP < 2)
		{
			kRP += 10;
			cout << "updateKR : kRP are going to be negative,so add 10 toit self !" << endl;
		}

		kRP = kRP - 2;
		kR = kRP >> ((int)log2(L));
		//m = pow(2, kR);
	}
	else if (p == 1)
	{
		//no change
	}
	else if(p > 1)
	{
		kRP = kRP +p+1;
		kR = kRP >> ((int)log2(L));
		//m = pow(2, kR);
	}
	else
	{
		//p������С��0
		assert(0);
	}
}

void RLGR::updateK()
{
	if (k == 0)
	{
		if (u == 0)
		{
			kP = kP + u0;
			k = kP >> ((int)log2(L));
			m = pow(2, k);
		}
		else if (u > 0)
		{
			//assert(kP >0);
			if (kP < 1)
			{
				kP += 10;
				cout << "updateK: kP are going to be negative,so add 10 toit self !" << endl;
			}

			kP = kP - d0;
			k = kP >> ((int)log2(L));
			m = pow(2, k);
		}
		else
		{
			//������С��0
			assert(0);
		}
	}
	else if(k > 0)
	{
		//����run�����ͽ��в���

		switch (type)
		{
		case CompleteRun:
			kP = kP + u1;
			k = kP >> ((int)log2(L));
			m = pow(2, k);
			break;
		case PartialRun:
			//assert(kP >0 );
			
			if (kP < 1)
			{
				kP += 10;
				cout << "updateK: kP are going to be negative,so add 10 toit self !" << endl;
			}

			kP = kP - d1;
			k = kP >> ((int)log2(L));
			m = pow(2, k);
			break;
		default:
			assert(0);
		}
	}
	else
	{
		//������С��0
		assert(0);
	}
}

bool RLGR::rice_golombEncode(uint64_t num)
{
	//���ƣ��൱�� q = num/ (x^k)
	p = num >> kR;
	//�൱�� r = num - q * (x^k)
	uint64_t r = num & ((int)pow(2,kR) - 1);

	// �������Ԫ�ĳ���
	auto len = p + 1 + kR;

	//�����жϻ����Ƿ�Ϊ����ֱ��������ţ�����Ļ����浽bit buffer��
	bitWriteFile->putBit(1, (int)p);

	bitWriteFile->putBit(0);

	//�����Ƿ�������ġ�
	for (int i = 0; i < kR; i++)
	{
		//�ж�r�����һλ�������1����ôputBit true,�����0 ,��ôputBit false;
		bitWriteFile->putBit(static_cast<bool>(r & 0x01));
		//r����һλ��
		r >>= 1;
	}
	return true;
}

bool RLGR::encodeOneZero()
{
	bitWriteFile->putBit(0);
	return true;
}

//zeroNum ��ʾ0�ĸ���
bool RLGR::encodeStringWithZeros(int zeroNum ,uint64_t &u)
{
	bitWriteFile->putBit(1);
	//zeroNum ��kΪ����ʾ�ɶ�����.��λ�ȴ��
	for (int i = 0; i < k; i++)
	{
		bitWriteFile->putBit(zeroNum & 0x01);
		zeroNum >>= 1;
	}

	rice_golombEncode(u - 1);

	return true;
}

bool RLGR::rice_golombDecode(uint64_t & num)
{
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
	for (int i = 0; i < kR; i++)
	{
		bitReadFile->getBit(b);
		bits.set(i, b);
	}
	num = unary * pow(2, kR) + bits.to_ulong();

	//save to p for update
	p = unary;
	
	return true;
}
