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
	//��ָ��Ŀ¼������ļ��У����еݹ�����ҵ����е��ļ�
	void getFilesRecurs(string path, vector<string>& files);
	//��ָ��Ŀ¼������ļ��е�ֱ�ӵ������ļ��������еݹ�
	void getFiles(string filePath, vector<string>& files);
	//��ָ��Ŀ¼������ļ��е�ֱ�ӵ������ļ��������������еݹ�
	int getFilesNum(string filePath);
	void printFiles();
};