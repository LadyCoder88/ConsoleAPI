#pragma once
#include <string>
#include "IConsoleMovie.h"

class ConsoleMovieProjectorException : public std::exception
{
public:
    ConsoleMovieProjectorException(std::string _info) : info(_info) {} // Wydawa³o mi siê ¿e mo¿na po prostu zakoñczyæ œrednikiem

private:
    std::string info;
};

class ConsoleMovieProjector
{
public:
    static void Show(IConsoleMovie& movie);

    ConsoleMovieProjector() = delete;
};

