#include "ImageDescriptor.h"
#include <assert.h>

using namespace Gif;

void ImageDescriptor::Read(std::ifstream& file)
{
    file.read((char*)&m_imageLeftPosition, sizeof(m_imageLeftPosition));
    file.read((char*)&m_imageTopPosition, sizeof(m_imageTopPosition));
    file.read((char*)&m_imageWidth, sizeof(m_imageWidth));
    file.read((char*)&m_imageHeight, sizeof(m_imageHeight));

    file.read((char*)&m_fields.value, sizeof(m_fields.value));

    assert((bool)m_fields.fields.interlaceMask != true);
}