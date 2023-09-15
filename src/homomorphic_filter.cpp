#include "homomorphic_filter.hpp"

/// shift from center to corner
void dftShift(cv::InputOutputArray _out)
{
    cv::Mat out = _out.getMat();

    if (out.rows == 1 && out.cols == 1) {
        return;
    }

    std::vector<cv::Mat> planes;
    split(out, planes);

    const auto xMid = out.cols >> 1;
    const auto yMid = out.rows >> 1;

    if (xMid == 0 || yMid == 0) {
        int is_odd = (xMid > 0 && (out.cols & 1)) || (yMid > 0 && (out.rows & 1));
        const auto mid = xMid + yMid;

        for (size_t i = 0; i < planes.size(); i++) {
            cv::Mat tmp;
            cv::Mat half0(planes[i], cv::Rect(0, 0, mid + is_odd, 1));
            cv::Mat half1(planes[i], cv::Rect(mid + is_odd, 0, xMid, 1));

            half0.copyTo(tmp);
            half1.copyTo(planes[i](cv::Rect(0, 0, mid, 1)));
            tmp.copyTo(planes[i](cv::Rect(mid, 0, mid + is_odd, 1)));
        }
    }
    else {
        int isXodd = out.cols & 1;
        int isYodd = out.rows & 1;
        for (size_t i = 0; i < planes.size(); ++i) {
            // perform quadrant swaps...
            cv::Mat q0(planes[i], cv::Rect(0, 0, xMid + isXodd, yMid + isYodd));
            cv::Mat q1(planes[i], cv::Rect(xMid + isXodd, 0, xMid, yMid + isYodd));
            cv::Mat q2(planes[i], cv::Rect(0, yMid + isYodd, xMid + isXodd, yMid));
            cv::Mat q3(planes[i], cv::Rect(xMid + isXodd, yMid + isYodd, xMid, yMid));

            if (!isXodd && !isYodd) {
                cv::Mat tmp;
                q0.copyTo(tmp);
                q3.copyTo(q0);
                tmp.copyTo(q3);

                q1.copyTo(tmp);
                q2.copyTo(q1);
                tmp.copyTo(q2);
            }
            else {
                cv::Mat tmp0, tmp1, tmp2, tmp3;
                q0.copyTo(tmp0);
                q1.copyTo(tmp1);
                q2.copyTo(tmp2);
                q3.copyTo(tmp3);

                tmp0.copyTo(planes[i](cv::Rect(xMid, yMid, xMid + isXodd, yMid + isYodd)));
                tmp3.copyTo(planes[i](cv::Rect(0, 0, xMid, yMid)));

                tmp1.copyTo(planes[i](cv::Rect(0, yMid, xMid, yMid + isYodd)));
                tmp2.copyTo(planes[i](cv::Rect(xMid, 0, xMid + isXodd, yMid)));
            }
        }
    }

    merge(planes, out);
}

cv::Mat prepareImageSize(cv::Mat in)
{
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(in.rows);
    int n = cv::getOptimalDFTSize(in.cols);
    cv::copyMakeBorder(in,
        padded,
        0,
        m - in.rows,
        0,
        n - in.cols,
        cv::BORDER_CONSTANT,
        cv::Scalar::all(0));
    return padded;
}

/// distance to frequency rectangle (D function 4.8-2 in Digital Image Processing by Rafael C. Gonzalez)
float distanceToFreqRect(int x, int y, const cv::Size& size)
{
    auto u = (x - size.width * 0.5f);
    u = u * u;
    auto v = (y - size.height * 0.5f);
    v = v * v;
    return sqrtf(u + v);
}

cv::Mat createPassFilter(const cv::Size& size, float cutoff)
{
    const float d0_inv = 1.f / (2.f * cutoff * cutoff);
    cv::Mat filter(size, CV_32FC1, 0.f);
    cv::parallel_for_(cv::Range(0, size.height * size.width), [&](const cv::Range& range) {
        for (int r = range.start; r < range.end; ++r) {
            int x = r % size.width;
            int y = r / size.width;
            float dist = distanceToFreqRect(x, y, size);
            filter.at<float>(y, x) = 1.f - expf(-1 * dist * dist * d0_inv);
        }
        });

    dftShift(filter);
    return filter;
}

cv::Mat applyHomomorphicFilter(cv::Mat input, cv::Mat filter)
{
    using Mat = cv::UMat;

    Mat log;
    input.convertTo(log, CV_32F, 1.f / 255.f, 1);
    cv::log(log, log);

    // dft
    std::vector planes = { log, Mat::zeros(log.size(), CV_32FC1) };
    Mat complex;
    cv::merge(planes, complex);
    cv::dft(complex, complex);
    cv::split(complex, planes);

    // filtering
    Mat processedReal(planes[0].size(), planes[0].type());
    Mat processedComplex(planes[1].size(), planes[1].type());
    cv::multiply(planes[0], filter, processedReal);
    cv::multiply(planes[1], filter, processedComplex);
    std::vector processed = { processedReal, processedComplex };
    cv::merge(processed, complex);

    // inverse dft
    Mat output;
    cv::idft(complex, complex);
    cv::extractChannel(complex, output, 0);
    cv::normalize(output, output, 0, 1, cv::NORM_MINMAX);
    cv::exp(output, output);
    cv::add(output, cv::Scalar(-1), output);

    cv::Mat dest;
    output.convertTo(dest, CV_8UC1, 255);
    return dest;
}
