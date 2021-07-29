#pragma once
#include <vcruntime_exception.h>
#include <string>

namespace Gif
{
    enum class ELabel : unsigned char
    {
        ApplicationExtension = 0xFF,
        CommentExtension = 0xFE,
        ExtensionIntroducer = 0x21,
        GraphicControlExtension = 0xF9,
        ImageDescriptor = 0x2C,
        PlainTextExtension = 0x01,
        Trailer = 0x3B,
    };

    enum class EScope : unsigned char
    {
        GraphicRenderBlockStart = 0x00,
        GraphicRenderBlockEnd = 0x7F,
        ControlBlockStart = 0x80,
        ControlBlockEnd = 0xF9,
        SpecialPurposeBlockStart = 0xFA,
        SpecialPurposeBlockEnd = 0xFF
    };

    class DataStreamException : std::exception
    {
    public:
        DataStreamException(const char* _info) : info(_info) {}
        virtual const char* what() const noexcept override { return info.c_str(); };

        std::string info;
    };

    //Header - REQUIRED
    // - Exactly one Header must be present per Data Stream.
    // - The scope of this block is the entire Data Stream.
    // - This block cannot be modified by any extension
    struct Header
    {
        unsigned  char m_Signature[3];  //Identifies the GIF Data Stream. This field contains
                                        //the fixed value 'GIF'.
        unsigned  char m_Version[3];    //"87a" - May 1987
                                        //"89a" - July 1989
    };

    struct Color
    {
        unsigned char m_red = 0;
        unsigned char m_green = 0;
        unsigned char m_blue = 0;

        static unsigned short ConvertTo4Bit(const Gif::Color& value);
    };
}