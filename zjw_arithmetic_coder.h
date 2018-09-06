#pragma once

#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <cstdint>


/*
* 自适应的算术编码，概率是自动更新的.用于颜色的压缩
*/
class AdaptiveArithmeticCoder
{

public:

	/** \brief Empty constructor. */
	AdaptiveArithmeticCoder() : outputCharVector_()	{	}

	/** \brief Empty deconstructor. */
	virtual	~AdaptiveArithmeticCoder()	{	}

	/** \brief Encode char vector to output stream
	* \param inputByteVector_arg input vector
	* \param outputByteStream_arg output stream containing compressed data
	* \return amount of bytes written to output stream
	*/
	unsigned long encodeCharVectorToStream(const std::vector<char>& inputByteVector_arg, 
		std::ostream& outputByteStream_arg);

	/** \brief Decode char stream to output vector
	* \param inputByteStream_arg input stream of compressed data
	* \param outputByteVector_arg decompressed output vector
	* \return amount of bytes read from input stream
	*/
	unsigned long decodeStreamToCharVector(std::istream& inputByteStream_arg, 
			std::vector<char>& outputByteVector_arg);

protected:
	typedef uint32_t DWord; // 4 bytes

private:
	/** vector containing compressed data
	*/
	std::vector<char> outputCharVector_;

};

