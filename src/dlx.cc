#include <iostream>

#include "dlx.hh"

// stealing pseudocode from a paper is like copying code but smart

bool ExactCoverSolver::search(int k) {
	// if the root header is the only column, the algorithm completes
	if (rootHeader->right == rootHeader) {
		//printSolution();
		// if terminate after one solution, return true
		solutions++;
		return searchOneSolution;
		// return true;
	}

	// Select based on size heuristic
	ColumnHeader* c = selectSmallestColumn();
	// remove selected column
	cover(c);

	// traverse removed column. Select each node and attempt to solve
	for (Node *r = c->down; r != c; r=r->down) {
		solution.push_back(r);
		// cover every column selected row covers
		for (Node *j = r->right; j != r; j=j->right) {
			cover(j->column);
		}
		if (search(k + 1)) {
			return true;
		}
		// order matters here too to properly reconstruct rows
		for (Node *j = r->left; j != r; j=j->left) {
			uncover(j->column);
		}
		solution.pop_back();
	}

	// cleanup
	uncover(c);
	return false;
}

void ExactCoverSolver::cover(ColumnHeader* col) {
	// cover the column header
	col->right->left = col->left;
	col->left->right = col->right;

	// for loops are cool
	// since columns already contain every "1", we go to every column entry and remove it's corresponding row.
	for (Node *row = col->down; row != col; row=row->down) {
		for (Node *j = row->right; j != row; j=j->right) {
			// remove from column. Horizontal links don't need to be removed because the entire row is removed at once.
			j->down->up = j->up;
			j->up->down = j->down;
			j->column->size--;
		}
	}
}

// linked lists are OP because adding is the exact inverse of removing
void ExactCoverSolver::uncover(ColumnHeader* col) {
	// adding rows in reverse order (up instead of down) is very important because columns need to be reconstructed in reverse order
	// removing row nodes is actually order independent, but doing it backwards feels better (the paper also does it backwards)
	for (Node *row = col->up; row != col; row=row->up) {
		for (Node *j = row->left; j != row; j=j->left) {
			j->down->up = j;
			j->up->down = j;
			j->column->size++;
		}
	}

	col->right->left = col;
	col->left->right = col;
}

// pick column with smallest size (# of Nodes). Chosing the column with the fewest ways to fill generally finds dead ends faster
ColumnHeader *ExactCoverSolver::selectSmallestColumn() {
	ColumnHeader *c = rootHeader->right->column;

	// there has to be a better way to do this, but this is fine to keep type safety
	// because we're checking the column, the root header is when i == nullptr
	for (ColumnHeader *i = c->right->column; i != nullptr; i=i->right->column) {
		if (i->size < c->size) {
			c = i;
		}
	}

	return c;
}

void ExactCoverSolver::printSolution() const {
	if (solution.size() == 0) {
		std::cout << "No solutions :( \n";
		return;
	}

	for (uint i = 0; i < solution.size() - 1; i++) {
		std::cout << solution.at(i)->row << ",";
	}
	std::cout << solution.at(solution.size() - 1)->row << std::endl;
}

void ExactCoverSolver::printNumberOfSolutions() const {
	std::cout << "Solutions found: " << solutions << std::endl;
}

void appendToDoublyLinkedListRow(Node *startNode, Node *node) {
	Node *last = startNode->left;
	last->right = node;
	startNode->left = node;

	node->left = last;
	node->right = startNode;
}

void appendToDoublyLinkedListColumn(Node *startNode, Node *node) {
	Node *last = startNode->up;
	last->down = node;
	startNode->up = node;

	node->up = last;
	node->down = startNode;
}

Node *getLinkedListItemRow(Node* startNode, int index) {
	Node *returnNode = startNode;

	for (int i = 0; i < index; i++) {
		returnNode = returnNode->right;
	}

	return returnNode;
}

// create data structure. matrix is m*n elements, 2d arrays are cringe
ExactCoverSolver::ExactCoverSolver(int m, int n, int matrix[], bool searchOneSolution) {
	this->searchOneSolution = searchOneSolution;

	this->matrix = matrix;

	rootHeader = new ColumnHeader;
	// add column headers
	for (int i = 0; i < n; i++) {
		ColumnHeader *newHeader = new ColumnHeader;
		newHeader->column = newHeader;
		appendToDoublyLinkedListRow(rootHeader, newHeader);
	}
	// add rows one at a time
	for (int i = 0; i < m; i++) {
		// adding first node is different. Once it's added, use it as reference to append
		Node *rowNode = new Node;
		for (int j = 0; j < n; j++) {
		if (matrix[i * n + j] == 1) {
				Node *nodeToAdd;
				// need to add first element only
				if (rowNode->column == nullptr) {
					nodeToAdd = rowNode;
				}
				else {
					nodeToAdd = new Node;
					appendToDoublyLinkedListRow(rowNode, nodeToAdd);
				}
				// "casting" the header by getting the column node seems scuffed
				// TODO: ask professor if a seperate column header is good
				nodeToAdd->column = getLinkedListItemRow(rootHeader, j + 1)->column;
				appendToDoublyLinkedListColumn(nodeToAdd->column, nodeToAdd);

				nodeToAdd->column->size++;

				nodeToAdd->row = i;
			}
		}
	}
}

void ExactCoverSolver::coverZeroColumns() {
	for (Node *col = rootHeader->right; col != rootHeader; col = col->right) {
		if (col->column->size == 0) {
			col->left->right = col->right;
			col->right->left = col->left;
		}
	}
}
