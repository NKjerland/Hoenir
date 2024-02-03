#include "Functions.h"

template<class Type>
const Type Functions::clamp(const Type& in_value, const Type& in_low, const Type& in_high)
{
    return
        (in_value >= in_low && in_value <= in_high) * in_value +
        (in_value < in_low) * in_low +
        (in_value > in_high) * in_high;
}

template const int Functions::clamp(const int& in_value, const int& in_low, const int& in_high);

ull Functions::LS1B(ull in_ull)
{
    return in_ull & (~in_ull + 1);
}

bool Functions::IsNumber(char in_character)
{
    return in_character >= '0' && in_character <= '9';
}

bool Functions::IsPiece(char in_character)
{
    for (char test_character : piece_chars)
    {
        if (test_character == in_character)
        {
            return true;
        }
    }
    return false;
}

int Functions::CharToInt(char in_character)
{
    return in_character - '0';
}

char Functions::IntToChar(int in_int)
{
    return '0' + in_int;
}

byte Functions::CharToPiece(char in_character)
{
    return char_to_piece[in_character];
}

char Functions::PieceToChar(byte in_piece)
{
    return piece_to_char[in_piece];
}

ull Functions::CharSquareToUll(char in_x, char in_y)
{
    int x = in_x - 'a';
    int y = in_y - '0';

    return 1ULL << (y * 8 + x);
}

bool Functions::ContainsOnlyDigits(std::string in_string)
{
    for (char character : in_string)
    {
        if (!IsNumber(character))
        {
            return false;
        }
    }
    return true;
}

