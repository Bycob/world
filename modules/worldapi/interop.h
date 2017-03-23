#pragma once

#include <armadillo/armadillo>
#include <opencv2/opencv.hpp>

#include "worldapidef.h"

cv::Mat WORLDAPI_EXPORT armaToCV(const arma::Mat<double> & mat);
cv::Mat WORLDAPI_EXPORT armaToCV(const arma::Cube<double> & cube);
