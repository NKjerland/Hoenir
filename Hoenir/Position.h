#ifndef POSITION
#define POSITION

#include "Functions.h"
#include "CommonTypes.h"
#include "Rays.h"
#include "MoveStack.h"
#include "Move.h"

#include<string>
#include<vector>

/*
* TODO:
* WRITE:
* UndoMove(Move* in_move)
* CountMoves();
* GenerateMovesQuiescence();
* FoolsCountMoves();
* FoolsGenerateMoves();
* FoolsGenerateMovesQuiescence();
*/

class Position
{

	// Variables

public:

	static const int piece_type_count = 12;
	static const int board_square_count = 64;

	// Declared in Position.cpp
	static const std::string STANDARD_FEN;

	static const ull CASTLE_RIGHT_WHITE_SHORT = 1ULL;
	static const ull CASTLE_RIGHT_WHITE_LONG = 2ULL;
	static const ull CASTLE_RIGHT_BLACK_SHORT = 3ULL;
	static const ull CASTLE_RIGHT_BLACK_LONG = 4ULL;

	static const ull WHITE_FIRST_RANK = 0xFF;
	static const ull BLACK_FIRST_RANK = 0xFF00000000000000;
	static const ull BACK_RANK = WHITE_FIRST_RANK | BLACK_FIRST_RANK;
	static const ull WHITE_SECOND_RANK = 0xFF00;
	static const ull BLACK_SECOND_RANK = 0xFF000000000000;
	static const ull BACK_SECOND_RANK = WHITE_SECOND_RANK | BLACK_SECOND_RANK;

	static const ull WHITE_KING_START = 0x0000000000000010;
	static const ull WHITE_LONG_CASTLE_KING_END = 0x0000000000000004;
	static const ull WHITE_SHORT_CASTLE_KING_END = 0x0000000000000040;
	static const ull WHITE_LONG_CASTLE_ROOK_START = 0x0000000000000001;
	static const ull WHITE_LONG_CASTLE_ROOK_END = 0x0000000000000008;
	static const ull WHITE_LONG_CASTLE_EMPTY = 0x000000000000000E;
	static const ull WHITE_SHORT_CASTLE_ROOK_START = 0x0000000000000080;
	static const ull WHITE_SHORT_CASTLE_ROOK_END = 0x0000000000000020;
	static const ull WHITE_SHORT_CASTLE_EMPTY = 0x0000000000000060;

	static const ull BLACK_KING_START = 0x1000000000000000;
	static const ull BLACK_LONG_CASTLE_KING_END = 0x0400000000000000;
	static const ull BLACK_SHORT_CASTLE_KING_END = 0x4000000000000000;
	static const ull BLACK_LONG_CASTLE_ROOK_START = 0x0100000000000000;
	static const ull BLACK_LONG_CASTLE_ROOK_END = 0x0800000000000000;
	static const ull BLACK_LONG_CASTLE_EMPTY = 0x0E00000000000000;
	static const ull BLACK_SHORT_CASTLE_ROOK_START = 0x8000000000000000;
	static const ull BLACK_SHORT_CASTLE_ROOK_END = 0x2000000000000000;
	static const ull BLACK_SHORT_CASTLE_EMPTY = 0x6000000000000000;

	static const ull VALID_WHITE_PASSANT = 0xFF0000;
	static const ull VALID_BLACK_PASSANT = 0xFF0000000000;
	static const ull VALID_PASSANT = VALID_WHITE_PASSANT | VALID_BLACK_PASSANT;

	static const ull AVOID_UP = 0x00FFFFFFFFFFFFFF;
	static const ull AVOID_DOWN = 0xFFFFFFFFFFFFFF00;
	static const ull AVOID_LEFT = 0xFEFEFEFEFEFEFEFE;
	static const ull AVOID_RIGHT = 0x7F7F7F7F7F7F7F7F;

	static const ull AVOID_UP_UP = 0x0000FFFFFFFFFFFF;
	static const ull AVOID_DOWN_DOWN = 0xFFFFFFFFFFFF0000;
	static const ull AVOID_LEFT_LEFT = 0xFCFCFCFCFCFCFCFC;
	static const ull AVOID_RIGHT_RIGHT = 0x3F3F3F3F3F3F3F3F;

	static const ull AVOID_UP_LEFT = AVOID_UP & AVOID_LEFT;
	static const ull AVOID_UP_RIGHT = AVOID_UP & AVOID_RIGHT;
	static const ull AVOID_DOWN_LEFT = AVOID_DOWN & AVOID_LEFT;
	static const ull AVOID_DOWN_RIGHT = AVOID_DOWN & AVOID_RIGHT;

	static const ull AVOID_UP_UP_LEFT = AVOID_UP_UP & AVOID_LEFT;
	static const ull AVOID_UP_UP_RIGHT = AVOID_UP_UP & AVOID_RIGHT;
	static const ull AVOID_LEFT_LEFT_UP = AVOID_LEFT_LEFT & AVOID_UP;
	static const ull AVOID_RIGHT_RIGHT_UP = AVOID_RIGHT_RIGHT & AVOID_UP;
	static const ull AVOID_LEFT_LEFT_DOWN = AVOID_LEFT_LEFT & AVOID_DOWN;
	static const ull AVOID_RIGHT_RIGHT_DOWN = AVOID_RIGHT_RIGHT & AVOID_DOWN;
	static const ull AVOID_DOWN_DOWN_LEFT = AVOID_DOWN_DOWN & AVOID_LEFT;
	static const ull AVOID_DOWN_DOWN_RIGHT = AVOID_DOWN_DOWN & AVOID_RIGHT;

	static const ull LEAST_SIG_BITS = 0xF;
	static const ull MOST_SIG_BITS = 0xF0;

	static const ull PASSANT_TARGET = 0xFF0000FF0000;

	const int WHITE_PIECE = 6;

	const enum PIECE_TYPE
	{
		BLACK_PAWN,
		BLACK_KNIGHT,
		BLACK_BISHOP,
		BLACK_ROOK,
		BLACK_QUEEN,
		BLACK_KING,
		WHITE_PAWN,
		WHITE_KNIGHT,
		WHITE_BISHOP,
		WHITE_ROOK,
		WHITE_QUEEN,
		WHITE_KING,
		EMPTY_PIECE = 0xF

	};

	const enum PIECE_TYPE_COLORLESS
	{
		PAWN,
		KNIGHT,
		BISHOP,
		ROOK,
		QUEEN,
		KING
	};

	const int index64l[64] = {
		0,  1, 48,  2, 57, 49, 28,  3,
		61, 58, 50, 42, 38, 29, 17,  4,
		62, 55, 59, 36, 53, 51, 43, 22,
		45, 39, 33, 30, 24, 18, 12,  5,
		63, 47, 56, 27, 60, 41, 37, 16,
		54, 35, 52, 21, 44, 32, 23, 11,
		46, 26, 40, 15, 34, 20, 31, 10,
		25, 14, 19,  9, 13,  8,  7,  6
	};

	const int index64h[64] = {
		0, 47,  1, 56, 48, 27,  2, 60,
	   57, 49, 41, 37, 28, 16,  3, 61,
	   54, 58, 35, 52, 50, 42, 21, 44,
	   38, 32, 29, 23, 17, 11,  4, 62,
	   46, 55, 26, 59, 40, 36, 15, 53,
	   34, 51, 20, 43, 31, 22, 10, 45,
	   25, 39, 14, 33, 19, 30,  9, 24,
	   13, 18,  8, 12,  7,  6,  5, 63
	};

protected:

	ull pieces_bitboard[piece_type_count];
	byte pieces_byte[board_square_count];
	int pieces_count[board_square_count];
	byte castle_rights;
	ull passant_target;
	bool white_turn;
	int half_moves;
	int full_moves;

	// Functions

public:

	Position();
	Position(const Position& in_position);
	Position(std::string in_fen);

	int CountMoves();
	int GenerateMoves(MoveStack* in_movestack);
	int GenerateMovesQuiescence(MoveStack* in_movestack);

	int FoolsCountMoves();
	int FoolsGenerateMoves(MoveStack* in_movestack);
	int FoolsGenerateMovesQuiescence(MoveStack* in_movestack);

	void MakeMove(Move* in_move);
	void UndoMove(Move* in_move);

	ull GetPiecesBitboard(int piece_type) const;
	byte GetPiecesByte(int in_square) const;
	int GetPiecesCount(int in_square) const;
	byte GetCastleRights() const;
	ull GetPassantTarget() const;
	bool GetWhiteTurn() const;
	int GetHalfMoves() const;
	int GetFullMoves() const;

	std::string ToString();
	std::string ToFen();

	Position& operator=(const Position& in_position);

protected:

	inline byte GetPiece(ull in_square);
	inline void SetPiece(int in_piece, ull in_square);
	inline void RemovePiece(ull in_square);

	void AddMove(
		MoveStack* in_movestack,
		int& in_move_count,
		byte in_piece_type,
		int in_player_index,
		ull in_occupied,
		ull in_vertical_pin,
		ull in_horizontal_pins,
		ull in_leading_pins,
		ull in_following_pins,
		ull in_legal_move_to,
		ull in_player_pieces,
		ull in_passant_pin
	);

	inline ull Occupied();
	inline ull GetPlayerPieces(int player_index);
	inline void GetPins(ull& in_vertical_pins, ull& in_horizontal_pins, ull& in_leading_pins, ull& in_following_pins);
	inline void GetPassantPin(ull& in_passant_pin);
	inline void GetBlockSquares(
		ull& vertical_block_squares,
		ull& horizontal_block_squares,
		ull& leading_block_squares,
		ull& following_block_squares,
		ull& block_squares
	);

	void LoadFen(std::string in_fen);
	void LoadFenPieces(std::string in_fen);
	void LoadFenTurn(std::string in_fen);
	void LoadFenCastleRights(std::string in_fen);
	void LoadFenPassantTarget(std::string in_fen);
	void LoadFenHalfMoves(std::string in_fen);
	void LoadFenFullMoves(std::string in_fen);

	std::string ToFenPieces();
	std::string ToFenTurn();
	std::string ToFenCastleRights();
	std::string ToFenPassantTarget();
	std::string ToFenHalfMoves();
	std::string ToFenFullMoves();

	inline ull RemoveBit(ull in_ull);
	inline ull LowestSignificantBit(ull in_ull);

	inline int BSLfast(ull in_ull);
	inline int BSHfast(ull in_ull);

	inline ull UpShift(ull in_ull);
	inline ull DownShift(ull in_ull);
	inline ull LeftShift(ull in_ull);
	inline ull RightShift(ull in_ull);

	inline ull UpUpShift(ull in_ull);
	inline ull DownDownShift(ull in_ull);

	inline ull UpLeftShift(ull in_ull);
	inline ull UpRightShift(ull in_ull);
	inline ull DownLeftShift(ull in_ull);
	inline ull DownRightShift(ull in_ull);

	inline ull UpUpLeftShift(ull in_ull);
	inline ull UpUpRightShift(ull in_ull);
	inline ull UpLeftLeftShift(ull in_ull);
	inline ull UpRightRightShift(ull in_ull);

	inline ull DownDownLeftShift(ull in_ull);
	inline ull DownDownRightShift(ull in_ull);
	inline ull DownLeftLeftShift(ull in_ull);
	inline ull DownRightRightShift(ull in_ull);

	inline ull UpSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull DownSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull LeftSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull RightSlide(ull in_piece, ull in_empty, ull in_opponents);

	inline ull UpLeftSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull UpRightSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull DownLeftSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull DownRightSlide(ull in_piece, ull in_empty, ull in_opponents);

	inline ull HorizontalSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull VerticalSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull LeadingSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull FollowingSlide(ull in_piece, ull in_empty, ull in_opponents);

	inline ull StraightSlide(ull in_piece, ull in_empty, ull in_opponents);
	inline ull DiagonalSlide(ull in_piece, ull in_empty, ull in_opponents);

	inline ull FastBishopMoves(ull in_bishop, ull in_blockers, ull in_move_leading_pins, ull in_move_following_pins);
	inline ull FastRookMoves(ull in_rook, ull in_blockers, ull in_move_vertical_pins, ull in_move_horizontal_pins);
	inline ull FastQueenMoves(ull in_queen, ull in_blockers, ull in_move_vertical_pins, ull in_move_horizontal_pins, ull in_move_leading_pins, ull in_move_following_pins);

	inline ull KnightMoves(ull in_piece, ull in_empty, ull in_opponents = 0ULL);
	inline ull PawnMoves(ull in_pieces, ull in_empty, ull in_opponents, bool in_turn, ull in_vertical_pin = 0ULL, ull in_horizontal_pin = 0ULL, ull in_leading_pin = 0ULL, ull in_following_pin = 0ULL, ull in_passant_target = 0ULL, ull in_passant_pin = 0ULL);
	inline ull KingMoves(ull in_king, ull in_empty, ull in_opponents);



};

#endif POSITION