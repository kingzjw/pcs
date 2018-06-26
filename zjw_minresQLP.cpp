#include "zjw_minresQLP.h"

VectorXd MinresQLP::minresxxxA(MatrixXd & A, VectorXd & x)
{
	return A * x;
}

void MinresQLP::minresxxxA(MatrixXd & A, VectorXd & x, VectorXd & res_out)
{
	res_out = A * x;
}

int MinresQLP::sign(double num)
{
	if (num < 0)
		return -1;
	else if (num == 0)
		return 0;
	else
		return 1;

}

void MinresQLP::SymGivens2(double & a, double & b, double & c, double & s, double & d)
{
	if (b == 0)
	{
		if (a == 0)
			c = 1;
		else
		{
			c = sign(a); // NOTE: sign(0) = 0 in MATLAB
		}

		s = 0;
		d = abs(a);
	}
	else if (a == 0)
	{
		c = 0;
		s = sign(b);
		d = abs(b);
	}
	else if (abs(b) > abs(a))
	{
		double t = a / b;
		s = sign(b) / sqrt(1 + t*t);
		c = s*t;
		d = b / s; // computationally better than d = a / c since | c | <= | s |
	}
	else
	{
		double  t = b / a;
		c = sign(a) / sqrt(1 + t*t);
		s = c*t;
		d = a / c; // computationally better than d = b / s since | s | <= | c |
	}
}

void MinresQLP::zjw_minres_QLP(VectorXd & x_out, MatrixXd A, VectorXd b, double rtol, int maxit, bool M,
	double shift, double maxxnorm, double Acondlim, double TranCond, bool show, bool rnormvec)

{
	assert(A.rows() == b.rows());
	int n = b.rows();
	bool  precon = true;

	if (maxit == 0)
		maxit = n;

	if (!M)
	{
		//如果M是空的，没有值。这也是本程序默认的
		precon = false;
	}

	//不需要返回这两个参数。所以关于这个另个参数的就没实现
	VectorXd resvec, Aresvec;
	if (rnormvec)
	{
		resvec.setZero(maxit + 1, 1);
		Aresvec.setZero(maxit + 1, 1);
	}
	else
	{

	}

	VectorXd r2 = b;
	VectorXd r3 = b;

	//范数
	double beta1 = r2.norm();

	if (precon) {
		//如果M是空的，那么就不用执行这里
		//add something
	}

	// --------------Initialize other quantities.---------------
	double flag0 = -2;
	double flag = flag0;
	double iter = 0;
	double QLPiter = 0;
	double lines = 1;      //m
	double headlines = 20; //m
	double beta = 0;
	double tau = 0;
	double taul = 0;
	double phi = beta1;
	double betan = beta1;
	double gmin = 0;
	double cs = -1;
	double sn = 0;
	double cr1 = -1;
	double sr1 = 0;
	double cr2 = -1;
	double sr2 = 0;
	double dltan = 0;
	double eplnn = 0;
	double gama = 0;
	double gamal = 0;
	double gamal2 = 0;
	double eta = 0;
	double etal = 0;
	double etal2 = 0;
	double vepln = 0;
	double veplnl = 0;
	double veplnl2 = 0;
	double ul3 = 0;
	double ul2 = 0;
	double ul = 0;
	double u = 0;
	double rnorm = betan;
	double xnorm = 0;
	double xl2norm = 0;
	double Axnorm = 0;
	double Anorm = 0;
	double Acond = 1;
	double relres = rnorm / (beta1 + 1e-50);   //Safeguard for beta1 = 0
	VectorXd w, wl;
	x_out.setZero(n, 1);
	w.setZero(n, 1);
	wl.setZero(n, 1);//WL


					 //---------------print header if show------------------

	string first = "Enter minresQLP.  ";
	string last = "Exit minresQLP.  ";
	string msg[11] = { " beta2 = 0.  b and x are eigenvectors                   ",  //-1
		" beta1 = 0.  The exact solution is  x = 0               ", // 0
		" A solution to Ax = b found, given rtol                 ", // 1
		" Min-length solution for singular LS problem, given rtol", // 2
		" A solution to Ax = b found, given eps                  ", // 3
		" Min-length solution for singular LS problem, given eps ", // 4
		" x has converged to an eigenvector                      ", // 5
		" xnorm has exceeded maxxnorm                            ", // 6
		" Acond has exceeded Acondlim                            ", // 7
		" The iteration limit was reached                        ", // 8
		" Least-squares problem but no converged solution yet    " }; // 9

																	  //default : 不显示。
																	  //need add something show info

																	  //判断这种情况 b = 0 = > x = 0.  We will skip the main loop.
	if (beta1 == 0)
		flag = 0;

	//----------------- Main iteration ---------------------

	VectorXd r1 = VectorXd::Zero(n, 1);

	while (flag == flag0 && iter < maxit)
	{
		//===Lanczos===
		iter = iter + 1;
		double betal = beta;
		beta = betan;
		VectorXd v = r3*(1 / beta);
		minresxxxA(A, v, r3);

		//test
		//cout << r3 << endl;

		if (shift != 0)
			r3 = r3 - shift*v;
		if (iter  >  1)
			r3 = r3 - (beta / betal)* r1;

		//Allow for Hermitian A.  Must get real alfa here.
		//alfa = real(r3.transpose()* v); 
		double alfa = (r3.transpose()* v).real();
		r3 = r3 - (alfa / beta)*r2;
		r1 = r2;
		r2 = r3;


		if (!precon)
		{
			//precon = false
			betan = r3.norm();

			//Something special can happen
			if (iter == 1)
			{
				//beta2 = 0
				if (betan == 0)
				{
					if (alfa == 0)
					{
						//Ab = 0 and x = 0         ("A" = (A - shift*I)
						flag = 0;
						break;
					}
					else
					{
						flag = -1;
						// full 函数--将稀疏矩阵转化为满矩阵
						x_out = b / alfa;
						break;
					}
				}
			}
		}
		else
		{
			//precon = true
			//need to modify 下面的内容

			/*r3 = minresxxxM(M, r2);
			betan = r2.transpose * r3;
			if (betan > 0)
			betan = sqrt(betan);
			else
			printf(" M appears to be indefinite or singular.");*/
		}
		Vector3d v3d(betal, alfa, betan);
		double pnorm = v3d.norm();

		//===Apply previous left rotation Q_{k-1}===

		double dbar = dltan;
		double dlta = cs*dbar + sn*alfa;
		double epln = eplnn;
		double gbar = sn*dbar - cs*alfa;
		eplnn = sn*betan;
		dltan = -cs*betan;
		double dlta_QLP = dlta;

		//===Compute the current left plane rotation Q_k===

		double gamal3 = gamal2;
		gamal2 = gamal;
		gamal = gama;

		//double cs,sn,gama;
		SymGivens2(gbar, betan, cs, sn, gama);

		double gama_tmp = gama;
		double taul2 = taul;
		taul = tau;
		tau = cs*phi;
		Vector2d tempNorm;
		tempNorm(0) = Axnorm;
		tempNorm(1) = tau;
		Axnorm = tempNorm.norm();
		phi = sn*phi;

		//=== Apply the previous right plane rotation P{k-2,k}===
		if (iter > 2)
		{
			veplnl2 = veplnl;
			etal2 = etal;
			etal = eta;
			double dlta_tmp = sr2*vepln - cr2*dlta;
			veplnl = cr2*vepln + sr2*dlta;
			dlta = dlta_tmp;
			eta = sr2*gama;
			gama = -cr2*gama;
		}


		//===Compute the current right plane rotation P{k-1,k}, P_12, P_23,...===
		if (iter > 1)
		{
			SymGivens2(gamal, dlta, cr1, sr1, gamal);
			vepln = sr1*gama;
			gama = -cr1*gama;
		}


		//===Update xnorm ===

		double xnorml = xnorm;
		double ul4 = ul3;
		ul3 = ul2;
		if (iter > 2)
			ul2 = (taul2 - etal2*ul4 - veplnl2*ul3) / gamal2;

		if (iter > 1)
			ul = (taul - etal *ul3 - veplnl *ul2) / gamal;

		VectorXd tempV3 = VectorXd::Zero(3, 1);
		tempV3(0) = xl2norm;
		tempV3(1) = ul2;
		tempV3(2) = ul;
		double xnorm_tmp = tempV3.norm();

		//matlab中，realmin 最小正浮点数
		double realmin = 2.2251e-308;
		if (abs(gama) > realmin && xnorm_tmp < maxxnorm)
		{
			u = (tau - eta*ul2 - vepln*ul) / gama;

			VectorXd tempV2 = VectorXd::Zero(2, 1);
			tempV2(0) = xnorm_tmp;
			tempV2(1) = u;
			double xu = tempV2.norm();

			if (xu > maxxnorm)
			{
				u = 0;
				flag = 6;
			}
		}
		else
		{
			u = 0;
			flag = 9;
		}
		Vector2d temp(xl2norm, ul2);
		xl2norm = temp.norm();
		Vector3d temp3(xl2norm, ul, u);
		xnorm = temp3.norm();

		//===Update w. Update x except if it will become too big ===
		static VectorXd xl2;
		static VectorXd wl2;
		static double gamal_QLP;
		static double vepln_QLP;
		static double gama_QLP;
		static double ul_QLP;
		static double u_QLP;
		if ((Acond < TranCond) && flag != flag0 && QLPiter == 0)
		{
			//MINRES updates
			wl2 = wl;
			wl = w;
			w = (v - epln*wl2 - dlta_QLP*wl) * (1 / gama_tmp);
			if (xnorm < maxxnorm)
				x_out = x_out + tau*w;
			else
				flag = 6;
		}
		else
		{
			//MINRES-QLP updates
			QLPiter = QLPiter + 1;
			if (QLPiter == 1)
			{
				xl2 = VectorXd::Zero(n, 1);
				if (iter > 1)
				{
					//construct w_{k-3}, w_{k-2}, w_{k-1} 
					if (iter > 3)
					{
						//w_{k-3}
						wl2 = gamal3*wl2 + veplnl2*wl + etal*w;
					}

					if (iter > 2)
					{
						// w_{ k - 2 }
						wl = gamal_QLP*wl + vepln_QLP*w;
					}

					w = gama_QLP*w;
					xl2 = x_out - wl*ul_QLP - w*u_QLP;
				}
			} //end if (QLPiter == 1)

			if (iter == 1)
			{
				wl2 = wl;
				wl = v*sr1;
				w = -v*cr1;
			}
			else if (iter == 2)
			{
				wl2 = wl;
				wl = w*cr1 + v*sr1;
				w = w*sr1 - v*cr1;
			}
			else
			{
				wl2 = wl;
				wl = w;
				w = wl2*sr2 - v*cr2;
				wl2 = wl2*cr2 + v*sr2;
				v = wl *cr1 + w*sr1;
				w = wl *sr1 - w*cr1;
				wl = v;
			}
			//test
			/*cout << "xl2:" << endl;
			cout << xl2 << endl;
			cout << "wl2:" << endl;
			cout << wl2 << endl;
			cout << "ul2:" << endl;
			cout << ul2 << endl;*/
			//end test
			xl2 = xl2 + wl2*ul2;
			x_out = xl2 + wl *ul + w*u;

			//test
			//cout << "x:" << endl;
			//cout << x_out << endl;

		}//end if (Acond < TranCond) && flag != flag0 && QLPiter == 0


		 //===Compute the next right plane rotation P{k-1,k+1}===

		double gamal_tmp = gamal;
		SymGivens2(gamal, eplnn, cr2, sr2, gamal);

		//===Store quantities for transfering from MINRES to MINRES-QLP ===

		gamal_QLP = gamal_tmp;
		vepln_QLP = vepln;
		gama_QLP = gama;
		ul_QLP = ul;
		u_QLP = u;

		//===** Estimate various norms ===

		double abs_gama = abs(gama);
		double Anorml = Anorm;
		Vector4d tempV4(Anorm, pnorm, gamal, abs_gama);
		Anorm = tempV4.maxCoeff();
		//Anorm = max([Anorm, pnorm, gamal, abs_gama]);

		static double gminl;
		if (iter == 1)
		{
			gmin = gama;
			gminl = gmin;
		}
		else if (iter > 1)
		{
			double gminl2 = gminl;
			gminl = gmin;

			Vector3d tempV3(gminl2, gamal, abs_gama);
			gmin = tempV3.minCoeff();
			//gmin = min([gminl2, gamal, abs_gama]);
		}

		double Acondl = Acond;
		Acond = Anorm / gmin;
		double rnorml = rnorm;
		double relresl = relres;

		if (flag != 9)
			rnorm = phi;

		relres = rnorm / (Anorm*xnorm + beta1);
		Vector2d tempV2(gbar, dltan);
		double rootl = tempV2.norm();
		double Arnorml = rnorml*rootl;
		double relAresl = rootl / Anorm;

		//===See if any of the stopping criteria are satisfied.===
		double eps = 2.2204e-16;
		double epsx = Anorm*xnorm*eps;
		if ((flag == flag0) || (flag == 9))
		{

			double t1 = 1 + relres;
			double t2 = 1 + relAresl;

			if (iter >= maxit)
				flag = 8; // Too many itns
			if (Acond >= Acondlim)
				flag = 7; // Huge Acond
			if (xnorm >= maxxnorm)
				flag = 6; // xnorm exceeded its limit
			if (epsx >= beta1)
				flag = 5; // x is an eigenvector
			if (t2 <= 1)
				flag = 4;// Accurate LS solution
			if (t1 <= 1)
				flag = 3; //Accurate Ax = b solution
			if (relAresl <= rtol)
				flag = 2; //Good enough LS solution
			if (relres <= rtol)
				flag = 1; //Good enough Ax = b solution
		}


		/** The "disable" option allowed iterations to continue until xnorm
		* became large and x was effectively a nullvector.
		* We know that r will become a nullvector much sooner,
		* so we now disable the disable option : )

		* if disable && (iter < maxit)
		*	flag = 0;
		*    if Axnorm < rtol*Anorm*xnorm
		*      flag = 10;
		*    end
		*  end*/

		// Possibly singular
		if (flag == 2 || flag == 4 || flag == 6 || flag == 7)
		{
			iter = iter - 1;
			Acond = Acondl;
			rnorm = rnorml;
			relres = relresl;
		}
		else
		{
			//if (~isempty(resvec))
			if (resvec.rows() >0)
			{
				resvec(iter + 1) = rnorm;
				Aresvec(iter) = Arnorml;
			}

			//if (show && mod(iter, lines) == 0)
			if (show && ((int)iter * (int)lines) == 0)
			{
				if (iter == 101)
				{
					lines = 10;
					headlines = 20 * lines;
				}
				else if (iter == 1001)
				{
					lines = 100;
					headlines = 20 * lines;
				}

				if (QLPiter == 1)
					printf("*s", "P");
				else
					printf("*s", " ");

				printf("*7g *10.2e *10.2e *10.2e *10.2e *10.2e *10.2e *10.2e\n", iter - 1, rnorml, Arnorml, relresl, relAresl, Anorml, Acondl, xnorml);

				//if (iter > 1 && mod(iter, headlines) == 1)
				if (iter > 1 && ((int)iter * (int)headlines) == 1)
					printf("\n    iter     rnorm     Arnorm   Compatible     LS        Anorm      Acond      xnorm \n");
			}
		}
	}//while



	 //We have exited the main loop.
	if (QLPiter == 1)
	{
		printf("*s", "P");
	}
	else
	{

		printf("*s", " ");
	}
	double Miter = iter - QLPiter;

	//Compute final quantities directly.

	//r1 is workspace for residual vector
	//r1 = b - minresxxxA(A, x_out) + shift*x_out;   
	VectorXd tempV;
	minresxxxA(A, x_out, tempV);
	r1 = b - tempV + shift*x_out;

	rnorm = r1.norm();
	//double Arnorm = (minresxxxA(A, r1) - shift*r1).norm();
	minresxxxA(A, r1, tempV);
	double Arnorm = (tempV - shift*r1).norm();
	xnorm = x_out.norm();
	relres = rnorm / (Anorm*xnorm + beta1);
	double relAres = 0;

	if (rnorm > 2.2251e-308)
		relAres = Arnorm / (Anorm*rnorm);


	if (Aresvec.rows() > 0)
	{
		//default 参数是不做的这个的

		//Aresvec(iter + 1) = Arnorm;
		//Aresvec = Aresvec(1:iter + 1);
	}


	if (resvec.rows() > 0)
	{
		//default 参数是不做的这个的
		//resvec = resvec(1:iter + 1);
	}

	if (show)
	{
		if (rnorm > 2.2251e-308)
			printf("*7g *10.2e *10.2eD*10.2e *10.2eD*10.2e *10.2e *10.2e\n\n", iter, rnorm, Arnorm, relres, relAres, Anorm, Acond, xnorm);
		else
			printf("*7g *10.2e *10.2eD*10.2e *10.2e             *10.2e *10.2e\n\n", iter, rnorm, Arnorm, relres, Anorm, Acond, xnorm);
	}

	//	printf("*s flag  =*7g  *s\n", last, flag, msg[(int)flag + 2]);
	//printf("*s iter  =*7g   (MINRES*7g, MINRES-QLP*7g)\n", last, iter, Miter, QLPiter);
	/*printf("*s rnorm = *11.4e     rnorm  direct = *11.4e\n", last, rnorm, r1.norm());
	printf("*s                         Arnorm direct = *11.4e\n", last, Arnorm);
	printf("*s xnorm = *11.4e     xnorm  direct = *11.4e\n", last, xnorm, x_out.norm());
	printf("*s Anorm = *11.4e     Acond         = *11.4e\n", last, Anorm, Acond);*/


}

int MinresQLP::example1()

{
	int num = 100;
	MatrixXd A = MatrixXd::Random(num, num);

	//对称矩阵
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < i; j++)
		{
			A(i, j) = A(j, i);
		}
	}

	cout << "A:" << endl;
	//cout << A << endl << endl;


	VectorXd x_input = VectorXd::Random(num, 1);

	VectorXd b = A * x_input;
	VectorXd x;
	//ZjwTimer time;
	//time.Start();
	zjw_minres_QLP(x, A, b, 1e-6, num);
	//time.Stop();
	//time.printTimeInMs("minres QLP : ");
	cout << "x input :" << endl;
	//cout << x_input << endl<<endl;

	cout << "res x :" << endl;
	//cout << x << endl;

	cout << "diff:" << endl;
	cout << x - x_input << endl;

	system("pause");

	return 0;
}
