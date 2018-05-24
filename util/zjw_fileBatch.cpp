#include "zjw_fileBatch.h"

FileBatch::FileBatch(string path, string& fileNameFormat)
{
	fileNum = 0;
	filePath = path;
	this->fileNameFormat = fileNameFormat;
}

void FileBatch::getFilesRecurs(string path, vector<string>& files)

{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFilesRecurs(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void FileBatch::getFiles(string filePath, vector<string>& files)

{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(filePath).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果不是目录,加入列表
			if (!(fileinfo.attrib &  _A_SUBDIR))
			{
				files.push_back(p.assign(filePath).append("/").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int FileBatch::getFilesNum(string filePath)

{
	int count = 0;
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(filePath).append("/*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果不是目录,加入列表
			if (!(fileinfo.attrib &  _A_SUBDIR))
			{
				count++;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

	return count;
}

void FileBatch::printFiles()
{
	cout << "############################" << endl;
	cout << "file num : " << files.size() << endl;

	for (int i = 0; i < files.size(); i++)
	{
		cout << files[i] << endl;
	}
	cout << "############################" << endl;
}