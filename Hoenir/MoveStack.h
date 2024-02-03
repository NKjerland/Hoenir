#ifndef MOVESTACK
#define MOVESTACK

#include<string>
#include<algorithm>

#include"Functions.h"
#include"Move.h"

/*
* TODO:
* WRITE:
* ToString()
*/

class MoveStack
{
public:

	const static int MAX_MOVES_POSITION = 218;
	const static int MAX_MOVES_FULL_GAME = 12000;

private:
	
	Move* list;
	int length;
	int max_length;

public:

	MoveStack();
	MoveStack(const MoveStack& in_movestack);
	MoveStack(int in_length);

	~MoveStack();

	int GetLength() const;
	int GetMaxLength() const;
	void CopyList(Move* in_list, int in_length) const;

	void Add(
		ull in_from,
		ull in_to,
		byte in_move_piece,
		byte in_capture_piece,
		byte in_end_piece,
		byte in_castle_rights,
		ull in_passant_target,
		int in_half_moves
	);

	void Add(const Move& in_move);
	Move* Get(int in_index);
	Move* Peak();
	Move* Pop();
	void Remove(int in_remove_length);
	//void Resize(int in_size);
	//void Reset();

	std::string ToString();

	Move& operator[](size_t in_index) {
		if (in_index >= 0 && in_index < length)
		{
			return list[in_index];
		}
		return list[0];
	}
};

#endif MOVESTACK