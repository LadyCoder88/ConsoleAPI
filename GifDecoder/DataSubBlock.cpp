#include "DataSubBlock.h"
#include <fstream>
#include <vector>
#include <assert.h>

using namespace Gif;

DataSubBlock::DataSubBlock(char bites_to_follor) : m_bitesToFollow(bites_to_follor)
{
    m_data = new unsigned char[m_bitesToFollow];
}

DataSubBlock::DataSubBlock(const DataSubBlock& data)
{
    m_bitesToFollow = data.m_bitesToFollow;
    m_data = new unsigned char[m_bitesToFollow];
    for (int i = 0; i < m_bitesToFollow; ++i)
        m_data[i] = data.m_data[i];
}

DataSubBlock& DataSubBlock::operator=(const DataSubBlock& data)
{
    m_bitesToFollow = data.m_bitesToFollow;
    m_data = new unsigned char[m_bitesToFollow];
    for (int i = 0; i < m_bitesToFollow; ++i)
        m_data[i] = data.m_data[i];

    return *this;
}

DataSubBlock::DataSubBlock(DataSubBlock&& data)
{
    m_bitesToFollow = data.m_bitesToFollow;
    m_data = data.m_data;

    data.m_bitesToFollow = 0;
    data.m_data = nullptr;
}

DataSubBlock& DataSubBlock::operator=(DataSubBlock&& data)
{
    m_bitesToFollow = data.m_bitesToFollow;
    m_data = data.m_data;

    data.m_bitesToFollow = 0;
    data.m_data = nullptr;

    return *this;
}

DataSubBlock::~DataSubBlock()
{
    delete[] m_data;
}

void DataSubBlock::Read(std::ifstream& file, std::vector<DataSubBlock>& data)
{
    unsigned char bitesTofllow = 0;

    while (true)
    {
        file.read((char*)&bitesTofllow, sizeof(bitesTofllow));
        if (bitesTofllow == 0x00)
        {
            return;
        }

        DataSubBlock data_subblock(bitesTofllow);
        file.read((char*)data_subblock.m_data, bitesTofllow);

        data.push_back(data_subblock);
    }
}

void DataSubBlock::SkipBlock(std::ifstream& file)
{
    unsigned char bitesTofllow = 0;

    static char buffer[255];
    while (true)
    {
        file.read((char*)&bitesTofllow, sizeof(bitesTofllow));
        if (bitesTofllow == 0x00)
        {
            return;
        }

        file.read(buffer, bitesTofllow);
    }
}