/*
* 求解函数的在区间范围内容的最小值
*/
#pragma once
#include <string>
#include <algorithm>
using namespace std;

/*
测试用例：
	double f(double d)
	{
		//return cos(d);
		//return d * d;
		//)x. ^ 3 - 2 * x - 5;
		return 3 * exp(-d - 4 * d*d);
		//return d*d*d - 2 * d - 5;
	}
	double testFmin()
	{
		int xmin = 1;
		int xmax = 3;
		return fminbnd(f, xmin, xmax);
	}
*/
typedef double(*G)(double x);

static int sign(double x)
{
	return x > 0 ? 1 : (x == 0 ? 0 : -1);
}

/*
	x = fminbnd(fun,x1,x2) returns a value x that is a local minimizer of the function
	that is described in fun in the interval x1 <= x <= x2. fun is a function handle.

	求函数funfcn的最小值
*/
static double fminbnd(G funfcn, double ax, double bx)
{
	//nargin==3 && nargout == 1

	/*
	defaultopt = struct(...
		'Display', 'notify', ...
		'FunValCheck', 'off', ...
		'MaxFunEvals', 500, ...
		'MaxIter', 500, ...
		'OutputFcn', [], ...
		'PlotFcns', [], ...
		'TolX', 1e-4);
	*/
	//option = []

	string printtype = "notify";
	double tol = 1e-4;
	int funValCheck = 0;

	int maxfun = 500, maxiter = 500;
	int funccount = 0, iter = 0;
	//xf = []; fx = [];

	int print = 1;
	//outputfcn = [];
	bool haveoutptfcn = false;
	//plotfcns = [];
	bool haveplotfcn = false;

	/*% checkbounds
		if ax > bx
			exitflag = -2;
	xf = []; fval = [];
	msg = getString(message('MATLAB:fminbnd:ExitingLowerBoundExceedsUpperBound'));
	if print > 0
		disp(' ')
		disp(msg)
		end
		output.iterations = 0;
	output.funcCount = 0;
	output.algorithm = 'golden section search, parabolic interpolation';
	output.message = msg;
	% Have not initialized OutputFcn; do not need to call it before returning
		return
		end*/

	int exitflag = 1;

	string header = " Func-count     x          f(x)         Procedure";
	string procedure = "       initial";

	//Compute the start point
	double seps = sqrt(pow(2, -52));
	double c = 0.5*(3.0 - sqrt(5.0));
	double a = ax, b = bx;
	double v = a + c*(b - a);
	double w = v, xf = v;
	double d = 0.0, e = 0.0;
	double x = xf, fx = funfcn(x);
	funccount = funccount + 1;

	double fv = fx, fw = fx;
	double xm = 0.5*(a + b);
	double tol1 = seps*abs(xf) + tol / 3.0;
	double tol2 = 2.0*tol1;

	//Main loop
	while (abs(xf - xm) > (tol2 - 0.5*(b - a)))
	{
		int gs = 1;
		if (abs(e) > tol1)
		{
			gs = 0;
			double r = (xf - w)*(fx - fv);
			double q = (xf - v)*(fx - fw);
			double p = (xf - v)*q - (xf - w)*r;
			q = 2.0*(q - r);
			if (q > 0.0)
				p = -p;
			q = abs(q);
			r = e, e = d;

			if ((abs(p) < abs(0.5*q*r)) && (p > q*(a - xf)) && (p < q*(b - xf)))
			{
				d = p / q;
				x = xf + d;
				procedure = "       parabolic";

				if ((x - a) < tol2 || (b - x) < tol2)
				{
					double si = sign(xm - xf) + ((xm - xf) == 0);
					d = tol1*si;
				}
			}
			else
			{
				gs = 1;
			}
		}
		if (gs)
		{
			if (xf >= xm)
				e = a - xf;
			else
				e = b - xf;
			d = c*e;
			procedure = "       golden";
		}
		double si = sign(d) + (d == 0);
		x = xf + si * max(abs(d), tol1);
		double fu = funfcn(x);
		funccount = funccount + 1;

		iter = iter + 1;
		//Update a, b, v, w, x, xm, tol1, tol2
		if (fu <= fx)
		{
			if (x >= xf)
				a = xf;
			else
				b = xf;
			v = w; fv = fw;
			w = xf; fw = fx;
			xf = x; fx = fu;
		}
		else
		{
			if (x < xf)
				a = x;
			else
				b = x;
			if (((fu <= fw) || (w == xf)))
			{
				v = w; fv = fw;
				w = x; fw = fu;
			}
			else if ((fu <= fv) || (v == xf) || (v == w))
			{
				v = x; fv = fu;
			}
			else;
		}
		xm = 0.5*(a + b);
		tol1 = seps*abs(xf) + tol / 3.0; tol2 = 2.0*tol1;

		if ((funccount >= maxfun) || (iter >= maxiter))
		{
			exitflag = 0;
			return xf;
		}
	}
	return xf;
}