#include "Header.h"

using namespace Gif;

unsigned short Color::ConvertTo4Bit(const Gif::Color& value)
{
    unsigned char red_color = value.m_red & 0b10000000;
    unsigned char green_color = value.m_green & 0b10000000;
    unsigned char blue_color = value.m_blue & 0b10000000;

    //double luminance = 0.2126 * value.m_red + 0.7152 * value.m_green + 0.0722 * value.m_blue;
    double luminance = (0.299 * (double)value.m_red) + (0.587 * (double)value.m_green) + (0.114 * (double)value.m_blue);

    unsigned short new_color = 0;
    new_color = (luminance > 128 ? 0b10000000 : 0b00000000) | red_color >> 1 | (green_color >> 2) | (blue_color >> 3);

    return new_color;
}