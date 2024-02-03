#ifndef FUNCTIONS
#define FUNCTIONS

#include<string>
#include<vector>
#include<sstream>
#include<map>

#include"Position.h"
#include"CommonTypes.h"

namespace Functions
{

    const char piece_chars[12] = { 'p','n','b','r','q','k','P','N','B','R','Q','K' };

    std::map<char, byte> char_to_piece = {
        {'p', 0},
        {'n', 1},
        {'b', 2},
        {'r', 3},
        {'q', 4},
        {'k', 5},
        {'P', 6},
        {'N', 7},
        {'B', 8},
        {'R', 9},
        {'Q', 10},
        {'K', 11}
    };

    std::map<byte, char> piece_to_char{
        {0, 'p'},
        {1, 'n'},
        {2, 'b'},
        {3, 'r'},
        {4, 'q'},
        {5, 'k'},
        {6, 'P'},
        {7, 'N'},
        {8, 'B'},
        {9, 'R'},
        {10, 'Q'},
        {11, 'K'},
        {Position::EMPTY_PIECE, ' '}
    };

    template<typename Type>
    const Type clamp(const Type& in_value, const Type& in_low, const Type& in_high);

    std::vector<std::string>SplitString(const std::string& in_string)
    {
        std::vector<std::string> tokens;
        std::istringstream string_stream(in_string);

        std::string token;

        while (string_stream >> std::noskipws >> token)
        {
            tokens.push_back(token);
            char next_char;
            if (string_stream >> next_char && std::isspace(next_char))
            {
                while (std::isspace(string_stream.peek()))
                {
                    string_stream.ignore();
                }
            }
        }
        return tokens;
    }

    // LS1B returns a Ull containing only the lowest active bit of x
    inline ull LS1B(ull in_ull);

    bool IsNumber(char in_character);
    bool IsPiece(char in_character);
    int CharToInt(char in_character);
    char IntToChar(int in_int);
    byte CharToPiece(char in_character);
    char PieceToChar(byte in_piece);
    ull CharSquareToUll(char in_x, char in_y);
    bool ContainsOnlyDigits(std::string in_string);
};

#endif FUNCTIONS

