#pragma once

enum compression_Profiles_e
{
	LOW_RES_ONLINE_COMPRESSION_WITHOUT_COLOR,
	LOW_RES_ONLINE_COMPRESSION_WITH_COLOR,

	MED_RES_ONLINE_COMPRESSION_WITHOUT_COLOR,
	MED_RES_ONLINE_COMPRESSION_WITH_COLOR,

	HIGH_RES_ONLINE_COMPRESSION_WITHOUT_COLOR,
	HIGH_RES_ONLINE_COMPRESSION_WITH_COLOR,

	LOW_RES_OFFLINE_COMPRESSION_WITHOUT_COLOR,
	LOW_RES_OFFLINE_COMPRESSION_WITH_COLOR,

	MED_RES_OFFLINE_COMPRESSION_WITHOUT_COLOR,
	MED_RES_OFFLINE_COMPRESSION_WITH_COLOR,

	HIGH_RES_OFFLINE_COMPRESSION_WITHOUT_COLOR,
	HIGH_RES_OFFLINE_COMPRESSION_WITH_COLOR,

	COMPRESSION_PROFILE_COUNT,
	MANUAL_CONFIGURATION
};

// compression configuration profile
struct configurationProfile_t
{
	double pointResolution;
	const double octreeResolution;
	bool doVoxelGridDownSampling;
	unsigned int iFrameRate;
	const unsigned char colorBitResolution;
	bool doColorEncoding;
};

// predefined configuration parameters
const struct configurationProfile_t compressionProfiles_[COMPRESSION_PROFILE_COUNT] = {
	{
		// PROFILE: LOW_RES_ONLINE_COMPRESSION_WITHOUT_COLOR
		0.01, /* pointResolution = */
		0.01, /* octreeResolution = */
		true, /* doVoxelGridDownDownSampling = */
		50, /* iFrameRate = */
		4, /* colorBitResolution = */
		false /* doColorEncoding = */
	},{
		// PROFILE: LOW_RES_ONLINE_COMPRESSION_WITH_COLOR
		0.01, /* pointResolution = */
		0.01, /* octreeResolution = */
		true, /* doVoxelGridDownDownSampling = */
		50, /* iFrameRate = */
		4, /* colorBitResolution = */
		true /* doColorEncoding = */
	},{
		// PROFILE: MED_RES_ONLINE_COMPRESSION_WITHOUT_COLOR
		0.005, /* pointResolution = */
		0.01, /* octreeResolution = */
		false, /* doVoxelGridDownDownSampling = */
		40, /* iFrameRate = */
		5, /* colorBitResolution = */
		false /* doColorEncoding = */
	},{
		// PROFILE: MED_RES_ONLINE_COMPRESSION_WITH_COLOR
		0.005, /* pointResolution = */
		0.01, /* octreeResolution = */
		false, /* doVoxelGridDownDownSampling = */
		40, /* iFrameRate = */
		5, /* colorBitResolution = */
		true /* doColorEncoding = */
	},{
		// PROFILE: HIGH_RES_ONLINE_COMPRESSION_WITHOUT_COLOR
		0.0001, /* pointResolution = */
		0.01, /* octreeResolution = */
		false, /* doVoxelGridDownDownSampling = */
		30, /* iFrameRate = */
		7, /* colorBitResolution = */
		false /* doColorEncoding = */
	},{
		// PROFILE: HIGH_RES_ONLINE_COMPRESSION_WITH_COLOR
		0.0001, /* pointResolution = */
		0.01, /* octreeResolution = */
		false, /* doVoxelGridDownDownSampling = */
		30, /* iFrameRate = */
		7, /* colorBitResolution = */
		true /* doColorEncoding = */
	},{
		// PROFILE: LOW_RES_OFFLINE_COMPRESSION_WITHOUT_COLOR
		0.01, /* pointResolution = */
		0.01, /* octreeResolution = */
		true, /* doVoxelGridDownDownSampling = */
		100, /* iFrameRate = */
		4, /* colorBitResolution = */
		false /* doColorEncoding = */
	},{
		// PROFILE: LOW_RES_OFFLINE_COMPRESSION_WITH_COLOR
		0.01, /* pointResolution = */
		0.01, /* octreeResolution = */
		true, /* doVoxelGridDownDownSampling = */
		100, /* iFrameRate = */
		4, /* colorBitResolution = */
		true /* doColorEncoding = */
	},{
		// PROFILE: MED_RES_OFFLINE_COMPRESSION_WITHOUT_COLOR
		0.005, /* pointResolution = */
		0.005, /* octreeResolution = */
		true, /* doVoxelGridDownDownSampling = */
		100, /* iFrameRate = */
		5, /* colorBitResolution = */
		false /* doColorEncoding = */
	},{
		// PROFILE: MED_RES_OFFLINE_COMPRESSION_WITH_COLOR
		0.005, /* pointResolution = */
		0.01, /* octreeResolution = */
		false, /* doVoxelGridDownDownSampling = */
		100, /* iFrameRate = */
		5, /* colorBitResolution = */
		true /* doColorEncoding = */
	},{
		// PROFILE: HIGH_RES_OFFLINE_COMPRESSION_WITHOUT_COLOR
		0.0001, /* pointResolution = */
		0.0001, /* octreeResolution = */
		true, /* doVoxelGridDownDownSampling = */
		100, /* iFrameRate = */
		8, /* colorBitResolution = */
		false /* doColorEncoding = */
	},{
		// PROFILE: HIGH_RES_OFFLINE_COMPRESSION_WITH_COLOR
		0.0001, /* pointResolution = */
		0.01, /* octreeResolution = */
		false, /* doVoxelGridDownDownSampling = */
		100, /* iFrameRate = */
		8, /* colorBitResolution = */
		true /* doColorEncoding = */
	} };