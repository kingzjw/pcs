#pragma once
#include <math.h>
#include <iostream>
#include <iomanip>
using namespace std;

extern "C" void dsaupd_(int *ido, char *bmat, int *n, char *which,
	int *nev, double *tol, double *resid, int *ncv,
	double *v, int *ldv, int *iparam, int *ipntr,
	double *workd, double *workl, int *lworkl,
	int *info);

extern "C" void dseupd_(int *rvec, char *All, int *select, double *d,
	double *z, int *ldz, double *sigma,
	char *bmat, int *n, char *which, int *nev,
	double *tol, double *resid, int *ncv, double *v,
	int *ldv, int *iparam, int *ipntr, double *workd,
	double *workl, int *lworkl, int *ierr);

/*!
 * \class Dsaupd
 *
 * \brief 只针对一种对称矩阵，矩阵的数字都是实数。
 *  方阵如果维度是n，那么最多只能求解出n-1个特征向量和特征值
 * \author King
 * \date 四月 2018
 */
class Dsaupd
{
public:
	//存储矩阵
	double ** mat;
	//元素的个数 n * n
	int totalNum;

	//n：方阵A的阶数
	int n;
	// The number of values to calculate
	//nev：从中开始的特征值的数量底部。 请注意，最高的特征值，或一些其他选择，可以找到。 现在，选择最低的nev特征值是硬编码的。
	int nev;
	//Evals：长度为nev的一维数组来保存特征值。
	double *Evals;
	//Evecs：由n维持的一个尺寸为nv的二维数组特征向量。 如果没有提供这个论点，那么特征向量不计算。 请注意向量被存储为Evecs的列向量，所以向量i的元素是值Evecs [j] [i]
	double **Evecs;

public:
	Dsaupd();
	~Dsaupd();

	void initParam(int matSize);
	//矩阵中的元素被初始化为0
	void initMat();
	//row 0,n-1  col 0,n-1
	void setMat(int row, int col, double  matEle);
	void setAddMat(int pos, double matEle);
	//针对对称矩阵，求解出特征值
	void dsaupdEvals()
	{
		/* Initialization of the reverse communication parameter. */
		int ido = 0;

		/* Specifies that the right hand side matrix
		should be the identity matrix; this makes
		the problem a standard eigenvalue problem.
		Setting bmat = "G" would have us solve the
		problem Av = lBv (this would involve using
		some other programs from BLAS, however). */
		char bmat[2] = "I";

		/* Ask for the nev eigenvalues of smallest
		magnitude.  The possible options are
		LM: largest magnitude
		SM: smallest magnitude
		LA: largest real component
		SA: smallest real compoent
		LI: largest imaginary component
		SI: smallest imaginary component */
		char which[3] = "SM";

		/* Sets the tolerance; tol<=0 specifies	machine precision */
		double tol = 0.0;

		//Double precision array of length N,RESID contains the final residual vector.
		double *resid;
		resid = new double[n];

		/* The largest number of basis vectors that will be used in the Implicitly Restarted Arnoldi Process.
		Work per major iteration is proportional to N*NCV*NCV. */
		//Number of columns of the matrix V.  2 <= NCV-NEV and NCV <= N
		int ncv = 4 * nev;
		if (ncv > n)
			ncv = n;

		// final set of Arnoldi basis vectors
		double *v;
		int ldv = n;
		v = new double[ldv*ncv];

		int *iparam;
		iparam = new int[11]; /* An array used to pass information to the routines
							  about their functional modes. */
		iparam[0] = 1;   // Specifies the shift strategy (1->exact)
		iparam[2] = 3 * n; // Maximum number of iterations
		iparam[6] = 1;   /* Sets the mode of dsaupd.
						 1 is exact shifting,
						 2 is user-supplied shifts,
						 3 is shift-invert mode,
						 4 is buckling mode,
						 5 is Cayley mode. */

		int *ipntr;
		ipntr = new int[11]; /* Indicates the locations in the work array workd
							 where the input and output vectors in the
							 callback routine are located. */

		double *workd;
		workd = new double[3 * n];

		double *workl;
		workl = new double[ncv*(ncv + 8)];

		int lworkl = ncv*(ncv + 8); /* Length of the workl array */

		int info = 0; /* Passes convergence information out of the iteration
					  routine. */

		int rvec = 0; /* Specifies that eigenvectors should not be calculated */

		int *select;
		select = new int[ncv];
		double *d;
		d = new double[2 * ncv]; /* This vector will return the eigenvalues from
								 the second routine, dseupd. */
		double sigma;
		int ierr;

		/* Here we enter the main loop where the calculations are
		performed.  The communication parameter ido tells us when
		the desired tolerance is reached, and at that point we exit
		and extract the solutions. */

		do {
			dsaupd_(&ido, bmat, &n, which, &nev, &tol, resid,
				&ncv, v, &ldv, iparam, ipntr, workd, workl,
				&lworkl, &info);

			if ((ido == 1) || (ido == -1))
				av(n, workd + ipntr[0] - 1, workd + ipntr[1] - 1);
		} while ((ido == 1) || (ido == -1));

		/* From those results, the eigenvalues and vectors are
		extracted. */

		if (info < 0) {
			cout << "Error with dsaupd, info = " << info << "\n";
			cout << "Check documentation in dsaupd\n\n";
		}
		else {
			dseupd_(&rvec, "All", select, d, v, &ldv, &sigma, bmat,
				&n, which, &nev, &tol, resid, &ncv, v, &ldv,
				iparam, ipntr, workd, workl, &lworkl, &ierr);

			if (ierr != 0) {
				cout << "Error with dseupd, info = " << ierr << "\n";
				cout << "Check the documentation of dseupd.\n\n";
			}
			else if (info == 1) {
				cout << "Maximum number of iterations reached.\n\n";
			}
			else if (info == 3) {
				cout << "No shifts could be applied during implicit\n";
				cout << "Arnoldi update, try increasing NCV.\n\n";
			}

			/* Before exiting, we copy the solution information over to
			the arrays of the calling program, then clean up the
			memory used by this routine.  For some reason, when I
			don't find the eigenvectors I need to reverse the order of
			the values. */

			int i;
			for (i = 0; i < nev; i++)
				Evals[i] = d[nev - 1 - i];

			delete resid;
			delete v;
			delete iparam;
			delete ipntr;
			delete workd;
			delete workl;
			delete select;
			delete d;
		}
	}
	//针对对称矩阵，求解出特征值和特征向量
	void dsaupdEvalsEvecs()
	{
		int ido = 0;
		char bmat[2] = "I";
		char which[3] = "SM";
		double tol = 0.0;
		double *resid;
		resid = new double[n];
		int ncv = 4 * nev;
		if (ncv > n) ncv = n;
		double *v;
		int ldv = n;
		v = new double[ldv*ncv];
		int *iparam;
		iparam = new int[11];
		iparam[0] = 1;
		iparam[2] = 3 * n;
		iparam[6] = 1;
		int *ipntr;
		ipntr = new int[11];
		double *workd;
		workd = new double[3 * n];
		double *workl;
		workl = new double[ncv*(ncv + 8)];
		int lworkl = ncv*(ncv + 8);
		int info = 0;
		int rvec = 1;  // Changed from above
		int *select;
		select = new int[ncv];
		double *d;
		d = new double[2 * ncv];
		double sigma;
		int ierr;

		do {
			dsaupd_(&ido, bmat, &n, which, &nev, &tol, resid,
				&ncv, v, &ldv, iparam, ipntr, workd, workl,
				&lworkl, &info);

			if ((ido == 1) || (ido == -1))
				av(n, workd + ipntr[0] - 1, workd + ipntr[1] - 1);
		} while ((ido == 1) || (ido == -1));

		if (info < 0) {
			cout << "Error with dsaupd, info = " << info << "\n";
			cout << "Check documentation in dsaupd\n\n";
		}
		else {
			dseupd_(&rvec, "All", select, d, v, &ldv, &sigma, bmat,
				&n, which, &nev, &tol, resid, &ncv, v, &ldv,
				iparam, ipntr, workd, workl, &lworkl, &ierr);

			if (ierr != 0) {
				cout << "Error with dseupd, info = " << ierr << "\n";
				cout << "Check the documentation of dseupd.\n\n";
			}
			else if (info == 1) {
				cout << "Maximum number of iterations reached.\n\n";
			}
			else if (info == 3) {
				cout << "No shifts could be applied during implicit\n";
				cout << "Arnoldi update, try increasing NCV.\n\n";
			}

			int i, j;
			for (i = 0; i < nev; i++)
				Evals[i] = d[i];
			for (i = 0; i < nev; i++)
				for (j = 0; j < n; j++)
					Evecs[j][i] = v[i*n + j];

			delete resid;
			delete v;
			delete iparam;
			delete ipntr;
			delete workd;
			delete workl;
			delete select;
			delete d;
		}
	}
	//av函数表示的A*x的乘法规则
	void av(int n, double *in, double *out)
	{
		int i, j;

		for (i = 0; i < n; i++)
			out[i] = 0;
		for (i = 0; i < totalNum; i++)
			out[(int)mat[i][0]] += in[(int)mat[i][1]] * mat[i][2];
	}

	void printMat();
	void printValueVector();
};