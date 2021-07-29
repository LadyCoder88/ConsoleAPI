#pragma once
#include <string>
#include "IConsoleMovie.h"

class ConsoleMovieProjectorException : public std::exception
{
public:
    ConsoleMovieProjectorException(std::string _info) : info(_info) {} // Wydawa�o mi si� �e mo�na po prostu zako�czy� �rednikiem

private:
    std::string info;
};

class ConsoleMovieProjector
{
public:
    static void Show(IConsoleMovie& movie);

    ConsoleMovieProjector() = delete;
};

