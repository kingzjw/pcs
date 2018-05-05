/*!
 * \file zjw_material.h
 *
 * \author King
 * \date 三月 2018
 *  dependies: must include the zjw_math.h
 *
 */

#pragma once
#include "zjw_math.h"
#include <string>
#include<vector>
#include <iostream>
#include <fstream>
#define ZJW_DEBUG

using std::cout;
using std::endl;

//封装材料
class Material
{
public:
	// Material Name
	std::string name;
	// Ambient Color
	Vec3 Ka;
	// Diffuse Color
	Vec3 Kd;
	// Specular Color
	Vec3 Ks;
	//滤光透射率 Tf
	Vec3 Tf;
	// 高光系数 Specular Exponent
	//反射指数描述Ns exponent 指定材质的反射指数，定义了反射高光度。exponent是反射指数值，该值越高则高光越密集，一般取值范围在0~1000。
	float Ns;

	/*指定材质表面的光密度，即折射率，就是光线折射定理里面的那个。
		ptical density可在0.001到10之间进行取值。若取值为1.0，光在通过物体的时候不发生弯曲。
		玻璃的折射率为1.5。取值小于1.0的时候可能会产生奇怪的结果，不推荐
	*/
	float Ni;
	// Dissolve 透明度
	float d;

	//透明度 0： 完全透明  1：完全不透明(默认值)
	float Tr;

	// Illumination
	int illum;
	// Ambient Texture Map
	std::string map_Ka;
	// Diffuse Texture Map
	std::string map_Kd;
	// Specular Texture Map
	std::string map_Ks;
	// Specular Hightlight Map
	std::string map_Ns;
	// Alpha Texture Map
	std::string map_d;
	// Bump Map
	std::string map_bump;

public:
	Material();

	//---------------针对个别应用可以删除----------------

	//标记是否是光源的材质
	bool light;
	//标光线和球求交
	bool ball;
	//球体碰撞检测，存储圆心和半径
	Vec3 center;
	double radius;
	//-------------------------end --------------------------
};

//文件解析算法函数
namespace algorithm
{
	// Split a String into a string array at a given token
	inline void split(const std::string &in,
		std::vector<std::string> &out,
		std::string token)
	{
		out.clear();

		std::string temp;

		for (int i = 0; i < int(in.size()); i++)
		{
			std::string test = in.substr(i, token.size());

			if (test == token)
			{
				if (!temp.empty())
				{
					out.push_back(temp);
					temp.clear();
					i += (int)token.size() - 1;
				}
				else
				{
					out.push_back("");
				}
			}
			else if (i + token.size() >= in.size())
			{
				temp += in.substr(i, token.size());
				out.push_back(temp);
				break;
			}
			else
			{
				temp += in[i];
			}
		}
	}

	// Get tail of string after first token and possibly following spaces
	inline std::string tail(const std::string &in)
	{
		size_t token_start = in.find_first_not_of(" \t");
		size_t space_start = in.find_first_of(" \t", token_start);
		size_t tail_start = in.find_first_not_of(" \t", space_start);
		size_t tail_end = in.find_last_not_of(" \t");
		if (tail_start != std::string::npos && tail_end != std::string::npos)
		{
			return in.substr(tail_start, tail_end - tail_start + 1);
		}
		else if (tail_start != std::string::npos)
		{
			return in.substr(tail_start);
		}
		return "";
	}

	// Get first token of string。拿到第一串字符的第一个标记
	inline std::string firstToken(const std::string &in)
	{
		if (!in.empty())
		{
			size_t token_start = in.find_first_not_of(" \t");
			size_t token_end = in.find_first_of(" \t", token_start);
			if (token_start != std::string::npos && token_end != std::string::npos)
			{
				return in.substr(token_start, token_end - token_start);
			}
			else if (token_start != std::string::npos)
			{
				return in.substr(token_start);
			}
		}
		return "";
	}

	// Get element at given index position
	template <class T>
	inline const T & getElement(const std::vector<T> &elements, std::string &index)
	{
		int idx = std::stoi(index);
		if (idx < 0)
			idx = int(elements.size()) + idx;
		else
			idx--;
		return elements[idx];
	}
}

//封装材料的文件
class MaterialFile
{
public:
	int materialNum;
	std::vector<Material> materialList;
public:
	MaterialFile();
	// Load Materials from .mtl file
	bool loadMaterial(std::string path);
};
