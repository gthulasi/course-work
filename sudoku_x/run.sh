gcc sudoku_x.c -o sudoku_x
gcc sudoku_x_sets.c -o sudoku_x_sets

echo "\n\nNaive Backtracking"
./sudoku_x 1
echo "\n\nOrdered Backtracking"
./sudoku_x_sets 1

rm sudoku_x sudoku_x_sets
