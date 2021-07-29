#pragma once
#include "IConsoleMovie.h"
class ConsoleMovieTest : public IConsoleMovie
{
public:
    static ConsoleMovieTest CreateBigMovie();
    static ConsoleMovieTest CreateCleanMovie();

    virtual CHAR_INFO* GetFrame(int frame_number) const override { return m_frames; }
    virtual FrameInfo& GetFrameInfo(int /*frame_number*/) override { return m_frameInfo; }
    ConsoleMovieTest(const ConsoleMovieTest& movie);
    ConsoleMovieTest(ConsoleMovieTest&& movie);

    ConsoleMovieTest& operator=(const ConsoleMovieTest&);
    ConsoleMovieTest& operator=(ConsoleMovieTest&&);

    ~ConsoleMovieTest();

private:
    ConsoleMovieTest(unsigned short frame_size_x, unsigned short frame_size_y);

    CHAR_INFO* m_frames = nullptr;
    FrameInfo m_frameInfo;
};

