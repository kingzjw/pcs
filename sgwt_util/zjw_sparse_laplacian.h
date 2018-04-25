#pragma once
#include <iostream>
#include <cmath>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Sparse>
using namespace std;
using namespace Eigen;

struct Option {
	int symmetrize;
	int normalize;

	Option(int _symmetrize, int _normalize) :symmetrize(_symmetrize), normalize(_normalize)
	{};
};

enum Laplacian_Type {
	combinatorial, distance, conformal
};

typedef Eigen::SparseMatrix<double> SpMat;
// 声明一个列优先的双精度稀疏矩阵类型

/*
% compute_mesh_laplacian - compute a laplacian matrix
%
%   L = compute_mesh_laplacian(vertex, face, type, options);
%
%   If options.symmetrize = 1 and options.normalize = 0 then
%       L = D - W
%   If options.symmetrize = 1 and options.normalize = 1 then
%       L = eye(n) - D^{ -1 / 2 }*W*D^{ -1 / 2 }
%   If options.symmetrize = 0 and options.normalize = 1 then
%       L = eye(n) - D^{ -1 }*W.
%   where D = diag(sum(W, 2)) and W is the unormalized weight matrix
% (see compute_mesh_weight).
%
%   type can 'combinatorial', 'distance', 'conformal'.
%
%   See also compute_mesh_weight.
%
%   Copyright(c) 2007 Gabriel Peyre
*/
SpMat sparse(VectorXi fi, VectorXi fj, VectorXd s)
{
	int size = 0;
	for (int i = 0; i < fi.size(); i++)
		if (fi(i) > size)
			size = fi(i);
	SpMat A(size, size);
	for (int i = 0; i < fi.size(); i++)
	{
		A.coeffRef(fi(i) - 1, fj(i) - 1) = s(i);
	}
	A.makeCompressed();
	return A;
}

//行列求和
VectorXd sum(SpMat W, int type)
{
	//每一列求和
	if (type == 1)
	{
		VectorXd sums(W.cols());
		sums.setZero();
		for (int k = 0; k < W.outerSize(); ++k)
			for (SparseMatrix<double>::InnerIterator it(W, k); it; ++it)
			{
				sums(it.col()) += it.value();
			}
		return sums;
	}
	else
	{
		//每一行求和
		VectorXd sums(W.rows());
		sums.setZero();
		for (int k = 0; k < W.outerSize(); ++k)
			for (SparseMatrix<double>::InnerIterator it(W, k); it; ++it)
			{
				sums(it.row()) += it.value();
			}
		return sums;
	}
}

SpMat diag(VectorXd vec)
{
	SpMat dia(vec.size(), vec.size());
	dia.reserve(VectorXi::Constant(vec.size(), 1));
	for (int i = 0; i < vec.size(); i++)
	{
		dia.insert(i, i) = vec[i];
	}

	//压缩剩余的空间
	dia.makeCompressed();
	return dia;
}

//单位稀疏矩阵
SpMat speye(int n)
{
	SpMat eyes(n, n);
	eyes.reserve(VectorXi::Constant(n, 1));
	for (int i = 0; i < n; i++)
	{
		eyes.insert(i, i) = 1;
	}
	eyes.makeCompressed();		//压缩剩余的空间
	return eyes;
}

template<class T>
T check_size(T A, int vmin, int vmax)
{
	if (A.rows() > A.cols())
		return A.adjoint();
	double x = 0;
	for (int i = 0; i < A.rows(); i++)
		x += abs(A(i, 2));
	if (A.rows() <= 3 && A.cols() >= 3 && x == 0)
		return A.adjoint();
	/*if (a.rows() < vmin || a.cols() > vmax)
	{
	//error('face or vertex is not of correct size');
	}*/
	return A;
}

SpMat triangulation2adjacency(MatrixXi _face)
{
	MatrixXi face = check_size<MatrixXi>(_face, 3, 4);
	MatrixXi f = _face.adjoint();

	VectorXi f1(f.rows() * 6);
	f1 << f.col(0), f.col(0), f.col(1), f.col(1), f.col(2), f.col(2);

	VectorXi f2(f.rows() * 6);
	f2 << f.col(1), f.col(2), f.col(0), f.col(2), f.col(0), f.col(1);

	VectorXd s(f.rows() * 6);
	s.setConstant(s.size(), 1.0);

	SpMat A = sparse(f1, f2, s);

	/*% avoid double links
		a = double(a>0);*/
	return A;
}

SpMat my_euclidean_distance(SpMat A, MatrixXd vertex)
{
	if (vertex.rows() < vertex.cols())
		return my_euclidean_distance(A, vertex.adjoint());
	VectorXi fi(A.nonZeros()), fj(A.nonZeros());
	VectorXd s(A.nonZeros());
	int count = 0;
	for (int k = 0; k < A.outerSize(); ++k)
	{
		for (SparseMatrix<double>::InnerIterator it(A, k); it; ++it)
		{
			fi(count) = it.row() + 1;   // 行标row index
			fj(count++) = it.col() + 1;   // 列标（此处等于k）
		}
	}
	VectorXd d(A.nonZeros());
	d.setZero();
	for (int i = 0; i < A.nonZeros(); i++)
	{
		for (int j = 0; j < vertex.cols(); j++)
		{
			double dif = vertex(fi(i) - 1, j) - vertex(fj(i) - 1, j);
			d(i) += dif * dif;
		}
	}
	SpMat W = sparse(fi, fj, d);
	return W;
}

SpMat compute_mesh_weight(MatrixXd _vertex, MatrixXi _face, Laplacian_Type type, Option options)
{
	/*% compute_mesh_weight - compute a weight matrix
		%
		%   W = compute_mesh_weight(vertex, face, type, options);
	%
		%   W is sparse weight matrix and W(i, j) = 0 is vertex i and vertex j are not
		%   connected in the mesh.
		%
		%   type is either
		%       'combinatorial': W(i, j) = 1 is vertex i is conntected to vertex j.
		%       'distance' : W(i, j) = 1 / d_ij ^ 2 where d_ij is distance between vertex
		%           i and j.
		%       'conformal' : W(i, j) = cot(alpha_ij) + cot(beta_ij) where alpha_ij and
		%           beta_ij are the adjacent angle to edge(i, j)
		%
		%   If options.normalize = 1, the the rows of W are normalize to sum to 1.
		%
		%   Copyright(c) 2007 Gabriel Peyre*/
	MatrixXd vertex = check_size<MatrixXd>(_vertex, 2, 4);
	MatrixXi face = check_size<MatrixXi>(_face, 3, 4);

	int nface = face.rows();
	int n = 0;
	for (int i = 0; i < face.rows(); i++)
		for (int j = 0; j < face.cols(); j++)
			if (face(i, j) > n)
				n = face(i, j);
	SpMat W, _W;
	switch (type)
	{
	case combinatorial:
		break;
	case Laplacian_Type::distance:
		W = my_euclidean_distance(triangulation2adjacency(face), vertex);
		for (int k = 0; k < W.outerSize(); ++k)
			for (SparseMatrix<double>::InnerIterator it(W, k); it; ++it)
			{
				if (it.value() > 0)
					W.coeffRef(it.row(), it.col()) = 1.0 / it.value();
			}

		_W = W.adjoint();
		W = (W + _W) / 2;
		break;
	case conformal:
		break;
	}
	return W;
}

SpMat compute_mesh_laplacian(MatrixXd vertex, MatrixXi face, Laplacian_Type type, Option options)
{
	int symmetrize = options.symmetrize;
	int normalize = options.normalize;
	options.normalize = 0;
	SpMat W = compute_mesh_weight(vertex, face, type, options);

	int n = W.rows();
	SpMat L(vertex.rows(), vertex.rows());
	if (symmetrize == 1 && normalize == 0)
	{
		L = diag(sum(W, 2)) - W;
	}
	else if (symmetrize == 1 && normalize == 1)
	{
		VectorXd _vec = sum(W, 2);
		for (int i = 0; i < _vec.size(); i++)
			_vec(i) = pow(_vec(i), -1 / 2);
		L = speye(n) - diag(_vec) * W * diag(_vec);
	}
	else if (symmetrize == 0 && normalize == 1)
	{
		VectorXd _vec = sum(W, 2);
		for (int i = 0; i < _vec.size(); i++)
			_vec(i) = pow(_vec(i), -1);
		L = speye(n) - diag(_vec) * W;
	}
	else;
	return L;
}