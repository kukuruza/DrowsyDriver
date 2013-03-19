#include "RectOperations.h"
#include <algorithm>


using namespace cv;
using namespace std;


void evg::validateRect (const cv::Size imsize, const int inner, const int border, cv::Rect& rect)
{

    assert(inner > 0);
    assert(border >= 0);
    if (imsize.width < inner + 2 * border ||
        imsize.height < inner + 2 * border)
        throw std::exception();

    // expand rectangle up to the minimum required width
    if (rect.width <= inner)
    {
        rect.x = rect.x - int((inner - rect.width) / 2);
        rect.width = inner;
    }
    // shrink up to the maximum required width
    if (rect.width >= imsize.width - 2 * border)
    {
        rect.x = border + 1;
        rect.width = imsize.width - 2 * border - 1;
    }
    // move rectangle to the right - up to required offset
    rect.x = rect.x + std::max(0, -(rect.x - border));
    // move rectangle to the left - up to required offset from the right
    rect.x = rect.x - std::max(0, -(imsize.width-1 - border - rect.width - rect.x));
    // check that we sattisfy everything. It should be so.
    assert(rect.x + rect.width + border < imsize.width);

    // expand rectangle up to the minimum required height
    if (rect.height <= inner)
    {
        rect.y = rect.y - int((inner - rect.height) / 2);
        rect.height = inner;
    }
    // shrink to the maximum required height
    if (rect.height >= imsize.height - 2 * border)
    {
        rect.y = border + 1;
        rect.height = imsize.height - 2 * border - 1;
    }
    // move rectangle to the right - up to required offset
    rect.y = rect.y + std::max(0, -(rect.y - border));
    // move rectangle to the left - up to required offset from the right
    rect.y = rect.y - std::max(0, -(imsize.height-1 - border - rect.height - rect.y));
    // check that we sattisfy everything. It should be so.
    assert(rect.y + rect.height + border < imsize.height);
}



Rect evg::expandRect (const Rect& _srcRect, const unsigned int _depth,
                      const Size _imageSize, const unsigned int _minSize, const unsigned int _minBorder)
{
    cv::Rect result;
    result.x = _srcRect.x - _depth;
    result.y = _srcRect.y - _depth;
    result.width = _srcRect.width + 2 * _depth;
    result.height = _srcRect.height + 2 * _depth;

    // validate if _imageSize is given
    if (_imageSize != Size())
        evg::validateRect (_imageSize, _minSize, _minBorder, result);

    return result;
}



Rect evg::expandRect (const Rect& _srcRect, const vector<int>& _shiftVect,
                      const Size _imageSize, const unsigned int _minSize, const unsigned int _minBorder)
{
    cv::Rect result;

    const int shiftLeft   = _shiftVect[0];
    const int shiftTop    = _shiftVect[1];
    const int shiftRight  = _shiftVect[2];
    const int shiftBottom = _shiftVect[3];

    result.x = _srcRect.x - shiftLeft;
    result.y = _srcRect.y - shiftTop;
    result.width = std::max (_srcRect.width + shiftLeft + shiftRight, 0);
    result.height = std::max (_srcRect.height + shiftTop + shiftBottom, 0);

    // validate if _imageSize is given
    if (_imageSize != Size())
        evg::validateRect (_imageSize, _minSize, _minBorder, result);

    return result;
}


Rect evg::expandRectPercImage (const Rect& _srcRect, const double _imagePerc,
                               const Size _imageSize, const unsigned int _minSize,
                               const unsigned int _minBorder)
{
    Rect resRect;

    // how much to expand on width & height (in every direction) in pxls
    Size depth (_imagePerc * _imageSize.width, _imagePerc * _imageSize.height);

    vector<int> shiftVect(4);
    shiftVect[0] = depth.width;
    shiftVect[1] = depth.height;
    shiftVect[2] = depth.width;
    shiftVect[3] = depth.height;

    resRect = evg::expandRect (_srcRect, shiftVect, _imageSize, _minSize, _minBorder);

    return resRect;
}


Rect evg::expandRectPercRect (const Rect& _srcRect, const double _rectPerc,
                              const Size _imageSize, const unsigned int _minSize,
                              const unsigned int _minBorder)
{
    Rect resRect;
      //
      // how much to expand on width & height (in every direction) in pxls
    int depth = _rectPerc * (_srcRect.width + _srcRect.height) / 2;
      //
    resRect = evg::expandRect (_srcRect, depth, _imageSize, _minSize, _minBorder);
      //
    return resRect;
}


Rect evg::expandRectPercRect (const Rect& _srcRect, const vector<double>& _rectPerc,
                              const Size _imageSize, const unsigned int _minSize,
                              const unsigned int _minBorder)
{
    Rect resRect;
      //
    vector<int> shiftVect(4);
    shiftVect[0] = _srcRect.width * _rectPerc[0];
    shiftVect[1] = _srcRect.height * _rectPerc[1];
    shiftVect[2] = _srcRect.width * _rectPerc[2];
    shiftVect[3] = _srcRect.height * _rectPerc[3];
      //
    resRect = evg::expandRect (_srcRect, shiftVect, _imageSize, _minSize, _minBorder);
      //
    return resRect;
}
