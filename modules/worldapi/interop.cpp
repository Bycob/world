#include "interop.h"

cv::Mat armaToCV(const arma::Mat<double>& mat) {
	cv::Mat output(mat.n_cols, mat.n_rows, CV_8UC1);

	for (int x = 0; x < mat.n_cols; x++) {
		for (int y = 0; y < mat.n_rows; y++) {
			output.at<unsigned char>(y, x) = (unsigned char)(mat(x, y) * 255);
		}
	}

	return output;
}

cv::Mat armaToCV(const arma::Cube<double> & cube) {
	cv::Mat output(cube.n_rows, cube.n_cols, CV_8UC3);

	if (cube.n_elem < 3) throw std::runtime_error("conversion cube -> image : pas assez de données");

	for (int x = 0; x < cube.n_cols; x++) {
		for (int y = 0; y < cube.n_rows; y++) {
			output.at<cv::Vec3b>(y, x) = cv::Vec3b(
				(unsigned char)(cube(x, y, 2) * 255),
				(unsigned char)(cube(x, y, 1) * 255), 
				(unsigned char)(cube(x, y, 0) * 255));
		}
	}

	return output;
}