#include "rooks.hh"

int *createRooksMatrix(int size) {
	int m = size * size;
	int n = size + size;

	int *returnArray = new int[m * n];

	for (int i = 0; i < m; i++) {
		// row constraints
		for (int j = 0; j < size; j++) {
			returnArray[i * n + j] = (j == i / size) ? 1 : 0;
		}

		// col constraints
		for (int k = 0; k < size; k++) {
			returnArray[i * n + size + k] = (k == i % size) ? 1 : 0;
		}
	}

	return returnArray;
}
