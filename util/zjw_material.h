/*!
 * \file zjw_material.h
 *
 * \author King
 * \date ���� 2018
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

//��װ����
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
	//�˹�͸���� Tf
	Vec3 Tf;
	// �߹�ϵ�� Specular Exponent
	//����ָ������Ns exponent ָ�����ʵķ���ָ���������˷���߹�ȡ�exponent�Ƿ���ָ��ֵ����ֵԽ����߹�Խ�ܼ���һ��ȡֵ��Χ��0~1000��
	float Ns;

	/*ָ�����ʱ���Ĺ��ܶȣ��������ʣ����ǹ������䶨��������Ǹ���
		ptical density����0.001��10֮�����ȡֵ����ȡֵΪ1.0������ͨ�������ʱ�򲻷���������
		������������Ϊ1.5��ȡֵС��1.0��ʱ����ܻ������ֵĽ�������Ƽ�
	*/
	float Ni;
	// Dissolve ͸����
	float d;

	//͸���� 0�� ��ȫ͸��  1����ȫ��͸��(Ĭ��ֵ)
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

	//---------------��Ը���Ӧ�ÿ���ɾ��----------------

	//����Ƿ��ǹ�Դ�Ĳ���
	bool light;
	//����ߺ�����
	bool ball;
	//������ײ��⣬�洢Բ�ĺͰ뾶
	Vec3 center;
	double radius;
	//-------------------------end --------------------------
};

//�ļ������㷨����
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

	// Get first token of string���õ���һ���ַ��ĵ�һ�����
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

//��װ���ϵ��ļ�
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
