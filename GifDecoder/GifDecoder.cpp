#include "GifDecoder.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <assert.h>

using namespace std;
using namespace Gif;

GifObject GifDecoder::LoadGifFromFile(const char* filename)
{
    DataStream data_stream = DataStream::Load(filename);

    return GifObject(data_stream);
}
