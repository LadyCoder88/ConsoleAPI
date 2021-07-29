#include "ConsoleMovie.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <assert.h>
#include <bitset>
#include "../GifDecoder/GifDecoder.h"

using namespace std;

ConsoleMovie::ConsoleMovie(MovieCOORD image_size, int frames_count, int interval, bool loop, const char* fileName)
    :IConsoleMovie(image_size, frames_count, loop, fileName)
{
    m_frames.reserve(m_frames_count);
    int frame_characters = image_size.X * image_size.Y;

    FrameInfo info(image_size.X, image_size.Y);
    info.m_coord = { 0,0 };
    info.m_delay = interval;

    for (int i = 0; i < m_frames_count; ++i)
    {
        m_frames.emplace_back(new CHAR_INFO[frame_characters], info);
    }

    m_background = new CHAR_INFO[frame_characters];
}

FrameInfo& ConsoleMovie::GetFrameInfo(int frame_number)
{
    return m_frames[frame_number].second;
}

void CopyTable(CHAR_INFO* source, CHAR_INFO* dest, int size)
{
    for (int i = 0; i < size; ++i)
    {
        dest[i] = source[i];
    }
}

void InitTable(CHAR_INFO* dest, const CHAR_INFO& value, int size)
{
    for (int i = 0; i < size; ++i)
    {
        dest[i] = value;
    }
}

static void FrameDump(CHAR_INFO* frame, int size, int width)
{
    std::ofstream file("D:\\Programy\\Other\\ConsoleApiTest\\GifDecoder\\frame_dump.txt");
    if (!file)
    {
        std::cout << "An error occurred opening the file" << std::endl;
    }

    int count = 0;
    for (int i = 0; i < size; ++i)
    {
        ++count;
        file << (int)frame[i].Attributes << '\t';    
        
        if (count == width)
        {
            file << std::endl;
            count = 0;
        }
    }

    file.close();
}

ConsoleMovie::ConsoleMovie(const Gif::GifObject& gifObject)
    :IConsoleMovie({gifObject.GetImageWigth(), gifObject.GetImageHeight()}, gifObject.GetFrameCount(), true)
{
    m_frames.reserve(gifObject.GetFrameCount());
    int image_size = m_imageSize.X * m_imageSize.Y;

    m_backgroundColor = Gif::Color::ConvertTo4Bit(gifObject.GetBackgroundColor().m_RGBColor);

    for (int i = 0; i < m_frames_count; ++i)
    {
        const Gif::GifObject::Frame& frame = gifObject.GetFrame(i);
        const Gif::GifObject::FrameInfo& frameinfo = frame.GetFrameInfo();

        FrameInfo frame_info(frameinfo.GetFrameSizeX(),
            frameinfo.GetFrameSizeY(),
            frameinfo.GetFramePositionX(),
            frameinfo.GetFramePositionY(),
            frameinfo.GetDelayTime() * 10);
        frame_info.m_restore_to_backgroundColor = frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::RestoreToBackgroundColor;

        m_frames.emplace_back(new CHAR_INFO[image_size], frame_info);

        int frames_info_count = 0;
        for (int j = 0; j < frame_info.m_size.Y; ++j)
        {
            for (int k = 0; k < frame_info.m_size.X; ++k)
            {
                const Gif::RGBAColor color = frame.GetData()[frames_info_count++];

                int idx = (j + frame_info.m_coord.Y) * m_imageSize.X + (k + frame_info.m_coord.X);
                CHAR_INFO& info = m_frames[i].first[idx];
                info.Char.AsciiChar = ' ';
                info.Char.UnicodeChar = info.Char.AsciiChar;

                if (color.m_alpha == 0)
                {
                    if (i > 0)
                    {
                        info.Attributes = m_frames[i - 1].first[idx].Attributes;
                    }
                    else
                    {
                        info.Attributes = m_backgroundColor;
                    }
                }
                else
                {
                    info.Attributes = Gif::Color::ConvertTo4Bit(color.m_RGBColor);// 7
                }
            }
        }

        //FrameDump(ptr, frame_characters, m_frame_size_x);
    }

    CHAR_INFO background;
    background.Attributes = m_backgroundColor;
    background.Char.UnicodeChar = ' ';
    background.Char.AsciiChar = ' ';

    m_background = new CHAR_INFO[image_size];

    InitTable(m_background, background, image_size);

    m_unitSize = 1;
}

// ConsoleMovie::ConsoleMovie(Gif::DataStream& data_stream)
//     :IConsoleMovie({ data_stream.m_logicalScreenDescriptor.m_screenWidth, data_stream.m_logicalScreenDescriptor.m_screenHeight }, data_stream.m_TableData.size(), true)
// {
//     m_frames.reserve(m_frames_count);
//     int image_size = m_imageSize.X * m_imageSize.Y;
// 
//     if (data_stream.m_logicalScreenDescriptor.GetIsGlobalColorTable())
//     {
//         m_backgroundColor = Gif::Color::ConvertTo4Bit(data_stream.m_GlobalColorTable[data_stream.m_logicalScreenDescriptor.m_backgroundColorIndex]);
//     }
// 
//     for (int i = 0; i < m_frames_count; ++i)
//     {
//         Gif::TableBasedImage* table_image = data_stream.m_TableData[i];
//  
//         FrameInfo frame_info(table_image->m_imageDescriptor.m_imageWidth,
//             table_image->m_imageDescriptor.m_imageHeight,
//             table_image->m_imageDescriptor.m_imageLeftPosition,
//             table_image->m_imageDescriptor.m_imageTopPosition,
//             table_image->m_graphicControlExtension->m_delayTime * 10);
//         frame_info.m_restore_to_backgroundColor = table_image->m_graphicControlExtension->GetDisposeMethos() == Gif::GraphicControlExtension::EDisposeMethod::RestoreToBackgroundColor;
// 
//         //int frame_characters = frame_info.m_size.X * frame_info.m_size.Y;
//         //m_frames[i] = new CHAR_INFO[frame_characters];
//         m_frames.emplace_back(new CHAR_INFO[image_size], frame_info);
// //         if (i > 0)
// //         {
// //             CopyTable(m_frames[i-1], m_frames[i], image_size);
// //         }
// 
//         int frames_info_count = 0;
//         //for (int j = 0; j < frame_characters; ++j)
//         for (int j = 0; j < frame_info.m_size.Y; ++j)
//         {
//             for (int k = 0; k < frame_info.m_size.X; ++k)
//             {
//                 const Gif::PixelInfo& value = table_image->m_imageData->m_decodedData[frames_info_count++];
// 
//                 int idx = (j + frame_info.m_coord.Y) * m_imageSize.X + (k + frame_info.m_coord.X);
//                 CHAR_INFO& info = m_frames[i].first[idx];
//                 info.Char.AsciiChar = ' ';
//                 info.Char.UnicodeChar = info.Char.AsciiChar;
// 
//                 if (value.m_isTransparent &&  i > 0)
//                 {
//                     info.Attributes = m_frames[i - 0].first[idx].Attributes;
//                 }
//                 else
//                 {
//                     info.Attributes = (i == 0) ? m_backgroundColor : Gif::Color::ConvertTo4Bit(value.m_color);// 7
//                 }  
//             }  
//         }
//         //FrameDump(ptr, frame_characters, m_frame_size_x);
//     }
// 
//     CHAR_INFO background;
//     background.Attributes = m_backgroundColor;
//     background.Char.UnicodeChar = ' ';
//     background.Char.AsciiChar = ' ';
// 
//     m_background = new CHAR_INFO[image_size];
// 
//     InitTable(m_background, background, image_size);
// 
//     m_unitSize = 1;
// }
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie::ConsoleMovie(const ConsoleMovie& movie): IConsoleMovie(movie)
{
    m_frames = movie.m_frames;
    m_frames.reserve(m_frames_count);

    for (int i = 0; i < m_frames_count; ++i)
    {
        const FrameInfo& info = GetFrameInfo(i);
        int frame_characters = info.m_size.X * info.m_size.Y;

        m_frames.emplace_back(new CHAR_INFO[frame_characters], info);

        for (int j = 0; j < frame_characters; ++j)
        {
            m_frames[i].first[j] = movie.m_frames[i].first[j];
        }
    }

    int size = m_imageSize.X * m_imageSize.Y;
    m_background = new CHAR_INFO[size];
    CopyTable(movie.m_background, m_background, size);
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie::ConsoleMovie(ConsoleMovie&& movie): IConsoleMovie(std::move(movie))
{
    m_frames = std::move(movie.m_frames);
    m_background = std::move(movie.m_background);
    movie.m_background = nullptr;
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie& ConsoleMovie::operator=(const ConsoleMovie& movie)
{
    __super::operator=(movie);

    m_frames = movie.m_frames;
    m_frames.reserve(m_frames_count);

    for (int i = 0; i < m_frames_count; ++i)
    {
        const FrameInfo& info = GetFrameInfo(i);
        int frame_characters = info.m_size.X * info.m_size.Y;
        
        m_frames.emplace_back(new CHAR_INFO[frame_characters], info);

        for (int j = 0; j < frame_characters; ++j)
        {
            m_frames[i].first[j] = movie.m_frames[i].first[j];
        }
    }

    int size = m_imageSize.X * m_imageSize.Y;
    m_background = new CHAR_INFO[size];
    CopyTable(movie.m_background, m_background, size);

    return *this;
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie& ConsoleMovie::operator=(ConsoleMovie&& movie)
{
    __super::operator=(std::move(movie));

    m_frames = std::move(movie.m_frames);
    m_background = std::move(movie.m_background);
    movie.m_background = nullptr;

    return *this;
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie::~ConsoleMovie()
{
    if (m_frames.size())
    {
        for (int i = 0; i < m_frames_count; ++i)
        {
            delete[] m_frames[i].first;
            m_frames[i].first = nullptr;
        }

        m_frames.clear();
    }

    delete[] m_background;
    m_background = nullptr;
}
///////////////////////////////////////////////////////////////////////////////
ConsoleMovie ConsoleMovie::LoadConsoleMovie(std::string file_full_path)
{
    std::string ext = file_full_path.substr(file_full_path.find_last_of('.'));

    if (ext.compare(".csol") == 0)
    {
        return LoadCSolFile(file_full_path);
    }
    else if (ext.compare(".gif") == 0)
    {
        return LoadGifFile(file_full_path);
    }

    throw ConsolMovieException("File extension not supported");
}

////////////////////////////////////////////////////////////////////////////////
ConsoleMovie ConsoleMovie::LoadCSolFile(std::string csol_file_full_path)
{
    ifstream myfile(csol_file_full_path.c_str());

    if (!myfile.is_open())
    {
        throw ConsolMovieException("Couldn't open .scol file");
    }

    string line;
    getline(myfile, line);

    if (line != "V.1.0")
    {
        throw ConsolMovieException("Wrong .csol file version");
    }

    getline(myfile, line);

    std::istringstream iss(line);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
        std::istream_iterator<std::string>());

    if (results.size() != 5)
    {
        throw ConsolMovieException(".scol file has wrong format");
    }

    unsigned short frame_size_x = stoi(results[0]);
    unsigned short frame_size_y = stoi(results[1]);
    int frames = stoi(results[2]);
    bool loop = stoi(results[3]) != 0;
    int interval = stoi(results[4]);

    ConsoleMovie movie({ frame_size_x, frame_size_y }, frames, interval, loop, csol_file_full_path.c_str());
    movie.FillFrames(myfile);

    myfile.close();
    return movie;
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie ConsoleMovie::LoadGifFile(std::string gif_file_full_path)
{
//     Gif::DataStream data_stream = Gif::DataStream::Load(gif_file_full_path.c_str());
//     ConsoleMovie movie(data_stream);
//     movie.m_fileName = gif_file_full_path;
//     return movie;

    ConsoleMovie movie(Gif::GifDecoder::LoadGifFromFile(gif_file_full_path.c_str()));
    return movie;
}

////////////////////////////////////////////////////////////////////////////////
void ConsoleMovie::FillFrames(ifstream& myfile)
{
    string frames_info;
    string line;
    while (getline(myfile, line))
    {
        frames_info += line;
    }

    int frames_info_count = 0;
    for (int i = 0; i < m_frames_count; ++i)
    {
        const FrameInfo& info = GetFrameInfo(i);
        int frame_characters = info.m_size.X * info.m_size.Y;
        for (int j = 0; j < frame_characters; ++j)
        {
            CHAR_INFO& char_info = m_frames[i].first[j];
            char_info.Char.AsciiChar = frames_info[frames_info_count++];
            char_info.Char.UnicodeChar = char_info.Char.AsciiChar;
            char_info.Attributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED;// 7
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
CHAR_INFO* ConsoleMovie::GetFrame(int frame_number) const
{
    if (frame_number >= m_frames_count)
        throw ConsolMovieException("Index out of range. Frame doesn;t exist");

    return m_frames[frame_number].first;
}
////////////////////////////////////////////////////////////////////////////////