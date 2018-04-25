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
 * \brief ֻ���һ�ֶԳƾ��󣬾�������ֶ���ʵ����
 *  �������ά����n����ô���ֻ������n-1����������������ֵ
 * \author King
 * \date ���� 2018
 */
class Dsaupd
{
public:
	//�洢����
	double ** mat;
	//Ԫ�صĸ��� n * n
	int totalNum;

	//n������A�Ľ���
	int n;
	// The number of values to calculate
	//nev�����п�ʼ������ֵ�������ײ��� ��ע�⣬��ߵ�����ֵ����һЩ����ѡ�񣬿����ҵ��� ���ڣ�ѡ����͵�nev����ֵ��Ӳ����ġ�
	int nev;
	//Evals������Ϊnev��һά��������������ֵ��
	double *Evals;
	//Evecs����nά�ֵ�һ���ߴ�Ϊnv�Ķ�ά�������������� ���û���ṩ����۵㣬��ô�������������㡣 ��ע���������洢ΪEvecs������������������i��Ԫ����ֵEvecs [j] [i]
	double **Evecs;

public:
	Dsaupd();
	~Dsaupd();

	void initParam(int matSize);
	//�����е�Ԫ�ر���ʼ��Ϊ0
	void initMat();
	//row 0,n-1  col 0,n-1
	void setMat(int row, int col, double  matEle);
	void setAddMat(int pos, double matEle);
	//��ԶԳƾ�����������ֵ
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
	//��ԶԳƾ�����������ֵ����������
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
	//av������ʾ��A*x�ĳ˷�����
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