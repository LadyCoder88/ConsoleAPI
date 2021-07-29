#pragma once
#include "Header.h"
#include <fstream>

namespace Gif
{
    struct GraphicControlExtension
    {
        struct SPackedFields
        {
            unsigned char transparentColorMask : 1,
                                 userInputMask : 1,
                            disposalMethodMask : 3;
        };

        union
        {
            unsigned char value;
            SPackedFields fields;
        } m_fields;

        enum class EDisposeMethod : short int
        {
            //No disposal specified.The decoder is not required to take any action.
            NoAction = 0,

            //Do not dispose.The graphic is to be left in place.
            DoNotDispose = 1,

            //Restore to background color. The area used by the graphic must be restored to the background color.
            RestoreToBackgroundColor = 2,

            //Restore to previous.The decoder is required to restore the area overwritten by the graphic with
            //what was there prior to rendering the graphic.
            RestoreToPrevious = 3,

            //4 - 7 - To be defined.
            ToBeDefined1 = 4,
            ToBeDefined2 = 5,
            ToBeDefined3 = 6,
        };

        // - Number of bytes in the block, after the Block Size field and up to but not including the Block Terminator.
        // - This field contains the fixed value 4.
        unsigned char m_blockSize;
        
        EDisposeMethod GetDisposeMethos() { return static_cast<EDisposeMethod>(m_fields.fields.disposalMethodMask); }
        bool GetIsWaitForUserInput() { return m_fields.fields.userInputMask; }
        bool GetIsTransparentColorSet() { return m_fields.fields.transparentColorMask; }

        //Delay Time
        // - If not 0, this field specifies the number of hundredths (1/100) of a second to wait before continuing with the
        //   processing of the Data Stream. The clock starts ticking immediately after the graphic is rendered.
        // - This field may be used in conjunction with the User Input Flag field.
        unsigned short int m_delayTime;

        //Transparency Index
        // - The Transparency Index is such that when encountered, the corresponding pixel of the display device is not
        //   modified and processing goes on to the next pixel. The index is present if and only if the Transparency Flag is set to 1.
        unsigned char m_transparentColorIndex;

        void Read(std::ifstream &file);
    };
}
