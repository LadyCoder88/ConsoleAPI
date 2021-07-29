#pragma once
#include <fstream>

namespace Gif
{
    struct LogicalScreenDescriptor
    {
        struct SPackedFields
        {
            unsigned char sizeOfGlobalColorTableMask : 3,
                                        sortFlagMask : 1,
                                 colorResolutionMask : 3,
                                globalColorTableMask : 1;
        };

        union
        {
            unsigned char value;
            SPackedFields fields;
        } m_fields;

        //Width, in pixels, of the Logical Screen where the images will be rendered in the displaying device
        unsigned short int m_screenWidth = 0;

        //Height, in pixels, of the Logical Screen where the images will be rendered in the displaying device.
        unsigned short int m_screenHeight = 0;

        bool GetIsGlobalColorTable() const { return m_fields.fields.globalColorTableMask; }
        bool GetIsTableSorted() const { return m_fields.fields.sortFlagMask; }

        /*Number of bits per primary color available to the original image, minus 1. This value represents the size of
        the entire palette from which the colors in the graphic were selected, not the number of colors actually used in the graphic.
        For example, if the value in this field is 3, then the palette of the original image had 4 bits per primary color 
        available to create the image.This value should be set to indicate the richness of the original palette, even if 
        not every color from the whole palette is available on the source machine.*/
        unsigned char GetColorResolution() const { return m_fields.fields.colorResolutionMask; }
        unsigned char GetGlobalColorTableSize() const { return m_fields.fields.sizeOfGlobalColorTableMask; }

        //The Background Color is the color used for those pixels on the screen that are not covered by an image.
        // If the Global Color Table Flag is set to(zero), this field should be zero and should be ignored.
        unsigned char m_backgroundColorIndex;

        //Pixel Aspect Ratio
        // - Factor used to compute an approximation of the aspect ratio of the pixel in the original image.
        // - If the value of the field is not 0, this approximation of the aspect ratio is computed based on the formula:
        //   Aspect Ratio = (Pixel Aspect Ratio + 15) / 64
        // - The Pixel Aspect Ratio is defined to be the quotient of the pixel's width over its height.
        // - The value range in this field allows specification of the widest pixel of 4:1 to the tallest pixel of
        //   1:4 in increments of 1/64th.
        //   Values :
        //     0      - No aspect ratio information is given.
        //     1..255 -   Value used in the computation.
        unsigned char m_pixelAspectRation; 

        void Read(std::ifstream &file);
    };
}

