#include "zjw_arithmetic_coder.h"

unsigned long AdaptiveArithmeticCoder::encodeCharVectorToStream(const std::vector<char>& inputByteVector_arg, std::ostream & outputByteStream_arg)
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

unsigned long AdaptiveArithmeticCoder::decodeStreamToCharVector(std::istream & inputByteStream_arg, std::vector<char>& outputByteVector_arg)
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