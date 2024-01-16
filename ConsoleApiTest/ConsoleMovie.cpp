#include "ConsoleMovie.h"

#include <iostream>
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
    std::ofstream file("GifDecoder\\frame_dump.txt");
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
    :IConsoleMovie({gifObject.GetImageWigth(), gifObject.GetImageHeight()}, gifObject.GetFrameCount(), true, gifObject.GetFileName().c_str())
{
    m_frames.reserve(gifObject.GetFrameCount());
    unsigned int image_size = m_imageSize.X * m_imageSize.Y;

    m_backgroundColor = Gif::Color::ConvertTo4Bit(gifObject.GetBackgroundColor().m_RGBColor);

    unsigned short* m_TemporaryBackgroundBuffer = new unsigned short[image_size];
    for (unsigned int i = 0; i < image_size; ++i)
    {
        m_TemporaryBackgroundBuffer[i] = m_backgroundColor;
    }

    for (int i = 0; i < m_frames_count; ++i)
    {
        const Gif::GifObject::Frame& frame = gifObject.GetFrame(i);
        const Gif::GifObject::FrameInfo& frameinfo = frame.GetFrameInfo();

        if (i > 0 && (gifObject.GetFrame(i - 1).GetFrameInfo().GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::RestoreToPrevious ||
                gifObject.GetFrame(i - 1).GetFrameInfo().GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::RestoreToBackgroundColor))
        {
            const Gif::GifObject::Frame& prev_frame = gifObject.GetFrame(i-1);
            const Gif::GifObject::FrameInfo& prev_frameinfo = prev_frame.GetFrameInfo();

            short position_min_x = frameinfo.GetFramePositionX() < prev_frameinfo.GetFramePositionX() ? frameinfo.GetFramePositionX() : prev_frameinfo.GetFramePositionX();
            short position_min_y = frameinfo.GetFramePositionY() < prev_frameinfo.GetFramePositionY() ? frameinfo.GetFramePositionY() : prev_frameinfo.GetFramePositionY();
            
            short diff_pos_x = frameinfo.GetFramePositionX() - prev_frameinfo.GetFramePositionX();
            short diff_pos_y = frameinfo.GetFramePositionY() - prev_frameinfo.GetFramePositionY();

            short position_max_x = (frameinfo.GetFramePositionX() + frameinfo.GetFrameSizeX()) > (prev_frameinfo.GetFramePositionX() + prev_frameinfo.GetFrameSizeX()) 
                ? (frameinfo.GetFramePositionX() + frameinfo.GetFrameSizeX()) : (prev_frameinfo.GetFramePositionX() + prev_frameinfo.GetFrameSizeX());
            short position_max_y = (frameinfo.GetFramePositionY() + frameinfo.GetFrameSizeY()) > (prev_frameinfo.GetFramePositionY() + prev_frameinfo.GetFrameSizeY())
                ? (frameinfo.GetFramePositionY() + frameinfo.GetFrameSizeY()) : (prev_frameinfo.GetFramePositionY() + prev_frameinfo.GetFrameSizeY());

            FrameInfo frame_info(position_max_x - position_min_x,
                position_max_y - position_min_y,
                position_min_x,
                position_min_y,
                frameinfo.GetDelayTime() * 10);

            unsigned int current_image_size = frame_info.m_size.X * frame_info.m_size.Y;
            m_frames.emplace_back(new CHAR_INFO[current_image_size], frame_info);

            int frames_info_count = 0;
            for (int j = 0; j < frame_info.m_size.Y; ++j)
            {
                for (int k = 0; k < frame_info.m_size.X; ++k)
                {
                    int buffer_idx = (j + frame_info.m_coord.Y) * m_imageSize.X + (k + frame_info.m_coord.X);
                    int frame_idx = j * frame_info.m_size.X + k;

                    CHAR_INFO& info = m_frames[i].first[frame_idx];
                    info.Char.AsciiChar = ' ';
                    info.Char.UnicodeChar = info.Char.AsciiChar;
                    info.Attributes = m_TemporaryBackgroundBuffer[buffer_idx];

                    if (((diff_pos_x < 0 && k < frameinfo.GetFrameSizeX()) || (diff_pos_x >= 0 && k >= diff_pos_x && k < diff_pos_x + frameinfo.GetFrameSizeX()))
                        &&
                        ((diff_pos_y < 0 && j < frameinfo.GetFrameSizeY()) || (diff_pos_y >= 0 && j >= diff_pos_y && j < diff_pos_y + frameinfo.GetFrameSizeY())))
                    {
                        const Gif::RGBAColor color = frame.GetData()[frames_info_count++];
                        if (color.m_alpha != 0)
                        {
                            info.Attributes = Gif::Color::ConvertTo4Bit(color.m_RGBColor);
                        }
 
                        if (frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::NoAction ||
                            frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::DoNotDispose)
                        {
                            m_TemporaryBackgroundBuffer[buffer_idx] = info.Attributes;
                        }
                        else if (frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::RestoreToBackgroundColor)
                        {
                            m_TemporaryBackgroundBuffer[buffer_idx] = m_backgroundColor;
                        }
                    }
                }
            }
        }
        else
        {
            FrameInfo frame_info(frameinfo.GetFrameSizeX(),
                frameinfo.GetFrameSizeY(),
                frameinfo.GetFramePositionX(),
                frameinfo.GetFramePositionY(),
                frameinfo.GetDelayTime() * 10);

            unsigned int current_image_size = frame_info.m_size.X * frame_info.m_size.Y;
            m_frames.emplace_back(new CHAR_INFO[current_image_size], frame_info);

            int frames_info_count = 0;
            for (int j = 0; j < frame_info.m_size.Y; ++j)
            {
                for (int k = 0; k < frame_info.m_size.X; ++k)
                {
                    const Gif::RGBAColor color = frame.GetData()[frames_info_count++];

                    int buffer_idx = (j + frame_info.m_coord.Y) * m_imageSize.X + (k + frame_info.m_coord.X);
                    int frame_idx = j * frame_info.m_size.X + k;

                    CHAR_INFO& info = m_frames[i].first[frame_idx];
                    info.Char.AsciiChar = ' ';
                    info.Char.UnicodeChar = info.Char.AsciiChar;
                    info.Attributes = m_TemporaryBackgroundBuffer[buffer_idx];

                    if (color.m_alpha != 0)
                    {
                        info.Attributes = Gif::Color::ConvertTo4Bit(color.m_RGBColor);// 7
                    }

                    if (frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::NoAction ||
                        frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::DoNotDispose)
                    {
                        m_TemporaryBackgroundBuffer[buffer_idx] = info.Attributes;
                    }
                    else if (frameinfo.GetDisposeMethod() == Gif::GraphicControlExtension::EDisposeMethod::RestoreToBackgroundColor)
                    {
                        m_TemporaryBackgroundBuffer[buffer_idx] = m_backgroundColor;
                    }
                }
            }
        }

        //FrameDump(ptr, frame_characters, m_frame_size_x);
    }

    m_unitSize = 1;
}

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
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie::ConsoleMovie(ConsoleMovie&& movie): IConsoleMovie(std::move(movie))
{
    m_frames = std::move(movie.m_frames);
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
    return *this;
}
////////////////////////////////////////////////////////////////////////////////
ConsoleMovie& ConsoleMovie::operator=(ConsoleMovie&& movie)
{
    __super::operator=(std::move(movie));

    m_frames = std::move(movie.m_frames);

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
    std::ifstream myfile(csol_file_full_path.c_str());

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

    short frame_size_x = stoi(results[0]);
    short frame_size_y = stoi(results[1]);
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
    ConsoleMovie movie(Gif::GifDecoder::LoadGifFromFile(gif_file_full_path.c_str()));
    return movie;
}

////////////////////////////////////////////////////////////////////////////////
void ConsoleMovie::FillFrames(std::ifstream& myfile)
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