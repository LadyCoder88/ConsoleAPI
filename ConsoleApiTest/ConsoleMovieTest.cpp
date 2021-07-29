#include "ConsoleMovieTest.h"

ConsoleMovieTest::ConsoleMovieTest(unsigned short frame_size_x, unsigned short frame_size_y)
    :IConsoleMovie({ frame_size_x, frame_size_y }, 1, true), m_frameInfo(frame_size_x, frame_size_y, 0, 0, 5000)
{
    m_frames = new CHAR_INFO[frame_size_x * frame_size_y];
}
///////////////////////////////////////////////////////////
ConsoleMovieTest::ConsoleMovieTest(const ConsoleMovieTest& movie)
    :IConsoleMovie(movie), m_frameInfo(movie.m_frameInfo)
{
    int frame_characters = m_frameInfo.m_size.X * m_frameInfo.m_size.Y;
    m_frames = new CHAR_INFO[frame_characters];

    for (int i = 0; i < frame_characters; ++i)
    {
        m_frames[i] = movie.m_frames[i];
    }
}
///////////////////////////////////////////////////////////
ConsoleMovieTest::ConsoleMovieTest(ConsoleMovieTest&& movie)
    :IConsoleMovie(std::move(movie)), m_frameInfo(std::move(movie.m_frameInfo))
{
    m_frames = movie.m_frames;
    movie.m_frames = nullptr;
}
///////////////////////////////////////////////////////////
ConsoleMovieTest& ConsoleMovieTest::operator=(const ConsoleMovieTest& movie)
{
    __super::operator=(movie);

    int frame_characters = m_frameInfo.m_size.X * m_frameInfo.m_size.Y;
    m_frames = new CHAR_INFO[frame_characters];

    for (int i = 0; i < frame_characters; ++i)
    {
        m_frames[i] = movie.m_frames[i];
    }

    return *this;
}
///////////////////////////////////////////////////////////
ConsoleMovieTest& ConsoleMovieTest::operator=(ConsoleMovieTest&& movie)
{
    __super::operator=(std::move(movie));

    m_frames = movie.m_frames;
    movie.m_frames = nullptr;
    movie.m_frameInfo = std::move(movie.m_frameInfo);

    return *this;
}
///////////////////////////////////////////////////////////
ConsoleMovieTest::~ConsoleMovieTest()
{
    delete[] m_frames;
    m_frames = nullptr;
}
///////////////////////////////////////////////////////////
ConsoleMovieTest ConsoleMovieTest::CreateBigMovie()
{
    ConsoleMovieTest movie(120,60);

    const FrameInfo& info = movie.GetFrameInfo(0);
    int frame_characters = info.m_size.X * info.m_size.Y;
    for (int i = 0; i < frame_characters; ++i)
    {
        movie.m_frames[i].Char.UnicodeChar = '*';
        movie.m_frames[i].Char.AsciiChar= '*';
        movie.m_frames[i].Attributes = 7;
    }
      
    return movie;
}
//////////////////////////////////////////////////////////
ConsoleMovieTest ConsoleMovieTest::CreateCleanMovie()
{
    ConsoleMovieTest movie(120, 60);

    const FrameInfo& info = movie.GetFrameInfo(0);
    int frame_characters = info.m_size.X * info.m_size.Y;
    for (int i = 0; i < frame_characters; ++i)
    {
        movie.m_frames[i].Char.UnicodeChar = ' ';
        movie.m_frames[i].Char.AsciiChar = ' ';
        movie.m_frames[i].Attributes = 7;
    }

    return movie;
}
//////////////////////////////////////////////////////////