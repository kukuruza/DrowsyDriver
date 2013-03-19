#ifndef RECTOPERATIONS_H
#define RECTOPERATIONS_H

#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <opencv2/core/core.hpp>

namespace evg {

// correct the position of rectangle inside the image if necessary
//   innerDepth - minimum width of rectangle
//   border - border depth - minimum offset from every border of image
void validateRect (const cv::Size imsize, const int inner, const int border,
                   cv::Rect& rect);


// increase the rectangle by expanding its borders on "depth" in every direction
cv::Rect expandRect (const cv::Rect& rect, const unsigned int depth,
                     const cv::Size imageSize = cv::Size(), const unsigned int minSize = 0,
                     const unsigned int minBorder = 0);

// increase the rectangle by shifting its borders on depthVect (right, top, bottom, left)
cv::Rect expandRect (const cv::Rect& rect, const std::vector<int>& shiftVect,
                     const cv::Size imageSize = cv::Size(), const unsigned int minSize = 0,
                     const unsigned int minBorder = 0);



// expand on percentage of IMAGE size in every direction
cv::Rect expandRectPercImage (const cv::Rect& srcRect, const double imagePerc,
                              const cv::Size imageSize = cv::Size(), const unsigned int minSize = 0,
                              const unsigned int minBorder = 0);



// expand on percentage of RECTANGLE size in every direction
cv::Rect expandRectPercRect (const cv::Rect& srcRect, const double rectPerc,
                             const cv::Size imageSize = cv::Size(), const unsigned int minSize = 0,
                             const unsigned int minBorder = 0);

// expand on percentage of RECTANGLE size in every direction
cv::Rect expandRectPercRect (const cv::Rect& srcRect, const std::vector<double>& rectPerc,
                             const cv::Size imageSize = cv::Size(), const unsigned int minSize = 0,
                             const unsigned int minBorder = 0);


} // namespace evg

#endif // RECTOPERATIONS_H
