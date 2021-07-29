#pragma once
#include <windows.h>
#include <string>

class ConsolMovieException : public std::exception
{
public:
    ConsolMovieException(const char* _info) : info(_info) {} // Wydawa³o mi siê ¿e mo¿na po prostu zakoñczyæ œrednikiem

private:
    std::string info;
};

struct MovieCOORD
{
    unsigned short X;
    unsigned short Y;

    operator COORD()
    {
        return COORD{ (SHORT)X, (SHORT)Y };
    }
};

struct FrameInfo
{
    FrameInfo() :m_size{ 0,0 }, m_coord{ 0,0 } {}
    FrameInfo(unsigned short size_x, unsigned short size_y) :m_size{ size_x, size_y }, m_coord { 0,0 } {}
    FrameInfo(unsigned short size_x, unsigned short size_y, unsigned short coord_x, unsigned short coord_y, short _delay)
        :m_size{ size_x, size_y }, m_coord{ coord_x, coord_y }, m_delay(_delay){}
    MovieCOORD m_size;
    MovieCOORD m_coord;
    int m_delay = 0;
    //bool m_restore_to_backgroundColor = false;
};

class IConsoleMovie
{
public:
    virtual CHAR_INFO* GetFrame(int /*frame_number*/) const = 0;
    virtual FrameInfo& GetFrameInfo(int /*frame_count*/) = 0;
    virtual MovieCOORD GetImageSize() const { return m_imageSize; }
    virtual unsigned short GetBackgroundColor() { return m_backgroundColor; }

    const std::string& GetFileName() const { return m_fileName; }
    virtual int GetFramesCount() const { return m_frames_count; }
    virtual int GetUnitSize() const { return m_unitSize; }

    virtual bool IsMovieLooped() const { return m_loop; }

    virtual ~IConsoleMovie() {}

protected:
    IConsoleMovie(MovieCOORD image_size, int frames_count, bool loop, const char* file_name = "");

    IConsoleMovie(const IConsoleMovie& movie);
    IConsoleMovie(IConsoleMovie&& movie);

    IConsoleMovie& operator=(const IConsoleMovie&);
    IConsoleMovie& operator=(IConsoleMovie&&);

    void Copy(const IConsoleMovie& movie);

    MovieCOORD m_imageSize;
    int m_frames_count = 0;
    int m_unitSize = 7;
    std::string m_fileName;
    bool m_loop = false;
    unsigned short m_backgroundColor = 0;
};