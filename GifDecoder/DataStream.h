#pragma once
#include "Header.h"
#include <fstream>
#include "LogicalScreenDescriptor.h"
#include <vector>
#include "TableBasedImage.h"

namespace Gif
{
    struct DataStream
    {
        DataStream();
        ~DataStream();
        DataStream(const DataStream& data_stream);
        DataStream(DataStream&& data_stream);

        std::string m_fileName;
        Header m_header;

        LogicalScreenDescriptor m_logicalScreenDescriptor;

        //GLOBAL COLOR TABLE - OPTIONAL
        // - Contains a color table, which is a sequence of bytes representing red - green - blue color triplets
        // - Is used by images without a Local Color Table and by Plain Text Extensions
        // - Its presence is marked by the Global Color Table Flag being set to 1 in the Logical Screen Descriptor
        // - if present, it immediately follows the Logical Screen Descriptor and contains a number of bytes
        //   equal to 3 x 2 ^ (Size of Global Color Table + 1).
        int m_globalColorTableSize = 0;
        Color* m_GlobalColorTable; 

        std::vector<TableBasedImage*> m_TableData;

        static DataStream Load(std::string file_name);
        void Read(std::ifstream &file);
        void ReportNotFoundBlock(unsigned char label, std::ifstream& file);
    };
}