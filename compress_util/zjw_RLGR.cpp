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

	//判断是否run length的k
	k = 0;

	//default, L一定是2的pow
	L = 4;
	kRP = 16;

	//kR = kRP/L;
	kR = kRP >> ((int)log2(L));
	//Golomb Rice 
	m = pow(2, kR);

	//四个参数
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

	//多在数据末尾放入一个非0的符号，便于处理文件末尾的多个0的情况。
	codeData->push_back(10);
	
#define COMPRESS_NUM_TO_FILE
#ifdef COMPRESS_NUM_TO_FILE
	//把数据的个数利用GR coder压缩到文件的第一个数据。
	uint64_t totalNum = codeData->size();
	//设置GR coder中的k,这是解压和压缩两边约定好的。要改就只能一起改了。
	kR = 10;
	rice_golombEncode(totalNum);
	cout << "start to encode the data ( num: " << totalNum - 1 << " )" << endl;
#endif

	//kR等值会初始化
	initParam();

	//对每一个数字进行编码
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
			//no run model  GR的方式code.
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
				//u等于0，后面还有数字

				//还原num_it
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
					//数字的连续为m个0000000，编码为0
					//把多取的那个数字退回去
					num_it--;
					//encode 为0
					encodeOneZero();

					//更新K的时候不同模式
					type = CompleteRun;

					//update
					updateKR();
					updateK();

					continue;
				}

				//else if (u == 0 && ++num_it > codeData->size())
				//{
				//	//u等于0，后面已经没有数字了。达到文件的末尾。那么按照Partion run来进行处理。
				//	uIsZero = 0;
				//	//数字的连续为uIsZero个0000000 加上一个u，编码为0
				//	encodeStringWithZeros(uIsZero, u);
				//	//更新K的时候不同模式
				//	type = PartialRun;
				//	updateKR();
				//	updateK();
				//}

				else if (u != 0)
				{
					//数字的连续为uIsZero个0000000 加上一个u，编码为0
					encodeStringWithZeros(uIsZero, u);

					//更新K的时候不同模式
					type = PartialRun;

					updateKR();
					updateK();
				}
				else
				{
					//u= 0。这种情况是可能出现在最后几个数字上的，0的个数不够的情况。
					assert(0);
					//需要处理这种情况
					//add something
				}
			}
			else if (u > 0)
			{
				// run model 下面遇到单个非0的数字，前面没有0的
				//rice_golombEncode(u);

				//当作 前面是0个0，
				encodeStringWithZeros(0, u);

				//更新K的时候不同模式
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
	//删除文件结束符号
	codeData->erase(codeData->begin() + codeData->size() - 1);
	//关闭
	cout << "finish  encode the data !!!"<< endl;
	bitWriteFile->close();


}

bool RLGR::decode()
{

	bitReadFile->open();

#define COMPRESS_NUM_TO_FILE
#ifdef COMPRESS_NUM_TO_FILE
	//设置GR coder中的k,这是解压和压缩两边约定好的。要改就只能一起改了。
	kR = 10;
	//得到后面还有多少个数据(GR coder中的decoder),包括文件结束符。
	uint64_t totalNum =0;
	rice_golombDecode(totalNum);
	cout << "start parse the coded file (num: " << totalNum-1 << ")" << endl;
#endif

	//RLGR的参数化初始
	initParam();
	while (resData->size() < totalNum)
	{
		if (k == 0)
		{
			//该函数里面里面会更新p
			rice_golombDecode(u);
			//saveData
			resData->push_back(u);

			//根据这个decodeNum也要更新原有那个 kR和k的update
			updateKR();
			//u = decodeNum;
			updateK();
		}
		else if(k>0)
		{
			//add something
			//拿到下一个bit进行判断，如果是0那么，就是m个0，否则就是000000u的形式
			if (nextBitIsZero())
			{
				u = 0;
				//根据上一次更新后的m来恢复出 m个0
				for (int i = 0; i < m; i++)
				{
					resData->push_back(u);
				}
				
				//更新K的时候不同模式
				type = CompleteRun;

				//根据这个decodeNum也要更新原有那个 kR和k的update
				updateKR();
				//u = decodeNum;
				updateK();
			}
			else
			{
				//数字串 000000u  或者是 没有0，只有非0的u的形式

				//得到0的个数,并保存
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

				//处理后面那个不是0的数字
				rice_golombDecode(u);
				u++;
				resData->push_back(u);

				//更新K的时候不同模式
				type = PartialRun;

				//根据这个decodeNum也要更新原有那个 kR和k的update
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
		cout << "file 中没有数据了" << endl;
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
		//p不可能小于0
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
			//不可能小于0
			assert(0);
		}
	}
	else if(k > 0)
	{
		//根据run的类型进行操作

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
		//不可能小于0
		assert(0);
	}
}

bool RLGR::rice_golombEncode(uint64_t num)
{
	//右移，相当于 q = num/ (x^k)
	p = num >> kR;
	//相当于 r = num - q * (x^k)
	uint64_t r = num & ((int)pow(2,kR) - 1);

	// 编码后码元的长度
	auto len = p + 1 + kR;

	//不会判断缓存是否为满，直接向里面放，不足的话缓存到bit buffer中
	bitWriteFile->putBit(1, (int)p);

	bitWriteFile->putBit(0);

	//余数是反着来存的。
	for (int i = 0; i < kR; i++)
	{
		//判断r的最低一位，如果是1，那么putBit true,如果是0 ,那么putBit false;
		bitWriteFile->putBit(static_cast<bool>(r & 0x01));
		//r右移一位。
		r >>= 1;
	}
	return true;
}

bool RLGR::encodeOneZero()
{
	bitWriteFile->putBit(0);
	return true;
}

//zeroNum 表示0的个数
bool RLGR::encodeStringWithZeros(int zeroNum ,uint64_t &u)
{
	bitWriteFile->putBit(1);
	//zeroNum 用k为来表示成二进制.低位先存的
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

	//余数是先存低位的，所以解析出来也是从低位开始解析
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
