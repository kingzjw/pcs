#include "zjw_entropy_range_coder.h"
#include <map>
#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <stdio.h>

 //////////////////////////////////////////////////////////////////////////////////////////////
unsigned long AdaptiveRangeCoder::encodeCharVectorToStream(const std::vector<char>& inputByteVector_arg,
	std::ostream& outputByteStream_arg)
{
	DWord freq[257];
	uint8_t ch;
	unsigned int i, j, f;
	char out;

	// define limits
	const DWord top = static_cast<DWord> (1) << 24;
	const DWord bottom = static_cast<DWord> (1) << 16;
	const DWord maxRange = static_cast<DWord> (1) << 16;

	DWord low, range;

	unsigned int readPos;
	unsigned int input_size;

	input_size = static_cast<unsigned> (inputByteVector_arg.size());

	// init output vector
	outputCharVector_.clear();
	outputCharVector_.reserve(sizeof(char) * input_size);

	readPos = 0;

	low = 0;
	range = static_cast<DWord> (-1);

	// initialize cumulative frequency table
	for (i = 0; i < 257; i++)
		freq[i] = i;

	// scan input
	while (readPos < input_size)
	{

		// read byte
		ch = inputByteVector_arg[readPos++];

		// map range
		low += freq[ch] * (range /= freq[256]);
		range *= freq[ch + 1] - freq[ch];

		// check range limits
		while ((low ^ (low + range)) < top || ((range < bottom) && ((range = -int(low) & (bottom - 1)), 1)))
		{
			out = static_cast<char> (low >> 24);
			range <<= 8;
			low <<= 8;
			outputCharVector_.push_back(out);
		}

		// update frequency table
		for (j = ch + 1; j < 257; j++)
			freq[j]++;

		// detect overflow
		if (freq[256] >= maxRange)
		{
			// rescale
			for (f = 1; f <= 256; f++)
			{
				freq[f] /= 2;
				if (freq[f] <= freq[f - 1])
					freq[f] = freq[f - 1] + 1;
			}
		}

	}

	// flush remaining data
	for (i = 0; i < 4; i++)
	{
		out = static_cast<char> (low >> 24);
		outputCharVector_.push_back(out);
		low <<= 8;
	}

	// write to stream
	outputByteStream_arg.write(&outputCharVector_[0], outputCharVector_.size());

	return (static_cast<unsigned long> (outputCharVector_.size()));

}

//////////////////////////////////////////////////////////////////////////////////////////////
unsigned long
AdaptiveRangeCoder::decodeStreamToCharVector(std::istream& inputByteStream_arg,
	std::vector<char>& outputByteVector_arg)
{
	uint8_t ch;
	DWord freq[257];
	unsigned int i, j, f;

	// define limits
	const DWord top = static_cast<DWord> (1) << 24;
	const DWord bottom = static_cast<DWord> (1) << 16;
	const DWord maxRange = static_cast<DWord> (1) << 16;

	DWord low, range;
	DWord code;

	unsigned int outputBufPos;
	unsigned int output_size = static_cast<unsigned> (outputByteVector_arg.size());

	unsigned long streamByteCount;

	streamByteCount = 0;

	outputBufPos = 0;

	code = 0;
	low = 0;
	range = static_cast<DWord> (-1);

	// init decoding
	for (i = 0; i < 4; i++)
	{
		inputByteStream_arg.read(reinterpret_cast<char*> (&ch), sizeof(char));
		streamByteCount += sizeof(char);
		code = (code << 8) | ch;
	}

	// init cumulative frequency table
	for (i = 0; i <= 256; i++)
		freq[i] = i;

	// decoding loop
	for (i = 0; i < output_size; i++)
	{
		uint8_t symbol = 0;
		uint8_t sSize = 256 / 2;

		// map code to range
		DWord count = (code - low) / (range /= freq[256]);

		// find corresponding symbol
		while (sSize > 0)
		{
			if (freq[symbol + sSize] <= count)
			{
				symbol = static_cast<uint8_t> (symbol + sSize);
			}
			sSize /= 2;
		}

		// output symbol
		outputByteVector_arg[outputBufPos++] = symbol;

		// update range limits
		low += freq[symbol] * range;
		range *= freq[symbol + 1] - freq[symbol];

		// decode range limits
		while ((low ^ (low + range)) < top || ((range < bottom) && ((range = -int(low) & (bottom - 1)), 1)))
		{
			inputByteStream_arg.read(reinterpret_cast<char*> (&ch), sizeof(char));
			streamByteCount += sizeof(char);
			code = code << 8 | ch;
			range <<= 8;
			low <<= 8;
		}

		// update cumulative frequency table
		for (j = symbol + 1; j < 257; j++)
			freq[j]++;

		// detect overflow
		if (freq[256] >= maxRange)
		{
			// rescale
			for (f = 1; f <= 256; f++)
			{
				freq[f] /= 2;
				if (freq[f] <= freq[f - 1])
					freq[f] = freq[f - 1] + 1;
			}
		}
	}

	return (streamByteCount);

}

//////////////////////////////////////////////////////////////////////////////////////////////
unsigned long
StaticRangeCoder::encodeIntVectorToStream(std::vector<unsigned int>& inputIntVector_arg,
	std::ostream& outputByteStream_arg)
{

	unsigned int inputsymbol;
	unsigned int i, f;
	char out;

	uint64_t frequencyTableSize;
	uint8_t frequencyTableByteSize;

	// define numerical limits
	const uint64_t top = static_cast<uint64_t> (1) << 56;
	const uint64_t bottom = static_cast<uint64_t> (1) << 48;
	const uint64_t maxRange = static_cast<uint64_t> (1) << 48;

	unsigned long input_size = static_cast<unsigned long> (inputIntVector_arg.size());
	uint64_t low, range;

	unsigned int inputSymbol;

	unsigned int readPos;

	unsigned long streamByteCount;

	streamByteCount = 0;

	// init output vector
	outputCharVector_.clear();
	outputCharVector_.reserve((sizeof(char) * input_size * 2));

	frequencyTableSize = 1;

	readPos = 0;

	// calculate frequency table
	cFreqTable_[0] = cFreqTable_[1] = 0;
	while (readPos < input_size)
	{
		inputSymbol = inputIntVector_arg[readPos++];

		if (inputSymbol + 1 >= frequencyTableSize)
		{
			// frequency table is to small -> adaptively extend it
			uint64_t oldfrequencyTableSize;
			oldfrequencyTableSize = frequencyTableSize;

			do
			{
				// increase frequency table size by factor 2
				frequencyTableSize <<= 1;
			} while (inputSymbol + 1 > frequencyTableSize);

			if (cFreqTable_.size() < frequencyTableSize + 1)
			{
				// resize frequency vector
				cFreqTable_.resize(static_cast<std::size_t> (frequencyTableSize + 1));
			}

			// init new frequency range with zero
			memset(&cFreqTable_[static_cast<std::size_t> (oldfrequencyTableSize + 1)], 0,
				sizeof(uint64_t) * static_cast<std::size_t> (frequencyTableSize - oldfrequencyTableSize));
		}
		cFreqTable_[inputSymbol + 1]++;
	}
	frequencyTableSize++;

	// convert to cumulative frequency table
	for (f = 1; f < frequencyTableSize; f++)
	{
		cFreqTable_[f] = cFreqTable_[f - 1] + cFreqTable_[f];
		if (cFreqTable_[f] <= cFreqTable_[f - 1])
			cFreqTable_[f] = cFreqTable_[f - 1] + 1;
	}

	// rescale if numerical limits are reached
	while (cFreqTable_[static_cast<std::size_t> (frequencyTableSize - 1)] >= maxRange)
	{
		for (f = 1; f < cFreqTable_.size(); f++)
		{
			cFreqTable_[f] /= 2;
			;
			if (cFreqTable_[f] <= cFreqTable_[f - 1])
				cFreqTable_[f] = cFreqTable_[f - 1] + 1;
		}
	}

	// calculate amount of bytes per frequency table entry
	frequencyTableByteSize = static_cast<uint8_t> (ceil(
		Log2(static_cast<double> (cFreqTable_[static_cast<std::size_t> (frequencyTableSize - 1)])) / 8.0));

	// write size of frequency table to output stream
	outputByteStream_arg.write(reinterpret_cast<const char*> (&frequencyTableSize), sizeof(frequencyTableSize));
	outputByteStream_arg.write(reinterpret_cast<const char*> (&frequencyTableByteSize), sizeof(frequencyTableByteSize));

	streamByteCount += sizeof(frequencyTableSize) + sizeof(frequencyTableByteSize);

	// write cumulative  frequency table to output stream
	for (f = 1; f < frequencyTableSize; f++)
	{
		outputByteStream_arg.write(reinterpret_cast<const char*> (&cFreqTable_[f]), frequencyTableByteSize);
		streamByteCount += frequencyTableByteSize;
	}

	readPos = 0;
	low = 0;
	range = static_cast<uint64_t> (-1);

	// start encoding
	while (readPos < input_size)
	{

		// read symol
		inputsymbol = inputIntVector_arg[readPos++];

		// map to range
		low += cFreqTable_[inputsymbol] * (range /= cFreqTable_[static_cast<std::size_t> (frequencyTableSize - 1)]);
		range *= cFreqTable_[inputsymbol + 1] - cFreqTable_[inputsymbol];

		// check range limits  range =
		while ((low ^ (low + range)) < top || ((range < bottom) && ((range = -int(low) & (bottom - 1)), 1)))
		{
			out = static_cast<char> (low >> 56);
			range <<= 8;
			low <<= 8;
			outputCharVector_.push_back(out);
		}

	}

	// flush remaining data
	for (i = 0; i < 8; i++)
	{
		out = static_cast<char> (low >> 56);
		outputCharVector_.push_back(out);
		low <<= 8;
	}

	// write encoded data to stream
	outputByteStream_arg.write(&outputCharVector_[0], outputCharVector_.size());

	streamByteCount += static_cast<unsigned long> (outputCharVector_.size());

	return (streamByteCount);
}

//////////////////////////////////////////////////////////////////////////////////////////////
unsigned long
StaticRangeCoder::decodeStreamToIntVector(std::istream& inputByteStream_arg,
	std::vector<unsigned int>& outputIntVector_arg)
{
	uint8_t ch;
	unsigned int i, f;

	// define range limits
	const uint64_t top = static_cast<uint64_t> (1) << 56;
	const uint64_t bottom = static_cast<uint64_t> (1) << 48;

	uint64_t low, range;
	uint64_t code;

	unsigned int outputBufPos;
	unsigned long output_size;

	uint64_t frequencyTableSize;
	unsigned char frequencyTableByteSize;

	unsigned long streamByteCount;

	streamByteCount = 0;

	outputBufPos = 0;
	output_size = static_cast<unsigned long> (outputIntVector_arg.size());

	// read size of cumulative frequency table from stream
	inputByteStream_arg.read(reinterpret_cast<char*> (&frequencyTableSize), sizeof(frequencyTableSize));
	inputByteStream_arg.read(reinterpret_cast<char*> (&frequencyTableByteSize), sizeof(frequencyTableByteSize));

	streamByteCount += sizeof(frequencyTableSize) + sizeof(frequencyTableByteSize);

	// check size of frequency table vector
	if (cFreqTable_.size() < frequencyTableSize)
	{
		cFreqTable_.resize(static_cast<std::size_t> (frequencyTableSize));
	}

	// init with zero
	memset(&cFreqTable_[0], 0, sizeof(uint64_t) * static_cast<std::size_t> (frequencyTableSize));

	// read cumulative frequency table
	for (f = 1; f < frequencyTableSize; f++)
	{
		inputByteStream_arg.read(reinterpret_cast<char *> (&cFreqTable_[f]), frequencyTableByteSize);
		streamByteCount += frequencyTableByteSize;
	}

	// initialize range & code
	code = 0;
	low = 0;
	range = static_cast<uint64_t> (-1);

	// init code vector
	for (i = 0; i < 8; i++)
	{
		inputByteStream_arg.read(reinterpret_cast<char*> (&ch), sizeof(char));
		streamByteCount += sizeof(char);
		code = (code << 8) | ch;
	}

	// decoding
	for (i = 0; i < output_size; i++)
	{
		uint64_t count = (code - low) / (range /= cFreqTable_[static_cast<std::size_t> (frequencyTableSize - 1)]);

		// symbol lookup in cumulative frequency table
		uint64_t symbol = 0;
		uint64_t sSize = (frequencyTableSize - 1) / 2;
		while (sSize > 0)
		{
			if (cFreqTable_[static_cast<std::size_t> (symbol + sSize)] <= count)
			{
				symbol += sSize;
			}
			sSize /= 2;
		}

		// write symbol to output stream
		outputIntVector_arg[outputBufPos++] = static_cast<unsigned int> (symbol);

		// map to range
		low += cFreqTable_[static_cast<std::size_t> (symbol)] * range;
		range *= cFreqTable_[static_cast<std::size_t> (symbol + 1)] - cFreqTable_[static_cast<std::size_t> (symbol)];

		// check range limits
		while ((low ^ (low + range)) < top || ((range < bottom) && ((range = -int(low) & (bottom - 1)), 1)))
		{
			inputByteStream_arg.read(reinterpret_cast<char*> (&ch), sizeof(char));
			streamByteCount += sizeof(char);
			code = code << 8 | ch;
			range <<= 8;
			low <<= 8;
		}

	}

	return streamByteCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////
unsigned long
StaticRangeCoder::encodeCharVectorToStream(const std::vector<char>& inputByteVector_arg,
	std::ostream& outputByteStream_arg)
{
	DWord freq[257];
	uint8_t ch;
	int i, f;
	char out;

	// define numerical limits
	const DWord top = static_cast<DWord> (1) << 24;
	const DWord bottom = static_cast<DWord> (1) << 16;
	const DWord maxRange = static_cast<DWord> (1) << 16;

	DWord low, range;

	unsigned int input_size;
	input_size = static_cast<unsigned int> (inputByteVector_arg.size());

	unsigned int readPos;

	unsigned long streamByteCount;

	streamByteCount = 0;

	// init output vector
	outputCharVector_.clear();
	outputCharVector_.reserve(sizeof(char) * input_size);

	uint64_t FreqHist[257];

	// calculate frequency table
	memset(FreqHist, 0, sizeof(FreqHist));
	readPos = 0;
	while (readPos < input_size)
	{
		uint8_t symbol = static_cast<uint8_t> (inputByteVector_arg[readPos++]);
		FreqHist[symbol + 1]++;
	}

	// convert to cumulative frequency table
	freq[0] = 0;
	for (f = 1; f <= 256; f++)
	{
		freq[f] = freq[f - 1] + static_cast<DWord> (FreqHist[f]);
		if (freq[f] <= freq[f - 1])
			freq[f] = freq[f - 1] + 1;
	}

	// rescale if numerical limits are reached
	while (freq[256] >= maxRange)
	{
		for (f = 1; f <= 256; f++)
		{
			freq[f] /= 2;
			;
			if (freq[f] <= freq[f - 1])
				freq[f] = freq[f - 1] + 1;
		}
	}

	// write cumulative  frequency table to output stream
	outputByteStream_arg.write(reinterpret_cast<const char*> (&freq[0]), sizeof(freq));
	streamByteCount += sizeof(freq);

	readPos = 0;

	low = 0;
	range = static_cast<DWord> (-1);

	// start encoding
	while (readPos < input_size)
	{

		// read symol
		ch = inputByteVector_arg[readPos++];

		// map to range
		low += freq[ch] * (range /= freq[256]);
		range *= freq[ch + 1] - freq[ch];

		// check range limits
		while ((low ^ (low + range)) < top || ((range < bottom) && ((range = -int(low) & (bottom - 1)), 1)))
		{
			out = static_cast<char> (low >> 24);
			range <<= 8;
			low <<= 8;
			outputCharVector_.push_back(out);
		}

	}

	// flush remaining data
	for (i = 0; i < 4; i++)
	{
		out = static_cast<char> (low >> 24);
		outputCharVector_.push_back(out);
		low <<= 8;
	}

	// write encoded data to stream
	outputByteStream_arg.write(&outputCharVector_[0], outputCharVector_.size());

	streamByteCount += static_cast<unsigned long> (outputCharVector_.size());

	return (streamByteCount);
}

//////////////////////////////////////////////////////////////////////////////////////////////
unsigned long
StaticRangeCoder::decodeStreamToCharVector(std::istream& inputByteStream_arg,
	std::vector<char>& outputByteVector_arg)
{
	uint8_t ch;
	DWord freq[257];
	unsigned int i;

	// define range limits
	const DWord top = static_cast<DWord> (1) << 24;
	const DWord bottom = static_cast<DWord> (1) << 16;

	DWord low, range;
	DWord code;

	unsigned int outputBufPos;
	unsigned int output_size;

	unsigned long streamByteCount;

	streamByteCount = 0;

	output_size = static_cast<unsigned int> (outputByteVector_arg.size());

	outputBufPos = 0;

	// read cumulative frequency table
	//test
	int temp = sizeof(freq);
	//end test
	inputByteStream_arg.read(reinterpret_cast<char*> (&freq[0]), sizeof(freq));
	streamByteCount += sizeof(freq);

	code = 0;
	low = 0;
	range = static_cast<DWord> (-1);

	// init code
	for (i = 0; i < 4; i++)
	{
		inputByteStream_arg.read(reinterpret_cast<char*> (&ch), sizeof(char));
		streamByteCount += sizeof(char);
		code = (code << 8) | ch;
	}

	// decoding
	for (i = 0; i < output_size; i++)
	{
		// symbol lookup in cumulative frequency table
		uint8_t symbol = 0;
		uint8_t sSize = 256 / 2;

		DWord count = (code - low) / (range /= freq[256]);

		while (sSize > 0)
		{
			if (freq[symbol + sSize] <= count)
			{
				symbol = static_cast<uint8_t> (symbol + sSize);
			}
			sSize /= 2;
		}

		// write symbol to output stream
		outputByteVector_arg[outputBufPos++] = symbol;

		low += freq[symbol] * range;
		range *= freq[symbol + 1] - freq[symbol];

		// check range limits
		while ((low ^ (low + range)) < top || ((range < bottom) && ((range = -int(low) & (bottom - 1)), 1)))
		{
			inputByteStream_arg.read(reinterpret_cast<char*> (&ch), sizeof(char));
			streamByteCount += sizeof(char);
			code = code << 8 | ch;
			range <<= 8;
			low <<= 8;
		}

	}

	return (streamByteCount);
}

