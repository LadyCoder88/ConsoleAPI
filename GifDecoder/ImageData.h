#pragma once
#include "DataSubBlock.h"
#include <fstream>
#include <vector>
#include <map>
#include "Header.h"

namespace Gif
{
    struct ImageData
    {
        struct DecoderHelper
        {
            unsigned int first_idx;
            unsigned int idx;
            DecoderHelper* previous_helper;

            DecoderHelper(unsigned int _first_idx, unsigned int _idx, DecoderHelper* _previous_helper)
                :first_idx(_first_idx), idx(_idx), previous_helper(_previous_helper) {}
        };

        struct EncodeHelper
        {
            int value;
            std::map<int, ImageData::EncodeHelper*> children;

            EncodeHelper(EncodeHelper* _previous_helper) : previous_helper(_previous_helper), value(-1) {}
            EncodeHelper(EncodeHelper* _previous_helper, int _value) : previous_helper(_previous_helper), value(_value) {}
            ~EncodeHelper()
            {
                for (std::map<int, ImageData::EncodeHelper*>::iterator it = children.begin(); it!= children.end(); ++it)
                {
                    delete it->second;
                }
                children.clear();
            }

            EncodeHelper* previous_helper;
        };

        struct PixelInfo
        {
            PixelInfo() :m_isTransparent(false), m_indexInColorTable(-1) 
            {}
            PixelInfo(const Color& color, bool is_transparent, int indexInColorTable)
                :m_color(color), 
                m_isTransparent(is_transparent),
                m_indexInColorTable(indexInColorTable) 
            {}

            Color m_color;
            bool m_isTransparent;
            int m_indexInColorTable;
        };

        unsigned int m_clearCode;
        unsigned int m_endCode;

        unsigned int m_lastCodeIdx;
        unsigned char m_LZWMinimumCodeSize;
        unsigned char m_LZWCurrentCodeSide;

        std::vector<DataSubBlock> m_data;
        //std::vector<PixelInfo> m_decodedData;
        unsigned int m_decodedDataSize;
        PixelInfo* const m_decodedData;

        const Color* m_ColorTable;
        unsigned int m_ColorTableSize;

        unsigned int m_CurrentDataIdx;
        unsigned int m_CurrentSubBlockIdx;
        int m_transparentIdx;

        unsigned char m_SourceIdx;

        ImageData(Color* color_table, int color_table_size, int tranparent_idx, unsigned int decoded_image_size);
        ImageData(const ImageData& image_data);
        ImageData(ImageData&& image_data);
        ~ImageData();

        void Read(std::ifstream& file);
        unsigned int UnboxIdx();
        void CheckLZWMinimumCodeSize(unsigned int index);

        void ResetToStartupState(std::vector<DecoderHelper*>& table);
        void InitCodeTable(std::vector<DecoderHelper*>& table);
        void ClearTable(std::vector<DecoderHelper*>& table);
        void DecodeColors(DecoderHelper* const helper, PixelInfo* & indexInTable, unsigned int& pixel_info_count);
        //void DecodeColors(DecoderHelper* const helper, std::vector<PixelInfo>::iterator& it);
        void Decode();
        void ClearCodeTable(std::map<int, EncodeHelper*>& table);
        void InitCodeTable(std::map<int, EncodeHelper*>& table, unsigned int& lastCodeIdx);
        EncodeHelper* FoundInTable(const std::vector<int>& buffer, int currentChar, std::map<int, EncodeHelper*>& table);
        void Encode();
        
        void Log(std::ofstream& file, unsigned int unboxed_value, unsigned int table_size);
    };
}
