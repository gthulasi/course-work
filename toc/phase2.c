#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

int search_dfa_sets(dfa_state_set *dfa_sets, uint64_t set)
{
	int i;

	for (i = 0; i < MAX_STATES; i++) {
		if (dfa_sets[i].status == INVALID)
			continue;
		if (dfa_sets[i].state_set == set)
			return i;
	}

	return -1;
}

uint64_t epsilon_closure(nfa_state *nfa, int state)
{
	struct node *node = nfa[state].delta[eps];
	uint64_t set = (1ULL << state);

	while (node != NULL) {
		set |= epsilon_closure(nfa, node->data);
		node = node->next;
	}

	return set;
}

uint64_t sym_closure(nfa_state *nfa, uint64_t state_set, int sym)
{
	int i;
	uint64_t set = 0;

	/* If current state is an element of input set, check where it goes on symbol 'sym' and find their epsilon closures */
	for (i = 0; i < MAX_STATES; i++) {

		if (nfa[i].status == INVALID) continue;

		if (state_set & (1ULL << i)) {
			struct node *node = nfa[i].delta[sym];
			while (node != NULL) {
				set |= epsilon_closure(nfa, node->data);
				node = node->next;
			}
		}
	}

	return set;
}


dfa_state * nfa_to_dfa(nfa_state *nfa)
{
	int i, sym, num_dfa_sets = 0, start = 0, final = 0, set_no = -1;
	uint64_t state_set = 0;
	dfa_state *dfa = NULL;
	dfa_state_set *dfa_sets = (dfa_state_set *)calloc(1, MAX_STATES * sizeof(dfa_state_set));

	start = find_nfa_start_state(nfa);
	final = find_nfa_final_state(nfa);

	dfa_sets[0].state_set = epsilon_closure(nfa, start);
	dfa_sets[0].status = VALID;
	dfa_sets[0].status |= START;
	if (dfa_sets[0].state_set & (1ULL << final)) {
		dfa_sets[0].status |= FINAL;
	}

	num_dfa_sets++;

	for (i = 0; dfa_sets[i].status != INVALID; i++) {
		for (sym = 0; sym < num_alphabet - 1; sym++) {
			state_set = sym_closure(nfa, dfa_sets[i].state_set, sym);
			if (state_set == 0) {
				dfa_sets[i].delta[sym] = INVALID_STATE;
				continue;
			}
			set_no = search_dfa_sets(dfa_sets, state_set);
			if (set_no == -1) {
				set_no = num_dfa_sets;
				num_dfa_sets++;
				/* Create new set */
				dfa_sets[set_no].state_set = state_set;
				dfa_sets[set_no].status = VALID;

				if (state_set & (1ULL << start)) {
					dfa_sets[set_no].status |= START;
				}

				if (state_set & (1ULL << final)) {
					dfa_sets[set_no].status |= FINAL;
				}
			}
			dfa_sets[i].delta[sym] = set_no;
		}
	}

	/* From DFA sets to dfa */
	dfa = allocate_dfa(MAX_STATES);
	for (i=0; i < num_dfa_sets; i++) {
		dfa[i].status = dfa_sets[i].status;
		for (sym = 0; sym < num_alphabet - 1; sym++)
			dfa[i].delta[sym] = dfa_sets[i].delta[sym];
	}

	if (dfa_sets)
		free(dfa_sets);

	return dfa;
}

int main(int argc, char *argv[])
{
	int ret = -1;
	FILE *fp_log = stderr;
	FILE *fp_in = NULL, *fp_out = NULL, *fp_dot_out = NULL;
	char *buf = NULL;
	size_t len;
	nfa_state *nfa = NULL;
	dfa_state *dfa = NULL;

	if (argc != 3) {
		fprintf(fp_log, "\nIncorrect syntax."
				"\nsyntax: %s <input file(NFA)> <output file(DFA)>\n\n", argv[0]);
		return -1;
	}

	fp_in = fopen(argv[1], "r");
	fp_out = fopen(argv[2], "w");
	fp_dot_out = fopen("dfa.dot", "w");
	if (!fp_in || !fp_out || !fp_dot_out) {
		fprintf(fp_log, "\nFailed to open in/out file");
		goto error;
	}

	nfa = read_nfa(fp_in);
	if (nfa == NULL) {
		fprintf(fp_log, "\nError: couldn't build nfa from file: %s\n", argv[1]);
		goto error;
	}

	dfa = nfa_to_dfa(nfa);

	print_dfa(dfa, "DFA", fp_out);

	dfa2dot(dfa, "DFA", fp_dot_out);

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
	if (nfa)
		delete_nfa(nfa);
	if (dfa)
		free(dfa);
	return ret;
}
