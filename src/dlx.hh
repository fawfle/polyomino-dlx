#include <string>
#include <vector>

// Knuth's paper about DLX https://arxiv.org/pdf/cs/0011047

struct Node;
struct ColumnHeader;
class ExactCoverSolver;

// structs are public by default
struct Node {
	Node *left = this;
	Node *right = this;
	Node *up = this;
	Node *down = this;

	int row;

	ColumnHeader *column = nullptr;
};

struct ColumnHeader: Node {
	int size = 0;
	std::string name;
};

// classes are private by default
class ExactCoverSolver {
	private:
		int *matrix;
		Node *rootHeader;
		Node *nodes;
		std::vector<Node *> solution;
		bool searchOneSolution;
		long solutions = 0;
		ColumnHeader* selectSmallestColumn();

		int deepest = 0;
	public:
		ExactCoverSolver(int m, int n, int matrix[], bool searchOneSolution = true);
		bool search(int k = 0);
		void cover(ColumnHeader *col);
		void uncover(ColumnHeader *col);
		std::vector<Node *> getSolution() const { return solution; };
		void printSolution() const;
		void printNumberOfSolutions() const;
		int *getMatrix() const { return matrix; };

		// prepare matrixes that start with zero columns (such as covered grid spaces)
		void coverZeroColumns();
};
