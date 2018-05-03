#include "zjw_material.h"

Material::Material()
{
	name;
	Ns = 0.0f;
	//不发生材质
	Ni = 0.0f;
	//透明度
	Tr = 0.0f;
	d = 0.0f;
	illum = 0;

	//下面的针对单独的应用，可以删除
	light = false;
	ball = false;
	//end
}

MaterialFile::MaterialFile()
{
	materialNum = 0;
}

bool MaterialFile::loadMaterial(std::string path)
{
	// If the file is not a material file return false
	if (path.substr(path.size() - 4, path.size()) != ".mtl")
	{
		std::cout << "this is not .mtl file" << std::endl;
		return false;
	}

	std::ifstream file(path);

	// If the file is not found return false
	if (!file.is_open())
	{
		std::cout << "open file failed! maybe there is no such file: " << path << std::endl;
		return false;
	}
#ifdef ZJW_DEDUG
	cout << "starting load the materail: " << path << endl;
#endif // ZJW_DEUG

	Material tempMaterial;

	bool listening = false;

	// Go through each line looking for material variables
	std::string curline;
	while (std::getline(file, curline))
	{
		// new material and material name
		if (algorithm::firstToken(curline) == "newmtl")
		{
			if (!listening)
			{
				listening = true;

				if (curline.size() > 7)
				{
					tempMaterial.name = algorithm::tail(curline);
				}
				else
				{
					tempMaterial.name = "none";
				}
			}
			else
			{
				// Generate the material

				// Push Back loaded Material
				materialList.push_back(tempMaterial);

				// Clear Loaded Material
				tempMaterial = Material();

				if (curline.size() > 7)
				{
					tempMaterial.name = algorithm::tail(curline);
				}
				else
				{
					tempMaterial.name = "none";
				}
			}
		}
		// Ambient Color
		if (algorithm::firstToken(curline) == "Ka")
		{
			std::vector<std::string> temp;
			algorithm::split(algorithm::tail(curline), temp, " ");

			if (temp.size() != 3)
				continue;

			tempMaterial.Ka.x = std::stof(temp[0]);
			tempMaterial.Ka.y = std::stof(temp[1]);
			tempMaterial.Ka.z = std::stof(temp[2]);
		}
		// Diffuse Color
		if (algorithm::firstToken(curline) == "Kd")
		{
			std::vector<std::string> temp;
			algorithm::split(algorithm::tail(curline), temp, " ");

			if (temp.size() != 3)
				continue;

			tempMaterial.Kd.x = std::stof(temp[0]);
			tempMaterial.Kd.y = std::stof(temp[1]);
			tempMaterial.Kd.z = std::stof(temp[2]);
		}
		// Specular Color
		if (algorithm::firstToken(curline) == "Ks")
		{
			std::vector<std::string> temp;
			algorithm::split(algorithm::tail(curline), temp, " ");

			if (temp.size() != 3)
				continue;

			tempMaterial.Ks.x = std::stof(temp[0]);
			tempMaterial.Ks.y = std::stof(temp[1]);
			tempMaterial.Ks.z = std::stof(temp[2]);
		}
		// Tf 滤光透射率
		if (algorithm::firstToken(curline) == "Tf")
		{
			std::vector<std::string> temp;
			algorithm::split(algorithm::tail(curline), temp, " ");

			if (temp.size() != 3)
				continue;

			tempMaterial.Tf.x = std::stof(temp[0]);
			tempMaterial.Tf.y = std::stof(temp[1]);
			tempMaterial.Tf.z = std::stof(temp[2]);
		}
		// Specular Exponent
		if (algorithm::firstToken(curline) == "Ns")
		{
			tempMaterial.Ns = std::stof(algorithm::tail(curline));
		}
		// Optical Density
		if (algorithm::firstToken(curline) == "Ni")
		{
			tempMaterial.Ni = std::stof(algorithm::tail(curline));
		}
		// Dissolve
		if (algorithm::firstToken(curline) == "d")
		{
			tempMaterial.d = std::stof(algorithm::tail(curline));
		}
		if (algorithm::firstToken(curline) == "Tr")
		{
			tempMaterial.Tr = std::stof(algorithm::tail(curline));
		}

		// Illumination
		if (algorithm::firstToken(curline) == "illum")
		{
			tempMaterial.illum = std::stoi(algorithm::tail(curline));
		}
		// Ambient Texture Map
		if (algorithm::firstToken(curline) == "map_Ka")
		{
			tempMaterial.map_Ka = algorithm::tail(curline);
		}
		// Diffuse Texture Map
		if (algorithm::firstToken(curline) == "map_Kd")
		{
			tempMaterial.map_Kd = algorithm::tail(curline);
		}
		// Specular Texture Map
		if (algorithm::firstToken(curline) == "map_Ks")
		{
			tempMaterial.map_Ks = algorithm::tail(curline);
		}
		// Specular Hightlight Map
		if (algorithm::firstToken(curline) == "map_Ns")
		{
			tempMaterial.map_Ns = algorithm::tail(curline);
		}
		// Alpha Texture Map
		if (algorithm::firstToken(curline) == "map_d")
		{
			tempMaterial.map_d = algorithm::tail(curline);
		}
		// Bump Map
		if (algorithm::firstToken(curline) == "map_Bump" || algorithm::firstToken(curline) == "map_bump" || algorithm::firstToken(curline) == "bump")
		{
			tempMaterial.map_bump = algorithm::tail(curline);
		}
	}

	// Deal with last material

	// Push Back loaded Material
	materialList.push_back(tempMaterial);

	// Test to see if anything was loaded
	// If not return false
	if (materialList.empty())
	{
		std::cout << "can not find any material List" << std::endl;
		return false;
	}
	// If so return true
	else {
		materialNum = materialList.size();
#ifdef ZJW_DEDUG
		cout << "finish load the materail ! " << endl;
#endif // ZJW_DEUG
		return true;
	}

	return false;
}