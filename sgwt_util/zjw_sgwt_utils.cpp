#include "zjw_sgwt_utils.h"

GN::GN()
{
	tj = 0;
}

double GN::operator()(double x)
{
	x = x*tj;
	return Sgwt::sgwt_kernel_abspline3(x);
}

void GN::setTj(const double tj)
{
	this->tj = tj;
}

//h(x)�й�
double Hx(double x)
{
	return exp(-pow(x, 4));
}

//h(x)��
//double G1P(double x)
//{
//	return -4 * pow(x, 3)*exp(-pow(x, 4));
//}

Sgwt::Sgwt(const int _m, const int _Nscales, SpMat _L) :m(_m), Nscales(_Nscales), lap(_L)
{
#ifdef ZJW_DEBUG
	cout << "num of scales : " << _Nscales << endl << "polygon max rank : " << _m << endl;
#endif //zjw_debug

	arange[0] = -1;
	arange[1] = 1;

	//g,������h
	g = new GN[_Nscales];

	//coeff,����h
	VectorXd init_c(m + 1);
	init_c.setZero();
	for (int i = 0; i < _Nscales + 1; i++)
		coeff.push_back(init_c);

	for (int i = 0; i < sgwtCoeff_WS.size(); i++)
	{
		sgwtCoeff_WS[i].clear();
	}
	sgwtCoeff_WS.clear();

	//6���źţ�8������
	sgwtCoeff_WS.resize(6 * 8);
	/*signalList.clear();
	quadrantList.clear();*/

};

double Sgwt::sgwt_rough_lmax()
{
	//change the matrix to��������ĺ���
	SparseGenMatProd<double> op(lap);

	// Construct eigen solver object, requesting the largest three eigenvalues
	GenEigsSolver< double, LARGEST_MAGN, SparseGenMatProd<double> > eigs(&op, 1, 10);

	// Initialize and compute
	eigs.init();
	int nconv = eigs.compute();

	// Retrieve results
	Eigen::VectorXcd evalues;
	if (eigs.info() == SUCCESSFUL)
	{
		evalues = eigs.eigenvalues();
	}

	double size = 1.01;

#ifdef ZJW_DEBUG
	std::cout << "max Eigenvalues found:  " << evalues(0).real() <<
		"    ( but we magnify 1.01 , use " << evalues(0).real() * size << " instead !!!!!!!!!) " << std::endl;
#endif //zjw_debug

	return evalues(0).real() * size;
}

void Sgwt::setArange(const double lmin, const double lmax)
{
#ifdef ZJW_DEBUG
	cout << "lmin : " << lmin << "  lamx : " << lmax << endl;
#endif //zjw_debug
	arange[0] = lmin;
	arange[1] = lmax;
}

double Sgwt::sgwt_kernel_abspline3(double x)
{
	int alpha = 2, beta = 2, t1 = 1, t2 = 2;
	Matrix4d M;
	M << 1, t1, t1*t1, t1*t1*t1,
		1, t2, t2*t2, t2*t2*t2,
		0, 1, 2 * t1, 3 * t1*t1,
		0, 1, 2 * t2, 3 * t2*t2;
	Vector4d v;
	v << 1, 1, pow(t1, -alpha)*alpha*pow(t1, alpha - 1), -beta*pow(t2, -beta - 1)*pow(t2, beta);
	Vector4d a;
	a = M.inverse()*v;
	if (x >= 0 && x < t1)
		return pow(x, alpha)*pow(t1, -alpha);
	else if (x >= t1&&x < t2)
		return a(0) + a(1)*x + a(2)*x*x + a(3)*x*x*x;
	else
		return pow(x, -beta)*pow(t2, beta);
}

double Sgwt::_sgwt_kernel_abspline3(double x)
{
	return -sgwt_kernel_abspline3(x);
}

//����kernel g ��scales��������vector
VectorXd Sgwt::sgwt_setscales(double lmin, double lmax)
{
	int t1 = 1;
	int t2 = 2;

	double smin = t1 / lmax;
	double smax = t2 / lmin;
	//scales should be decreasing ... higher j should give larger s
	//�ܹ����� Nscales���㣬jԽ��ά��Խ��
	VectorXd s = VectorXd::LinSpaced(Nscales, log(smax), log(smin));

	////paper zjw
	//double smin = va.t2 / lmax;
	//double smax = va.t2 / lmin;

	//VectorXd s = VectorXd::LinSpaced(Nscales, log(smax), log(smin));

	//test
	//cout << s;
	//end test
	//VectorXd::LinSpaced(size,low,high)

	//zjw
	//return s;

	for (int i = 0; i < Nscales; i++)
	{
		s[i] = exp(s[i]);
	}

#ifdef ZJW_DEBUG
	cout << "( sgwt_setscales ) g scales: ";
	for (int i = 0; i < s.rows(); i++)
	{
		cout << s[i] << "  ";
	}
	cout << endl;
#endif // ZJW_DEDUG

	return s;
}

//�õ�h(x)�������ú���صĲ������ڵ�������ӿ�֮ǰ�����õ�lamda max ������ֵ
void Sgwt::sgwt_filter_design(double lmax, Varargin varargin)
{
	if (varargin.designtype == "abspline3")					//��ʱ��ʵ��ʹ�õ���һ��
	{
		//ͨ��lamda max������С�� lamda min
		double lmin = lmax / varargin.K;

		t = sgwt_setscales(lmin, lmax);

		//hx��ָ��h(x)�ĺ���ָ��
		G hx = Hx;
		//G g1p = G1P;

		//scales ����
		for (int j = 0; j < Nscales; j++) {
			g[j].setTj(t[j]);
		}

		//��������Сֵ�ĺ����������ֵ����Ҫ�ְ�ֵ��ȡ��
		//fָ�� _sgwt_kernel_abspline3 ���������Եõ�  g(x)�ĸ�ֵ
		G f = _sgwt_kernel_abspline3;
		//��x1��x2֮����f����Сֵ����ʵ��g��x�������ֵ
		double xstar = fminbnd(f, 1, 2);

		//����õ� h(x)��gama������g(x)�����䷶Χ�ڵ����ֵ����Ϊǰ��ȡ��������Ҫ��������

		//��ʾ: ��
		double gamma_l = -(f(xstar));
		//��ʾ: ��min * 0.6
		double lminfac = 0.6*lmin;
		h0 = new HX(hx, lminfac, gamma_l);
		//cout << xstar << endl;   !!!
		//cout << gamma_l << endl; !!!
#ifdef ZJW_DEBUG
		cout << "( sgwt_filter_design ) we get hx function !! " << endl;
#endif //ZJW_DEDUG
	}
	else
	{
		cout << "sgwt_filter_design not support !!!" << endl;
	}
}

template<class T>
void Sgwt::sgwt_cheby_coeff(int j, T g)
{
	//N ��ʾ�����ö��ٴε�������
	int N = m + 1;

	double a1 = (arange[1] - arange[0]) / 2;
	double a2 = (arange[1] + arange[0]) / 2;

	//zjw  ��������������
	/*double a1 = (arange[1]) / 2;
	double a2 = a1;*/

	//cout << c[k] << endl;
	//cout << c[k].size() << endl;

	//�ӵͽ���0,�������߽���m
	for (int m_it = 0; m_it <= m; m_it++)
	{
		//����������ȡ������
		//(M_PI*(i - 0.5)) / N��ȡֵ��Χ: ����0 - Pi
		for (int i = 1; i <= N; i++)
		{
			//���㵱ǰ��theta
			double theta = 3.1415926 * (i - 0.5) / N;
			//  2/M_PI    *    cos(k_it*theta) * g(a1 * cos((theta) + a2) * (M_PI-0)/N ����õ������ʽ��
			//  tn�ĳ߶���Ϣ����g�����ڲ���
			coeff[j](m_it) += 2.0 / N * cos(m_it*theta) * g(a1 * cos((theta)+a2));

			//test
			/*cout << 2.0 / N << endl;
			cout << cos(m_it*theta) << endl;
			cout << 2.0 / N * cos(m_it*theta) << endl;
			cout << g(a1 * cos((theta)+a2)) << endl;
			cout << "coeff : " << coeff[j](m_it) << endl;*/
			//end
		}
	}
}

vector<VectorXd> Sgwt::sgwt_cheby_op(VectorXd f, vector<VectorXd> coeff)
{
	//timer
	clock_t start, finish;
	double duration;
	start = clock();

	//����ÿ���߶��µģ���߽׵�ϵ����
	//cur nsacles ����h ��g
	int cur_nscales = coeff.size();
	VectorXd M(cur_nscales);

	//�洢ÿ���߶��µ�ϵ��
	vector<VectorXd> sgwtCoeff_WS;
	VectorXd init_r(f.size());
	init_r.setZero();

	for (int i = 0; i < cur_nscales; i++)
		sgwtCoeff_WS.push_back(init_r);

	//��¼��ͬ�߶��£������Ǹ�Mj (����ʽ����)
	int maxM = 0;
	for (int i = 0; i < cur_nscales; i++)
	{
		M[i] = coeff[i].size();
		if (M[i] > maxM)
			maxM = M[i];
	}

	double a1 = (arange[1] - arange[0]) / 2;
	double a2 = (arange[1] + arange[0]) / 2;
	//zjw
	/*double a1 = (arange[1]) / 2;
	double a2 = a1;*/

	//Twf_old������ԭ�е��ź� f
	VectorXd Twf_old(f);
	//���þ�����˵ĵݹ飬�������ʽ����
	VectorXd Twf_cur(f.size());
	//����չ��ʽ�� T1����Ľ��  (L-a)/a * f
	Twf_cur = (lap*f - a2*f) / a1;

	//-----����ÿ���߶�����ģ�����Wf����չ��ʽ�еĵ�0��͵�һ�� �ĺ�,�Ѽ��������ϵ�����浽sgwtCoeff_W ��-------
	for (int j = 0; j < cur_nscales; j++)
	{
		// t[i]  n * 1
		sgwtCoeff_WS[j] = 0.5*coeff[j](0)*Twf_old + coeff[j](1)*Twf_cur;
	}

	//----------------------����չ���е�ÿһ��ۼӵ�ϵ��sgwtCoeff_W��-------------------------
	VectorXd Twf_new;
	for (int k = 1; k < maxM; k++)
	{
		/*VectorXd tmp = L*Twf_cur;		//< 3ms
		Twf_new = (tmp - a2*Twf_cur);		// <2ms
		Twf_new *= 2 / a1;					// <1ms
		Twf_new-= Twf_old;				// <1ms*/

		//����� k+1 ���Ǹ�Tk�ġ���k = 1 ,����� T2��ֵ
		// T2 = T1��T0�����
		Twf_new = (2.0 / a1) * (lap * Twf_cur - a2 * Twf_cur) - Twf_old;			// 6~7ms
		//------���Ե����Ϸ����ķ���������ݹ飬ͬʱ�������߶�----
		for (int j = 0; j < cur_nscales; j++)
		{
			if (1 + k < M(j))
			{
				//�ۼӵ�sgwt��ϵ����  r[j]��ʾ�߶ȵ�j���ϵ��
				sgwtCoeff_WS[j] = sgwtCoeff_WS[j] + coeff[j](k + 1)*Twf_new;
			}
		}
		Twf_old = Twf_cur;
		Twf_cur = Twf_new;
	}
	// time
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	return sgwtCoeff_WS;
}

bool Sgwt::sgwt_cheby_op(VectorXd * f, vector<VectorXd>* c, vector<VectorXd>* sgwt_out)
{
#ifdef  ZJW_TIMER
	//timer
	clock_t start, finish;
	double duration;
	start = clock();
#endif //  zjw_TIMER

	//����ÿ���߶��µģ���߽׵�ϵ����
	//cur nsacles ����h ��g
	int cur_nscales = coeff.size();
	VectorXd M(cur_nscales);

	//�洢ÿ���߶��µ�ϵ��
	assert(sgwt_out);
	VectorXd init_r(f->size());
	init_r.setZero();

	for (int i = 0; i < cur_nscales; i++)
		sgwt_out->push_back(init_r);

	//��¼��ͬ�߶��£������Ǹ�Mj (����ʽ����)
	int maxM = 0;
	for (int i = 0; i < cur_nscales; i++)
	{
		M[i] = coeff[i].size();
		if (M[i] > maxM)
			maxM = M[i];
	}

	double a1 = (arange[1] - arange[0]) / 2;
	double a2 = (arange[1] + arange[0]) / 2;
	//zjw
	/*double a1 = (arange[1]) / 2;
	double a2 = a1;*/

	//Twf_old������ԭ�е��ź� f
	VectorXd Twf_old(f);
	//���þ�����˵ĵݹ飬�������ʽ����
	VectorXd Twf_cur(f->size());
	//����չ��ʽ�� T1����Ľ��  (L-a)/a * f
	Twf_cur = (lap*(*f) - a2*(*f)) / a1;

	//-----����ÿ���߶�����ģ�����Wf����չ��ʽ�еĵ�0��͵�һ�� �ĺ�,�Ѽ��������ϵ�����浽sgwtCoeff_W ��-------
	for (int j = 0; j < cur_nscales; j++)
	{
		// t[i]  n * 1
		(*sgwt_out)[j] = 0.5*coeff[j](0)*Twf_old + coeff[j](1)*Twf_cur;
	}

	//----------------------����չ���е�ÿһ��ۼӵ�ϵ��sgwtCoeff_W��-------------------------
	VectorXd Twf_new;
	for (int k = 1; k < maxM; k++)
	{
		/*VectorXd tmp = L*Twf_cur;		//< 3ms
		Twf_new = (tmp - a2*Twf_cur);		// <2ms
		Twf_new *= 2 / a1;					// <1ms
		Twf_new-= Twf_old;				// <1ms*/

		//����� k+1 ���Ǹ�Tk�ġ���k = 1 ,����� T2��ֵ
		// T2 = T1��T0�����
		Twf_new = (2.0 / a1) * (lap * Twf_cur - a2 * Twf_cur) - Twf_old;
		//------���Ե����Ϸ����ķ���������ݹ飬ͬʱ�������߶�----
		for (int j = 0; j < cur_nscales; j++)
		{
			if (1 + k < M(j))
			{
				//�ۼӵ�sgwt��ϵ����  r[j]��ʾ�߶ȵ�j���ϵ��
				(*sgwt_out)[j] = (*sgwt_out)[j] + coeff[j](k + 1)*Twf_new;
			}
		}
		Twf_old = Twf_cur;
		Twf_cur = Twf_new;
	}
#ifdef  ZJW_TIMER
	// time
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
#endif //  zjw_TIMER

	return true;
}

bool Sgwt::saveSgwtCoeff(SignalType type, int quadrant, VectorXd * f, vector<VectorXd>* c)
{
	assert(quadrant > -1 && quadrant < 8);
	assert(f);
	assert(c);

	int index = type * 8 + quadrant;
	vector<VectorXd> tempSGWT;
	sgwt_cheby_op(f, c, &tempSGWT);
	sgwtCoeff_WS[index] = tempSGWT;
	return true;
}

bool Sgwt::sgwt_cheby_op(int nodeIdx, int signalType, int quadrantType, VectorXd * sgwt_out)
{
	assert(quadrantType > -1 && quadrantType < 8);

	int index = signalType * 8 + quadrantType;

	if (sgwt_out)
		delete sgwt_out;
	sgwt_out = new VectorXd(coeff.size());

	for (int i = 0; i < coeff.size(); i++)
	{
		(*sgwt_out)(i) = sgwtCoeff_WS[index][i](nodeIdx);
	}

	return true;
}

VectorXd Sgwt::sgwt_adjoint(vector<VectorXd> y)
{
	VectorXd adj(lap.rows());
	adj.setZero();

	for (int j = 0; j < coeff.size(); j++)
	{
		vector<VectorXd> _c;
		_c.push_back(coeff[j]);
		VectorXd tmp = sgwt_cheby_op(y[j], _c)[0];
		adj = adj + tmp;
	}
	return adj;
}

VectorXd Sgwt::sgwt_cheby_square(VectorXd c)
{
	int M = c.size() - 1;
	VectorXd cp(c);
	cp(0) = 0.5*c(0);

	VectorXd dp(2 * M + 1);
	dp.setZero();

	for (int m = 0; m <= 2 * M; m++)
	{
		if (m == 0)
		{
			dp(m) = dp(m) + 0.5*cp(0)*cp(0);
			for (int i = 0; i <= M; i++)
				dp(m) = dp(m) + 0.5*cp(i)*cp(i);
		}
		else if (m <= M)
		{
			for (int i = 0; i <= m; i++)
				dp(m) = dp(m) + 0.5*cp(i)*cp(m - i);
			for (int i = 0; i <= M - m; i++)
				dp(m) = dp(m) + 0.5*cp(i)*cp(m + i);
			for (int i = m; i <= M; i++)
				dp(m) = dp(m) + 0.5*cp(i)*cp(i - m);
		}
		else
		{
			for (int i = m - M; i <= M; i++)
			{
				dp(m) = dp(m) + 0.5*cp(i)*cp(m - i);
			}
		}
	}
	VectorXd d(dp);
	d(0) = 2 * dp(0);
	return d;
}

void Sgwt::printVectorVectorXd(vector<VectorXd>& vv)
{
	cout << "==============================" << endl;
	for (int i = 0; i < vv.size(); i++)
	{
		cout << "vector( column ) " << i << endl << "rows " << vv[i].rows() << endl;
		for (int j = 0; j < vv[i].rows(); j++)
		{
			cout << vv[i](j) << " ";
		}
		cout << endl << "--------------------------------" << endl;
	}
	cout << "==============================" << endl;
}

void Sgwt::getVectorVectorXdInfo(vector<VectorXd>& vv)
{
	cout << "vector( column ) " << vv.size() << endl << "vectorXd rows " << vv[0].rows() << endl;
}

//VectorXd Sgwt::sgwt_inverse(vector<VectorXd> y)
//{
//	cout << "sgwt_inverse is not implement !!" << endl;
//
//	double tol = 1e-6;
//	int N = lap.rows();
//	clock_t start, finish;
//	double duration;
//	start = clock();
//	VectorXd adj = sgwt_adjoint(y);
//	finish = clock();
//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	cout << "time for sgwt_adjoint is " << duration << " seconds" << endl;
//
//	int maxM = 0;
//	VectorXi M(c.size());
//	for (int i = 0; i < c.size(); i++)
//	{
//		M[i] = c[i].size();
//		if (M[i] > maxM)
//			maxM = M[i];
//	}
//
//	VectorXd init_d(1 + 2 * (maxM - 1));
//	init_d.setZero();
//	vector<VectorXd> d;
//	d.push_back(init_d);
//
//	start = clock();
//	vector<VectorXd> cpad;
//	for (int j = 0; j < c.size(); j++)
//	{
//		VectorXd init_cpad(maxM);
//		for (int i = 0; i < M[j]; i++)
//			init_cpad[i] = c[j](i);
//		cpad.push_back(init_cpad);
//		VectorXd S = sgwt_cheby_square(cpad[j]);
//		d[0] = d[0] + S;
//	}
//	finish = clock();
//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	cout << "time for sgwt_cheby_square is " << duration << " seconds" << endl;
//
//	start = clock();
//	Handle_sgwt_cheby_op wstarw(m, Nscales, lap, d, arange);
//	VectorXd r = pcg(wstarw, adj, tol);
//	finish = clock();
//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	cout << "time for PCG is " << duration << " seconds" << endl;
//	return r;
//}

SgwtCheby::SgwtCheby(int m, int Nscales, SpMat& L, vector<VectorXd> &coeff, double *arange)
{
	sgwt = new Sgwt(m, Nscales, L);
	sgwt->setArange(arange[0], arange[1]);
	chebyCoeff = coeff;
}

SgwtCheby::SgwtCheby(int m, int Nscales, SpMat &L)
{
#ifdef ZJW_DEBUG
	cout << "SgwtCheby: polygon max degree= " << m << "  g func scale num= " << Nscales << endl;
#endif //zjw_debug
	sgwt = new Sgwt(m, Nscales, L);

	sgwtDoChebyPrepare();
	chebyCoeff = sgwt->coeff;

#ifdef PRINT_CHEBY_COEFF
	cout << "chebyCoeff: " << endl;
	sgwt->printVectorVectorXd(chebyCoeff);
	cout << "chebyCoeff!!!!!" << endl << endl << endl;
#endif //PRINT_CHEBY_COEFF
}

void SgwtCheby::sgwtDoChebyPrepare()
{
	//�õ�����ֵ����������ز���
	double lamdaMax = sgwt->sgwt_rough_lmax();
	double lamdaMin = lamdaMax / sgwt->va.K;
	sgwt->setArange(lamdaMin, lamdaMax);

	//���ó߶���Ϣ�������ú� g h�����������Ϣ
	sgwt->sgwt_filter_design(lamdaMax, sgwt->va);

	//Ϊ���еĳ߶� g��h ��������Ӧ��ϵ��
	for (int j = 0; j < sgwt->Nscales; j++) {
		sgwt->sgwt_cheby_coeff(j, sgwt->g[j]);
	}
	sgwt->sgwt_cheby_coeff(sgwt->Nscales, *(sgwt->h0));
}

//����һ���ź�
vector<VectorXd> SgwtCheby::operator()(VectorXd f)
{
	assert(f.rows() > 0);
	return sgwt->sgwt_cheby_op(f, chebyCoeff);
}

void SgwtCheby::operator()(int nodeIdx, int signalType, int quadrantType, VectorXd* sgwt_out)
{
	sgwt->sgwt_cheby_op(nodeIdx, signalType, quadrantType, sgwt_out);

	return; 
}