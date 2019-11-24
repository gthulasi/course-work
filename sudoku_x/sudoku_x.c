#include <stdio.h>
#define UNASSIGNED 0
#define N 9

int num_backtrackings;
int FindUnassignedLocation(int grid[N][N], int *row, int *col);
int isSafe(int grid[N][N], int row, int col, int num);

int SolveSudoku(int grid[N][N])
{
	int row, col, num;

	if (!FindUnassignedLocation(grid, &row, &col))
		return 1;

	for (num = 1; num <= 9; num++)
	{
		if (isSafe(grid, row, col, num))
		{
			grid[row][col] = num;

			if (SolveSudoku(grid))
				return 1;

			grid[row][col] = UNASSIGNED;
			num_backtrackings++;
		}
	}
	return 0;
}

int FindUnassignedLocation(int grid[N][N], int *_row, int *_col)
{
	int row, col;
	for (row = 0; row < N; row++)
		for (col = 0; col < N; col++)
			if (grid[row][col] == UNASSIGNED) {
				*_row = row;
				*_col = col;
				return 1;
			}
	return 0;
}

int UsedInRow(int grid[N][N], int row, int num)
{
	int col;
	for (col = 0; col < N; col++)
		if (grid[row][col] == num)
			return 1;
	return 0;
}

int UsedInCol(int grid[N][N], int col, int num)
{
	int row;
	for (row = 0; row < N; row++)
		if (grid[row][col] == num)
			return 1;

	return 0;
}

int UsedInBox(int grid[N][N], int boxStartRow, int boxStartCol, int num)
{
	int row, col;
	for (row = 0; row < 3; row++)
		for (col = 0; col < 3; col++)
			if (grid[row+boxStartRow][col+boxStartCol] == num)
				return 1;
	return 0;
}

int UsedInDiag(int grid[N][N], int _row, int _col, int num)
{
	int row;
	if (_row != _col) return 0;

	for (row = 0; row < N; row++)
		if (grid[row][row] == num)
			return 1;
	return 0;
}

int UsedInAntiDiag(int grid[N][N], int _row, int _col, int num)
{
	int row;
	if (_row + _col != N - 1) return 0;

	for (row = 0; row < N; row++)
		if (grid[row][N-1-row] == num)
			return 1;
	return 0;
}

int isSafe(int grid[N][N], int row, int col, int num)
{
	return !UsedInRow(grid, row, num) &&
		!UsedInCol(grid, col, num) &&
		!UsedInDiag(grid, row, col, num) &&
		!UsedInAntiDiag(grid, row, col, num) &&
		!UsedInBox(grid, row - row%3 , col - col%3, num)&&
		grid[row][col]==UNASSIGNED;
}

void printGrid(int grid[N][N])
{
	int row, col;
	printf("\n\n");
	for (row = 0; row < N; row++)
	{
		for (col = 0; col < N; col++)
			printf("%2d", grid[row][col]);
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int grid[N][N] = {{0, 0, 0, 0, 0, 3, 0, 0, 0},
		{9, 0, 2, 0, 0, 0, 0, 0, 3},
		{0, 0, 3, 0, 0, 0, 4, 9, 1},
		{3, 0, 0, 1, 0, 7, 6, 0, 0},
		{0, 0, 0, 0, 9, 0, 0, 0, 0},
		{0, 0, 4, 2, 0, 8, 0, 0, 9},
		{4, 9, 6, 0, 0, 0, 5, 0, 0},
		{5, 0, 0, 0, 0, 0, 8, 0, 4},
		{0, 0, 0, 5, 0, 0, 0, 0, 0}};

	printf("Given:\n");
	printGrid(grid);
	printf("\n\n");
	printf("Solution:\n");
	if (SolveSudoku(grid) == 1)
		printGrid(grid);
	else
		printf("No solution exists\n");
	printf("number of backtrackings: %d\n", num_backtrackings);

	return 0;
}
