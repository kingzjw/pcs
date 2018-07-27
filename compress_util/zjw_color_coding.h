#pragma once

#include <iterator>
#include <iostream>
#include <vector>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief @b ColorCoding class
 *  \note This class encodes 8-bit color information for octree-based point cloud compression.
 *  \note typename: PointT: type of point used in pointcloud
 */
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename PointT>
class ColorCoding
{
public:

	ColorCoding() : pointAvgColorDataVector_(), pointAvgColorDataVector_Iterator_(),
		pointDiffColorDataVector_(), pointDiffColorDataVector_Iterator_(), colorBitReduction_(0)
	{	}

	virtual	~ColorCoding() {	}

	/** \brief Define color bit depth of encoded color information.
	  * \param bitDepth_arg: amounts of bits for representing one color component
	  */
	inline	void setBitDepth(unsigned char bitDepth_arg)
	{
		colorBitReduction_ = static_cast<unsigned char> (8 - bitDepth_arg);
	}

	/** \brief Retrieve color bit depth of encoded color information.
	  * \return amounts of bits for representing one color component
	  */
	inline unsigned char getBitDepth()
	{
		return (static_cast<unsigned char> (8 - colorBitReduction_));
	}

	/** \brief Set amount of voxels containing point color information and reserve memory
	  * \param voxelCount_arg: amounts of voxels 叶子节点的数量
	  */
	inline void	setVoxelCount(unsigned int voxelCount_arg)
	{
		pointAvgColorDataVector_.reserve(voxelCount_arg * 3);
	}

	/** \brief Set amount of points within point cloud to be encoded and reserve memory
	 *  \param pointCount_arg: amounts of points within point cloud 点云中的点的数量
	 * */
	inline	void setPointCount(unsigned int pointCount_arg)
	{
		pointDiffColorDataVector_.reserve(pointCount_arg * 3);
	}

	/** \brief Initialize encoding of color information
	 * */
	void initializeEncoding()
	{
		pointAvgColorDataVector_.clear();
		pointDiffColorDataVector_.clear();
	}

	/** \brief Initialize decoding of color information
	 * */
	void initializeDecoding()
	{
		pointAvgColorDataVector_Iterator_ = pointAvgColorDataVector_.begin();
		pointDiffColorDataVector_Iterator_ = pointDiffColorDataVector_.begin();
	}

	/** \brief Get reference to vector containing averaged color data
	 * */
	std::vector<char>&	getAverageDataVector()
	{
		return pointAvgColorDataVector_;
	}

	/** \brief Get reference to vector containing differential color data
	 * */
	std::vector<char>&	getDifferentialDataVector()
	{
		return pointDiffColorDataVector_;
	}

	/** \brief 编码一个叶子节点的averaged color information
	 * \param indexVector_arg 一个叶子节点上的所有的color信息的序号
	 * \param frameColorList_in中的值是从0-1的
	 * */
	void encodeAverageOfPoints(const typename std::vector<int>& indexVector_arg,  vector<PointT>& frameColorList_in)
	{
		std::size_t i, len;

		unsigned int avgRed;
		unsigned int avgGreen;
		unsigned int avgBlue;

		// initialize
		avgRed = avgGreen = avgBlue = 0;

		// iterate over points
		len = indexVector_arg.size();
		for (i = 0; i < len; i++)
		{
			// get color information from points
			const int& idx = indexVector_arg[i];
			
			// add color information
			avgRed += int(frameColorList_in[idx].x * 255);
			avgGreen += int(frameColorList_in[idx].y * 255);
			avgBlue += int(frameColorList_in[idx].z * 255);

		}

		// calculated average color information
		if (len > 1)
		{
			avgRed /= static_cast<unsigned int> (len);
			avgGreen /= static_cast<unsigned int> (len);
			avgBlue /= static_cast<unsigned int> (len);
		}

		// remove least significant bits
		avgRed >>= colorBitReduction_;
		avgGreen >>= colorBitReduction_;
		avgBlue >>= colorBitReduction_;

		// add to average color vector
		pointAvgColorDataVector_.push_back(static_cast<char> (avgRed));
		pointAvgColorDataVector_.push_back(static_cast<char> (avgGreen));
		pointAvgColorDataVector_.push_back(static_cast<char> (avgBlue));
	}

	/** \brief 同时计算color avg and color diff信息保存到 相关的vector中
	 * \param indexVector_arg indices defining a subset of points from points cloud
	 * \param rgba_offset_arg offset to color information
	 * \param inputCloud_arg input point cloud
	 * */
	void encodePoints(const typename std::vector<int>& indexVector_arg, const vector<PointT>& frameColorList_in)
	{
		std::size_t i, len;

		unsigned int avgRed;
		unsigned int avgGreen;
		unsigned int avgBlue;

		// initialize
		avgRed = avgGreen = avgBlue = 0;

		// iterate over points
		len = indexVector_arg.size();
		for (i = 0; i < len; i++)
		{
			// get color information from points
			const int& idx = indexVector_arg[i];

			// add color information
			avgRed += int(frameColorList_in[idx].x * 255);
			avgGreen += int(frameColorList_in[idx].y * 255);
			avgBlue += int(frameColorList_in[idx].z * 255);
		}

		if (len > 1)
		{
			unsigned char diffRed;
			unsigned char diffGreen;
			unsigned char diffBlue;

			// calculated average color information
			avgRed /= static_cast<unsigned int> (len);
			avgGreen /= static_cast<unsigned int> (len);
			avgBlue /= static_cast<unsigned int> (len);

			// iterate over points for differential encoding
			for (i = 0; i < len; i++)
			{
				// get color information from points
				const int& idx = indexVector_arg[i];

				// add color information
				avgRed += int(frameColorList_in[idx].x * 255);
				avgGreen += int(frameColorList_in[idx].y * 255);
				avgBlue += int(frameColorList_in[idx].z * 255);

				// extract color components and do XOR encoding with predicted average color
				diffRed = (static_cast<unsigned char> (avgRed)) ^ static_cast<unsigned char> (int(frameColorList_in[idx].x * 255));
				diffGreen = (static_cast<unsigned char> (avgGreen)) ^ static_cast<unsigned char> (int(frameColorList_in[idx].y * 255));
				diffBlue = (static_cast<unsigned char> (avgBlue)) ^ static_cast<unsigned char> (int(frameColorList_in[idx].z * 255));

				// remove least significant bits
				diffRed = static_cast<unsigned char> (diffRed >> colorBitReduction_);
				diffGreen = static_cast<unsigned char> (diffGreen >> colorBitReduction_);
				diffBlue = static_cast<unsigned char> (diffBlue >> colorBitReduction_);

				// add to differential color vector
				pointDiffColorDataVector_.push_back(static_cast<char> (diffRed));
				pointDiffColorDataVector_.push_back(static_cast<char> (diffGreen));
				pointDiffColorDataVector_.push_back(static_cast<char> (diffBlue));
			}
		}

		// remove least significant bits from average color information
		avgRed >>= colorBitReduction_;
		avgGreen >>= colorBitReduction_;
		avgBlue >>= colorBitReduction_;

		// add to differential color vector
		pointAvgColorDataVector_.push_back(static_cast<char> (avgRed));
		pointAvgColorDataVector_.push_back(static_cast<char> (avgGreen));
		pointAvgColorDataVector_.push_back(static_cast<char> (avgBlue));

	}

	/** \brief 从当前的pointDiffDataVectorIterator_位置，开始解压点的信息
	* \param beginIdx_arg index 和endIdx_arg表示一个voxel压缩文件中点的位置的起点序号，和终点序号
	* \param rgba_offset_arg offset to color information
	*/
	void decodePoints(vector<PointT>& frameColorList_out, std::size_t beginIdx_arg, std::size_t endIdx_arg)
	{
		std::size_t i;
		unsigned int pointCount;
		unsigned int colorInt;

		assert(beginIdx_arg <= endIdx_arg);

		// amount of points to be decoded
		pointCount = static_cast<unsigned int> (endIdx_arg - beginIdx_arg);

		// get averaged color information for current voxel
		unsigned char avgRed = *(pointAvgColorDataVector_Iterator_++);
		unsigned char avgGreen = *(pointAvgColorDataVector_Iterator_++);
		unsigned char avgBlue = *(pointAvgColorDataVector_Iterator_++);

		// invert bit shifts during encoding
		avgRed = static_cast<unsigned char> (avgRed << colorBitReduction_);
		avgGreen = static_cast<unsigned char> (avgGreen << colorBitReduction_);
		avgBlue = static_cast<unsigned char> (avgBlue << colorBitReduction_);

		// iterate over points
		for (i = 0; i < pointCount; i++)
		{
			Vec3 tempColor;
			if (pointCount > 1)
			{
				// get differential color information from input vector
				unsigned char diffRed = static_cast<unsigned char> (*(pointDiffColorDataVector_Iterator_++));
				unsigned char diffGreen = static_cast<unsigned char> (*(pointDiffColorDataVector_Iterator_++));
				unsigned char diffBlue = static_cast<unsigned char> (*(pointDiffColorDataVector_Iterator_++));

				// invert bit shifts during encoding
				diffRed = static_cast<unsigned char> (diffRed << colorBitReduction_);
				diffGreen = static_cast<unsigned char> (diffGreen << colorBitReduction_);
				diffBlue = static_cast<unsigned char> (diffBlue << colorBitReduction_);

				// decode color information
				tempColor.x = diffRed / 255.0;
				tempColor.y = diffGreen / 255.0;
				tempColor.z = diffBlue / 255.0;
			}
			else
			{
				// decode color information
				//colorInt = (avgRed << 0) | (avgGreen << 8) | (avgBlue << 16);
				
				tempColor.x = avgRed / 255.0;
				tempColor.y = avgGreen / 255.0;
				tempColor.z = avgBlue / 255.0;
			}
			
			frameColorList_out.push_back(tempColor);
			//char* idxPointPtr = reinterpret_cast<char*> (frameColorList_out[beginIdx_arg + i]);
			////int& pointColor = *reinterpret_cast<int*> (idxPointPtr + rgba_offset_arg);
			//int& pointColor = *reinterpret_cast<int*> (idxPointPtr + rgba_offset_arg);
			//// assign color to point from point cloud
			//pointColor = colorInt;
		}
	}

	/** \brief Set default color to points
	 * \param outputCloud_arg output point cloud
	 * \param beginIdx_arg index indicating first point to be assigned with color information
	 * \param endIdx_arg index indicating last point to be assigned with color information
	 * \param rgba_offset_arg offset to color information
	 * */
	void setDefaultColor(vector<PointT>& frameColorList_out, std::size_t beginIdx_arg, std::size_t endIdx_arg)
	{
		std::size_t i;
		unsigned int pointCount;

		assert(beginIdx_arg <= endIdx_arg);

		// amount of points to be decoded
		pointCount = static_cast<unsigned int> (endIdx_arg - beginIdx_arg);

		// iterate over points
		for (i = 0; i < pointCount; i++)
		{
			frameColorList_out.push_back(Vec3(1.0, 1.0, 1.0));
		}
	}


protected:

	/** \brief Vector for storing average color information  */
	std::vector<char> pointAvgColorDataVector_;

	/** \brief Iterator on average color information vector */
	std::vector<char>::const_iterator pointAvgColorDataVector_Iterator_;

	/** \brief Vector for storing differential color information  */
	std::vector<char> pointDiffColorDataVector_;

	/** \brief Iterator on differential color information vector */
	std::vector<char>::const_iterator pointDiffColorDataVector_Iterator_;

	/** \brief Amount of bits to be removed from color components before encoding */
	unsigned char colorBitReduction_;

	// 默认的color的颜色再下面已经定义好了
	static const int defaultColor_;
};

//#define PCL_INSTANTIATE_ColorCoding(T) template class PCL_EXPORTS pcl::octree::ColorCoding<T>;

