#include "zjw_bitFile.h"

//=====================ReadBuffer=======================

ReadBuffer::ReadBuffer(ifstream * filePtr)
{
	assert(filePtr);
	assert(bitBufferLength % 8 == 0);
	assert(byteBufferLength % 8 == 0);


	//file
	this->filePtr = filePtr;

	//bit 
    bitBuffer.reset();
	bitPos = 0;
	validBitsNum = 0;

	//byte
	byteBuffer = new uint8_t[byteBufferLength];
	bytePos = 0;
	validByteNum = 0;

}

ReadBuffer::~ReadBuffer()
{
	delete byteBuffer;
}

bool ReadBuffer::getBit(bool &bit)
{
	if (bitPos < validBitsNum)
	{
		bit = bitBuffer[bitPos];
		bitPos++;
		return true;
	}
	else
	{
		//bitbuffer�е����ݶ������꣬��Ҫ��bytebuffer�л�ȡ���ݡ�
		if (getNextBytesFromByteBuffer()>0)
		{
			bit = bitBuffer[bitPos];
			bitPos++;
			return true;
		}
		else
		{
			//bytebuffer��û���㹻��������
			bitPos = -1;
			return false;
		}
	}
}

bool ReadBuffer::checkTerminator()
{
	//�ս�� ����64��0
	int count = 0;
	
	int temp = bitPos;
	while (temp < validBitsNum && count < 64)
	{
		if (bitBuffer[temp])
		{
			//�����ս��
			return false;
		}
		temp++;
		count++;
	} 

	int needCheckByte = (64 - count) / 8;
	if (((64 - count) % 8) != 0)
	{
		needCheckByte++;
	}
	
	if ((validByteNum - bytePos) < needCheckByte)
	{
		//bytebuffer�е����ݲ��㹻���ڼ��
		//��file�ж�ȡbyte���²���bytebuffer������pos������
		getSomeBytesFromFile();
	}

	//��Ҫ��byteBuffer��ȥ���
	temp = bytePos;

	while ((64 - count) >= 8)
	{
		if ((int)(byteBuffer[temp]))
		{
			//�����ս��
			return false;
		}
		count += 8;
		temp++;
	}

	//ֻʣ�²���8λ����Ҫ�����ж�,����ByteBuffer�Ѿ�������
	while (count < 64)
	{
		uint8_t byte = byteBuffer[temp];
		//�Ƚϸ�λ
		if (byte & 0x01)
		{
			return false;
		}
		//����һλ
		byte >> 1;
		count++;
	}

	return true;
}

bool ReadBuffer::getByteFromByteBuffer(uint8_t & byte) {

	if (bytePos < validByteNum)
	{
		byte = byteBuffer[bytePos];
		bytePos++;
		return true;
	}
	else
	{
		if (getBytesFromFile()>0)
		{
			//byte buffer�е����ݲ�������Ҫ��ȡ���ݡ�
			byte = byteBuffer[bytePos];
			bytePos++;
			return true;
		}
		else
		{
			//û���㹻��������
			bytePos = -1;
			return false;
		}
	}
}

//ֻ��֤bitBufferLength������64��ʱ���ǶԵ�,��������ӿ�֮ǰ��֤��bitbuffer�е������Ѿ����ꡣ 
uint8_t ReadBuffer::getNextBytesFromByteBuffer()
{
	assert(bitPos >= validBitsNum);

	//init bitBuffer ��Ϊ0
	bitBuffer.reset();
	
	//��byteBuffer�ж�ȡ���ɸ���Ч���ݡ�
	for (int b_it = 0; b_it < bitBufferLength / 8; b_it++)
	{
		uint8_t  byte;
		if (getByteFromByteBuffer(byte))
		{
			//bitBufferLength���64λ  ����ŵ���0���ұ�
			uint64_t tmp = (byte << (b_it * 8));
			bitBuffer |= tmp;
		}
		else
		{
			//�ļ������ݲ����ˡ�
			validBitsNum = b_it * 8;
			bitPos = 0;
			return validBitsNum;
		}
	}

	bitPos = 0;
	validBitsNum = bitBufferLength;

	return validBitsNum;
}

//��������ӿڵ�ǰ�ᣬbyteBuffer�е����ݶ��������ˡ�
uint64_t ReadBuffer::getBytesFromFile()
{
	//init
	memset(byteBuffer, 0, byteBufferLength);
	//read
	filePtr->read((char*)byteBuffer, byteBufferLength);

	//�жϳ�ʵ�ʵõ���byte��������
	validByteNum = filePtr->gcount();

	//��ʼ�������Ӳ���
 	bytePos = 0;

	//test
	//cout << "getBytesFromFile: read number " << validByteNum << endl;
	//end test
	return validByteNum;
}

uint64_t ReadBuffer::getSomeBytesFromFile()
{
	//���ڻ���Ч������,�ƶ���ByteBuffer����ǰ��
	int validNum = validByteNum - bytePos;

	for (int i = 0; i < validNum; i++)
	{
		byteBuffer[i] = byteBuffer[bytePos + i];
	}

	//��ȡByteNum
	if(validByteNum!=0)
		filePtr->read((char*)(byteBuffer + validNum), bytePos);
	else
		filePtr->read((char*)(byteBuffer + validNum), byteBufferLength);
	
	//�жϳ�ʵ�ʵõ���byte��������
	validByteNum = filePtr->gcount() + validNum;
	//��ʼ�������Ӳ���
	bytePos = 0;

	return validByteNum;
}


//===================== writeBuffer =======================

WriteBuffer::WriteBuffer(ofstream * filePtr)
{
	assert(filePtr);
	assert(bitBufferLength % 8 == 0);
	assert(byteBufferLength % 8 == 0);

	//file
	this->filePtr = filePtr;

	//bit 
	bitBuffer.reset();
	bitPos = 0;
	validBitsNum = 0;

	//byte
	byteBuffer = new uint8_t[byteBufferLength];
	bytePos = 0;
	validByteNum = 0;

}

WriteBuffer::~WriteBuffer()
{
	//delete filePtr;
	delete byteBuffer;
}

void WriteBuffer::putBit(bool b)
{
	if (bitPos < bitBufferLength)
	{
		bitBuffer[bitPos] = b;
		bitPos++;
		validBitsNum++;
	}
	else
	{
		//bitBuffer д�뵽 bytebuffer
		putBitsToByteBuffer();

		//save b
		bitBuffer[bitPos] = b;
		bitPos++;
		validBitsNum++;
	}

}

void WriteBuffer::putBit(bool b, int n)
{
	for (int i = 0; i < n; i++)
	{
		putBit(b);
	}
}

bool WriteBuffer::getOneByteFromBitBuffer(uint8_t & byte)
{
	if (validBitsNum >= 8)
	{
		byte = static_cast<uint8_t>(bitBuffer.to_ulong());

		//����8λ
		bitBuffer >>= 8;
		bitPos -= 8;
		validBitsNum -= 8;
		return true;

	}
	else if (validBitsNum > 0)
	{
		byte = static_cast<uint8_t>(bitBuffer.to_ulong());
		bitBuffer >>= 8;
		bitPos = 0;
		validBitsNum = 0;

		return true;
	}
	else
	{
		//bitbuffer��û������
		return false;
	}

}

void WriteBuffer::putOneByteToByteBuffer(uint8_t  byte)
{
	if (bytePos < byteBufferLength)
	{
		byteBuffer[bytePos]= byte;
		bytePos++;
		validByteNum++;
	}
	else
	{
		//byte buffer��Ҫд�� �ļ���,����ʼ��bytePos �� validByteNum
		putBytesBufferToFile();

		//�������Byte
		byteBuffer[bytePos] = byte;
		bytePos++;
		validByteNum++;
	}
}

void WriteBuffer::putBitsToByteBuffer()
{
	uint8_t  byte;
	//�������8�����Ͳ�Ҫ���ˡ�
	while (validBitsNum >= 8)
	{
		getOneByteFromBitBuffer(byte);
		putOneByteToByteBuffer(byte);
	}
}

void WriteBuffer::putBytesBufferToFile()
{
	filePtr->write((const char*)byteBuffer, validByteNum);
	bytePos = 0;
	validByteNum = 0;
}

void WriteBuffer::putTerminator()
{
	for (int i = 0; i < 64; i++)
	{
		putBit(0);
	}
}

void WriteBuffer::fillout()
{
	//-------bit buffer����byte buffer------
	//��8λ��bits���byteд��ByteBuffer��
	putBitsToByteBuffer();

	//�������8λ��Ҳ��0���byte buffer��
	if (validBitsNum > 0)
	{
		uint8_t  byte;
		getOneByteFromBitBuffer(byte);
		putOneByteToByteBuffer(byte);
	}

	//-------bytebuffer  ���� file-------
	putBytesBufferToFile();
}

//==================bitfile=====================

BitReadFile::BitReadFile(string path)
{
	this->path = path;
}

BitReadFile::~BitReadFile()
{
	//delete path;
	if(readBuffer)
		delete readBuffer;
}

bool BitReadFile::isOpen()
{
	return file.is_open();
}

bool BitReadFile::open(string filePath)
{
	file.open(filePath, ios::binary | ios::out);
	if (!file)
	{
		cout << "open BitReadFile file: " << filePath << " failed !!!" << endl;
	}
	else
	{
		cout << "open BitReadFile file: " << filePath << "success." << endl;
	}
	readBuffer = new ReadBuffer(&file);
	return true;
}

bool BitReadFile::open()
{
	file.open(path, ios::binary| ios::out);
	if (!file)
	{
		cout << "open BitReadFile file: " << path << " failed !!!" << endl;
	}
	else
	{
		cout << "open BitReadFile file: " << path << "success." << endl;
	}
	readBuffer = new ReadBuffer(&file);
	return false;
}

void BitReadFile::close()
{
	cout << "close the BitReadFile file: " << path << "success." << endl;
	file.close();
}

bool BitReadFile::checkTerminator()
{
	return readBuffer->checkTerminator();
}

bool BitReadFile::getBit(bool & b_out)
{
	return readBuffer->getBit(b_out);;
}

BitWriteFile::BitWriteFile(string path)
{
	this->path = path;
	
}

BitWriteFile::~BitWriteFile()
{
	//delete path;
	if(writeBuffer)
		delete writeBuffer;
}

bool BitWriteFile::isOpen()
{
	return file.is_open();;
}

bool BitWriteFile::open(string filePath)
{
	file.open(filePath, ios::binary);
	if (!file)
	{
		cout << "BitWriteFile open file: " << filePath << " failed !!!" << endl;
		cout << "create new file : " << filePath << " !!!" << endl;
		//ios::app���Ϻ�����������ļ�����ô�ᴴ���ļ���
		file.open(filePath, ios::binary| ios::app);
		file.close();
		file.open(filePath, ios::binary);
		if (file)
		{
			cout << "BitWriteFile open file: " << filePath << "success." << endl;
		}
		else
		{
			cout << "create: " << filePath << " failed." << endl;
			//�ж�
			assert(0);
		}
	}
	else
	{
		cout << "BitWriteFile open file: " << filePath << "success." << endl;
	}
	writeBuffer = new WriteBuffer(&file);
	//writeBuffer = WriteBuffer(&file);
	return true;
}

bool BitWriteFile::open()
{
	file.open(path, ios::binary);
	if (!file)
	{
		cout << "open BitWriteFile  file: " << path << " failed !!!" << endl;
		cout << "create new file : " << path << " !!!" << endl;
		//ios::app���Ϻ�����������ļ�����ô�ᴴ���ļ���
		file.open(path, ios::binary |ios::app);
		file.close();
		file.open(path, ios::binary );
		if (file)
		{
			cout << "open BitWriteFile  file: " << path << "success." << endl;
		}
		else
		{
			cout << "create: " << path << " failed." << endl;
			//�ж�
			assert(0);
		}
	}
	else
	{
		cout << "open BitWriteFile  file: " << path << "success." << endl;
	}
	writeBuffer = new WriteBuffer(&file);
	//writeBuffer = WriteBuffer(&file);
	return true;
}

void BitWriteFile::close()
{
	//д�������
	//writeBuffer->putTerminator();
	cout << "close the BitWriteFile file: " << path << "success." << endl;

	writeBuffer->fillout();
	//writeBuffer.fillout();
	file.close();
}



void BitWriteFile::putBit(bool b_in)
{
	writeBuffer->putBit(b_in);
	//writeBuffer.putBit(b_in);
}

void BitWriteFile::putBit(int  b_in)
{
	writeBuffer->putBit(b_in);
	//writeBuffer.putBit(b_in);

}

void BitWriteFile::putBit(bool  b_in,  int  n)
{
	writeBuffer->putBit(b_in, n);
	//writeBuffer.putBit(b_in, n);

}

void BitWriteFile::putBit(int  b_in, int  n)
{
	writeBuffer->putBit(b_in, n);
	//writeBuffer.putBit(b_in, n);
}


