#pragma once
#include <fstream>

namespace Gif
{
    struct ImageDescriptor
    {
        struct SPackedFields
        {
            unsigned char sizeOfLocalColorMask : 3,
                                                     : 2,
                                            sortMask : 1,
                                       interlaceMask : 1,
                               isLocalColorTableMask : 1;
        };

        union UPackedFields
        {
            unsigned char value;
            SPackedFields fields;
        } m_fields;

        //Column number, in pixels, of the left edge of the image, with respect to the left edge of the Logical Screen.
        //Leftmost column of the Logical Screen is 0.
        unsigned short int m_imageLeftPosition;

        //Row number, in pixels, of the top edge of the image with respect to the top edge of the Logical Screen.Top
        //Topmost row of the Logical Screen is 0.
        unsigned short int m_imageTopPosition; 

        //Width of the image in pixels.
        unsigned short int m_imageWidth;

        //Height of the image in pixels.
        unsigned short int m_imageHeight;

        bool GetIsLocalColorTablePresent() { return m_fields.fields.isLocalColorTableMask; }
        unsigned short int GetLocalColorTableSize() { return m_fields.fields.sizeOfLocalColorMask; }
        bool GetIsDataSorted() { return m_fields.fields.sortMask; }
        bool GetIsDataInterlanced() { return m_fields.fields.interlaceMask; }

        void Read(std::ifstream& file);
    };
}