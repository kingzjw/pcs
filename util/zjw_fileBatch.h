#pragma once
#include <string>
#include <vector>
#include <io.h>
#include <iostream>

#include "zjw_macro.h"

using namespace std;

class FileBatch
{
public:
	vector<string> files;
	int fileNum;
	string filePath;
	string fileNameFormat;

public:
	FileBatch(string path, string& fileNameFormat);
	//对指定目录下面的文件夹，进行递归遍历找到所有的文件
	void getFilesRecurs(string path, vector<string>& files);
	//对指定目录下面的文件夹的直接的所有文件，不进行递归
	void getFiles(string filePath, vector<string>& files);
	//对指定目录下面的文件夹的直接的所有文件的数量，不进行递归
	int getFilesNum(string filePath);
	void printFiles();
};