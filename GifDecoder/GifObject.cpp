#include "GifObject.h"
#include "DataStream.h"
#include <iostream>
#include <filesystem>

using namespace Gif;

//-----------------------------------------------------------------------------------------------
GifObject::GifObject(const DataStream& data_stream)
{
    m_fileName = std::filesystem::path(data_stream.m_fileName.c_str()).filename().generic_string();
    m_imageHeight = data_stream.m_logicalScreenDescriptor.m_screenHeight;
    m_imageWidth = data_stream.m_logicalScreenDescriptor.m_screenWidth;

    m_framesCount = data_stream.m_TableData.size();
    m_frames.reserve(m_framesCount);

    if (data_stream.m_logicalScreenDescriptor.GetIsGlobalColorTable())
    {
        m_backgroundColor.m_RGBColor = data_stream.m_GlobalColorTable[data_stream.m_logicalScreenDescriptor.m_backgroundColorIndex];
    }

    for (unsigned int i = 0; i < m_framesCount; ++i)
    {
        Gif::TableBasedImage* table_image = data_stream.m_TableData[i];

        FrameInfo frame_info(table_image->m_imageDescriptor.m_imageWidth,
            table_image->m_imageDescriptor.m_imageHeight,
            table_image->m_imageDescriptor.m_imageLeftPosition,
            table_image->m_imageDescriptor.m_imageTopPosition,
            table_image->m_graphicControlExtension->m_delayTime,
            table_image->m_graphicControlExtension->GetDisposeMethos());

        Frame* current_frame = new Frame(frame_info);
        m_frames.push_back(current_frame);

        int frame_characters = 0;
        for (int j = 0; j < current_frame->GetDataSize(); ++j)
        {
            const Gif::ImageData::PixelInfo& value = table_image->m_imageData->m_decodedData[j];
            RGBAColor new_color(value.m_color, value.m_isTransparent);

            current_frame->SetColor(j, new_color);
        }
    }
}

//-----------------------------------------------------------------------------------------------
GifObject::GifObject(const GifObject& gif_object)
{
    CopyData(gif_object);
}

//-----------------------------------------------------------------------------------------------
GifObject::GifObject(GifObject&& gif_object)
{
    MoveData(std::move(gif_object));
}

//-----------------------------------------------------------------------------------------------
GifObject& GifObject::operator=(const GifObject& gif_object)
{
    if (&gif_object == this)
        return *this;

    CopyData(gif_object);

    return *this;
}

//-----------------------------------------------------------------------------------------------
GifObject& GifObject::operator=(GifObject&& gif_object)
{
    if (&gif_object == this)
        return *this;

    MoveData(std::move(gif_object));

    return *this;
}
//-----------------------------------------------------------------------------------------------
void GifObject::CopyData(const GifObject& gif_object)
{
    m_fileName = gif_object.m_fileName;
    m_framesCount = gif_object.m_framesCount;

    for (Frame* frame : gif_object.m_frames)
    {
        m_frames.push_back(new Frame(*frame));
    }

    m_backgroundColor = gif_object.m_backgroundColor;

    m_imageHeight = gif_object.m_imageHeight;
    m_imageWidth = gif_object.m_imageWidth;
}

//-----------------------------------------------------------------------------------------------
void GifObject::MoveData(GifObject&& gif_object)
{
    m_fileName = std::move(gif_object.m_fileName);

    m_framesCount = gif_object.m_framesCount;
    gif_object.m_framesCount = 0;

    m_frames = std::move(gif_object.m_frames);
    m_backgroundColor = std::move(gif_object.m_backgroundColor);

    m_imageHeight = gif_object.m_imageHeight;
    m_imageWidth = gif_object.m_imageWidth;
}

//-----------------------------------------------------------------------------------------------
GifObject::~GifObject()
{
    for (Frame* frame : m_frames)
    {
        delete frame;
    }

    m_frames.clear();
}

//-----------------------------------------------------------------------------------------------
const GifObject::Frame& GifObject::GetFrame(int idx) const 
{ 
    //At function returns a reference to the element at specified location idx, with bounds checking
    //If pos is not within the range of the container, an exception of type std::out_of_range is thrown
    Frame* frame = m_frames.at(idx);
    return *frame;
}

//-----------------------------------------------------------------------------------------------
GifObject::Frame::Frame(const GifObject::FrameInfo& frame_info) : m_frameInfo(frame_info)
{
    m_dataSize = frame_info.GetFrameSizeX() * frame_info.GetFrameSizeY();
    m_data = new RGBAColor[m_dataSize];
}

//-----------------------------------------------------------------------------------------------
GifObject::Frame::Frame(GifObject::Frame&& frame)
{
    m_dataSize = frame.m_dataSize;
    frame.m_dataSize = 0;

    m_data = frame.m_data;
    frame.m_data = nullptr;
}

//-----------------------------------------------------------------------------------------------
GifObject::Frame::Frame(const Frame& frame)
{
    m_dataSize = frame.m_dataSize;
    m_data = new RGBAColor[m_dataSize];

    for (unsigned int i = 0; i < m_dataSize; ++i)
    {
        m_data[i] = frame.m_data[i];
    }
}

//-----------------------------------------------------------------------------------------------
GifObject::Frame::~Frame()
{
    delete m_data;
    m_data = nullptr;
}

//-----------------------------------------------------------------------------------------------
void GifObject::Frame::SetColor(int idx, RGBAColor color)
{
    m_data[idx] = color;
}
//-----------------------------------------------------------------------------------------------