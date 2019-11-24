#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

int update_set_equivalence(dfa_state *dfa, uint64_t set[])
{
	int changed = 0, i, j, from1, from2, to1, to2, sym;
	uint64_t tmp_set = 0;
	for (i = 0; i < MAX_STATES; i++) {
		if (!set[i]) continue;
		tmp_set = set[i];
		from1 = LSBIT(tmp_set);
		tmp_set = CLEAR_LSBIT(tmp_set);
		while (LSBIT(tmp_set) != MAX_STATES) {
			from2 = LSBIT(tmp_set);
			for (sym = 0; sym < num_alphabet; sym++) {

				if ((dfa[from1].delta[sym] == INVALID_STATE) && (dfa[from2].delta[sym] == INVALID_STATE))
					continue;

				to1 = to2 = INVALID_STATE;

				if (dfa[from1].delta[sym] != INVALID_STATE)
					to1 = find_set(set, dfa[from1].delta[sym]);
				if (dfa[from2].delta[sym] != INVALID_STATE)
					to2 = find_set(set, dfa[from2].delta[sym]);

				if (to1 != to2)
				{
					split_set(set, i, from2);
					changed = 1;
					break;
				}
			}
			tmp_set = CLEAR_LSBIT(tmp_set);
		}
	}
	return changed;
}

dfa_state * dfa_to_min_dfa(dfa_state *dfa)
{
	dfa_state *min_dfa = allocate_dfa(MAX_STATES);
	int i, change = 0, sym, from;
	uint64_t set[MAX_STATES] = {0};

	/* group non final and final states into two sets/groups */
	for (i = 0; dfa[i].status != INVALID; i++) {
		if (dfa[i].status & FINAL)
			set[1] |= (1ULL << i);
		else
			set[0] |= (1ULL << i);
	}

	do {
		change = update_set_equivalence(dfa, set);
	} while (change);

	for (i = 0; i < MAX_STATES; i++) {
		if (set[i] == 0) continue;
		from = LSBIT(set[i]);
		min_dfa[i].status = dfa[from].status;
		for (sym = 0; sym < num_alphabet; sym++) {
			if (dfa[from].delta[sym] == INVALID_STATE) {
				continue;
			}
			min_dfa[i].delta[sym] = find_set(set, dfa[from].delta[sym]);
		}
	}

	return min_dfa;
}

int main(int argc, char *argv[])
{
	int ret = -1;
	FILE *fp_log = stderr;
	FILE *fp_in = NULL, *fp_out = NULL, *fp_dot_out = NULL;
	char *buf = NULL;
	size_t len;
	dfa_state *dfa = NULL, *min_dfa = NULL;

	if (argc != 3) {
		fprintf(fp_log, "\nIncorrect syntax."
				"\nsyntax: %s <input file(DFA)> <output file(min DFA)>\n\n", argv[0]);
		return -1;
	}

	fp_in = fopen(argv[1], "r");
	fp_out = fopen(argv[2], "w");
	fp_dot_out = fopen("mindfa.dot", "w");
	if (!fp_in || !fp_out || !fp_dot_out) {
		fprintf(fp_log, "\nFailed to open in/out file");
		goto error;
	}

	dfa = read_dfa(fp_in);
	if (dfa == NULL) {
		fprintf(fp_log, "\nError: couldn't build dfa from file: %s\n", argv[1]);
		goto error;
	}

	min_dfa = dfa_to_min_dfa(dfa);

	print_dfa(min_dfa, "Min DFA", fp_out);

	dfa2dot(min_dfa, "Min DFA", fp_dot_out);

	ret = 0;

error:
	if (buf)
		free(buf);
	if (fp_in)
		fclose(fp_in);
	if (fp_out)
		fclose(fp_out);
	if (fp_dot_out)
		fclose(fp_dot_out);
	if (dfa)
		free(dfa);
	if (min_dfa)
		free(min_dfa);
	return ret;
}
