#ifndef MOVE
#define MOVE

#include<string>

#include"CommonTypes.h"

/*
* TODO:
* WRITE:
* ToString()
*/

class Move
{

private:
	
	ull from;
	ull to;
	byte move_piece;
	byte capture_piece;
	byte end_piece;
	byte castle_rights;
	ull passant_target;
	int half_moves;

public:

	Move();
	Move(Move& in_move);
	Move(
		ull in_from,
		ull in_to,
		byte in_move_piece,
		byte in_capture_piece,
		byte in_end_piece,
		byte in_castle_rights,
		ull in_passant_target,
		int in_half_moves
	);

	bool SetMove(
		ull in_from,
		ull in_to,
		byte in_move_piece,
		byte in_capture_piece,
		byte in_end_piece,
		byte in_castle_rights,
		ull in_passant_target,
		int in_half_moves
	);

	ull GetFrom() const;
	ull GetTo() const;
	byte GetMovePiece() const;
	byte GetCapturePiece() const;
	byte GetEndPiece() const;
	byte GetCastleRights() const;
	ull GetPassantTarget() const;
	int GetHalfMoves() const;

	std::string ToString() const;
	
};

#endif MOVE