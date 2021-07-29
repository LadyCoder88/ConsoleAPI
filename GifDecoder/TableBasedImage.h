#pragma once
#include "Header.h"
#include "ImageDescriptor.h"
#include "ImageData.h"
#include "GraphicControlExtension.h"

namespace Gif
{
    //TABLE BASED IMAGE - OPTIONAL
    // - Contains a color table, which is a sequence of bytes representing red - green - blue color triplets
    // - Local Color Table is used by the image that immediately follows
    // - Its presence is marked by the Local Color Table Flag being set to 1 in the Image Descriptor
    //   if present, the Local Color Table immediately follows the Image Descriptor and contains a number of bytes equal to
    //   3x2 ^ (Size of Local Color Table + 1)
    // - If present, this color table temporarily becomes the active color table and the following image should be processed using it
    struct TableBasedImage
    {
        TableBasedImage(Color* globalColorTable, int globalColorTableSize, GraphicControlExtension* &graphicControlExtension);
        TableBasedImage(const TableBasedImage& table_based_image);
        TableBasedImage(TableBasedImage&& table_based_image);
        ~TableBasedImage();

        GraphicControlExtension* m_graphicControlExtension;

        //IMAGE DESCRIPTOR - REQUIRED
        // - Contains the parameters necessary to process a table based image
        // - The coordinates given in this block refer to coordinates within the Logical Screen, and are given in pixels
        // - This block is a Graphic - Rendering Block, optionally preceded by one or more Control followed by a Local Color Table
        ImageDescriptor m_imageDescriptor;

        //LOCAL COLOR TABLE - OPTIONAL
        Color* m_localColorTable;
        Color* const m_GlobalColorTable;
        int m_GlobalColorTableSize;

        //IMAGE DATA FOR TABLE BASED IMAGE
        // - Consists of a sequence of sub - blocks, of size at most 255 bytes each, containing an
        //   index into the active color table, for each pixel in the image
        // - Pixel indices are in order of left to right and from top to bottom
        // - Each index must be within the range of the size of the active color table, starting at 0
        // - The sequence of indices is encoded using the LZW Algorithm with variable - length code
        ImageData* m_imageData;

        void Read(std::ifstream& file);
    };
}