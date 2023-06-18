#pragma once
#include <opencv2/opencv.hpp>

class Filter
{
public:
    Filter(cv::Mat input, float sigma, float alpha, float beta);
    cv::Mat reapply(float sigma, float alpha, float beta);
};

cv::Mat applyHomomorphicFilter(cv::Mat input, cv::Mat filter);

/// creates a gauss filter
/// the filter is not at center but on edge position, like opencv dft expects
cv::Mat createPassFilter(const cv::Size &size, float cutoff);
cv::Mat prepareImageSize(cv::Mat in);
