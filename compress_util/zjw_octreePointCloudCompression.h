#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "zjw_macro.h"
#include "zjw_entropy_range_coder.h"
#include "zjw_pcs_double_buffer_octree.h"
#include "zjw_octree_compression_profiles.h"
#include "zjw_point_coding.h"
#include "zjw_color_coding.h"

/** \brief  Octree pointcloud compression class
*  \note This class enables compression and decompression of point cloud data based on octree data structures.
*  \author Jiawei
*/
class OctreePointCloudCompression
{
public:
	OctreePointCloudCompression(compression_Profiles_e compressionProfile_arg = MANUAL_CONFIGURATION,
		bool showStatistics_arg = true, const double pointResolution_arg = 0.001,
		const double octreeResolution_arg = 0.01,bool doVoxelGridDownDownSampling_arg = false,
		const unsigned int iFrameRate_arg = 30,	bool doColorEncoding_arg = false,
		const unsigned char colorBitResolution_arg = 6);

	virtual ~OctreePointCloudCompression();

public:
	void initialization();
	/** \brief Encode point cloud to output stream
	* \param compressed_tree_data_out_arg:  binary output stream containing compressed data
	*/
	void encodePointCloud(ObjMesh& frameObj, std::ostream& compressed_tree_data_out_arg);

	/** \brief Decode point cloud from input stream
	* \param compressed_tree_data_in_arg: binary input stream containing compressed data
	* \param cloud_arg: reference to decoded point cloud
	*/
	void decodePointCloud(ObjMesh& frameObj_out , std::istream& compressed_tree_data_in_arg);

protected:

	/** \brief Write frame information to output stream
	* \param compressed_tree_data_out_arg: binary output stream
	*/
	void writeFrameHeader(std::ostream& compressed_tree_data_out_arg);

	/** \brief Synchronize to frame header decoder这边读出 frame_header_identifier_ 先进行检测
	* \param compressed_tree_data_in_arg: binary input stream
	*/
	void syncToHeader(std::istream& compressed_tree_data_in_arg);
	/** \brief Read frame information to output stream
	* \param compressed_tree_data_in_arg: binary input stream
	*/
	void readFrameHeader(std::istream& compressed_tree_data_in_arg);


	/** \brief 利用熵编码对binary_tree_data_vector_ 中的Byte stream，还有color info , pos info进行编码，存储到文件中*/
	void entropyEncoding(std::ostream& compressed_tree_data_out_arg);

	/** \brief Entropy decoding of input binary stream and output to information vectors
	* \param compressed_tree_data_in_arg: binary input stream
	*/
	void entropyDecoding(std::istream& compressed_tree_data_in_arg);

	/** \brief Encode leaf node information during serialization*/
	virtual void serializeTreeForPosAndColor(bool isTarget);

	//brief Decode leaf nodes information during deserialization
	//根据得到的color pos and byteStream的信息，进行恢复重建八叉树，点云的位置和颜色信息
	virtual void deserializeTreeForPosAndColor(std::vector<Vec3>& verPosList_out, bool isTarget);

private:

	double getResolution();
	void setResolution( double octreeResolution);
	
	void getBoundingBox(double &min_x, double &min_y, double &min_z, double & max_x, double & max_y, double &max_z);
	void setBoundingBox(const double &min_x, const double &min_y, const double &min_z,
		const double & max_x, const double & max_y, const double &max_z);

	std::size_t getFramePointNum(bool isTarget = false) const;

protected:

	/** \brief Pointer to output point cloud dataset. */
	PcsDBufferOctree *dbOctree;

	/** \brief Vector for storing binary tree structure (byte stream)*/
	std::vector<char> binary_tree_data_vector_;

	/** \brief Interator on binary tree structure vector */
	std::vector<char> binary_color_tree_vector_;

	/** \brief 存储每个叶子节点中含有的points的个数*/
	std::vector<unsigned int> point_count_data_vector_;

	/** \brief Interator on points per voxel vector */
	std::vector<unsigned int>::const_iterator point_count_data_vector_iterator_;

	//three coder methods
	ColorCoding<Vec3> color_coder_;
	PointCoding<Vec3> point_coder_;
	StaticRangeCoder entropy_coder_;

	// frame info
	static const char* frame_header_identifier_;
	uint32_t frame_ID_;
	//i_frame or p_frame
	bool i_frame_;


	//30帧每秒，第一帧作为I-frame，后面的作为P-frame
	uint32_t i_frame_rate_;
	//对是frame rate中的一次计数
	uint32_t i_frame_counter_;

	//Number of encoded points
	uint64_t point_count_;

	//表示是否做叶子节点中的diff信息,false 表示做diff
	bool do_voxel_grid_enDecoding_;
	bool do_color_encoding_;
	
	//表示是否需要encode averaged voxel color information
	bool cloud_with_color_;
	bool data_with_color_;
	unsigned char point_color_offset_;

	//标志是否显示过程信息。
	bool b_show_statistics_;
	//记录point信息，压缩后的长度
	uint64_t compressed_point_data_len_;
	//记录color信息，压缩后的长度
	uint64_t compressed_color_data_len_;
	
	//压缩配置文件的类型
	const compression_Profiles_e selected_profile_;
	//八叉树层次的精度，对应到我的八叉树中就是cellsize
	double octree_resolution_;
	//voxel中的pos diff时候的精度
	const double point_resolution_;
	//color bit
	const unsigned char color_bit_resolution_;

	//点云中所有的点的个数
	std::size_t object_count_;
};

const char* OctreePointCloudCompression::frame_header_identifier_ = "<PCL-OCT-COMPRESSED>";