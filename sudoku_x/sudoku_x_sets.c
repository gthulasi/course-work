#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define UNASSIGNED 0
#define NOT_FOUND 0
#define CONSTRAINT_FAILED 2
#define N 9

#define box_no(row, col)             (row / 3 * 3 + col / 3)
#define CLEAR_LSBIT(X)               ((X) & ((X) - 1))
int num_backtrackings;

int LSBIT(uint16_t X)
{
	int pos = 0;
	while (pos < N)
	{
		if (X & (1 << pos))
			return pos;
		pos++;
	}
	return N;
}

int SET_BITS(unsigned int X)
{
	unsigned int c; // the total bits set in n
	for (c = 0; X; X = CLEAR_LSBIT(X))
		c++;
	return c;
}

uint16_t column_mask[N] = {0};
uint16_t row_mask[N] = {0};
uint16_t box_mask[N] = {0};
uint16_t diag_mask = {0};
uint16_t anti_diag_mask = {0};

uint16_t get(int row, int col)
{
	uint16_t mask = 0;
	mask |= column_mask[col];
	mask |= row_mask[row];
	mask |= box_mask[box_no(row, col)];
	if (row == col)
		mask |= diag_mask;
	if ((row + col) == N-1)
		mask |= anti_diag_mask;
	return mask;
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


void set(int grid[N][N], int row, int col)
{
	column_mask[col]            |= 1 << (grid[row][col] - 1);
	row_mask[row]               |= 1 << (grid[row][col] - 1);
	box_mask[box_no(row, col)]  |= 1 << (grid[row][col] - 1);
	if (row == col)
		diag_mask           |= 1 << (grid[row][col] - 1);
	if ((row + col) == N-1)
		anti_diag_mask      |= 1 << (grid[row][col] - 1);
}

void unset(int grid[N][N], int row, int col)
{
	column_mask[col]            ^= 1 << (grid[row][col] - 1);
	row_mask[row]               ^= 1 << (grid[row][col] - 1);
	box_mask[box_no(row, col)]  ^= 1 << (grid[row][col] - 1);
	if (row == col)
		diag_mask           ^= 1 << (grid[row][col] - 1);
	if ((row + col) == N-1)
		anti_diag_mask      ^= 1 << (grid[row][col] - 1);
}

void build_masks(int grid[N][N])
{
	int row, col;

	for (row = 0; row < N; row++)
		for (col = 0; col < N; col++)
			if (grid[row][col] != UNASSIGNED)
				set(grid, row, col);
}


/* Searches the grid to find an entry that is still unassigned and minimum no. of
   possibilities. If found, the reference parameters row, col will be set the location
   that is unassigned, and 1 is returned. If no unassigned entries
   remain, 0 is returned. If there is an unassigned entry, but there isn't any possibility,
   2 is returned to indicate constraint failure */
int FindUnassignedLocation(int grid[N][N], int *_row, int *_col, uint16_t *_mask)
{
	/* Find optimised unassigned location */
	int row, col, max_filled = 0;
	for (row = 0; row < N; row++)
		for (col = 0; col < N; col++)
		{
			if (grid[row][col] != UNASSIGNED)
				continue;

			uint16_t mask = get(row, col);
			int filled = SET_BITS(mask);
			if (filled == N) {
				*_mask = 0;
				return CONSTRAINT_FAILED;
			}

			if (filled > max_filled) {
				max_filled = filled;
				*_mask = mask;
				*_row = row;
				*_col = col;

				/* We cannot find other better than this */
				if (max_filled == N - 1) {
					*_mask ^= 0x1ff;
					return 1;
				}
			}
		}

	if (max_filled) {
		*_mask ^= 0x1ff;
		return 1;
	}

	return NOT_FOUND;
}


int SolveSudoku(int grid[N][N])
{
	int ret, row, col, num;
	uint16_t possibilities = 0;

	ret = FindUnassignedLocation(grid, &row, &col, &possibilities);
	if (ret == NOT_FOUND)
		return 1;

	if (possibilities == 0)
		return 0;

	for (; possibilities; possibilities = CLEAR_LSBIT(possibilities))
	{
		num = LSBIT(possibilities) + 1;
		grid[row][col] = num;
		set(grid, row, col);

		if (SolveSudoku(grid)) {
			return 1;
		}

		unset(grid, row, col);
		grid[row][col] = UNASSIGNED;
		num_backtrackings++;
	}

	return 0;
}


int main()
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

	build_masks(grid);
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
