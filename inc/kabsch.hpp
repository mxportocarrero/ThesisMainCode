#ifndef KABSCH_H
#define KABSCH_H

#include "general_includes.hpp"

// Es necesario hacer esta funcion inline, sino no compila
inline Eigen::Affine3d Find3DAffineTransform(Eigen::Matrix3Xd in, Eigen::Matrix3Xd out) {
//inline Eigen::Transform<long double,3,Eigen::Affine,Eigen::DontAlign> Find3DAffineTransform(Eigen::Matrix3Xd in, Eigen::Matrix3Xd out) {
	// Default output
	Eigen::Affine3d A;
	A.linear() = Eigen::Matrix3d::Identity(3, 3);
	A.translation() = Eigen::Vector3d::Zero();

	if (in.cols() != out.cols())
		throw "Find3DAffineTransform(): input data mis-match";

	// First find the scale, by finding the ratio of sums of some distances,
	// then bring the datasets to the same scale.
	double dist_in = 0, dist_out = 0;
	for (int col = 0; col < in.cols() - 1; col++) {
		dist_in += (in.col(col + 1) - in.col(col)).norm();
		dist_out += (out.col(col + 1) - out.col(col)).norm();
	}

	if (dist_in <= 0 || dist_out <= 0)
		return A;

	double scale = dist_out / dist_in;
	out /= scale;

	// Find the centroids then shift to the origin
	Eigen::Vector3d in_ctr = Eigen::Vector3d::Zero();
	Eigen::Vector3d out_ctr = Eigen::Vector3d::Zero();

	for (int col = 0; col < in.cols(); col++) {
		in_ctr += in.col(col);
		out_ctr += out.col(col);
	}

	in_ctr /= in.cols();
	out_ctr /= out.cols();

	for (int col = 0; col < in.cols(); col++) {
		in.col(col) -= in_ctr;
		out.col(col) -= out_ctr;
	}

	// SVD
	Eigen::MatrixXd Cov = in * out.transpose();
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(Cov, Eigen::ComputeThinU | Eigen::ComputeThinV);

	// Find the rotation
	double d = (svd.matrixV() * svd.matrixU().transpose()).determinant();

	if (d > 0)
		d = 1.0;
	else
		d = -1.0;
	
	Eigen::Matrix3d I = Eigen::Matrix3d::Identity(3, 3);
	I(2, 2) = d;
	Eigen::Matrix3d R = svd.matrixV() * I * svd.matrixU().transpose();

	// The final transform
	A.linear() = scale * R;
	A.translation() = scale*(out_ctr - R*in_ctr);

	return A;
} // Fin de la funcion original



// Esta funcion ayuda a revisar la escala de las transformaciones
inline Eigen::Affine3d Find3DAffineTransform(Eigen::Matrix3Xd in, Eigen::Matrix3Xd out, double &scale) {
//inline Eigen::Transform<long double,3,Eigen::Affine,Eigen::DontAlign> Find3DAffineTransform(Eigen::Matrix3Xd in, Eigen::Matrix3Xd out) {
	// Default output
	Eigen::Affine3d A;
	A.linear() = Eigen::Matrix3d::Identity(3, 3);
	A.translation() = Eigen::Vector3d::Zero();

	if (in.cols() != out.cols())
		throw "Find3DAffineTransform(): input data mis-match";

	// First find the scale, by finding the ratio of sums of some distances,
	// then bring the datasets to the same scale.
	double dist_in = 0, dist_out = 0;
	for (int col = 0; col < in.cols() - 1; col++) {
		dist_in += (in.col(col + 1) - in.col(col)).norm();
		dist_out += (out.col(col + 1) - out.col(col)).norm();
	}

	if (dist_in <= 0 || dist_out <= 0)
		return A;

	scale = dist_out / dist_in;
	out /= scale;

	// Find the centroids then shift to the origin
	Eigen::Vector3d in_ctr = Eigen::Vector3d::Zero();
	Eigen::Vector3d out_ctr = Eigen::Vector3d::Zero();

	for (int col = 0; col < in.cols(); col++) {
		in_ctr += in.col(col);
		out_ctr += out.col(col);
	}

	in_ctr /= in.cols();
	out_ctr /= out.cols();

	for (int col = 0; col < in.cols(); col++) {
		in.col(col) -= in_ctr;
		out.col(col) -= out_ctr;
	}

	// SVD
	Eigen::MatrixXd Cov = in * out.transpose();
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(Cov, Eigen::ComputeThinU | Eigen::ComputeThinV);

	// Find the rotation
	double d = (svd.matrixV() * svd.matrixU().transpose()).determinant();

	if (d > 0)
		d = 1.0;
	else
		d = -1.0;
	
	Eigen::Matrix3d I = Eigen::Matrix3d::Identity(3, 3);
	I(2, 2) = d;
	Eigen::Matrix3d R = svd.matrixV() * I * svd.matrixU().transpose();

	// The final transform
	//A.linear() = scale * R;
	//A.translation() = scale*(out_ctr - R*in_ctr);

	A.linear() = R;
	A.translation() = (out_ctr - R*in_ctr);

	return A;
} // Fin de la funcion original



// A function to test Find3DAffineTransform()
inline void TestFind3DAffineTransform(){

  // Create datasets with known transform
  Eigen::Matrix3Xd in(3, 100), out(3, 100);
  Eigen::Quaternion<double> Q(1, 3, 5, 2);
  Q.normalize();
  Eigen::Matrix3d R = Q.toRotationMatrix();
  double scale = 2.0;
  for (int row = 0; row < in.rows(); row++) {
    for (int col = 0; col < in.cols(); col++) {
      in(row, col) = log(2 * row + 10.0) / sqrt(1.0*col + 4.0) + sqrt(col*1.0) / (row + 1.0);
    }
  }
  Eigen::Vector3d S;
  S << -5, 6, -27;
  for (int col = 0; col < in.cols(); col++)
    out.col(col) = scale*R*in.col(col) + S;

  Eigen::Affine3d A = Find3DAffineTransform(in, out);

  // See if we got the transform we expected
  if ((scale*R - A.linear()).cwiseAbs().maxCoeff() > 1e-13 ||
    (S - A.translation()).cwiseAbs().maxCoeff() > 1e-13)
    throw "Could not determine the affine transform accurately enough";
} // Fin de la funcion Test

#endif // KABSCH_H