#include "zjw_dsaupd.h"

Dsaupd::Dsaupd()
{
	mat = nullptr;
	Evals = nullptr;
	Evecs = nullptr;
	n = 0;
	nev = 0;
	totalNum = 0;
}

Dsaupd::~Dsaupd()
{
	delete[] mat;
	delete Evals;
	delete[] Evecs;
}

void Dsaupd::initParam(int matSize)
{
	//init size
	n = matSize;
	totalNum = n * n;

	//init 需要得到的特征向量的个数
	nev = n - 1;

	//init  mat
	initMat();

	//分配特征值的特征向量的空间
	Evals = new double[nev];
	Evecs = new double*[n];
	for (int i = 0; i < n; i++)
	{
		Evecs[i] = new double[nev];
	}
}

void Dsaupd::initMat()
{
	mat = new double*[totalNum];
	for (int i = 0; i < totalNum; i++)
		mat[i] = new double[3];

	totalNum = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++) {
			mat[totalNum][0] = i;
			mat[totalNum][1] = j;
			//mat[totalNum][2] = powf(i + 1, j + 1) + powf(j + 1, i + 1);
			mat[totalNum][2] = 0;
			totalNum++;
		}
	}
}

void Dsaupd::setMat(int row, int col, double  matEle)
{
	mat[row * n + col][2] = matEle;
}

void Dsaupd::setAddMat(int pos, double  matEle)
{
	mat[pos * n + pos][2] += matEle;
}

void Dsaupd::printMat()
{
	cout << "------------------------------" << endl;
	cout << "materix : " << endl;

	if (mat)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				cout << setw(10) << mat[i * n + j][2];
			}
			cout << endl;
		}
	}
	cout << "------------------------------" << endl;
}

void Dsaupd::printValueVector()
{
	cout << "------------------------------" << endl;
	for (int i = 0; i < nev; i++)
	{
		cout << "Eigenvalue  " << i << ": " << Evals[i] << "\n";
		cout << "Eigenvector " << i << ": (";

		for (int j = 0; j < n; j++)
		{
			cout << Evecs[j][i] << ", ";
		}
		cout << '\b';
		cout << ")\n";
		cout << "--------------" << endl;
	}
	cout << "------------------------------" << endl;
}