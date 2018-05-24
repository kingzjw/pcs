//#pragma once
//#include <string>
//#include <cmath>
//#include <time.h>
//
//#include <Eigen/Dense>
//#include "Handle_sgwt_cheby_op.h"
//
//using namespace std;
//using namespace Eigen;
//

//
//VectorXd iterapp(Handle_sgwt_cheby_op afun, VectorXd x)
//{
//	return afun(x)[0];
//}
//
///*
//%   X = PCG(AFUN, B) accepts a function handle AFUN instead of the matrix A.
//%   AFUN(X) accepts a vector input X and returns the matrix - vector product
//%   A*X.In all of the following syntaxes, you can replace A by AFUN.
//*/
//VectorXd pcg(Handle_sgwt_cheby_op AFUN, VectorXd B, double tol)
//{
//	int m = B.size();
//	int n = m;
//	int warned = 0;
//	int maxit = n > 20 ? 20 : n;
//
//	//Check for all zero right hand side vector = > all zero solution
//	double n2b = B.norm();
//	if (n2b == 0)
//	{
//		VectorXd x(n);
//		x.setZero();
//		return x;
//	}
//	int existM1 = 0, existM2 = 0;
//	string mltype("matrix"), m2type("matrix");
//	VectorXd x(n);
//	x.setZero();
//
//	//Set up for the method
//	bool flag = 1;
//	VectorXd xmin(x);
//	int imin = 0;  //?
//	double tolb = tol*n2b;
//	VectorXd r(n);
//	r = B - iterapp(AFUN, x);
//	double normr = r.norm();
//	double normr_act = normr;
//
//	if (normr <= tolb)
//	{
//		flag = 0;
//		double relres = normr / n2b;
//		int iter = 0;
//		double resvec = normr;
//		VectorXd res(n);
//		res.setZero();
//		return res;
//	}
//
//	VectorXd resvec(maxit + 1);
//	resvec.setZero();
//	resvec(0) = normr;
//	double normrmin = normr;
//	double rho = 1;
//	int stag = 0, moresteps = 0;
//	int maxmsteps = (n / 50) < 5 ? (n / 50) : (5 < (n - maxit) ? 5 : (n - maxit));			//min(n / 50, 5, n - maxit)
//	int maxstagsteps = 3;
//
//	VectorXd y(n), z(n), p(n), q(n);
//	double alpha, beta, pq, rho1;
//	int iter, i;
//
//	clock_t start, finish;
//	double duration;
//	//start = clock();
//	//loop over maxit iterations(unless convergence or failure)
//	for (i = 0; i < maxit; i++)
//	{
//		y = r;
//		z = y;
//
//		rho1 = rho;
//		rho = r.adjoint()*z;
//		if ((rho == 0) || (isinf<double>(rho)))
//		{
//			flag = 4;
//			break;
//		}
//
//		if (i == 0) {
//			p = z;
//		}
//		else {
//			beta = rho / rho1;
//			if ((beta == 0) || isinf<double>(beta))
//			{
//				flag = 4;
//				break;
//			}
//			p = z + beta*p;
//		}
//
//		if (i == 0)
//			start = clock();
//		q = AFUN(p)[0];
//		if (i == 0)
//			finish = clock();
//		pq = p.adjoint()*q;
//		if ((pq <= 0) || isinf<double>(pq))
//		{
//			flag = 4;
//			break;
//		}
//		else {
//			alpha = rho / pq;
//		}
//
//		if (isinf<double>(alpha))
//		{
//			flag = 4;
//			break;
//		}
//
//		//Check for stagnation of the method
//		if (p.norm()*abs(alpha) < pow(2.0, -52)*x.norm()) {
//			stag++;
//		}
//		else {
//			stag = 0;
//		}
//
//		x = x + alpha * p;             // form new iterate
//		r = r - alpha * q;
//		normr = r.norm();
//		normr_act = normr;
//		resvec(i + 1) = normr;
//
//		// check for convergence
//		if (normr <= tolb || stag >= maxstagsteps || moresteps)
//		{
//			r = B - iterapp(AFUN, x);
//			normr_act = r.norm();
//			resvec(i + 1) = normr_act;
//			if (normr_act <= tolb)
//			{
//				flag = 0;
//				iter = i;
//				break;
//			}
//			else
//			{
//				if (stag >= maxstagsteps && moresteps == 0)
//					stag = 0;
//				moresteps = moresteps + 1;
//				if (moresteps >= maxmsteps)
//				{
//					flag = 3;
//					iter = i;
//					break;
//				}
//			}
//		}
//		if (normr_act < normrmin)							//update minimal norm quantities
//		{
//			normrmin = normr_act;
//			xmin = x;
//			imin = i;
//		}
//		if (stag >= maxstagsteps)
//		{
//			flag = 3;
//			break;
//		}
//	}
//	//finish = clock();
//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	cout << i << endl;
//	cout << "time for PCG Loop is " << i*duration << " seconds" << endl;
//
//	double relres;
//	// returned solution is first with minimal residual
//	if (flag == 0)
//		relres = normr_act / n2b;
//	else {
//		VectorXd r_comp = B - iterapp(AFUN, xmin);
//		if (r_comp.norm() <= normr_act)
//		{
//			x = xmin;
//			iter = imin;
//			relres = r_comp.norm() / n2b;
//		}
//		else
//		{
//			iter = i;
//			relres = normr_act / n2b;
//		}
//	}
//
//	/*
//	% truncate the zeros from resvec
//	if ((flag <= 1) || (flag == 3))
//	resvec = resvec(1:ii + 1, : );
//	else
//	resvec = resvec(1:ii, : );
//	end
//
//	% only display a message if the output flag is not used
//	if (nargout < 2)
//	itermsg('pcg', tol, maxit, ii, flag, iter, relres);
//	end
//	*/
//	return x;
//}
