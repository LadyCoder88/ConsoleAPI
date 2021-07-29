#pragma once
#include <vector>

namespace Gif
{
    //DATA SUB BLOCK
    // - Always occurs as part of a larger unit.
    // - It does not have a scope of itself.
    struct DataSubBlock
    {
        // - Data sub-block my contain from 0 to 255 bytes
        // - The size of the block does not account for the size byte itself, therefore, the empty
        //   sub-block is one whose size field contains 0x00.
        unsigned char m_bitesToFollow;
        
        // - Data table of size m_bitesToFollow;
        // - Any 8-bit value.
        // - There must be exactly as many Data Values as specified by the Block Size field
        unsigned char* m_data; 

        DataSubBlock(char bites_to_follor);
        DataSubBlock(const DataSubBlock& data);
        DataSubBlock& operator=(const DataSubBlock& data);
        DataSubBlock(DataSubBlock&& data);
        DataSubBlock& operator=(DataSubBlock&& data);
        ~DataSubBlock();

        static void Read(std::ifstream& file, std::vector<DataSubBlock>& data);
        static void SkipBlock(std::ifstream& file);
    };
}