#include "LogicalScreenDescriptor.h"

using namespace Gif;

void LogicalScreenDescriptor::Read(std::ifstream &file)
{
    file.read((char*)&m_screenWidth, sizeof(m_screenWidth));
    file.read((char*)&m_screenHeight, sizeof(m_screenHeight));

    file.read((char*)&m_fields.value, sizeof(m_fields.value));

    file.read((char*)&m_backgroundColorIndex, sizeof(m_backgroundColorIndex));
    file.read((char*)&m_pixelAspectRation, sizeof(m_pixelAspectRation));
}