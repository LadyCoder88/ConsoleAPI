// ConsoleApiTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include "ConsoleMovie.h"
#include "ConsoleMovieProjector.h"
#include "ConsoleMovieTest.h"

int main()
{
    /*
    std::cout << "----------------------------------Hello World!----------------------------------\n";
    std::cout << "01234567890123456789012345678901234567890123456789012345678901234567890123456789\n";
    std::cout << "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n";
    std::cout << "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU\n";
    std::cout << "BLSLDKGADFKGDKFHLGKODKFGOKDSOHGKODFGKDFODKFGOKDFOGKODJFSOFOSDKFOKDOIFODIFIOMVDJS\n";

    HANDLE hStdout, hNewScreenBuffer;
    SMALL_RECT srctReadRect;
    SMALL_RECT srctWriteRect;
    CHAR_INFO* chibuffer = new CHAR_INFO[160]; // [2][80]
    COORD coordBufSize;
    COORD coordBufCoord;
    BOOL fSuccess;

    // Get handle to the STDOUT screen buffer to copy from and
    // create a new screen buffer to copy to

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hNewScreenBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, //default security attributes
        CONSOLE_TEXTMODE_BUFFER, //must be TEXTMODE
        NULL); //reserved; must be unll

    if (hStdout == INVALID_HANDLE_VALUE || hNewScreenBuffer == INVALID_HANDLE_VALUE)
    {
        printf("Console creation screen buffer failed - (%d)\n", GetLastError());
        return 1;
    }

    //Make the new screen buffer the active screen buffer

    if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer))
    {
        printf("Set console active screen buffer failed - (%d)\n", GetLastError());
        return 1;
    }

    // Set the source rectangle
    srctReadRect.Top = 0; // top left: row 0, col 0
    srctReadRect.Left = 0;
    srctReadRect.Bottom = 1; // bot. right: row 1, col 79
    srctReadRect.Right = 79;

    //The temporary buffer size is 2 rows x 80 columns

    coordBufSize.Y = 2;
    coordBufSize.X = 80;

    //The top left  destination cell of the temporary buffer is row 0, col 0
    coordBufCoord.X = 0;
    coordBufCoord.Y = 0;

    //Copy the block from the screen buffer to the temp. buffer
    fSuccess = ReadConsoleOutput(
        hStdout, //Stream buffer to read from
        chibuffer, //buffer to copy to
        coordBufSize, //col-row size of chibuffer
        coordBufCoord, //top left dest. cell in chiBuffer
        &srctReadRect); // screen buffer source rectangle

    if (!fSuccess)
    {
        printf("Read console output failed - (%d)\n", GetLastError());
        return 1;
    }

    CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
    if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    {
        printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
        return 0;
    }
    */

    //Set destination rectangle
    /*srctWriteRect.Top = 20; //Top lt: row 10, col 1
    srctWriteRect.Left = 0;
    srctWriteRect.Bottom = 21; //bot. rt: rows 11, col 79
    srctWriteRect.Right = 79;*/

    /*
    int window_size_x = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left;
    int window_size_y = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top;

    srctWriteRect.Top = (window_size_y / 2); //Top lt: row 10, col 1
    srctWriteRect.Left = (window_size_x / 2) - 40;
    srctWriteRect.Bottom = srctWriteRect.Top + 1; //bot. rt: rows 11, col 79
    srctWriteRect.Right = srctWriteRect.Left + 79;

    //Copy from the temporary buffer to the new screen buffer

    chibuffer[0].Attributes = FOREGROUND_RED;
    chibuffer[1].Attributes = FOREGROUND_RED;
    chibuffer[2].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
    chibuffer[3].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
    chibuffer[4].Attributes = FOREGROUND_GREEN;
    chibuffer[5].Attributes = FOREGROUND_GREEN;
    chibuffer[6].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    chibuffer[7].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    chibuffer[8].Attributes = FOREGROUND_BLUE;
    chibuffer[9].Attributes = FOREGROUND_BLUE;
    chibuffer[10].Attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    chibuffer[11].Attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    chibuffer[12].Attributes = FOREGROUND_BLUE | FOREGROUND_RED;
    chibuffer[13].Attributes = FOREGROUND_BLUE | FOREGROUND_RED;
    chibuffer[14].Attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
    chibuffer[15].Attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
    chibuffer[16].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN;
    chibuffer[17].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN;
    chibuffer[18].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    chibuffer[19].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    chibuffer[20].Attributes = FOREGROUND_RED | FOREGROUND_GREEN;
    chibuffer[21].Attributes = FOREGROUND_RED | FOREGROUND_GREEN;
    chibuffer[22].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    chibuffer[23].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    chibuffer[24].Attributes = BACKGROUND_RED;
    chibuffer[25].Attributes = BACKGROUND_RED;
    chibuffer[26].Attributes = BACKGROUND_RED | BACKGROUND_INTENSITY;
    chibuffer[27].Attributes = BACKGROUND_RED | BACKGROUND_INTENSITY;
    chibuffer[28].Attributes = BACKGROUND_GREEN;
    chibuffer[29].Attributes = BACKGROUND_GREEN;
    chibuffer[30].Attributes = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    chibuffer[31].Attributes = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    chibuffer[32].Attributes = BACKGROUND_BLUE;
    chibuffer[33].Attributes = BACKGROUND_BLUE;
    chibuffer[34].Attributes = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
    chibuffer[35].Attributes = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
    chibuffer[36].Attributes = BACKGROUND_BLUE | BACKGROUND_RED;
    chibuffer[37].Attributes = BACKGROUND_BLUE | BACKGROUND_RED;
    chibuffer[38].Attributes = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
    chibuffer[39].Attributes = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
    chibuffer[40].Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN;
    chibuffer[41].Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN;
    chibuffer[42].Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    chibuffer[43].Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    chibuffer[44].Attributes = BACKGROUND_RED | BACKGROUND_GREEN;
    chibuffer[45].Attributes = BACKGROUND_RED | BACKGROUND_GREEN;
    chibuffer[46].Attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    chibuffer[47].Attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;

    fSuccess = WriteConsoleOutput(
        hNewScreenBuffer, //Screen buffer to write
        chibuffer, //buffer to copy from
        coordBufSize, //col-row size of chiBuffer
        coordBufCoord, //top left src cell in chiBuffer
        &srctWriteRect);

    if (!fSuccess)
    {
        printf("Write console output failed - (%d)\n", GetLastError());
        return 1;
    }

    Sleep(50000);
    */
    /*
    FOREGROUND_BLUE 0x0001
    Text color contains blue.

    FOREGROUND_GREEN 0x0002
    Text color contains green.

    FOREGROUND_RED 0x0004
    Text color contains red.

    FOREGROUND_INTENSITY 0x0008
    Text color is intensified.

    BACKGROUND_BLUE 0x0010
    Background color contains blue.

    BACKGROUND_GREEN 0x0020
    Background color contains green.

    BACKGROUND_RED 0x0040
    Background color contains red.

    BACKGROUND_INTENSITY 0x0080
    Background color is intensified.

    COMMON_LVB_LEADING_BYTE 0x0100
    Leading byte.

    COMMON_LVB_TRAILING_BYTE 0x0200
    Trailing byte.

    COMMON_LVB_GRID_HORIZONTAL 0x0400
    Top horizontal

    COMMON_LVB_GRID_LVERTICAL 0x0800
    Left vertical.

    COMMON_LVB_GRID_RVERTICAL 0x1000
    Right vertical.

    COMMON_LVB_REVERSE_VIDEO 0x4000
    Reverse foreground and background attribute.

    COMMON_LVB_UNDERSCORE 0x8000
    Underscore.
    */

/*
    for (int i = 0; i < 160; ++i)
    {
        chibuffer[i].Char.AsciiChar = '*';
        chibuffer[i].Attributes = FOREGROUND_RED;

        fSuccess = WriteConsoleOutput(
            hNewScreenBuffer, //Screen buffer to write
            chibuffer, //buffer to copy from
            coordBufSize, //col-row size of chiBuffer
            coordBufCoord, //top left src cell in chiBuffer
            &srctWriteRect);

        if (!fSuccess)
        {
            printf("Write console output failed - (%d)\n", GetLastError());
            return 1;
        }

        Sleep(100);
    }

    
    if (!SetConsoleActiveScreenBuffer(hStdout))
    {
        printf("Set console active screen buffer - (%d)\n", GetLastError());
        return 1;
    }

    delete[] chibuffer;

    */

    try
    {
        //ConsoleMovie movie = ConsoleMovie::LoadConsoleMovie("D:\\Programy\\Other\\ConsoleApiTest\\Data\\GifSample.gif");
        //ConsoleMovie movie = ConsoleMovie::LoadConsoleMovie("D:\\Programy\\Other\\ConsoleApiTest\\Data\\emot_icon.gif");
        //ConsoleMovie movie = ConsoleMovie::LoadConsoleMovie("D:\\Programy\\Other\\ConsoleApiTest\\Data\\miziu.gif");
        //ConsoleMovie movie = ConsoleMovie::LoadConsoleMovie("D:\\Programy\\Other\\ConsoleApiTest\\Data\\movie.csol");
        ConsoleMovie movie = ConsoleMovie::LoadConsoleMovie("D:\\Programy\\Other\\ConsoleApiTest\\Data\\pikaczu.gif");
        //ConsoleMovieTest movie_test = ConsoleMovieTest::CreateBigMovie();

        ConsoleMovieProjector::Show(movie);
        //ConsoleMovieProjector::Show(movie_test);
    }
    catch (ConsoleMovieProjectorException e)
    {
    }
    

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
