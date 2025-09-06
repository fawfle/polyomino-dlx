#include <algorithm>
#include <iostream>

#include "polyomino.hh"

// ###
// ##
int piece[PIECE_SIZE][PIECE_SIZE] = {
	{1, 1, 1},
	{1, 1, 0},
	{0, 0, 0}
};

// ###
//  #
int piece2[PIECE_SIZE][PIECE_SIZE] = {
	{1, 1, 1},
	{0, 1, 0},
	{0, 0, 0}
};

// ##
// #
int piece3[PIECE_SIZE][PIECE_SIZE] = {
	{1, 1, 0},
	{1, 0, 0},
	{0, 0, 0}
};

// ##
// ##
int piece4[PIECE_SIZE][PIECE_SIZE] = {
	{1, 1, 0},
	{1, 1, 0},
	{0, 0, 0}
};

Piece::Piece(int arr[PIECE_SIZE][PIECE_SIZE]) {
	// copy, probably bad or unnecessary idk
	std::copy(&arr[0][0], &arr[0][0]+PIECE_SIZE*PIECE_SIZE, &this->arr[0][0]);

	updateBoundingBox();
}

void createPolyominoMatrix(int grid[GRID_SIZE][GRID_SIZE], std::vector<int> &moves, std::vector<Piece> pieces, int &matrixHeight, int &matrixWidth) {
	int pieceCount = pieces.size();

	matrixWidth = pieceCount + GRID_SIZE * GRID_SIZE;

	// add every possible move for every piece
	for (int i = 0; i < pieceCount; i++) {
		addAllMoves(grid, moves, pieceCount, i, pieces.at(i));
	}

	matrixHeight = moves.size() / matrixWidth;
}

void addAllMoves(int grid[GRID_SIZE][GRID_SIZE], std::vector<int> &moves, int pieceCount, int pieceIndex, Piece piece) {
	std::vector<Piece> pieceOrientations = getUniqueOrientations(piece);

	for (uint i = 0; i < pieceOrientations.size(); i++) {
		Piece *pieceOrientation = &pieceOrientations.at(i);

		addMoveRows(grid, moves, pieceCount, pieceIndex, *pieceOrientation);
	}
}

// for a given piece, add rows to the matrix corresponding to every possible move
void addMoveRows(int grid[GRID_SIZE][GRID_SIZE], std::vector<int> &moves, int pieceCount, int pieceIndex, Piece piece) {
	for (int i = 0; i < GRID_SIZE - piece.effectiveHeight() + 1; i++) {
			for (int j = 0; j < GRID_SIZE - piece.effectiveWidth() + 1; j++) {
				if (!checkValidPlacement(grid, piece, i, j)) continue;
				addMoveRow(grid, moves, pieceCount, pieceIndex, piece, i, j);
		}
	}
}

void addMoveRow(int grid[GRID_SIZE][GRID_SIZE], std::vector<int> &moves, int pieceCount, int pieceIndex, Piece piece, int startI, int startJ) {
	// add piece constraint
	for (int i = 0; i < pieceCount; i++) {
		moves.push_back(i == pieceIndex ? 1 : 0);
	}

	// get what indexes should be covered in advance, there's probaby a better way to do this
	std::vector<int> coveredIndexes;
	for (int i = 0; i < piece.effectiveHeight(); i++) {
		for (int j = 0; j < piece.effectiveWidth(); j++) {
			if (piece.getCellRelative(i, j) != 0) coveredIndexes.push_back(cellToColumnIndex(startI + i, startJ + j));
		}
	}

	// add cell constraints
	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
		bool indexCovered = std::count(coveredIndexes.begin(), coveredIndexes.end(), i) > 0;

		moves.push_back(indexCovered ? 1 : 0);
	}
}

// check piece for overlap using bounding box
bool checkValidPlacement(int grid[GRID_SIZE][GRID_SIZE], Piece piece, int startI, int startJ) {
	for (int i = 0; i < piece.effectiveHeight(); i++) {
		for (int j = 0; j < piece.effectiveWidth(); j++) {
			if (piece.getCellRelative(i, j) != 0 && grid[startI + i][startJ + j] == 1) return false;
		}
	}

	return true;
}

std::vector<Piece> getUniqueOrientations(Piece piece) {
	std::vector<Piece> res;

	res.push_back(piece);

	for (int i = 0; i < 3; i++) {
		piece.rotate();

		bool added = false;
		for (uint j = 0; j < res.size(); j++) {
			if (res[j] == piece) {
				added = true;
				break;
			}
		}

		if (!added) res.push_back(piece);
	}

	return res;
}

void Piece::updateBoundingBox() {
	// get startI
	startI = 0;
	for (int i = 0; i < PIECE_SIZE - 1; i++) {
		if (checkZeroRow(i)) startI = i + 1;
		else break;
	}
	
	// get endI
	endI = PIECE_SIZE - 1;
	for (int i = PIECE_SIZE - 1; i > 0; i--) {
		if (checkZeroRow(i)) endI = i - 1;
		else break;
	}

	startJ = 0;
	for (int j = 0; j < PIECE_SIZE - 1; j++) {
		if (checkZeroCol(j)) startJ = j + 1;
		else break;
	}

	endJ = PIECE_SIZE - 1;
	for (int j = PIECE_SIZE - 1; j > 0; j--) {
		if (checkZeroCol(j)) endJ = j - 1;
		else break;
	}
}

bool Piece::checkZeroRow(int row) const {
	for (int j = 0; j < PIECE_SIZE; j++) {
		if (arr[row][j] != 0) return false;
	}
	return true;
}

bool Piece::checkZeroCol(int col) const {
	for (int i = 0; i < PIECE_SIZE; i++) {
		if (arr[i][col] != 0) return false;
	}
	return true;
}

// transposing the matrix and flipping it across the y axis rotates it by 90 degrees counterclockwise
// there's gotta be a better way, but performance doesn't really matter here
void Piece::rotate() {
	// std::cout << "\nBefore: \n";
	// printPiece();

	int tempPieceArr[PIECE_SIZE][PIECE_SIZE];

	// https://stackoverflow.com/questions/18709577/stdcopy-two-dimensional-array
	std::copy(&arr[0][0], &arr[0][0]+PIECE_SIZE*PIECE_SIZE, &tempPieceArr[0][0]);

	// std::cout << "\nTEMP: \n";
	// printPiece(tempPiece);

	// transpose
	// y = x;
	// x = y;
	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			arr[j][i] = tempPieceArr[i][j];
		}
	}

	// istd::cout << "\nAfter Transpose \n";
	// printPiece(piece);

	// flip y
	std::copy(&arr[0][0], &arr[0][0]+PIECE_SIZE*PIECE_SIZE, &tempPieceArr[0][0]);

	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			arr[i][j] = tempPieceArr[PIECE_SIZE - 1 - i][j];
		}
	}

	// std::cout << "\nAfter Flip \n";
	// printPiece();

	updateBoundingBox();
}

// compare pieces ignoring bounding box
bool operator == (Piece p1, Piece p2) {
	// compare bounding box
	if (p1.effectiveWidth() != p2.effectiveWidth() || p1.effectiveHeight() != p2.effectiveHeight()) return false;

	for (int i = 0; i < p1.effectiveHeight(); i++) {
		for (int j = 0; j < p1.effectiveWidth(); j++) {
			if  (p1.getCellRelative(i, j) != p2.getCellRelative(i, j)) return false;
		}
	}

	return true;
}

bool operator != (Piece p1, Piece p2) {
	return !(p1 == p2);
}

void Piece::printPiece() const {
	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			std::cout << arr[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void Piece::printBoundingBox() const {
	std::cout << "sI: " << startI << " eI: " << endI << " sJ: " << startJ << " eJ: " << endJ << std::endl;
}

int getGridSize(int arr[GRID_SIZE][GRID_SIZE]) {
	return GRID_SIZE * GRID_SIZE;
	int count = 0;
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			if (arr[i][j] == 0) count++;
		}
	}
	return count;
}


int cellToColumnIndex(int i, int j) { 
	return i * GRID_SIZE + j; 
}

void columnToCellIndex(int columnIndex, int &i, int &j) {
	i = columnIndex / GRID_SIZE;
	j = columnIndex % GRID_SIZE;
}
