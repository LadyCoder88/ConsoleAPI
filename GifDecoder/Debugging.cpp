#include "Debugging.h"
#include <fstream>
#include <iostream>

using namespace Gif;

void LogColorTable(Color* color_table, int size)
{
    std::ofstream file("GifDecoder\\color_debug.txt");
    if (!file)
    {
        std::cout << "An error occurred opening the file" << std::endl;
    }

    for (int i = 0; i < size; ++i)
    {
        file << std::dec << "Idx: " << i << " Red: " << (int)color_table[i].m_red << " Green: " << (int)color_table[i].m_green << " Blue: " << (int)color_table[i].m_blue << " Converted: " << std::hex << Color::ConvertTo4Bit(color_table[i]) << std::endl;
    }

    file.close();
}