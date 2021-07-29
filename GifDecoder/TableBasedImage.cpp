#include "TableBasedImage.h"
#include <fstream>
#include <iostream>
#include <assert.h>

using namespace Gif;

TableBasedImage::TableBasedImage(Color* globalColorTable, int globalColorTableSize, GraphicControlExtension* &graphicControlExtension)
    : m_localColorTable(nullptr), 
    m_GlobalColorTable(globalColorTable),
    m_GlobalColorTableSize(globalColorTableSize),
    m_graphicControlExtension(graphicControlExtension),
    m_imageData(nullptr)
{
    graphicControlExtension = nullptr;
}

TableBasedImage::TableBasedImage(const TableBasedImage& table_based_image)
    :m_GlobalColorTable(table_based_image.m_GlobalColorTable)
{
    m_GlobalColorTableSize = table_based_image.m_GlobalColorTableSize;
    m_localColorTable = table_based_image.m_localColorTable;
    m_graphicControlExtension = new GraphicControlExtension(*table_based_image.m_graphicControlExtension);
    m_imageData = new ImageData(*table_based_image.m_imageData);
}

TableBasedImage::TableBasedImage(TableBasedImage&& table_based_image)
    :m_GlobalColorTable(table_based_image.m_GlobalColorTable)
{
    m_GlobalColorTableSize = table_based_image.m_GlobalColorTableSize;
    m_localColorTable = table_based_image.m_localColorTable;
    table_based_image.m_localColorTable = nullptr;

    m_graphicControlExtension = table_based_image.m_graphicControlExtension;
    table_based_image.m_graphicControlExtension = nullptr;

    m_imageData = table_based_image.m_imageData;
    table_based_image.m_imageData = nullptr;
}

TableBasedImage::~TableBasedImage()
{
    delete m_imageData;
    m_imageData = nullptr;

    delete m_graphicControlExtension;
    m_graphicControlExtension = nullptr;
}

void TableBasedImage::Read(std::ifstream& file)
{
    m_imageDescriptor.Read(file);

    int transparentIdx = m_graphicControlExtension != nullptr && m_graphicControlExtension->GetIsTransparentColorSet() ? m_graphicControlExtension->m_transparentColorIndex : -1;
    if (m_imageDescriptor.GetIsLocalColorTablePresent())
    {
        // if present, the Local Color Table immediately follows the Image Descriptor
        // and contains a number of bytes equal to 3x2 ^ (Size of Local Color Table + 1)
        // sizeof Color struct is 3 bytes so our tableColorSize will be 2 ^ (Size of Local Color Table + 1)
        int tableColorSize = (int)pow(2.0, m_imageDescriptor.GetLocalColorTableSize() + 1);
        m_localColorTable = new Color[tableColorSize];

        assert(sizeof(Color) == 3);
        file.read((char*)m_localColorTable, sizeof(Color) * tableColorSize);

        m_imageData = new ImageData(m_localColorTable, tableColorSize, transparentIdx, m_imageDescriptor.m_imageHeight * m_imageDescriptor.m_imageWidth);
    }
    else
    {
        m_imageData = new ImageData(m_GlobalColorTable, m_GlobalColorTableSize, transparentIdx, m_imageDescriptor.m_imageHeight * m_imageDescriptor.m_imageWidth);
    }

    m_imageData->Read(file);

    m_imageData->Decode();
}