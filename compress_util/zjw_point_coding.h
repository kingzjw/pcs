#pragma once

#include <iterator>
#include <iostream>
#include <vector>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

//#define PCL_INSTANTIATE_ColorCoding(T) template class PCL_EXPORTS pcl::octree::ColorCoding<T>;

/** \brief @b PointCoding class
  * \note This class encodes 8-bit differential point information for octree-based point cloud compression.
  * \note typename: PointT: type of point used in pointcloud
  */
template<typename PointT = Vec3>
class PointCoding
{
public:
	/** \brief Constructor. */
	PointCoding() :pointDiffDataVector_(), pointDiffDataVectorIterator_(),
		pointCompressionResolution_(0.001f) // 1mm
	{
	}
	
	virtual	~PointCoding()
	{
	}
	
	inline void	setPrecision(float precision_arg)
	{
		pointCompressionResolution_ = precision_arg;
	}
	inline float getPrecision()
	{
		return (pointCompressionResolution_);
	}
	/** \brief ������frame�еĵ�ĸ���  */
	inline void	setPointCount(unsigned int pointCount_arg)
	{
		pointDiffDataVector_.reserve(pointCount_arg * 3);
	}

	/** \brief Initialize encoding of differential point */
	void initializeEncoding()
	{
		pointDiffDataVector_.clear();
	}

	/** \brief Initialize decoding of differential point */
	void initializeDecoding()
	{
		pointDiffDataVectorIterator_ = pointDiffDataVector_.begin();
	}

	/** \brief Get reference to vector containing differential color data */
	std::vector<char>& getDifferentialDataVector()
	{
		return (pointDiffDataVector_);
	}

	/** \brief ��ָ����һ��Ҷ�ӽڵ�����еĶ��㣬ѹ����vector��
	  * \param indexVector_arg ��һ��Ҷ�ӽڵ��к��е�Points�����
	  * \param referencePoint_arg ��һ��Ҷ�ӽڵ�����С�ĵ�
	  * \param inputCloud_arg input point cloud
	  */
	void encodePoints(const typename std::vector<int>& pointsInLeafNode, const PointT& referencePoint_arg,const vector<PointT>& frameVerList_in)
	{
		std::size_t i, len;

		len = pointsInLeafNode.size();

		// iterate over points within current voxel
		for (i = 0; i < len; i++)
		{
			unsigned char diffX, diffY, diffZ;

			// retrieve point from cloud
			const int& idx = pointsInLeafNode[i];
			const PointT& point = frameVerList_in[idx];

			// differentially encode point coordinates and truncate overflow
			diffX = static_cast<unsigned char> (max(-127, min<int>(127, static_cast<int> ((point.x - referencePoint_arg.x) / pointCompressionResolution_))));
			diffY = static_cast<unsigned char> (max(-127, min<int>(127, static_cast<int> ((point.y - referencePoint_arg.y) / pointCompressionResolution_))));
			diffZ = static_cast<unsigned char> (max(-127, min<int>(127, static_cast<int> ((point.z - referencePoint_arg.z) / pointCompressionResolution_))));

			// store information in differential point vector
			pointDiffDataVector_.push_back(diffX);
			pointDiffDataVector_.push_back(diffY);
			pointDiffDataVector_.push_back(diffZ);
		}
	}

	/** \brief �ӵ�ǰ��pointDiffDataVectorIterator_λ�ã���ʼ��ѹ�����Ϣ
	  * \param outputCloud_arg output point cloud
	  * \param referencePoint_arg ��һ��Ҷ�ӽڵ�����С�ĵ�
	  * \param beginIdx_arg index ��endIdx_arg��ʾһ��voxelѹ���ļ��е��λ�õ������ţ����յ����
	  * \param  
	  */
	void decodePoints(vector<PointT>& frameVerList_out, const PointT& referencePoint_arg, std::size_t beginIdx_arg, std::size_t endIdx_arg)
	{
		std::size_t i;
		unsigned int pointCount;

		assert(beginIdx_arg <= endIdx_arg);

		//��һ����Ҫ��ѹ�ĵ�ĸ���
		pointCount = static_cast<unsigned int> (endIdx_arg - beginIdx_arg);

		// iterate over points within current voxel
		for (i = 0; i < pointCount; i++)
		{
			// retrieve differential point information
			const unsigned char& diffX = static_cast<unsigned char> (*(pointDiffDataVectorIterator_++));
			const unsigned char& diffY = static_cast<unsigned char> (*(pointDiffDataVectorIterator_++));
			const unsigned char& diffZ = static_cast<unsigned char> (*(pointDiffDataVectorIterator_++));

			// �ָ�����ڵ��У�������λ��
			//PointT &point = outputCloud_arg->points[beginIdx_arg + i];
			PointT point;
			// decode point position
			point.x = static_cast<float> (referencePoint_arg[0] + diffX * pointCompressionResolution_);
			point.y = static_cast<float> (referencePoint_arg[1] + diffY * pointCompressionResolution_);
			point.z = static_cast<float> (referencePoint_arg[2] + diffZ * pointCompressionResolution_);

			frameVerList_out.push_back(point);
		}
	}

protected:
	
	/** \brief Vector for storing differential point information ������ point num������ */
	std::vector<char> pointDiffDataVector_;

	/** \brief Iterator on differential point information vector ��¼��ǰ��λ����Ϣ */
	std::vector<char>::const_iterator pointDiffDataVectorIterator_;

	/** \brief Precision of point coding*/
	float pointCompressionResolution_;
};
