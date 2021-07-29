#pragma once

#include "IConsoleMovie.h"

IConsoleMovie::IConsoleMovie(MovieCOORD image_size, int frames_count, bool loop, const char* file_name)
    : m_frames_count(frames_count),
    m_loop(loop),
    m_fileName(file_name),
    m_imageSize(image_size)
{
}

//////////////////////////////////////////////////////////////////////////
IConsoleMovie::IConsoleMovie(const IConsoleMovie& movie)
{
    Copy(movie);
}

////////////////////////////////////////////////////////////////////////////////
IConsoleMovie& IConsoleMovie::operator=(const IConsoleMovie& movie)
{
    Copy(movie);
    return *this;
}
////////////////////////////////////////////////////////////////////////////////
IConsoleMovie::IConsoleMovie(IConsoleMovie&& movie)
{
    m_imageSize = movie.m_imageSize;
    m_frames_count = movie.m_frames_count;
    m_loop = movie.m_loop;
    m_unitSize = movie.m_unitSize;
    m_fileName = movie.m_fileName;
    m_backgroundColor = movie.m_backgroundColor;
}
////////////////////////////////////////////////////////////////////////////////
IConsoleMovie& IConsoleMovie::operator=(IConsoleMovie&& movie)
{
    m_imageSize = movie.m_imageSize;
    m_frames_count = movie.m_frames_count;
    m_loop = movie.m_loop;
    m_unitSize = movie.m_unitSize;
    m_fileName = movie.m_fileName;
    m_backgroundColor = movie.m_backgroundColor;

    return *this;
}
////////////////////////////////////////////////////////////////////////////////
void IConsoleMovie::Copy(const IConsoleMovie& movie)
{
    m_imageSize = movie.m_imageSize;
    m_frames_count = movie.m_frames_count;
    m_loop = movie.m_loop;
    m_unitSize = movie.m_unitSize;
    m_fileName = movie.m_fileName;
    m_backgroundColor = movie.m_backgroundColor;
}
//////////////////////////////////////////////////////////////////////////