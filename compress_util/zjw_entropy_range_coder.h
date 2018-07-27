#pragma once

#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <cstdint>


class AdaptiveRangeCoder
{

public:

	/** \brief Empty constructor. */
	AdaptiveRangeCoder() : outputCharVector_()
	{
	}

	/** \brief Empty deconstructor. */
	virtual
		~AdaptiveRangeCoder()
	{
	}

	/** \brief Encode char vector to output stream
	 * \param inputByteVector_arg input vector
	 * \param outputByteStream_arg output stream containing compressed data
	 * \return amount of bytes written to output stream
	 */
	unsigned long
		encodeCharVectorToStream(const std::vector<char>& inputByteVector_arg, std::ostream& outputByteStream_arg);

	/** \brief Decode char stream to output vector
	 * \param inputByteStream_arg input stream of compressed data
	 * \param outputByteVector_arg decompressed output vector
	 * \return amount of bytes read from input stream
	 */
	unsigned long
		decodeStreamToCharVector(std::istream& inputByteStream_arg, std::vector<char>& outputByteVector_arg);

protected:
	typedef uint32_t DWord; // 4 bytes

private:
	/** vector containing compressed data
	 */
	std::vector<char> outputCharVector_;

};

class StaticRangeCoder
{
public:
	/** \brief Constructor. */
	StaticRangeCoder() :
		cFreqTable_(65537), outputCharVector_()
	{
	}

	/** \brief Empty deconstructor. */
	virtual
		~StaticRangeCoder()
	{
	}

	/** \brief Encode integer vector to output stream
	  * \param[in] inputIntVector_arg input vector
	  * \param[out] outputByterStream_arg output stream containing compressed data
	  * \return amount of bytes written to output stream
	  */
	unsigned long
		encodeIntVectorToStream(std::vector<unsigned int>& inputIntVector_arg, std::ostream& outputByterStream_arg);

	/** \brief Decode stream to output integer vector
	 * \param inputByteStream_arg input stream of compressed data
	 * \param outputIntVector_arg decompressed output vector
	 * \return amount of bytes read from input stream
	 */
	unsigned long
		decodeStreamToIntVector(std::istream& inputByteStream_arg, std::vector<unsigned int>& outputIntVector_arg);

	/** \brief Encode char vector to output stream
	 * \param inputByteVector_arg input vector
	 * \param outputByteStream_arg output stream containing compressed data
	 * \return amount of bytes written to output stream
	 */
	unsigned long
		encodeCharVectorToStream(const std::vector<char>& inputByteVector_arg, std::ostream& outputByteStream_arg);

	/** \brief Decode char stream to output vector
	 * \param inputByteStream_arg input stream of compressed data
	 * \param outputByteVector_arg decompressed output vector
	 * \return amount of bytes read from input stream
	 */
	unsigned long
		decodeStreamToCharVector(std::istream& inputByteStream_arg, std::vector<char>& outputByteVector_arg);

protected:
	typedef uint32_t DWord; // 4 bytes

	/** \brief Helper function to calculate the binary logarithm
	 * \param n_arg: some value
	 * \return binary logarithm (log2) of argument n_arg
	 */
	inline double
		Log2(double n_arg)
	{
		return log(n_arg) / log(2.0);
	}

private:
	/** \brief Vector containing cumulative symbol frequency table. */
	std::vector<uint64_t> cFreqTable_;

	/** \brief Vector containing compressed data. */
	std::vector<char> outputCharVector_;

};