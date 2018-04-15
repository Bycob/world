#pragma once

#include "core/WorldConfig.h"

#include <armadillo/armadillo>

#ifdef WORLD_BUILD_OPENCV_MODULES

#include <opencv2/opencv.hpp>

namespace world {

cv::Mat WORLDAPI_EXPORT armaToCV(const arma::Mat<double> &mat);

cv::Mat WORLDAPI_EXPORT armaToCV(const arma::Cube<double> &cube);
} // namespace world

#endif