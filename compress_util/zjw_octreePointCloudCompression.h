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

	/** \brief Synchronize to frame header decoder��߶��� frame_header_identifier_ �Ƚ��м��
	* \param compressed_tree_data_in_arg: binary input stream
	*/
	void syncToHeader(std::istream& compressed_tree_data_in_arg);
	/** \brief Read frame information to output stream
	* \param compressed_tree_data_in_arg: binary input stream
	*/
	void readFrameHeader(std::istream& compressed_tree_data_in_arg);


	/** \brief �����ر����binary_tree_data_vector_ �е�Byte stream������color info , pos info���б��룬�洢���ļ���*/
	void entropyEncoding(std::ostream& compressed_tree_data_out_arg);

	/** \brief Entropy decoding of input binary stream and output to information vectors
	* \param compressed_tree_data_in_arg: binary input stream
	*/
	void entropyDecoding(std::istream& compressed_tree_data_in_arg);

	/** \brief Encode leaf node information during serialization*/
	virtual void serializeTreeForPosAndColor(bool isTarget);

	//brief Decode leaf nodes information during deserialization
	//���ݵõ���color pos and byteStream����Ϣ�����лָ��ؽ��˲��������Ƶ�λ�ú���ɫ��Ϣ
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

	/** \brief �洢ÿ��Ҷ�ӽڵ��к��е�points�ĸ���*/
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


	//30֡ÿ�룬��һ֡��ΪI-frame���������ΪP-frame
	uint32_t i_frame_rate_;
	//����frame rate�е�һ�μ���
	uint32_t i_frame_counter_;

	//Number of encoded points
	uint64_t point_count_;

	//��ʾ�Ƿ���Ҷ�ӽڵ��е�diff��Ϣ,false ��ʾ��diff
	bool do_voxel_grid_enDecoding_;
	bool do_color_encoding_;
	
	//��ʾ�Ƿ���Ҫencode averaged voxel color information
	bool cloud_with_color_;
	bool data_with_color_;
	unsigned char point_color_offset_;

	//��־�Ƿ���ʾ������Ϣ��
	bool b_show_statistics_;
	//��¼point��Ϣ��ѹ����ĳ���
	uint64_t compressed_point_data_len_;
	//��¼color��Ϣ��ѹ����ĳ���
	uint64_t compressed_color_data_len_;
	
	//ѹ�������ļ�������
	const compression_Profiles_e selected_profile_;
	//�˲�����εľ��ȣ���Ӧ���ҵİ˲����о���cellsize
	double octree_resolution_;
	//voxel�е�pos diffʱ��ľ���
	const double point_resolution_;
	//color bit
	const unsigned char color_bit_resolution_;

	//���������еĵ�ĸ���
	std::size_t object_count_;
};

const char* OctreePointCloudCompression::frame_header_identifier_ = "<PCL-OCT-COMPRESSED>";