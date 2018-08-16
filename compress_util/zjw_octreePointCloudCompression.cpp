#include "zjw_octreePointCloudCompression.h"

OctreePointCloudCompressionZjw::OctreePointCloudCompressionZjw(compression_Profiles_e compressionProfile_arg,
	bool showStatistics_arg, const double pointResolution_arg, const double octreeResolution_arg,
	bool doVoxelGridDownDownSampling_arg, const unsigned int iFrameRate_arg, bool doColorEncoding_arg,
	const unsigned char colorBitResolution_arg) :
	binary_tree_data_vector_(),
	binary_color_tree_vector_(),
	point_count_data_vector_(),
	point_count_data_vector_iterator_(),
	color_coder_(),
	point_coder_(),
	entropy_coder_(),
	do_voxel_grid_enDecoding_(doVoxelGridDownDownSampling_arg),
	i_frame_rate_(iFrameRate_arg),
	i_frame_counter_(0),
	frame_ID_(0),
	point_count_(0),
	i_frame_(true),
	do_color_encoding_(doColorEncoding_arg),
	cloud_with_color_(false),
	data_with_color_(false),
	point_color_offset_(0),
	b_show_statistics_(showStatistics_arg),
	compressed_point_data_len_(),
	compressed_color_data_len_(),
	selected_profile_(compressionProfile_arg),
	point_resolution_(pointResolution_arg),
	octree_resolution_(octreeResolution_arg),
	color_bit_resolution_(colorBitResolution_arg),
	object_count_(0)
{
	dbOctree = new PcsDBufferOctree();
	setResolution(octreeResolution_arg);
	frame_header_identifier_ = "<PCL-OCT-COMPRESSED>";
	initialization();
}

OctreePointCloudCompressionZjw::~OctreePointCloudCompressionZjw() {
	if (dbOctree)
		delete dbOctree;
}

void OctreePointCloudCompressionZjw::useCase0_Encoder()
{
	//pcf: point cloud frame byte stream 修改
	std::ofstream of("frameCompressData.pcf", std::ios_base::binary);
	if (of)
	{
		cout << "open the frameCompressData.pcf " << endl;
	}
	
	//reference frame
	ObjMesh frameObj_ref;
	string path = "E://1.study//pointCloud//code//pcsCompress//pcsCompress//testData//walk_foot_rotate3//walkTexture_0_0.obj";
	frameObj_ref.loadObjMesh(path);

	this->encodePointCloud(frameObj_ref, of);
	of.close();
}

void OctreePointCloudCompressionZjw::useCase0_Decoder()
{
	std::ifstream in("frameCompressData.pcf", std::ios_base::binary);
	if (in)
	{
		cout << "open the frameCompressData.pcf " << endl;
	}

	//reference frame
	ObjMesh frameObj_ref_out;
	string path = "E://1.study//pointCloud//code//pcsCompress//pcsCompress//testData//walk_foot_rotate3//walkTexture_0_0.obj";
	this->decodePointCloud(frameObj_ref_out, in);

	in.close();
}

void OctreePointCloudCompressionZjw::initialization() {
	if (selected_profile_ != MANUAL_CONFIGURATION)
	{
		// apply selected compression profile

		// retrieve profile settings
		const configurationProfile_t selectedProfile = compressionProfiles_[selected_profile_];

		// apply profile settings
		i_frame_rate_ = selectedProfile.iFrameRate;
		do_voxel_grid_enDecoding_ = selectedProfile.doVoxelGridDownSampling;
		this->setResolution(selectedProfile.octreeResolution);
		point_coder_.setPrecision(static_cast<float> (selectedProfile.pointResolution));
		do_color_encoding_ = selectedProfile.doColorEncoding;
		color_coder_.setBitDepth(selectedProfile.colorBitResolution);

	}
	else
	{
		// configure point & color coder
		point_coder_.setPrecision(static_cast<float> (point_resolution_));
		color_coder_.setBitDepth(color_bit_resolution_);
	}

	if (point_coder_.getPrecision() == this->getResolution())
		//disable differential point coldingm
		do_voxel_grid_enDecoding_ = true;

}

void OctreePointCloudCompressionZjw::encodePointCloud(ObjMesh& frameObj, std::ostream & compressed_tree_data_out_arg)
{

	//init 确定是否需要编码颜色
	cloud_with_color_ = false;

	//根据i_frame_rate_来分辨referenframe 或者是target frame
	if (i_frame_counter_++ == i_frame_rate_)
	{
		i_frame_counter_ = 0;
		i_frame_ = true;
	}
	// increase frameID
	frame_ID_++;

	if (i_frame_)
	{
		//reference frame buildDBufferOctree会自己check
		dbOctree->clearDBufferOctree();

		dbOctree->buildDBufferOctree(false, &frameObj);
		//得到当前，double buffer octree上的叶子节点上的信息
		dbOctree->getLeafboundaryAndLeafNode(false);
	}
	else
	{
		//p-frame
		assert(dbOctree->referenceFrameLoaded);
		dbOctree->buildDBufferOctree(true, &frameObj);
		//得到当前，double buffer octree上的叶子节点上的信息
		dbOctree->getLeafboundaryAndLeafNode(true);
	}

	// do octree encoding
	if (!do_voxel_grid_enDecoding_)
	{
		point_count_data_vector_.clear();
		//叶子节点的个数
		point_count_data_vector_.reserve(dbOctree->ctLeaf->nodeList.size());
	}

	// initialize color encoding
	color_coder_.initializeEncoding();
	// initialize point encoding
	point_coder_.initializeEncoding();

	// serialize octree，得到当前frame的Byte stream 
	if (i_frame_)
	{
		color_coder_.setPointCount(static_cast<unsigned int> (dbOctree->refFramePointNum));
		color_coder_.setVoxelCount(static_cast<unsigned int> (dbOctree->refre_leaf_count_));
		point_coder_.setPointCount(static_cast<unsigned int> (dbOctree->refFramePointNum));
		// i-frame encoding - encode tree structure without referencing previous buffer
		dbOctree->getByteSteamOfOctreeSigleFrame(binary_tree_data_vector_, false);
		//avg color, color diff ，pos diff的压缩
		serializeTreeForPosAndColor(false);
	}
	else
	{
		color_coder_.setPointCount(static_cast<unsigned int> (dbOctree->targetFramePointNum));
		color_coder_.setVoxelCount(static_cast<unsigned int> (dbOctree->target_leaf_count_));
		point_coder_.setPointCount(static_cast<unsigned int> (dbOctree->targetFramePointNum));
		// p-frame encoding - XOR encoded tree structure
		dbOctree->getByteSteamOfOctreeXOR(binary_tree_data_vector_);
		//avg color, color diff ，pos diff的压缩
		serializeTreeForPosAndColor(true);
	}

	// ------------write frame header information to stream----------------
	this->writeFrameHeader(compressed_tree_data_out_arg);

	// ------------apply entropy coding to the content of all data vectors and send data to output stream------------
	this->entropyEncoding(compressed_tree_data_out_arg);

	// reset object count
	object_count_ = 0;

	if (b_show_statistics_)
	{
		float bytes_per_XYZ = static_cast<float> (compressed_point_data_len_) / static_cast<float> (point_count_);
		float bytes_per_color = static_cast<float> (compressed_color_data_len_) / static_cast<float> (point_count_);

		printf("*** POINTCLOUD ENCODING ***\n");
		printf("Frame ID: %d\n", frame_ID_);
		if (i_frame_)
			printf("Encoding Frame: Intra frame\n");
		else
			printf("Encoding Frame: Prediction frame\n");
		printf("Number of encoded points: %ld\n", point_count_);
		printf("XYZ compression percentage: %f%%\n", bytes_per_XYZ / (3.0f * sizeof(float)) * 100.0f);
		printf("XYZ bytes per point: %f bytes\n", bytes_per_XYZ);
		printf("Color compression percentage: %f%%\n", bytes_per_color / (sizeof(int)) * 100.0f);
		printf("Color bytes per point: %f bytes\n", bytes_per_color);
		printf("Size of uncompressed point cloud: %f kBytes\n", static_cast<float> (point_count_) * (sizeof(int) + 3.0f * sizeof(float)) / 1024.0f);
		printf("Size of compressed point cloud: %f kBytes\n", static_cast<float> (compressed_point_data_len_ + compressed_color_data_len_) / 1024.0f);
		printf("Total bytes per point: %f bytes\n", bytes_per_XYZ + bytes_per_color);
		printf("Total compression percentage: %f%%\n", (bytes_per_XYZ + bytes_per_color) / (sizeof(int) + 3.0f * sizeof(float)) * 100.0f);
		printf("Compression ratio: %f\n\n", static_cast<float> (sizeof(int) + 3.0f * sizeof(float)) / static_cast<float> (bytes_per_XYZ + bytes_per_color));
	}

	i_frame_ = false;
}

void OctreePointCloudCompressionZjw::decodePointCloud(ObjMesh& frameObj_out, std::istream & compressed_tree_data_in_arg)
{

	// synchronize to frame header
	syncToHeader(compressed_tree_data_in_arg);

	// color info
	cloud_with_color_ = false;

	// read header from input stream
	this->readFrameHeader(compressed_tree_data_in_arg);

	// decode data vectors from stream,然后存储在相应的变量中
	this->entropyDecoding(compressed_tree_data_in_arg);

	// initialize color and point encoding
	color_coder_.initializeDecoding();
	point_coder_.initializeDecoding();

	//init obj 
	frameObj_out.colorList.clear();
	frameObj_out.vertexList.clear();
	// initialize output cloud
	frameObj_out.vertexList.resize(point_count_);


	if (i_frame_)
	{
		// initialize octree
		dbOctree->clearDBufferOctree();

		// i-frame decoding - decode tree structure without referencing previous buffer
		// false 表示i-frame
		this->deserializeTreeForPosAndColor(frameObj_out.vertexList, false);
	}
	else
	{
		// p-frame decoding - decode XOR encoded tree structure
		// ture 表示p-frame
		this->deserializeTreeForPosAndColor(frameObj_out.vertexList, true);
	}

	if (b_show_statistics_)
	{
		float bytes_per_XYZ = static_cast<float> (compressed_point_data_len_) / static_cast<float> (point_count_);
		float bytes_per_color = static_cast<float> (compressed_color_data_len_) / static_cast<float> (point_count_);

		printf("*** POINTCLOUD DECODING ***\n");
		printf("Frame ID: %d\n", frame_ID_);
		if (i_frame_)
			printf("Decoding Frame: Intra frame\n");
		else
			printf("Decoding Frame: Prediction frame\n");
		printf("Number of decoded points: %ld\n", point_count_);
		printf("XYZ compression percentage: %f%%\n", bytes_per_XYZ / (3.0f * sizeof(float)) * 100.0f);
		printf("XYZ bytes per point: %f bytes\n", bytes_per_XYZ);
		printf("Color compression percentage: %f%%\n", bytes_per_color / (sizeof(int)) * 100.0f);
		printf("Color bytes per point: %f bytes\n", bytes_per_color);
		printf("Size of uncompressed point cloud: %f kBytes\n", static_cast<float> (point_count_) * (sizeof(int) + 3.0f * sizeof(float)) / 1024.0f);
		printf("Size of compressed point cloud: %f kBytes\n", static_cast<float> (compressed_point_data_len_ + compressed_color_data_len_) / 1024.0f);
		printf("Total bytes per point: %f bytes\n", bytes_per_XYZ + bytes_per_color);
		printf("Total compression percentage: %f%%\n", (bytes_per_XYZ + bytes_per_color) / (sizeof(int) + 3.0f * sizeof(float)) * 100.0f);
		printf("Compression ratio: %f\n\n", static_cast<float> (sizeof(int) + 3.0f * sizeof(float)) / static_cast<float> (bytes_per_XYZ + bytes_per_color));
	}
}

void OctreePointCloudCompressionZjw::setResolution(double  octreeResolution)
{
	assert(dbOctree);
	dbOctree->setCellSize(octreeResolution);
}

double OctreePointCloudCompressionZjw::getResolution()
{
	assert((dbOctree->cellSize.x == dbOctree->cellSize.y )&&( dbOctree->cellSize.x == dbOctree->cellSize.z));
	return dbOctree->cellSize.x;
}

void OctreePointCloudCompressionZjw::getBoundingBox(double & min_x, double & min_y, double & min_z, double & max_x, double & max_y, double & max_z)
{
	dbOctree->getBoundingBox(min_x, min_y, min_z, max_x, max_y, max_z);
}

void OctreePointCloudCompressionZjw::setBoundingBox(const double & min_x, const double & min_y, const double & min_z, const double & max_x, const double & max_y, const double & max_z)
{
	dbOctree->setBoundingBox(min_x, min_y, min_z, max_x, max_y, max_z);
}

std::size_t OctreePointCloudCompressionZjw::getFramePointNum(bool isTarget) const
{
	if (isTarget)
	{
		return dbOctree->targetFramePointNum;
	}
	else
	{
		return dbOctree->refFramePointNum;
	}
}

void OctreePointCloudCompressionZjw::writeFrameHeader(std::ostream & compressed_tree_data_out_arg)
{
	// encode header identifier
	compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (frame_header_identifier_), strlen(frame_header_identifier_));
	// encode point cloud header id
	compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&frame_ID_), sizeof(frame_ID_));
	// encode frame type (I/P-frame)
	compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&i_frame_), sizeof(i_frame_));
	if (i_frame_)
	{
		double min_x, min_y, min_z, max_x, max_y, max_z;
		double octree_resolution;
		unsigned char color_bit_depth;
		double point_resolution;

		// get current configuration
		octree_resolution = this->getResolution();
		color_bit_depth = color_coder_.getBitDepth();
		point_resolution = point_coder_.getPrecision();
		dbOctree->getBoundingBox(min_x, min_y, min_z, max_x, max_y, max_z);

		// encode amount of points
		if (do_voxel_grid_enDecoding_)
		{
			//不做叶子节点上的 Point diff ,那么Point_cout_ 表示的是叶子节点的个数
			if (i_frame_)
			{
				//not target
				point_count_ = dbOctree->refre_leaf_count_;
			}
			else
			{
				//p frame is target frame
				point_count_ = dbOctree->target_leaf_count_;
			}
		}
		else
		{
			//做叶子节点上的 Point diff ,那么Point_cout_ 表示的是点云帧中的点的个数
			//拿到reference 点云上的点的个数
			if (i_frame_)
			{
				//not target
				object_count_ = getFramePointNum(false);
			}
			else
			{
				//p frame is target frame
				object_count_ = getFramePointNum(true);
			}

			point_count_ = this->object_count_;
		}

		// encode coding configuration
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&do_voxel_grid_enDecoding_), sizeof(do_voxel_grid_enDecoding_));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&cloud_with_color_), sizeof(cloud_with_color_));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&point_count_), sizeof(point_count_));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&octree_resolution), sizeof(octree_resolution));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&color_bit_depth), sizeof(color_bit_depth));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&point_resolution), sizeof(point_resolution));

		// encode octree bounding box
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&min_x), sizeof(min_x));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&min_y), sizeof(min_y));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&min_z), sizeof(min_z));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&max_x), sizeof(max_x));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&max_y), sizeof(max_y));
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&max_z), sizeof(max_z));
	}
}

void OctreePointCloudCompressionZjw::syncToHeader(std::istream & compressed_tree_data_in_arg)
{
	// sync to frame header  读出 frame_header_identifier_的内容， 先进行检测
	unsigned int header_id_pos = 0;
	while (header_id_pos < strlen(frame_header_identifier_))
	{
		char readChar;
		compressed_tree_data_in_arg.read(static_cast<char*> (&readChar), sizeof(readChar));
		if (readChar != frame_header_identifier_[header_id_pos++])
			header_id_pos = (frame_header_identifier_[0] == readChar) ? 1 : 0;
	}
}

void OctreePointCloudCompressionZjw::readFrameHeader(std::istream & compressed_tree_data_in_arg)
{
	// read header
	compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&frame_ID_), sizeof(frame_ID_));
	compressed_tree_data_in_arg.read(reinterpret_cast<char*>(&i_frame_), sizeof(i_frame_));
	if (i_frame_)
	{
		double min_x, min_y, min_z, max_x, max_y, max_z;
		double octree_resolution;
		unsigned char color_bit_depth;
		double point_resolution;

		// read coder configuration
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&do_voxel_grid_enDecoding_), sizeof(do_voxel_grid_enDecoding_));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&data_with_color_), sizeof(data_with_color_));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&point_count_), sizeof(point_count_));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&octree_resolution), sizeof(octree_resolution));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&color_bit_depth), sizeof(color_bit_depth));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&point_resolution), sizeof(point_resolution));

		// read octree bounding box
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&min_x), sizeof(min_x));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&min_y), sizeof(min_y));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&min_z), sizeof(min_z));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&max_x), sizeof(max_x));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&max_y), sizeof(max_y));
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&max_z), sizeof(max_z));
		
		// reset octree and assign new bounding box & resolution
		if (dbOctree)
		{
			//删除八叉树
			delete dbOctree;
			dbOctree = new PcsDBufferOctree();
			this->setResolution(octree_resolution);
			dbOctree->setBoundingBox(min_x, min_y, min_z, max_x, max_y, max_z);
		}

		// configure color & point coding
		color_coder_.setBitDepth(color_bit_depth);
		point_coder_.setPrecision(static_cast<float> (point_resolution));
	}
}

//利用熵编码对binary_tree_data_vector_ 中的Byte stream,还有color info , pos info进行编码，存储到文件
void OctreePointCloudCompressionZjw::entropyEncoding(std::ostream & compressed_tree_data_out_arg)
{
	uint64_t binary_tree_data_vector_size;
	uint64_t point_avg_color_data_vector_size;

	compressed_point_data_len_ = 0;
	compressed_color_data_len_ = 0;

	// encode binary octree structure
	binary_tree_data_vector_size = binary_tree_data_vector_.size();
	//文件中写入byte的size多少
	compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&binary_tree_data_vector_size), sizeof(binary_tree_data_vector_size));

	//byte stream写入到文件中
	compressed_point_data_len_ += entropy_coder_.encodeCharVectorToStream(binary_tree_data_vector_, compressed_tree_data_out_arg);

	if (cloud_with_color_)
	{
		//encode averaged voxel color information
		std::vector<char>& pointAvgColorDataVector = color_coder_.getAverageDataVector();

		//把得到avg color 信息进行压缩，并保存到压缩文件中
		point_avg_color_data_vector_size = pointAvgColorDataVector.size();

		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&point_avg_color_data_vector_size), sizeof(point_avg_color_data_vector_size));

		compressed_color_data_len_ += entropy_coder_.encodeCharVectorToStream(pointAvgColorDataVector, compressed_tree_data_out_arg);
	}

	if (!do_voxel_grid_enDecoding_)
	{

		//写入每个叶子节点的个数，每个节点中含有点的个数。写入pos diff 的信息的个数，和pos diff信息
		uint64_t pointCountDataVector_size;
		uint64_t point_diff_data_vector_size;
		uint64_t point_diff_color_data_vector_size;

		// encode amount of points per voxel
		pointCountDataVector_size = point_count_data_vector_.size();
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&pointCountDataVector_size), sizeof(pointCountDataVector_size));

		compressed_point_data_len_ += entropy_coder_.encodeIntVectorToStream(point_count_data_vector_, compressed_tree_data_out_arg);

		// encode differential point information
		std::vector<char>& point_diff_data_vector = point_coder_.getDifferentialDataVector();
		point_diff_data_vector_size = point_diff_data_vector.size();
		compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&point_diff_data_vector_size), sizeof(point_diff_data_vector_size));
		compressed_point_data_len_ += entropy_coder_.encodeCharVectorToStream(point_diff_data_vector, compressed_tree_data_out_arg);

		if (cloud_with_color_)
		{
			// encode differential color information
			std::vector<char>& point_diff_color_data_vector = color_coder_.getDifferentialDataVector();

			point_diff_color_data_vector_size = point_diff_color_data_vector.size();
			compressed_tree_data_out_arg.write(reinterpret_cast<const char*> (&point_diff_color_data_vector_size), sizeof(point_diff_color_data_vector_size));

			compressed_color_data_len_ += entropy_coder_.encodeCharVectorToStream(point_diff_color_data_vector, compressed_tree_data_out_arg);
		}
	}
	// flush output stream
	compressed_tree_data_out_arg.flush();
}

//利用熵解码对Byte stream,color info , pos info进行解码，存储到相应的变量中
void OctreePointCloudCompressionZjw::entropyDecoding(std::istream & compressed_tree_data_in_arg)
{
	uint64_t binary_tree_data_vector_size;
	uint64_t point_avg_color_data_vector_size;

	compressed_point_data_len_ = 0;
	compressed_color_data_len_ = 0;

	// decode binary octree structure
	compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&binary_tree_data_vector_size), sizeof(binary_tree_data_vector_size));

	binary_tree_data_vector_.resize(static_cast<std::size_t> (binary_tree_data_vector_size));
	compressed_point_data_len_ += entropy_coder_.decodeStreamToCharVector(compressed_tree_data_in_arg, binary_tree_data_vector_);

	if (data_with_color_)
	{
		// decode averaged voxel color information
		std::vector<char>& point_avg_color_data_vector = color_coder_.getAverageDataVector();
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&point_avg_color_data_vector_size), sizeof(point_avg_color_data_vector_size));
		point_avg_color_data_vector.resize(static_cast<std::size_t> (point_avg_color_data_vector_size));
		compressed_color_data_len_ += entropy_coder_.decodeStreamToCharVector(compressed_tree_data_in_arg, point_avg_color_data_vector);
	}

	if (!do_voxel_grid_enDecoding_)
	{
		uint64_t point_count_data_vector_size;
		uint64_t point_diff_data_vector_size;
		uint64_t point_diff_color_data_vector_size;

		// decode amount of points per voxel
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&point_count_data_vector_size), sizeof(point_count_data_vector_size));
		point_count_data_vector_.resize(static_cast<std::size_t> (point_count_data_vector_size));
		compressed_point_data_len_ += entropy_coder_.decodeStreamToIntVector(compressed_tree_data_in_arg, point_count_data_vector_);
		point_count_data_vector_iterator_ = point_count_data_vector_.begin();

		// decode differential point information
		std::vector<char>& pointDiffDataVector = point_coder_.getDifferentialDataVector();
		compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&point_diff_data_vector_size), sizeof(point_diff_data_vector_size));
		pointDiffDataVector.resize(static_cast<std::size_t> (point_diff_data_vector_size));
		compressed_point_data_len_ += entropy_coder_.decodeStreamToCharVector(compressed_tree_data_in_arg, pointDiffDataVector);

		if (data_with_color_)
		{
			// decode differential color information
			std::vector<char>& pointDiffColorDataVector = color_coder_.getDifferentialDataVector();
			compressed_tree_data_in_arg.read(reinterpret_cast<char*> (&point_diff_color_data_vector_size), sizeof(point_diff_color_data_vector_size));
			pointDiffColorDataVector.resize(static_cast<std::size_t> (point_diff_color_data_vector_size));
			compressed_color_data_len_ += entropy_coder_.decodeStreamToCharVector(compressed_tree_data_in_arg,
				pointDiffColorDataVector);
		}
	}
}

//encoder: 使用这个函数的前提是八叉树的结构以及相应的建好，叶子节点上的信息也得到了。isTarget false表示是i_frame_
void OctreePointCloudCompressionZjw::serializeTreeForPosAndColor(bool isTarget)
{

	//遍历所有的叶子节点
	for (int leaf_it = 0; leaf_it < dbOctree->ctLeaf->nodeList.size(); leaf_it++)
	{
		// 一个叶子节点中含有的Points的下标
		const std::vector<Vec3>& leafPoints = dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointPosList;

		if (!do_voxel_grid_enDecoding_)
		{

			// 存储每个叶子节点中含有的points的个数 encode amount of points within voxel
			point_count_data_vector_.push_back(static_cast<int> (leafPoints.size()));

			// 叶子节点中的最小点lowerVoxelCorner
			Vec3 lowerVoxelCorner = dbOctree->ctLeaf->minVList[leaf_it];

			//得到point diff vector -- differentially encode points to lower voxel corner
			point_coder_.encodePoints(dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointIdxList,
				lowerVoxelCorner, dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointPosList);

			if (cloud_with_color_)
			{
				// encode color of points 在node data中pointColorList的信息没有存下来
				//color_coder_.encodePoints(dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointIdxList, dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointColorList);

				//need to add something
			}
		}
		else
		{
			if (cloud_with_color_)
			{
				// encode average color of all points within voxel
				//color_coder_.encodeAverageOfPoints(dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointIdxList, dbOctree->ctLeaf->nodeList[leaf_it]->getNodeData(isTarget)->pointColorList);

				//need to add something
			}
		}
	}
}

//decoder: 根据得到的color pos and byteStream的信息，进行恢复重建八叉树，点云的位置和颜色信息
void OctreePointCloudCompressionZjw::deserializeTreeForPosAndColor(std::vector<Vec3>& verPosList_out, bool isTarget)
{
	verPosList_out.clear();
	//得到叶子节点的数量
	int leafNodeNum = point_count_data_vector_.size();
	//得到frame中的点的个数
	assert(point_coder_.getDifferentialDataVector().size() & 3 == 0);
	int framePointsNUm = point_coder_.getDifferentialDataVector().size() / 3;

	//订
	std::size_t pointNumInLeafNode = 0;
	std::size_t framePointsNow = 0;
	Vec3 newPoint;
	pointNumInLeafNode = 1;

	//-----------------根据获得bytestream 恢复出octree-------------
	if (i_frame_)
	{
		dbOctree->recoverySigleFrameInDBufferOctree(binary_tree_data_vector_ ,false);
		//得到叶子节点的信息
		dbOctree->getLeafboundaryAndLeafNode(false);
	}
	else
	{
		dbOctree->recoverySecondFrameInDBufferOctreeXOR(binary_tree_data_vector_,true);
		//得到叶子节点的信息
		dbOctree->getLeafboundaryAndLeafNode(true);
	}

	//设置恢复八叉树的参数,在readFrame的接口中已经恢复了。

	if (!do_voxel_grid_enDecoding_)
	{
		//-----------恢复点的位置信息，遍历所有的叶子节点------------------
		for (int leaf_it = 0; leaf_it < leafNodeNum; leaf_it++)
		{
			//拿到当前这个叶子节点
			OctreeDoubelBufferNode<DBufferNodeData> * node = dbOctree->ctLeaf->nodeList[leaf_it];

			//这个叶子节点中有多少点
			pointNumInLeafNode = point_coder_.getDifferentialDataVector()[leaf_it];
			//目前要重建的frame中有多少点了。
			framePointsNow = verPosList_out.size();

			//计算这个叶子节点上边界最小点的值
			Vec3 lowerVoxelCorner = node->getMinPos();;

			// 从vector解析出这个叶子节点上的所有点的pos diff，然后计算出相应的Pos ,压入到frame的verrex list
			point_coder_.decodePoints(verPosList_out, lowerVoxelCorner, framePointsNow, framePointsNow + pointNumInLeafNode);
		}
	}
	else
	{
	
		//-----------恢复点的位置信息，遍历所有的叶子节点------------------
		for (int leaf_it = 0; leaf_it < leafNodeNum; leaf_it++)
		{
			//这个叶子节点中有多少点
			pointNumInLeafNode = point_coder_.getDifferentialDataVector()[leaf_it];
			
			//拿到当前这个叶子节点
			OctreeDoubelBufferNode<DBufferNodeData> * node = dbOctree->ctLeaf->nodeList[leaf_it];
			//计算这个叶子节点上边界最小点的值
			Vec3 midPosInNode = (node->getMinPos() + node->getMaxPos()) / 2;

		
			// add point to point cloud
			for (int leaf_it = 0; leaf_it < leafNodeNum; leaf_it++)
			{
				verPosList_out.push_back(midPosInNode);
			}
		}
	}

	if (cloud_with_color_)
	{
		if (data_with_color_)
		{
			// decode color information
			//color_coder_.decodePoints(output_, output_->points.size() - pointNumInLeafNode,	output_->points.size(), point_color_offset_);
			
			//need to modify something
		}
		else
		{
			// set default color information
			//color_coder_.setDefaultColor(output_, output_->points.size() - pointNumInLeafNode,output_->points.size(), point_color_offset_);

			//need to modify something
		}
	}	
}