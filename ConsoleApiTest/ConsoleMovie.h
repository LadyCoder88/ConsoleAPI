#pragma once
#include <string>
#include "IConsoleMovie.h"
#include <vector>
#include "../GifDecoder/GifObject.h"

class ConsoleMovie : public IConsoleMovie
{
public:
    static ConsoleMovie LoadConsoleMovie(std::string csol_file_full_path);
    
    CHAR_INFO* GetFrame(int frame_number) const override;
    FrameInfo& GetFrameInfo(int frame_number) override;

    bool PrepareBackground() const override { return true; }
    CHAR_INFO* GetBackgroundFrame() const override { return m_background; }

    ConsoleMovie(const ConsoleMovie& movie);
    ConsoleMovie(ConsoleMovie&& movie);

    ConsoleMovie& operator=(const ConsoleMovie&);
    ConsoleMovie& operator=(ConsoleMovie&&);

    ~ConsoleMovie();

private:
    ConsoleMovie(MovieCOORD image_size, int frames_count, int interval, bool loop, const char* fileName);
    //ConsoleMovie(Gif::DataStream& data_stream);
    ConsoleMovie(const Gif::GifObject& gifObject);

    void FillFrames(std::ifstream& myfile);

    std::vector<std::pair<CHAR_INFO*, FrameInfo>> m_frames;
    CHAR_INFO* m_background;

    static ConsoleMovie LoadCSolFile(std::string csol_file_full_path);
    static ConsoleMovie LoadGifFile(std::string gif_file_full_path);
};

