#include "DataStream.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include "TableBasedImage.h"
#include "GraphicControlExtension.h"

using namespace std;
using namespace Gif;

//------------------------------------------------------------------------------
DataStream DataStream::Load(std::string file_name)
{
    ifstream file(file_name.c_str(), std::ios::out | std::ios::binary);
    if (!file)
        throw DataStreamException("Couldn't load .gif file");

    DataStream data_stream;
    data_stream.m_fileName = file_name;
    data_stream.Read(file);

    file.close();

    return data_stream;
}

//------------------------------------------------------------------------------
DataStream::DataStream() : m_GlobalColorTable(nullptr)
{

}

//------------------------------------------------------------------------------
DataStream::DataStream(const DataStream& data_stream)
{
    m_header = data_stream.m_header;
    m_logicalScreenDescriptor = data_stream.m_logicalScreenDescriptor;

    m_globalColorTableSize = data_stream.m_globalColorTableSize;
    m_GlobalColorTable = new Color[m_globalColorTableSize];
    for (int i = 0; i < m_globalColorTableSize; ++i)
    {
        m_GlobalColorTable[i] = data_stream.m_GlobalColorTable[i];
    }

    for (unsigned int i = 0; i < data_stream.m_TableData.size(); ++i)
    {
        m_TableData.push_back(new TableBasedImage(*data_stream.m_TableData[i]));
    }
}

//------------------------------------------------------------------------------
DataStream::DataStream(DataStream&& data_stream)
{
    m_globalColorTableSize = data_stream.m_globalColorTableSize;
    data_stream.m_globalColorTableSize = 0;

    m_GlobalColorTable = data_stream.m_GlobalColorTable;
    data_stream.m_GlobalColorTable = nullptr;

    m_header = std::move(data_stream.m_header);
    m_logicalScreenDescriptor = std::move(data_stream.m_logicalScreenDescriptor);

    m_TableData = std::move(data_stream.m_TableData);
}

//------------------------------------------------------------------------------
DataStream::~DataStream()
{
    delete[] m_GlobalColorTable;
    m_GlobalColorTable = nullptr;

    for (auto* data : m_TableData)
        delete data;

    m_TableData.clear();
}

//------------------------------------------------------------------------------
void DataStream::Read(std::ifstream &file)
{
    // HEADER - REQUIRED
    // - Exactly one Header must be present per Data Stream
    // - Identifies the GIF Data Stream in context
    file.read((char*)m_header.m_Signature, sizeof(m_header.m_Signature));
    file.read((char*)m_header.m_Version, sizeof(m_header.m_Version));

    // LOGICAL SCREEN DESCRIPTOR - REQUIRED
    // - Exactly one Logical Screen Descriptor must be present per Data Stream
    // - Contains the parameters necessary to define the area of the display device within which the images will be rendered
    // - This block must appear immediately after the Header
    m_logicalScreenDescriptor.Read(file);

    if (m_logicalScreenDescriptor.GetIsGlobalColorTable())
    {
        m_globalColorTableSize = (int)std::pow(2.0, (double)m_logicalScreenDescriptor.GetGlobalColorTableSize() + 1.0);

        m_GlobalColorTable = new Color[m_globalColorTableSize];

        file.read((char*)m_GlobalColorTable, 3 * m_globalColorTableSize);
    }

    GraphicControlExtension* graphicControlExtension = nullptr;

    unsigned char label = 0;
    bool read = true;
    while (read)
    {
        file.read((char*)&label, sizeof(label));

        //Extension Introducer 
        // - Identifies the beginning of an extension block.
        // - This field contains the fixed value 0x21
        if (label == static_cast<unsigned char>(Gif::ELabel::ExtensionIntroducer))
        {
            file.read((char*)&label, sizeof(label));
            switch (static_cast<Gif::ELabel>(label))
            {
            //APPLICATION EXTENSION - OPTIONAL
            // - Contains application-specific information
            case Gif::ELabel::ApplicationExtension:
            {
                DataSubBlock::SkipBlock(file);
                break;
            }
            //COMMENT EXTENSION - OPTIONAL
            // - Contains textual information which is not part of the actual graphics in the GIF Data Stream
            // - Any number of them may appear in the Data Stream
            case Gif::ELabel::CommentExtension:
            {
                DataSubBlock::SkipBlock(file);
                break;
            }
            //GRAPHIC CONTROL EXTENSION - OPTIONAL
            // - Contains parameters used when processing a graphic rendering block.The scope of this extension is
            //   the first graphic rendering block to follow
            case Gif::ELabel::GraphicControlExtension:
            {
                graphicControlExtension = new GraphicControlExtension();
                graphicControlExtension->Read(file);

                break;
            }
            //PLAIN TEXT EXTENSION - OPTIONAL
            // - Contains textual data and the parameters necessary to render that data as a graphic, in a simple form
            case Gif::ELabel::PlainTextExtension:
            {
                DataSubBlock::SkipBlock(file);
                break;
            }
            default:
            {
                ReportNotFoundBlock(label, file);
                break;
            }
            }
        }
        //IMAGE DESCRIPTOR - REQUIRED
        // - Contains the parameters necessary to process a table based image
        // - Exactly one Image Descriptor per image in Data Stream
        else if (label == static_cast<unsigned char>(Gif::ELabel::ImageDescriptor))
        {
            TableBasedImage* data = new TableBasedImage(m_GlobalColorTable, m_globalColorTableSize, graphicControlExtension);
            data->Read(file);

            m_TableData.push_back(data);
        }
        //TRAILER - REQUIRED
        // - A single-field block indicating the end of the GIF Data Stream.
        // - It contains the fixed value 0x3B
        else if(label == static_cast<unsigned char>(Gif::ELabel::Trailer))
        {
            read = false;
        }
        else
        {
            ReportNotFoundBlock(label, file);
        }
    }

    if (graphicControlExtension)
    {
        delete graphicControlExtension;
        graphicControlExtension = nullptr;
    }
}

//------------------------------------------------------------------------------
void DataStream::ReportNotFoundBlock(unsigned char label, std::ifstream& file)
{
    //0x00 - 0x7F (0 - 127)
    if (label >= (unsigned char)Gif::EScope::GraphicRenderBlockStart && label <= (unsigned char)Gif::EScope::GraphicRenderBlockEnd)
    {
        std::cout << "Graphic Rendering Block not found: " << std::hex << label << std::endl;
        DataSubBlock::SkipBlock(file);
    }
    //0x80 - 0xF9 (128 - 249)
    else if (label >= (unsigned char)Gif::EScope::ControlBlockStart && label <= (unsigned char)Gif::EScope::ControlBlockEnd)
    {
        std::cout << "Control Block not found: " << std::hex << label << std::endl;
        DataSubBlock::SkipBlock(file);
    }
    //0xFA-0xFF (250-255)
    else if (label >= (unsigned char)Gif::EScope::SpecialPurposeBlockStart && label <= (unsigned char)Gif::EScope::SpecialPurposeBlockEnd)
    {
        std::cout << "Special Purpose Block not found: " << std::hex << label << std::endl;
        DataSubBlock::SkipBlock(file);
    }
}
//------------------------------------------------------------------------------