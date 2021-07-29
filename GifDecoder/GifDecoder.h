#pragma once
#include <vector>
#include "DataStream.h"
#include "GifObject.h"

namespace Gif
{
    class GifDecoder
    {
    public:
        static GifObject LoadGifFromFile(const char* filename);
    };
};
