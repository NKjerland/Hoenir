#include "Move.h"

Move::Move()
{
	from = 0;
	to = 0;
	move_piece = 0;
	capture_piece = 0;
	end_piece = 0;
	castle_rights = 0;
	passant_target = 0;
	half_moves = 0;
}

Move::Move(Move& in_move)
{
	from = in_move.from;
	to = in_move.to;
	move_piece = in_move.move_piece;
	capture_piece = in_move.capture_piece;
	end_piece = in_move.end_piece;
	castle_rights = in_move.capture_piece;
	passant_target = in_move.passant_target;
	half_moves = in_move.half_moves;
}

Move::Move(ull in_from, ull in_to, byte in_move_piece, byte in_capture_piece, byte in_end_piece, byte in_castle_rights, ull in_passant_target, int in_half_moves)
{
	from = in_from;
	to = in_to;
	move_piece = in_move_piece;
	capture_piece = in_capture_piece;
	end_piece = in_end_piece;
	castle_rights = in_castle_rights;
	passant_target = in_passant_target;
	half_moves = in_half_moves;
}

bool Move::SetMove(
	ull in_from,
	ull in_to,
	byte in_move_piece,
	byte in_capture_piece,
	byte in_end_piece,
	byte in_castle_rights,
	ull in_passant_target,
	int in_half_moves
) {
	from = in_from;
	to = in_to;
	move_piece = in_move_piece;
	capture_piece = in_capture_piece;
	end_piece = in_end_piece;
	castle_rights = in_castle_rights;
	passant_target = in_passant_target;
	half_moves = in_half_moves;
}

ull Move::GetFrom() const
{
	return from;
}

ull Move::GetTo() const
{
	return to;
}

byte Move::GetMovePiece() const
{
	return move_piece;
}

byte Move::GetCapturePiece() const
{
	return capture_piece;
}

byte Move::GetEndPiece() const
{
	return end_piece;
}

byte Move::GetCastleRights() const
{
	return castle_rights;
}

ull Move::GetPassantTarget() const
{
	return passant_target;
}

int Move::GetHalfMoves() const
{
	return half_moves;
}

std::string Move::ToString() const
{
	return std::string();
}
