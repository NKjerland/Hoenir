#include "Position.h"

const std::string Position::STANDARD_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/*
Position::Position()
{
	LoadFen(STANDARD_FEN);
}

Position::Position(const Position& in_position)
{

	for (int i = 0; i < piece_type_count; i++)
	{
		pieces_bitboard[i] = in_position.GetPiecesBitboard(i);
	}
	for (int i = 0; i < board_square_count; i++)
	{
		pieces_byte[i] = in_position.GetPiecesByte(i);
		pieces_count[i] = in_position.GetPiecesCount(i);
	}
	castle_rights = in_position.GetCastleRights();
	passant_target = in_position.GetPassantTarget();
	white_turn = in_position.GetWhiteTurn();
	half_moves = in_position.GetHalfMoves();
	full_moves = in_position.GetFullMoves();

}

Position::Position(std::string in_fen)
{
	LoadFen(in_fen);
}

Position::~Position()
{
	delete[] pieces_bitboard;
	delete[] pieces_byte;
	delete[] pieces_count;
}


int Position::GenerateMoves(MoveStack* in_movestack)
{
	return 0;
}
*/

Position::Position()
{
	LoadFen(STANDARD_FEN);
}

Position::Position(const Position& in_position)
{
	for (int i = 0; i < piece_type_count; i++)
	{
		pieces_bitboard[i] = in_position.GetPiecesBitboard(i);
	}
	for (int i = 0; i < board_square_count; i++)
	{
		pieces_byte[i] = in_position.GetPiecesByte(i);
		pieces_count[i] = in_position.GetPiecesCount(i);
	}
	castle_rights = in_position.GetCastleRights();
	passant_target = in_position.GetPassantTarget();
	white_turn = in_position.GetWhiteTurn();
	half_moves = in_position.GetHalfMoves();
	full_moves = in_position.GetFullMoves();
}

Position::Position(std::string in_fen)
{
	LoadFen(in_fen);
}

int Position::CountMoves()
{
	return 0;
}

int Position::GenerateMoves(MoveStack* in_movestack)
{

	int move_count = 0;

	int player_index = white_turn * 6;
	int opponent_index = !white_turn * 6;

	ull occupied = Occupied();
	ull empty = ~occupied;

	ull player_pieces = GetPlayerPieces(player_index);
	ull opponent_pieces = occupied & ~player_pieces;

	ull vertical_pins,
		horizontal_pins,
		leading_pins,
		following_pins;

	GetPins(vertical_pins, horizontal_pins, leading_pins, following_pins);

	ull passant_pin;

	GetPassantPin(passant_pin);

	ull vertical_block_squares,
		horizontal_block_squares,
		leading_block_squares,
		following_block_squares,
		block_squares;

	GetBlockSquares(vertical_block_squares, horizontal_block_squares, leading_block_squares, following_block_squares, block_squares);

	ull player_king = pieces_bitboard[KING + player_index];
	ull not_king = empty | player_king;
	ull opponent_pawn_attacks = PawnMoves(pieces_bitboard[PAWN + opponent_index], 0ULL, ~0ULL, !white_turn);
	ull opponent_knight_attacks = KnightMoves(pieces_bitboard[KNIGHT + opponent_index], not_king, occupied);
	ull opponent_bishop_attacks = DiagonalSlide(pieces_bitboard[BISHOP + opponent_index], not_king, occupied);
	ull opponent_rook_attacks = StraightSlide(pieces_bitboard[ROOK + opponent_index], not_king, occupied);
	ull opponent_queen_attacks = StraightSlide(pieces_bitboard[QUEEN + opponent_index], not_king, occupied) | DiagonalSlide(pieces_bitboard[QUEEN + opponent_index], not_king, occupied);
	ull opponent_king_attacks = KingMoves(pieces_bitboard[KING + opponent_index], empty, occupied);
	ull opponent_all_attacks = opponent_pawn_attacks | opponent_knight_attacks | opponent_bishop_attacks | opponent_rook_attacks | opponent_queen_attacks | opponent_king_attacks;

	// used to determine if a move checks opponent

	ull opponent_king_pawn_attacks = PawnMoves(pieces_bitboard[KING + opponent_index], 0ULL, ~0ULL, !white_turn);
	ull opponent_king_knight_attacks = KnightMoves(pieces_bitboard[KING + opponent_index], ~0ULL);
	ull opponent_king_bishop_attacks = FastBishopMoves(pieces_bitboard[KING + opponent_index], occupied, 0ULL, 0ULL);
	ull opponent_king_rook_attacks = FastRookMoves(pieces_bitboard[KING + opponent_index], occupied, 0ULL, 0ULL);
	ull opponent_king_queen_attacks = opponent_king_bishop_attacks | opponent_king_rook_attacks;

	// determine in check 

	ull player_king_pawn_attacks = PawnMoves(pieces_bitboard[KING + player_index], 0ULL, ~0ULL, white_turn);
	ull player_king_knight_attacks = KnightMoves(pieces_bitboard[KING + player_index], ~0ULL);
	ull player_king_bishop_attacks = DiagonalSlide(pieces_bitboard[KING + player_index], empty, opponent_pieces);
	ull player_king_rook_attacks = StraightSlide(pieces_bitboard[KING + player_index], empty, opponent_pieces);
	ull player_king_queen_attacks = player_king_bishop_attacks | player_king_rook_attacks;

	ull king_attackers = player_king_pawn_attacks & pieces_bitboard[PAWN + opponent_index];
	king_attackers |= player_king_knight_attacks & pieces_bitboard[KNIGHT + opponent_index];
	king_attackers |= player_king_bishop_attacks & pieces_bitboard[BISHOP + opponent_index];
	king_attackers |= player_king_rook_attacks & pieces_bitboard[ROOK + opponent_index];
	king_attackers |= player_king_queen_attacks & pieces_bitboard[QUEEN + opponent_index];

	bool check = king_attackers > 0;
	bool double_check = RemoveBit(king_attackers) > 0;

	ull legal_move_to = (~0ULL * !check) + ((king_attackers | block_squares) * check * !double_check);
	ull passant_legal = (passant_target & VALID_PASSANT * ((player_king_pawn_attacks & pieces_bitboard[PAWN + opponent_index]) > 0));

	// ** GENERATE MOVES ** //

	// King moves
	{
		ull temp_move_from = pieces_bitboard[KING + player_index];
		ull temp_moves = KingMoves(temp_move_from, empty, opponent_pieces) & ~opponent_all_attacks;

		while (temp_moves)
		{
			ull temp_move_to = LowestSignificantBit(temp_moves);

			in_movestack->Add(temp_move_from,
				temp_move_to,
				(KING + player_index),
				(GetPiece(temp_move_to)),
				EMPTY_PIECE,
				castle_rights,
				passant_target,
				half_moves
			);

			temp_moves = RemoveBit(temp_moves);
		}
	}
	if (double_check)
	{
		return move_count;
	}

	for (int i = QUEEN; i >= PAWN; i--)
	{
		AddMove(
			in_movestack,
			move_count,
			QUEEN,
			player_index,
			occupied,
			vertical_pins,
			horizontal_pins,
			leading_pins,
			following_pins,
			legal_move_to,
			player_pieces,
			passant_pin
		);
	}


	/*
	{
		ull temp_piece_list = pieces_bitboard[QUEEN + player_index];

		while (temp_piece_list)
		{
			ull temp_move_from = LowestSignificantBit(temp_piece_list);
			ull temp_moves = FastQueenMoves(
				temp_move_from,
				occupied,
				vertical_pins,
				horizontal_pins,
				leading_pins,
				following_pins
			) & legal_move_to & ~player_pieces;

			while (temp_moves)
			{
				ull temp_move_to = LowestSignificantBit(temp_moves);

				in_movestack->Add(
					temp_move_from,
					temp_move_to,
					QUEEN + player_index,
					GetPiece(temp_move_to),
					EMPTY_PIECE,
					castle_rights,
					passant_target,
					half_moves
				);

				move_count++;

				temp_moves = RemoveBit(temp_moves);
			}

			temp_piece_list = RemoveBit(temp_piece_list);
		}
	}

	// Rook moves

	{
		ull temp_piece_list = pieces_bitboard[ROOK + player_index];

		while (temp_piece_list)
		{
			ull temp_move_from = LowestSignificantBit(temp_piece_list);
			ull temp_moves = FastRookMoves(
				temp_move_from,
				occupied,
				vertical_pins,
				horizontal_pins
			) & legal_move_to & ~player_pieces;

			while (temp_moves)
			{
				ull temp_move_to = LowestSignificantBit(temp_moves);

				in_movestack->Add(
					temp_move_from,
					temp_move_to,
					ROOK + player_index,
					GetPiece(temp_move_to),
					EMPTY_PIECE,
					castle_rights,
					passant_target,
					half_moves
				);

				move_count++;

				temp_moves = RemoveBit(temp_moves);
			}

			temp_piece_list = RemoveBit(temp_piece_list);
		}
	}

	// Bishop moves

	{
		ull temp_piece_list = pieces_bitboard[BISHOP + player_index];

		while (temp_piece_list)
		{
			ull temp_move_from = LowestSignificantBit(temp_piece_list);
			ull temp_moves = FastBishopMoves(
				temp_move_from,
				occupied,
				leading_pins,
				following_pins
			) & legal_move_to & ~player_pieces;

			while (temp_moves)
			{
				ull temp_move_to = LowestSignificantBit(temp_moves);

				in_movestack->Add(
					temp_move_from,
					temp_move_to,
					BISHOP + player_index,
					GetPiece(temp_move_to),
					EMPTY_PIECE,
					castle_rights,
					passant_target,
					half_moves
				);

				move_count++;

				temp_moves = RemoveBit(temp_moves);
			}

			temp_piece_list = RemoveBit(temp_piece_list);
		}
	}

	*/


	return move_count;
}

int Position::GenerateMovesQuiescence(MoveStack* in_movestack)
{
	return 0;
}

void Position::MakeMove(Move* in_move)
{

	// Move main piece

	RemovePiece(in_move->GetFrom());
	RemovePiece(in_move->GetTo());
	SetPiece(in_move->GetEndPiece(), in_move->GetTo());

	switch (in_move->GetMovePiece())
	{
		case WHITE_KING:
			
			// Update castle rights
			castle_rights &= ~(
			CASTLE_RIGHT_WHITE_LONG |
			CASTLE_RIGHT_WHITE_SHORT
			);

			// Handle castle
			if (
				in_move->GetFrom() & WHITE_KING_START &&
				in_move->GetTo() & WHITE_SHORT_CASTLE_KING_END
				)
			{
				RemovePiece(WHITE_SHORT_CASTLE_ROOK_START);
				SetPiece(WHITE_ROOK, WHITE_SHORT_CASTLE_ROOK_END);
			}

			else if (
				in_move->GetFrom() & WHITE_KING_START &&
				in_move->GetTo() & WHITE_LONG_CASTLE_KING_END
				)
			{
				RemovePiece(WHITE_LONG_CASTLE_ROOK_END);
				SetPiece(WHITE_ROOK, WHITE_LONG_CASTLE_ROOK_END);
			}

			break;

		case BLACK_KING:

			// Update castle rights
			castle_rights &= ~(
				CASTLE_RIGHT_BLACK_LONG |
				CASTLE_RIGHT_BLACK_SHORT
				);

			// Handle castle
			if (
				in_move->GetFrom() & BLACK_KING_START &&
				in_move->GetTo() & BLACK_SHORT_CASTLE_KING_END
				)
			{
				RemovePiece(BLACK_SHORT_CASTLE_ROOK_START);
				SetPiece(BLACK_ROOK, BLACK_SHORT_CASTLE_ROOK_END);
			}

			else if (
				in_move->GetFrom() & BLACK_KING_START &&
				in_move->GetTo() & BLACK_LONG_CASTLE_KING_END
				)
			{
				RemovePiece(BLACK_LONG_CASTLE_ROOK_END);
				SetPiece(BLACK_ROOK, BLACK_LONG_CASTLE_ROOK_END);
			}

			break;

		case WHITE_ROOK:

			// Update castling rights
			castle_rights &= ~(
				(in_move->GetFrom() & WHITE_SHORT_CASTLE_ROOK_START) * CASTLE_RIGHT_WHITE_SHORT
				);

			castle_rights &= ~(
				(in_move->GetFrom() & WHITE_LONG_CASTLE_ROOK_START) * CASTLE_RIGHT_WHITE_LONG
				);
			break;

		case BLACK_ROOK:

			// Update castling rights
			castle_rights &= ~(
				(in_move->GetFrom() & BLACK_SHORT_CASTLE_ROOK_START) * CASTLE_RIGHT_BLACK_SHORT
				);

			castle_rights &= ~(
				(in_move->GetFrom() & BLACK_LONG_CASTLE_ROOK_START) * CASTLE_RIGHT_BLACK_LONG
				);
			break;

		case WHITE_PAWN:

			// Removes captured piece if move is en passant
			// Only resolves true when move is en passant
			if (passant_target & in_move->GetTo())
			{
				RemovePiece(DownShift(in_move->GetTo()));
			}

			// Update passant target
			// Zero if not double move
			passant_target = (UpUpShift(in_move->GetFrom()) & in_move->GetTo()) * in_move->GetTo();
			break;

		case BLACK_PAWN:

			// Removes captured piece if move is en passant
			// Only resolves true when move is en passant
			if (passant_target & in_move->GetTo())
			{
				RemovePiece(UpShift(in_move->GetTo()));
			}

			// Update passant target
			// Zero if not double move
			passant_target = (DownDownShift(in_move->GetFrom()) & in_move->GetTo()) * in_move->GetTo();

		default:
			break;
	}

	white_turn = !white_turn;

	half_moves =
		(half_moves + 1) *
		(
			castle_rights == in_move->GetCastleRights() ||
			in_move->GetCapturePiece() == EMPTY_PIECE ||
			in_move->GetMovePiece() % 6 == PAWN
		);

	full_moves += white_turn;
}

void Position::UndoMove(Move* in_move)
{
}

ull Position::GetPiecesBitboard(int piece_type) const
{
	return pieces_bitboard[piece_type];
}

byte Position::GetPiecesByte(int in_square) const
{
	return pieces_byte[in_square];
}

int Position::GetPiecesCount(int in_square) const
{
	return pieces_count[in_square];
}

byte Position::GetCastleRights() const
{
	return castle_rights;
}

ull Position::GetPassantTarget() const
{
	return passant_target;
}

bool Position::GetWhiteTurn() const
{
	return white_turn;
}

int Position::GetHalfMoves() const
{
	return half_moves;
}

int Position::GetFullMoves() const
{
	return full_moves;
}

std::string Position::ToString()
{
	std::stringstream return_stream;

	for (int y = 7; y >= 0; y--)
	{
		return_stream << "+---+---+---+---+---+---+---+---+";
		return_stream << '\n';

		for (int x = 0; x < 8; x++)
		{
			return_stream << "| " + Functions::PieceToChar(pieces_byte[(y * 8 + x)]);
		}

		return_stream << " | " + std::to_string(y + 8);
		return_stream << "\n";
	}

	return_stream << "+---+---+---+---+---+---+---+---+";
	return_stream << "\n";
	return_stream << "  a   b   c   d   e   f   g   h  ";
	return_stream << "\n\n";

	return_stream << "fen: " + ToFen();
	return_stream << "\n";

	return_stream << "To move: " + std::string(white_turn ? "White" : "Black");
	return_stream << "\n";

	return_stream << "Castle rights: " + ToFenCastleRights();
	return_stream << "\n";

	return_stream << "En passant target: " + ToFenPassantTarget();
	return_stream << "\n";

	return_stream << "Half moves: " + ToFenHalfMoves();
	return_stream << "\n";

	return_stream << "Full moves: " + ToFenFullMoves();

	return return_stream.str();
}

std::string Position::ToFen()
{
	std::string fen;

	fen += ToFenPieces();
	fen += " ";

	fen += ToFenTurn();
	fen += " ";

	fen += ToFenCastleRights();
	fen += " ";

	fen += ToFenPassantTarget();
	fen += " ";

	fen += ToFenHalfMoves();
	fen += " ";

	fen += ToFenFullMoves();

	return fen;
}

Position& Position::operator=(const Position& in_position)
{
	for (int i = 0; i < piece_type_count; i++)
	{
		pieces_bitboard[i] = in_position.GetPiecesBitboard(i);
	}
	for (int i = 0; i < board_square_count; i++)
	{
		pieces_byte[i] = in_position.GetPiecesByte(i);
		pieces_count[i] = in_position.GetPiecesCount(i);
	}
	castle_rights = in_position.GetCastleRights();
	passant_target = in_position.GetPassantTarget();
	white_turn = in_position.GetWhiteTurn();
	half_moves = in_position.GetHalfMoves();
	full_moves = in_position.GetFullMoves();

	return *this;
}

byte Position::GetPiece(ull in_square)
{
	return pieces_byte[BSLfast(in_square)];
}

void Position::SetPiece(int in_piece, ull in_square)
{
	pieces_bitboard[in_piece] |= in_square;
	pieces_byte[BSLfast(in_square)] = in_piece;
	pieces_count[in_piece]++;
}

void Position::RemovePiece(ull in_square)
{
	int index = BSLfast(in_square);
	int piece = pieces_byte[index];
	piece = piece * (piece != EMPTY_PIECE);
	pieces_count[piece] -= (piece != EMPTY_PIECE);
	pieces_bitboard[piece] &= ~in_square;
	pieces_byte[index] = EMPTY_PIECE;
}

void Position::AddMove(
	MoveStack* in_movestack,
	int& in_move_count,
	byte in_piece_type,
	int in_player_index,
	ull in_occupied,
	ull in_vertical_pins,
	ull in_horizontal_pins,
	ull in_leading_pins,
	ull in_following_pins,
	ull in_legal_move_to,
	ull in_player_pieces,
	ull in_passant_pin
)
{
	ull temp_piece_list = pieces_bitboard[in_piece_type + in_player_index];
	temp_piece_list =
		(in_piece_type != KNIGHT) * (temp_piece_list)+
		(in_piece_type == KNIGHT) * (temp_piece_list & ~(in_vertical_pins | in_horizontal_pins | in_leading_pins | in_following_pins));
	while (temp_piece_list)
	{
		ull temp_move_from = LowestSignificantBit(temp_piece_list);
		ull temp_moves;

		switch (in_piece_type)
		{
			case QUEEN:
				temp_moves = FastQueenMoves(
					temp_move_from,
					in_occupied,
					in_vertical_pins,
					in_horizontal_pins,
					in_leading_pins,
					in_following_pins
				) & in_legal_move_to & ~in_player_pieces;
				break;
			case ROOK:
				temp_moves = FastRookMoves(
					temp_move_from,
					in_occupied,
					in_vertical_pins,
					in_horizontal_pins
				) & in_legal_move_to & ~in_player_pieces;
				break;
			case BISHOP:
				temp_moves = FastBishopMoves(
					temp_move_from,
					in_occupied,
					in_leading_pins,
					in_following_pins
				) & in_legal_move_to & ~in_player_pieces;
				break;
			case KNIGHT:
				temp_moves = KnightMoves(
					temp_move_from,
					~in_occupied,
					in_occupied & ~in_player_pieces
				) & in_legal_move_to;
			case PAWN:
				temp_moves = PawnMoves(
					temp_move_from,
					~in_occupied,
					in_occupied & ~in_player_pieces,
					white_turn,
					in_vertical_pins,
					in_horizontal_pins,
					in_leading_pins,
					in_following_pins,
					passant_target,
					in_passant_pin
				);
				break;
			default:
				temp_moves = 0ULL;
				break;
		}

		if (in_piece_type == PAWN && (temp_move_from & BACK_SECOND_RANK))
		{
			while (temp_moves)
			{
				ull temp_move_to = LowestSignificantBit(temp_moves);

				for (int i = PAWN; i <= QUEEN; i++)
				{
					in_movestack->Add(
						temp_move_from,
						temp_move_to,
						in_piece_type + in_player_index,
						GetPiece(temp_move_to),
						i,
						castle_rights,
						passant_target,
						half_moves
					);
					in_move_count++;
				}
				temp_moves = RemoveBit(temp_moves);
			}
		}
		else
		{
			while (temp_moves)
			{
				ull temp_move_to = LowestSignificantBit(temp_moves);

				in_movestack->Add(
					temp_move_from,
					temp_move_to,
					in_piece_type + in_player_index,
					GetPiece(temp_move_to),
					in_piece_type + in_player_index,
					castle_rights,
					passant_target,
					half_moves
				);
				in_move_count++;
				
				temp_moves = RemoveBit(temp_moves); 
			}
		}
	}
}

void Position::LoadFen(std::string in_fen)
{
	for (ull bitboard : pieces_bitboard)
	{
		bitboard = 0;
	}

	for (byte byte_board : pieces_byte)
	{
		byte_board = 0;
	}

	for (int i : pieces_count)
	{
		i = 0;
	}

	castle_rights = 0;
	passant_target = 0;
	white_turn = 0;
	half_moves = 0;
	full_moves = 0;

	std::vector<std::string> tokens = Functions::SplitString(in_fen);

	if (tokens.size() < 4 || tokens.size() > 6)
	{
		return;
	}

	LoadFenPieces(tokens[0]);
	LoadFenTurn(tokens[1]);
	LoadFenCastleRights(tokens[2]);
	LoadFenPassantTarget(tokens[3]);

	if (tokens.size() != 6)
	{
		return;
	}

	LoadFenHalfMoves(tokens[4]);
	LoadFenFullMoves(tokens[5]);

	return;
}

void Position::LoadFenPieces(std::string in_fen)
{
	int index = 0;
	int x = 0;
	int y = 7;

	while (index < in_fen.size())
	{
		char character = in_fen[index];

		if (character == '/')
		{
			x = 0;
			y--;
		}

		else if (Functions::IsNumber(character))
		{
			x += Functions::CharToInt(character);
		}

		else if (Functions::IsPiece(character))
		{
			pieces_bitboard[Functions::CharToPiece(character)] |= 0ULL << (y * 8 + x);
			x++;
		}

		index++;
	}
	return;
}

void Position::LoadFenTurn(std::string in_fen)
{
	if (in_fen.size() == 1)
	{
		white_turn = (in_fen[0] == 'w');
	}
	return;
}

void Position::LoadFenCastleRights(std::string in_fen)
{
	for (char character : in_fen)
	{
		switch (character)
		{
			case 'K':
				castle_rights |= CASTLE_RIGHT_WHITE_SHORT;
				break;
			case 'Q':
				castle_rights |= CASTLE_RIGHT_WHITE_LONG;
				break;
			case 'k':
				castle_rights |= CASTLE_RIGHT_BLACK_SHORT;
				break;
			case 'q':
				castle_rights |= CASTLE_RIGHT_BLACK_LONG;
				break;
			default:
				break;
		}
	}
	return;
}

void Position::LoadFenPassantTarget(std::string in_fen)
{
	if (in_fen.size() == 2)
	{
		passant_target = Functions::CharSquareToUll(in_fen[0], in_fen[1]);
	}
	return;
}

void Position::LoadFenHalfMoves(std::string in_fen)
{
	if (Functions::ContainsOnlyDigits(in_fen))
	{
		half_moves = std::stoi(in_fen);
	}
	return;
}

void Position::LoadFenFullMoves(std::string in_fen)
{
	if (Functions::ContainsOnlyDigits(in_fen))
	{
		full_moves = std::stoi(in_fen);
	}
	return;
}

std::string Position::ToFenPieces()
{
	std::string fen;
	int space = 0;

	for (int y = 7; y >= 0; y--)
	{
		for (int x = 0; x < 8; x++)
		{
			byte piece = pieces_byte[(y * 8 + x)];
			if (piece == EMPTY_PIECE)
			{
				space++;
			}
			else
			{
				if (space != 0)
				{
					fen += Functions::IntToChar(space);
				}
				fen += Functions::PieceToChar(piece);
			}
		}
		if (space != 0)
		{
			fen += Functions::IntToChar(space);
		}
		if (7 != 0)
		{
			fen += "/";
		}
	}

	return fen;
}

std::string Position::ToFenTurn()
{
	if (white_turn)
	{
		return "w";
	}
	return "b";
}

std::string Position::ToFenCastleRights()
{
	std::string fen;

	if (castle_rights && CASTLE_RIGHT_WHITE_SHORT)
	{
		fen += "K";
	}
	if (castle_rights && CASTLE_RIGHT_WHITE_LONG)
	{
		fen += "Q";
	}
	if (castle_rights && CASTLE_RIGHT_BLACK_SHORT)
	{
		fen += "k";
	}
	if (castle_rights && CASTLE_RIGHT_BLACK_LONG)
	{
		fen += "q";
	}
	return fen;
}

std::string Position::ToFenPassantTarget()
{
	int square = BSLfast(passant_target);
	int column_int = square % 8;
	int row_int = square / 8;

	char column_char = 'a' + column_int;
	char row_char = '0' + row_int;

	return "" + column_char + row_char;
}

std::string Position::ToFenHalfMoves()
{
	return std::to_string(half_moves);
}

std::string Position::ToFenFullMoves()
{
	return std::to_string(full_moves);
}

ull Position::RemoveBit(ull in_ull)
{
	return in_ull & (in_ull - 1);
}

ull Position::LowestSignificantBit(ull in_ull)
{
	return in_ull & (~in_ull + 1);
}

ull Position::UpShift(ull in_ull)
{
	return (in_ull & AVOID_UP) << 8;
}

ull Position::DownShift(ull in_ull)
{
	return (in_ull & AVOID_DOWN) >> 8;
}

ull Position::LeftShift(ull in_ull)
{
	return (in_ull & AVOID_LEFT) >> 1;
}

ull Position::RightShift(ull in_ull)
{
	return (in_ull & AVOID_RIGHT) << 1;
}

ull Position::UpUpShift(ull in_ull)
{
	return (in_ull & AVOID_UP_UP) << 16;
}

ull Position::DownDownShift(ull in_ull)
{
	return (in_ull & AVOID_DOWN_DOWN) >> 16;
}

ull Position::UpLeftShift(ull in_ull)
{
	return (in_ull & AVOID_UP_LEFT) << 7;
}

ull Position::UpRightShift(ull in_ull)
{
	return (in_ull & AVOID_UP_RIGHT) << 9;
}

ull Position::DownLeftShift(ull in_ull)
{
	return (in_ull & AVOID_DOWN_LEFT) >> 9;
}

ull Position::DownRightShift(ull in_ull)
{
	return (in_ull & AVOID_DOWN_RIGHT) >> 7;
}

ull Position::UpUpLeftShift(ull in_ull)
{
	return (in_ull & AVOID_UP_UP_LEFT) << 15;
}

ull Position::UpUpRightShift(ull in_ull)
{
	return (in_ull & AVOID_UP_UP_RIGHT) << 17;
}

ull Position::UpLeftLeftShift(ull in_ull)
{
	return (in_ull & AVOID_LEFT_LEFT_UP) << 6;
}

ull Position::UpRightRightShift(ull in_ull)
{
	return (in_ull & AVOID_RIGHT_RIGHT_UP) << 10;
}

ull Position::DownDownLeftShift(ull in_ull)
{
	return (in_ull & AVOID_DOWN_DOWN_LEFT) >> 17;
}

ull Position::DownDownRightShift(ull in_ull)
{
	return (in_ull & AVOID_DOWN_DOWN_RIGHT) >> 15;
}

ull Position::DownLeftLeftShift(ull in_ull)
{
	return (in_ull & AVOID_LEFT_LEFT_DOWN) >> 10;
}
ull Position::DownRightRightShift(ull in_ull)
{
	return (in_ull & AVOID_RIGHT_RIGHT_DOWN) >> 6;
}

ull Position::UpSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = UpShift(in_piece) & in_empty;
	targets |= UpShift(targets) & in_empty;
	targets |= UpShift(targets) & in_empty;
	targets |= UpShift(targets) & in_empty;
	targets |= UpShift(targets) & in_empty;
	targets |= UpShift(targets) & in_empty;
	targets |= UpShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::DownSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = DownShift(in_piece) & in_empty;
	targets |= DownShift(targets) & in_empty;
	targets |= DownShift(targets) & in_empty;
	targets |= DownShift(targets) & in_empty;
	targets |= DownShift(targets) & in_empty;
	targets |= DownShift(targets) & in_empty;
	targets |= DownShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::LeftSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = LeftShift(in_piece) & in_empty;
	targets |= LeftShift(targets) & in_empty;
	targets |= LeftShift(targets) & in_empty;
	targets |= LeftShift(targets) & in_empty;
	targets |= LeftShift(targets) & in_empty;
	targets |= LeftShift(targets) & in_empty;
	targets |= LeftShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::RightSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = RightShift(in_piece) & in_empty;
	targets |= RightShift(targets) & in_empty;
	targets |= RightShift(targets) & in_empty;
	targets |= RightShift(targets) & in_empty;
	targets |= RightShift(targets) & in_empty;
	targets |= RightShift(targets) & in_empty;
	targets |= RightShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::UpLeftSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = UpLeftShift(in_piece) & in_empty;
	targets |= UpLeftShift(targets) & in_empty;
	targets |= UpLeftShift(targets) & in_empty;
	targets |= UpLeftShift(targets) & in_empty;
	targets |= UpLeftShift(targets) & in_empty;
	targets |= UpLeftShift(targets) & in_empty;
	targets |= UpLeftShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::UpRightSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = UpRightShift(in_piece) & in_empty;
	targets |= UpRightShift(targets) & in_empty;
	targets |= UpRightShift(targets) & in_empty;
	targets |= UpRightShift(targets) & in_empty;
	targets |= UpRightShift(targets) & in_empty;
	targets |= UpRightShift(targets) & in_empty;
	targets |= UpRightShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::DownLeftSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = DownLeftShift(in_piece) & in_empty;
	targets |= DownLeftShift(targets) & in_empty;
	targets |= DownLeftShift(targets) & in_empty;
	targets |= DownLeftShift(targets) & in_empty;
	targets |= DownLeftShift(targets) & in_empty;
	targets |= DownLeftShift(targets) & in_empty;
	targets |= DownLeftShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}

ull Position::DownRightSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	ull targets = DownRightShift(in_piece) & in_empty;
	targets |= DownRightShift(targets) & in_empty;
	targets |= DownRightShift(targets) & in_empty;
	targets |= DownRightShift(targets) & in_empty;
	targets |= DownRightShift(targets) & in_empty;
	targets |= DownRightShift(targets) & in_empty;
	targets |= DownRightShift((targets | in_piece)) & (in_empty | in_opponents);

	return targets;
}


ull Position::HorizontalSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	return LeftSlide(in_piece, in_empty, in_opponents) | RightSlide(in_piece, in_empty, in_opponents);
}

ull Position::VerticalSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	return UpSlide(in_piece, in_empty, in_opponents) | DownSlide(in_piece, in_empty, in_opponents);
}

ull Position::LeadingSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	return DownRightSlide(in_piece, in_empty, in_opponents) | UpLeftSlide(in_piece, in_empty, in_opponents);
}

ull Position::FollowingSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	return DownLeftSlide(in_piece, in_empty, in_opponents) | UpRightSlide(in_piece, in_empty, in_opponents);
}

ull Position::StraightSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	return HorizontalSlide(in_piece, in_empty, in_opponents) | VerticalSlide(in_piece, in_empty, in_opponents);
}

ull Position::DiagonalSlide(ull in_piece, ull in_empty, ull in_opponents)
{
	return LeadingSlide(in_piece, in_empty, in_opponents) | FollowingSlide(in_piece, in_empty, in_opponents);
}

ull Position::FastBishopMoves(ull in_bishop, ull in_blockers, ull in_move_leading_pins, ull in_move_following_pins)
{
	int pos = BSLfast(in_bishop);

	ull temp_move_to = 0ULL;
	ull rayblock;

	ull tempMoves;

	{
		rayblock = RAYS[1][pos] & in_blockers;
		tempMoves = RAYS[1][pos];
		if (rayblock)
		{
			int blockerIndex = BSLfast(rayblock);
			tempMoves &= ~RAYS[1][blockerIndex];
		}

		rayblock = RAYS[6][pos] & in_blockers;
		tempMoves |= RAYS[6][pos];
		if (rayblock)
		{
			int blockerIndex = BSHfast(rayblock);
			tempMoves &= ~RAYS[6][blockerIndex];
		}

		tempMoves = (tempMoves) * ((in_bishop & in_move_leading_pins) == 0);
		temp_move_to |= tempMoves;
	}

	{
		rayblock = RAYS[2][pos] & in_blockers;
		tempMoves = RAYS[2][pos];
		if (rayblock)
		{
			int blockerIndex = BSLfast(rayblock);
			tempMoves &= ~RAYS[2][blockerIndex];
		}

		rayblock = RAYS[5][pos] & in_blockers;
		tempMoves |= RAYS[5][pos];
		if (rayblock)
		{
			int blockerIndex = BSHfast(rayblock);
			tempMoves &= ~RAYS[5][blockerIndex];
		}

		tempMoves = (tempMoves) * ((in_bishop & in_move_following_pins) == 0);
		temp_move_to |= tempMoves;
	}
	return temp_move_to;
}

ull Position::FastRookMoves(ull in_rook, ull in_blockers, ull in_move_vertical_pins, ull in_move_horizontal_pins)
{
	int pos = BSLfast(in_rook);

	ull temp_move_to = 0ULL;
	ull rayblock;

	ull tempMoves;

	{
		rayblock = RAYS[0][pos] & in_blockers;
		tempMoves = RAYS[0][pos];
		if (rayblock)
		{
			int blockerIndex = BSLfast(rayblock);
			tempMoves &= ~RAYS[0][blockerIndex];
		}

		rayblock = RAYS[7][pos] & in_blockers;
		tempMoves |= RAYS[7][pos];
		if (rayblock)
		{
			int blockerIndex = BSHfast(rayblock);
			tempMoves &= ~RAYS[7][blockerIndex];
		}

		tempMoves = (tempMoves) * ((in_rook & in_move_vertical_pins) == 0);
		temp_move_to |= tempMoves;
	}

	{
		rayblock = RAYS[3][pos] & in_blockers;
		tempMoves = RAYS[3][pos];
		if (rayblock)
		{
			int blockerIndex = BSHfast(rayblock);
			tempMoves &= ~RAYS[3][blockerIndex];
		}


		rayblock = RAYS[4][pos] & in_blockers;
		tempMoves |= RAYS[4][pos];
		if (rayblock)
		{
			int blockerIndex = BSLfast(rayblock);
			tempMoves &= ~RAYS[4][blockerIndex];
		}

		tempMoves = (tempMoves) * ((in_rook & in_move_horizontal_pins) == 0);
		temp_move_to |= tempMoves;
	}

	return temp_move_to;
}

ull Position::FastQueenMoves(ull in_queen, ull in_blockers, ull in_move_vertical_pins, ull in_move_horizontal_pins, ull in_move_leading_pins, ull in_move_following_pins)
{
	return FastBishopMoves(in_queen, in_blockers, in_move_leading_pins, in_move_following_pins) | FastRookMoves(in_queen, in_blockers, in_move_vertical_pins, in_move_horizontal_pins);
}

ull Position::KnightMoves(ull in_piece, ull in_empty, ull in_opponents)
{
	ull temp_moves = 0ULL;
	temp_moves |= UpUpLeftShift(in_piece);
	temp_moves |= UpUpRightShift(in_piece);
	temp_moves |= UpLeftLeftShift(in_piece);
	temp_moves |= UpRightRightShift(in_piece);
	temp_moves |= DownLeftLeftShift(in_piece);
	temp_moves |= DownRightRightShift(in_piece);
	temp_moves |= DownDownLeftShift(in_piece);
	temp_moves |= DownDownRightShift(in_piece);

	return temp_moves & (in_empty | in_opponents);
}

ull Position::PawnMoves(ull in_pieces, ull in_empty, ull in_opponents, bool in_turn, ull in_vertical_pins, ull in_horizontal_pin, ull in_leading_pin, ull in_following_pin, ull in_passant_target, ull in_passant_pin)
{
	ull temp_moves = 0ULL;
	ull verticalHorizontalPins = in_vertical_pins | in_horizontal_pin;

	if (in_turn)
	{
		temp_moves |= UpShift(in_pieces & ~(in_horizontal_pin | in_leading_pin | in_following_pin)) & in_empty;
		ull ul = UpLeftShift(in_pieces & ~(verticalHorizontalPins | in_following_pin));
		ull ur = UpRightShift(in_pieces & ~(verticalHorizontalPins | in_leading_pin));
		temp_moves |= ul & in_opponents;
		temp_moves |= (ul & in_passant_target) * ((in_pieces & in_passant_pin) == 0);
		temp_moves |= ur & in_opponents;
		temp_moves |= (ur & in_passant_target) * ((in_pieces & in_passant_pin) == 0);
		temp_moves |= UpShift(((UpShift((in_pieces & ~(in_horizontal_pin | in_leading_pin | in_following_pin))) & in_empty & VALID_WHITE_PASSANT))) & in_empty;
	}
	else
	{
		temp_moves |= DownShift(in_pieces & ~(in_horizontal_pin | in_leading_pin | in_following_pin)) & in_empty;
		ull dl = DownLeftShift(in_pieces & ~(verticalHorizontalPins | in_leading_pin));
		ull dr = DownRightShift(in_pieces & ~(verticalHorizontalPins | in_following_pin));
		temp_moves |= dl & in_opponents;
		temp_moves |= (dl & in_passant_target) * ((in_pieces & in_passant_pin) == 0);
		temp_moves |= dr & in_opponents;
		temp_moves |= (dr & in_passant_target) * ((in_pieces & in_passant_pin) == 0);
		temp_moves |= DownShift(((DownShift((in_pieces & ~(in_horizontal_pin | in_leading_pin | in_following_pin))) & in_empty & VALID_BLACK_PASSANT))) & in_empty;
	}

	return temp_moves;
}

ull Position::KingMoves(ull in_king, ull in_empty, ull in_opponents)
{
	ull temp_moves = UpShift(in_king);
	temp_moves |= UpLeftShift(in_king);
	temp_moves |= UpRightShift(in_king);
	temp_moves |= LeftShift(in_king);
	temp_moves |= RightShift(in_king);
	temp_moves |= DownLeftShift(in_king);
	temp_moves |= DownRightShift(in_king);
	temp_moves |= DownShift(in_king);

	return temp_moves & (in_opponents | in_empty);
}

int Position::BSLfast(ull in_ull)
{
	const ull debruijn64 = ull(0x03f79d71b4cb0a89);
	return index64l[((in_ull & Functions::LS1B(in_ull)) * debruijn64) >> 58];
}

int Position::BSHfast(ull in_ull)
{
	const ull debruijn64 = ull(0x03f79d71b4cb0a89);
	in_ull |= in_ull >> 1;
	in_ull |= in_ull >> 2;
	in_ull |= in_ull >> 4;
	in_ull |= in_ull >> 8;
	in_ull |= in_ull >> 16;
	in_ull |= in_ull >> 32;
	return index64h[(in_ull * debruijn64) >> 58];
}
