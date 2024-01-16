#include "ConsoleMovieProjector.h"
#include <windows.h>
#include <stdio.h>

COORD CalculateWindowSize(HANDLE handle, const COORD& image_size)
{
    CONSOLE_SCREEN_BUFFER_INFOEX csbiInfo;
    csbiInfo.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

    if (!GetConsoleScreenBufferInfoEx(handle, &csbiInfo))
    {
        throw ConsoleMovieProjectorException("GetConsoleScreenBufferInfo : " + std::to_string(GetLastError()));
    }

    COORD max_window_size = GetLargestConsoleWindowSize(handle);
    if (max_window_size.X == 0 && max_window_size.Y == 0 )
    {
        throw ConsoleMovieProjectorException("GetLargestConsoleWindowSize : " + std::to_string(GetLastError()));
    }

    COORD current_window_size;
    current_window_size.X = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left + 1;
    current_window_size.Y = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top + 1;

    if (current_window_size.X > image_size.X && current_window_size.Y > image_size.Y)
        return current_window_size;

    COORD largest_size;
    largest_size.X = min(image_size.X, max_window_size.X);
    largest_size.Y = min(image_size.Y, max_window_size.Y);

    csbiInfo.dwSize.X = largest_size.X;
    csbiInfo.dwSize.Y = largest_size.Y;

    csbiInfo.srWindow.Top = 0;
    csbiInfo.srWindow.Bottom = largest_size.Y - 1;
    csbiInfo.srWindow.Left = 0;
    csbiInfo.srWindow.Right = largest_size.X - 1;

    if (!SetConsoleScreenBufferInfoEx(handle, &csbiInfo))
    {
        throw ConsoleMovieProjectorException("SetConsoleScreenBufferInfoEx : " + std::to_string(GetLastError()));
    }

    if (!SetConsoleWindowInfo(handle, TRUE, &csbiInfo.srWindow))
    {
        throw ConsoleMovieProjectorException("SetConsoleScreenBufferInfoEx : " + std::to_string(GetLastError()));
    }

    return largest_size;
//     int min_buffor_size_x = GetSystemMetrics(SM_CXMIN);
//     int min_buffor_size_y = GetSystemMetrics(SM_CYMIN);
}

void CalculateiImageMiddlePosition(COORD& coordBufSize, SMALL_RECT& srctWriteRect, const COORD& window_size, const COORD& image_size)
{
    coordBufSize.Y = image_size.Y > window_size.Y? window_size.Y : image_size.Y;
    coordBufSize.X = image_size.X > window_size.X ? window_size.X : image_size.X;

    srctWriteRect.Top = (window_size.Y / 2) - (image_size.Y / 2);
    srctWriteRect.Left = (window_size.X / 2) - (image_size.X / 2);
    srctWriteRect.Bottom = (window_size.Y / 2) + (image_size.Y / 2) - 1;
    srctWriteRect.Right = (window_size.X / 2) + (image_size.X / 2) - 1;

    if (srctWriteRect.Top < 0)
        srctWriteRect.Top = 0;
    if (srctWriteRect.Left < 0)
        srctWriteRect.Left = 0;
    if (srctWriteRect.Bottom > window_size.Y)
        srctWriteRect.Bottom = window_size.Y;
    if (srctWriteRect.Right > window_size.X)
        srctWriteRect.Right = window_size.X;
}

void ConsoleMovieProjector::Show(IConsoleMovie& movie)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hNewScreenBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        /*FILE_SHARE_READ |*/ FILE_SHARE_WRITE,
        NULL, //default security attributes
        CONSOLE_TEXTMODE_BUFFER, //must be TEXTMODE
        NULL); //reserved; must be unll

    if (hStdout == INVALID_HANDLE_VALUE || hNewScreenBuffer == INVALID_HANDLE_VALUE)
    {
        throw ConsoleMovieProjectorException("Console creation screen buffer failed : " + std::to_string(GetLastError()));
    }

    //Make the new screen buffer the active screen buffer

    if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer))
    {
        throw ConsoleMovieProjectorException("Set console active screen buffer failed : " + std::to_string(GetLastError()));
    }

    CONSOLE_FONT_INFOEX font_info;
    font_info.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    if (!GetCurrentConsoleFontEx(hNewScreenBuffer, FALSE, &font_info))
    {
        throw ConsoleMovieProjectorException("GetCurrentConsoleFontEx : " + std::to_string(GetLastError()));
    }
    font_info.dwFontSize.Y = 1;
    font_info.dwFontSize.X = 1;
    if (!SetCurrentConsoleFontEx(hNewScreenBuffer, FALSE, &font_info))
    {
        throw ConsoleMovieProjectorException("SetCurrentConsoleFontEx : " + std::to_string(GetLastError()));
    }

    WCHAR* str3 = new WCHAR[movie.GetFileName().size() + 1];
    MultiByteToWideChar(0, 0, movie.GetFileName().c_str(), movie.GetFileName().size() + 1, str3, movie.GetFileName().size() + 1);

    if (!SetConsoleTitle(str3))
    {
        throw ConsoleMovieProjectorException("SetConsoleTitle : " + std::to_string(GetLastError()));
    }
    delete str3;

    COORD image_size = movie.GetImageSize();
    COORD window_size = CalculateWindowSize(hNewScreenBuffer, image_size);

    COORD coordBufSize;
    SMALL_RECT srctWriteRect;

    CalculateiImageMiddlePosition(coordBufSize, srctWriteRect, window_size, image_size);
    
    //The top left  destination cell of the temporary buffer is row 0, col 0
    COORD coordBufCoord;
    coordBufCoord.X = 0;
    coordBufCoord.Y = 0;

    do
    {
        //To do
        //react to window resize
        for (int i = 0; i < movie.GetFramesCount(); ++i)
        {
            FrameInfo& frame_info = movie.GetFrameInfo(i);
            SMALL_RECT dstWriteRect;
            dstWriteRect.Left = frame_info.m_coord.X;
            dstWriteRect.Top = frame_info.m_coord.Y;
            dstWriteRect.Right = frame_info.m_coord.X + frame_info.m_size.X;
            dstWriteRect.Bottom = frame_info.m_coord.Y + frame_info.m_size.Y;

            COORD scrBufCoord{ 0, 0 };
            COORD scrBufSize{ frame_info.m_size.X, frame_info.m_size.Y };

            BOOL fSuccess = WriteConsoleOutput(
                hNewScreenBuffer, //Screen buffer to write
                movie.GetFrame(i), //buffer to copy from
                scrBufSize, //col-row size of buffer
                scrBufCoord, //top left src cell in buffer
                &dstWriteRect);

            if (!fSuccess)
            {
                throw ConsoleMovieProjectorException("Write console output failed. Frame : " + std::to_string(i));
            }

            Sleep(frame_info.m_delay);
        }
    } while (movie.IsMovieLooped());

    if (!SetConsoleActiveScreenBuffer(hStdout))
    {
        throw ConsoleMovieProjectorException("Set console active screen buffer : " + std::to_string(GetLastError()));
    }
}