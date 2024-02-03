#include "MoveStack.h"

MoveStack::MoveStack()
{
	length = 0;
	max_length = 0;
	list = new Move[max_length];
}

MoveStack::MoveStack(const MoveStack& in_movestack)
{
	length = in_movestack.GetLength();
	max_length = in_movestack.GetMaxLength();
	list = new Move[max_length];
	in_movestack.CopyList(list, max_length);
}

MoveStack::MoveStack(int in_length)
{
	length = 0;
	max_length = in_length;
	list = new Move[max_length];
}

MoveStack::~MoveStack()
{
	delete[] list;
}

int MoveStack::GetLength() const
{
	return length;
}

int MoveStack::GetMaxLength() const
{
	return max_length;
}

void MoveStack::CopyList(Move* in_list, int in_length) const
{
	if (in_list && in_length >= 0 && in_length <= max_length)
	{
		std::copy(list, list + in_length, in_list);
	}
}

void MoveStack::Add(
	ull in_from,
	ull in_to,
	byte in_move_piece,
	byte in_capture_piece,
	byte in_end_piece,
	byte in_castle_rights,
	ull in_passant_target,
	int in_half_moves
)
{
	list[length].SetMove(
		in_from,
		in_to,
		in_move_piece,
		in_capture_piece,
		in_end_piece,
		in_castle_rights,
		in_passant_target,
		in_half_moves
	);
}

void MoveStack::Add(const Move& in_move)
{
	if (length < max_length)
	{
		list[length] = in_move;
		length++;
	}
}

Move* MoveStack::Get(int in_index)
{
	if (in_index >= 0 && in_index < length)
	{
		return &list[in_index];
	}
	return NULL;
}

Move* MoveStack::Peak()
{
	return &list[length - 1];
}

Move* MoveStack::Pop()
{
	length--;
	return &list[length];
}

void MoveStack::Remove(int in_remove_length)
{
	length = Functions::clamp(length - in_remove_length, 0, max_length);
}

std::string MoveStack::ToString()
{
	return std::string();
}
