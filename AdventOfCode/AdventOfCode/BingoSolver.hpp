#include <vector>

class BingoCard5x5
{
public:
	BingoCard5x5( const std::vector<int>& boardNumbers );

	// Returns true on win
	bool MarkNumber(int newNumber);

private:
	bool CheckForWin();
	int GetRow(int index);
	int GetColumn(int index);
	int GetDiagonal(int index);

private:
	std::vector<int> numbers;
	std::vector<bool> isNumberMarked;
	short rowMatches[5] = { 0 };
	short columnMatches[5] = { 0 };
	short diagonalMatches[2] = { 0 };
};

class BingoSolver
{
public:
	void InitializeFromFile( const char* filename );

	int Solve();

private:
	std::vector<BingoCard5x5> m_bingoCards;
};
