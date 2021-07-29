#include "ImageData.h"
#include "DataSubBlock.h"
#include <assert.h>
#include <bitset>
#include <iostream>
#include <stack>

using namespace Gif;

ImageData::ImageData(Color* color_table, int color_table_size, int tranparent_idx, unsigned int decoded_image_size) 
    :m_ColorTable(color_table),
    m_ColorTableSize(color_table_size),
    m_transparentIdx(tranparent_idx), 
    m_decodedDataSize(decoded_image_size), 
    m_decodedData(new PixelInfo[decoded_image_size])
{
    //m_decodedData.resize(decoded_image_size);
}

//-----------------------------------------------------------------------
ImageData::ImageData(const ImageData& image_data)
    :m_decodedData(image_data.m_decodedData)
{
    //We just keep pointer
    //We didn't make the data

    m_decodedDataSize = image_data.m_decodedDataSize;

    m_CurrentDataIdx = image_data.m_CurrentDataIdx;
    m_CurrentSubBlockIdx = image_data.m_CurrentSubBlockIdx;
    m_SourceIdx = image_data.m_SourceIdx;

    m_transparentIdx = image_data.m_transparentIdx;

    m_ColorTable = image_data.m_ColorTable;
    m_ColorTableSize = image_data.m_ColorTableSize;
    m_data = image_data.m_data;
    //m_decodedData = image_data.m_decodedData;

    m_clearCode = image_data.m_clearCode;
    m_endCode = image_data.m_endCode;

    m_LZWMinimumCodeSize = image_data.m_LZWMinimumCodeSize;
    m_LZWCurrentCodeSide = image_data.m_LZWCurrentCodeSide;
}

//-----------------------------------------------------------------------
ImageData::ImageData(ImageData&& image_data):
    m_decodedData(std::move(image_data.m_decodedData))
{
    m_decodedDataSize = image_data.m_decodedDataSize;

    m_CurrentDataIdx = image_data.m_CurrentDataIdx;
    image_data.m_CurrentDataIdx = 0;

    m_CurrentSubBlockIdx = image_data.m_CurrentSubBlockIdx;
    image_data.m_CurrentSubBlockIdx = 0;

    m_SourceIdx = image_data.m_SourceIdx;
    m_SourceIdx = 0;

    m_LZWMinimumCodeSize = image_data.m_LZWMinimumCodeSize;
    m_LZWCurrentCodeSide = image_data.m_LZWCurrentCodeSide;
    image_data.m_LZWCurrentCodeSide = m_LZWMinimumCodeSize;

    m_transparentIdx = image_data.m_transparentIdx;

    m_ColorTable = image_data.m_ColorTable;
    m_ColorTableSize = image_data.m_ColorTableSize;

    m_data = std::move(image_data.m_data);
    //m_decodedData = std::move(image_data.m_decodedData);
}

//-----------------------------------------------------------------------
ImageData::~ImageData()
{
    delete m_decodedData;
}

//-----------------------------------------------------------------------
void ImageData::Read(std::ifstream& file)
{
    file.read((char*)&m_LZWMinimumCodeSize, sizeof(m_LZWMinimumCodeSize));
    DataSubBlock::Read(file, m_data);

    m_LZWCurrentCodeSide = m_LZWMinimumCodeSize;

    m_CurrentDataIdx = 0;
    m_CurrentSubBlockIdx = 0;
    m_SourceIdx = 0;
}

//-----------------------------------------------------------------------
void ImageData::ResetToStartupState(std::vector<DecoderHelper*>& table)
{
    m_LZWCurrentCodeSide = m_LZWMinimumCodeSize;

    InitCodeTable(table);
}

//-----------------------------------------------------------------------
void ImageData::ClearTable(std::vector<DecoderHelper*>& table)
{
    for (DecoderHelper* item : table)
    {
        delete item;
    }

    table.clear();
}

//-----------------------------------------------------------------------
void ImageData::InitCodeTable(std::vector<DecoderHelper*>& table)
{
    ClearTable(table);

    unsigned int lastCodeIdx = (unsigned int)pow(2.0, m_LZWMinimumCodeSize);

    //There can be situation when lastCodeIdx will be smaller than ColorTableSize
    //Thats why we iterate here to lastColorIdx not to ColorTableSize
    for (unsigned int i = 0; i < lastCodeIdx; ++i)
    {
        DecoderHelper* next_helper = new DecoderHelper(i, i, nullptr);
        table.push_back(next_helper);
    }

    //Czy mo¿liwe jest aby Color table size by³ mniejszy ni¿ lastCodeIdx?
    //Tak mo¿e byæ przy gifach czarno-bia³ych.
    //Pod indekasmi 0 i 1 jest kolor bia³y i czarny ale kody specjalne
    //s¹ dopiero na polach 2^2 i 2^2 + 1
    for (unsigned int i = m_ColorTableSize; i < lastCodeIdx; ++i)
    {
        DecoderHelper* next_helper = new DecoderHelper(0, 0, nullptr);
        table.push_back(next_helper);
    }

    m_clearCode = lastCodeIdx++;
    table.push_back(new DecoderHelper(m_clearCode, m_clearCode, nullptr)); //Clear code

    m_endCode = lastCodeIdx++;
    table.push_back(new DecoderHelper(m_endCode, m_endCode, nullptr)); //End code
}

//-----------------------------------------------------------------------
unsigned int ImageData::UnboxIdx()
{
    std::bitset<8> source_data = m_data[m_CurrentSubBlockIdx].m_data[m_CurrentDataIdx];

    std::bitset<32> destination_data = 0;
    unsigned char destination_idx = 0;

    unsigned short int code_count = m_LZWCurrentCodeSide + 1;

    for (int count = 0; count < code_count; ++destination_idx, ++count)
    {
        destination_data[destination_idx] = source_data[m_SourceIdx];

        if (m_SourceIdx < 7)
        {
            ++m_SourceIdx;
        }
        else
        {
            m_SourceIdx = 0;
            ++m_CurrentDataIdx;

            if (m_CurrentDataIdx >= m_data[m_CurrentSubBlockIdx].m_bitesToFollow)
            {
                ++m_CurrentSubBlockIdx;
                m_CurrentDataIdx = 0;
            }

            if (m_CurrentSubBlockIdx >= m_data.size())
            {
                break;
            }

            source_data = m_data[m_CurrentSubBlockIdx].m_data[m_CurrentDataIdx];
        }
    }

    return (int)destination_data.to_ulong();
}

//-----------------------------------------------------------------------
void ImageData::CheckLZWMinimumCodeSize(unsigned int index)
{
    if ((index >= (pow(2.0, m_LZWCurrentCodeSide + 1) - 1)) && m_LZWCurrentCodeSide < 11)
    {
        unsigned char store = m_LZWCurrentCodeSide;
        ++m_LZWCurrentCodeSide;
    }
}

//-----------------------------------------------------------------------
void ImageData::Log(std::ofstream& file, unsigned int unboxed_value, unsigned int table_size)
{
    file << "Unboxed: " << std::hex << unboxed_value;
    file << " code_table size: " << std::dec << table_size;
    file << " Sourde idx: " << (int)m_SourceIdx;
    file << " LZW current: " << (int)m_LZWCurrentCodeSide;
    file << " LZW min: " << (int)m_LZWMinimumCodeSize << std::endl;
}

//-----------------------------------------------------------------------
void ImageData::Decode()
{
    std::vector<DecoderHelper*> code_table;

    unsigned int unboxed_idx = UnboxIdx();
    InitCodeTable(code_table);

    assert(unboxed_idx == m_clearCode);

    unsigned int last_index = UnboxIdx();
    assert(last_index < code_table.size());

    PixelInfo* indexInTable = m_decodedData;
    unsigned int pixel_info_count = 0;

    //std::vector<PixelInfo>::iterator it = m_decodedData.begin();
    DecodeColors(code_table[last_index], indexInTable, pixel_info_count);

    while (true)
    {
        unboxed_idx = UnboxIdx();

        if (unboxed_idx == m_endCode)
            break;

        if (unboxed_idx == m_clearCode)
        {
            ResetToStartupState(code_table);

            last_index = UnboxIdx();

            assert(last_index < code_table.size());

            DecodeColors(code_table[last_index], indexInTable, pixel_info_count);

            continue;
        }

        if (unboxed_idx < code_table.size())
        {
            DecodeColors(code_table[unboxed_idx], indexInTable, pixel_info_count);

            DecoderHelper* helper = code_table[last_index];
            DecoderHelper* new_helper = new DecoderHelper(code_table[last_index]->first_idx, code_table[unboxed_idx]->first_idx, helper);

            code_table.push_back(new_helper);

            CheckLZWMinimumCodeSize(code_table.size() - 1);
        }
        else
        {
            DecoderHelper* helper = code_table[last_index];
            DecoderHelper* new_helper = new DecoderHelper(helper->first_idx, helper->first_idx, helper);

            code_table.push_back(new_helper);

            CheckLZWMinimumCodeSize(code_table.size() - 1);

            if ((code_table.size() - 1) != unboxed_idx)
            {
                //file.close();
            }
            assert((code_table.size() - 1) == unboxed_idx);

            DecodeColors(code_table[unboxed_idx], indexInTable, pixel_info_count);
        }

        last_index = unboxed_idx;

        assert(last_index < code_table.size());
    }

    ClearTable(code_table);
}

//-----------------------------------------------------------------------
void ImageData::DecodeColors(DecoderHelper* const helper, PixelInfo* & indexInTable, unsigned int& pixel_info_count)
//void ImageData::DecodeColors(DecoderHelper* const helper, std::vector<PixelInfo>::iterator& it)
{
    //if (it == m_decodedData.end())
    if (pixel_info_count >= m_decodedDataSize)
        throw DataStreamException("Image data corrupted");

    if (helper->previous_helper)
    {
        DecodeColors(helper->previous_helper, indexInTable, pixel_info_count);
    }

    unsigned int idx = helper->idx;
    //m_decodedData.push_back(PixelInfo(m_ColorTable[idx], m_transparentIdx != -1 && idx == m_transparentIdx, idx));
    (*indexInTable++) = PixelInfo(m_ColorTable[idx], m_transparentIdx != -1 && idx == m_transparentIdx, idx);
    ++pixel_info_count;
}

void ImageData::ClearCodeTable(std::map<int, EncodeHelper*>& table)
{
    for (std::map<int, ImageData::EncodeHelper*>::iterator it = table.begin(); it != table.end(); ++it)
    {
        delete it->second;
    }
    table.clear();
}

void ImageData::InitCodeTable(std::map<int, EncodeHelper*>& table, unsigned int& lastCodeIdx)
{
    ClearCodeTable(table);

    lastCodeIdx = (unsigned int)pow(2.0, m_LZWMinimumCodeSize);

    for (unsigned int i = 0; i < m_ColorTableSize; ++i)
    {
        table[i] = new EncodeHelper(nullptr, i);
    }

    lastCodeIdx += 2; //for clear code and end code
}

ImageData::EncodeHelper* ImageData::FoundInTable(const std::vector<int>& buffer, int currentChar, std::map<int, ImageData::EncodeHelper*>& table)
{
    std::map<int, ImageData::EncodeHelper*>* current_table = &table;
    EncodeHelper* current_encode_helper = nullptr;
    for (unsigned int i = 0; i < buffer.size(); ++i)
    {
        current_encode_helper = (*current_table)[buffer[i]];
        current_table = &current_encode_helper->children;
    }

    assert(current_encode_helper != nullptr);

    auto it = current_encode_helper->children.find(currentChar);
    if (it == current_encode_helper->children.end())
    {
        EncodeHelper* new_encoder_helper = new EncodeHelper(current_encode_helper);
        current_encode_helper->children[currentChar] = new_encoder_helper;
        return new_encoder_helper;
    }
    else
    {
        return it->second;
    }
}
//-----------------------------------------------------------------------
void ImageData::Encode()
{
    unsigned int count = 0;
    PixelInfo* dataCursor = m_decodedData;

    std::vector<int> buffer;
    std::vector<int> encoded_data;
    int currentChar;

    std::map<int, ImageData::EncodeHelper*> table;
    unsigned int lastCodeIdx = 0;

    InitCodeTable(table, lastCodeIdx);

    int current_idx = dataCursor->m_indexInColorTable;
    ++dataCursor;
    ++count;
    buffer.push_back(current_idx);

    while (count < m_decodedDataSize)
    {
        currentChar = dataCursor->m_indexInColorTable;
        ++dataCursor;
        ++count;

        EncodeHelper* found_in_table = FoundInTable(buffer, currentChar, table);

        if (found_in_table->value == -1)
        {
            encoded_data.push_back(found_in_table->previous_helper->value);
            found_in_table->value = lastCodeIdx++;

            buffer.clear();
            buffer.push_back(currentChar);
        }
        else
        {
            buffer.push_back(currentChar);
        }
    }

    EncodeHelper* found_in_table = FoundInTable(buffer, currentChar, table);
    encoded_data.push_back(found_in_table->previous_helper->value);
}
//-----------------------------------------------------------------------