#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "list.h"
#include "sets.h"

#define MAX_STATES             MAX_BITS
#define INVALID_STATE          MAX_STATES

#define INVALID 0
#define VALID   (1 << 0)
#define START   (1 << 1)
#define FINAL   (1 << 2)

#define num_alphabet           (10 + 26 + 26 + 1)
#define eps                    (num_alphabet - 1)

int to_int(char ch)
{
	if ((ch >= 0x30) && (ch <= 0x39))
		return ch - 0x30;
	else if ((ch >= 0x41) && (ch <= 0x5A))
		return ch - 0x41 + 10;
	else if ((ch >= 0x61) && (ch <= 0x7A))
		return ch - 0x61 + 36;
	return -1;
}

char to_char(int c)
{
	if ((c >= 0) && (c <= 9))
		return c + 0x30;
	else if ((c >= 10) && (c < 36))
		return c + 0x41 - 10;
	else if ((c >= 36) && (c < 62))
		return c + 0x61 - 36;
	return -1;
}


typedef struct {
	list * delta[num_alphabet];
	int status;
} nfa_state;

typedef struct {
	uint64_t state_set;
	int delta[num_alphabet];
	int status;
} dfa_state_set;

typedef struct {
	int delta[num_alphabet];
	int status;
} dfa_state;

void print_nfa_start_state(nfa_state *nfa, FILE *fp)
{
	int i = 0;

	fprintf(fp, "\nStart State: ");
	while (i < MAX_STATES) {
		if (nfa[i].status & START) {
			fprintf(fp, " q%d", i);
			break;
		}
		i++;
	}

}

void print_nfa_final_state(nfa_state *nfa, FILE *fp)
{
	int i = 0;

	fprintf(fp, "\nFinal State: ");
	while (i < MAX_STATES) {
		if (nfa[i].status & FINAL)
			fprintf(fp, " q%d", i);
		i++;
	}
	fprintf(fp, "\n");

}

void print_all_nfa_states(nfa_state *nfa, FILE *fp)
{
	int i = 0;

	fprintf(fp, "\nState(s): ");
	while (i < MAX_STATES) {
		if (nfa[i].status & VALID)
			fprintf(fp, " q%d", i);
		i++;
	}
	fprintf(fp, "\n");
}


void print_nfa(nfa_state * nfa, FILE *fp)
{
	int i, sym;

	if (!nfa || !fp) return;

	fprintf(fp, "\nNFA:");
	print_all_nfa_states(nfa, fp);
	print_nfa_start_state(nfa, fp);
	print_nfa_final_state(nfa, fp);
	fprintf(fp, "\n\nState\tSymbol\tState\n");
	for (i = 0; i < MAX_STATES; i++) {
		if (nfa[i].status == INVALID) continue;
		for (sym = 0; sym < num_alphabet; sym++) {
			struct node *node = nfa[i].delta[sym];
			while (node != NULL) {
				if (sym == eps)
					fprintf(fp, "q%d\teps\tq%d\n", i, node->data);
				else
					fprintf(fp, "q%d\t%c\tq%d\n", i, to_char(sym), node->data);
				node = node->next;
			}
		}
	}
	fprintf(fp, "\n");
}

void delete_nfa(nfa_state *nfa)
{
	int i, sym;

	if (!nfa) return;
	for (i = 0; i < MAX_STATES; i++) {
		if (nfa[i].status == INVALID) continue;
		for (sym = 0; sym < num_alphabet; sym++) {
			if (nfa[i].delta[sym])
				delete_list(nfa[i].delta[sym]);
		}
	}
	free(nfa);
}

int find_dfa_start_state(dfa_state *dfa)
{
	int i = 0;
	while (i < MAX_STATES) {
		if (dfa[i].status & START)
			return i;
		i++;
	}
	return i;
}


void print_dfa_start_state(dfa_state *dfa, FILE *fp)
{
	fprintf(fp, "\nStart State: q%d", find_dfa_start_state(dfa));
}

void print_dfa_final_states(dfa_state *dfa, FILE *fp)
{
	int i = 0;

	fprintf(fp, "\nFinal State(s): ");
	while (i < MAX_STATES) {
		if (dfa[i].status & FINAL)
			fprintf(fp, " q%d", i);
		i++;
	}
	fprintf(fp, "\n");
}

void print_all_dfa_states(dfa_state *dfa, FILE *fp)
{
	int i = 0;

	fprintf(fp, "\nState(s): ");
	while (i < MAX_STATES) {
		if (dfa[i].status & VALID)
			fprintf(fp, " q%d", i);
		i++;
	}
	fprintf(fp, "\n");
}

void print_dfa(dfa_state * dfa, char *title, FILE *fp)
{
	int i = 0, sym;

	if (!dfa || !fp) return;

	fprintf(fp, "\n%s:", title);
	print_all_dfa_states(dfa, fp);
	print_dfa_start_state(dfa, fp);
	print_dfa_final_states(dfa, fp);

	fprintf(fp, "\n\nState\tSymbol\tState\n");
	for (i = 0; i < MAX_STATES; i++) {
		if (dfa[i].status == INVALID) continue;
		for (sym = 0; sym < num_alphabet - 1; sym++) {
			if (dfa[i].delta[sym] == INVALID_STATE) continue;
			fprintf(fp, "q%d\t%c\tq%d\n", i, to_char(sym), dfa[i].delta[sym]);
		}
	}
	fprintf(fp, "\n");
}

dfa_state * allocate_dfa(int num_states)
{
	int st, sym;
	dfa_state *dfa = (dfa_state *)calloc(1, num_states * sizeof(dfa_state));
	for (st=0; st < num_states; st++)
		for (sym=0; sym < num_alphabet; sym++)
			dfa[st].delta[sym] = INVALID_STATE;
	return dfa;
}

int find_nfa_start_state(nfa_state *nfa)
{

	int i = 0;
	while (i < MAX_STATES) {
		if (nfa[i].status & START)
			return i;
		i++;
	}
}

int find_nfa_final_state(nfa_state *nfa)
{
	int i = 0;
	while (i < MAX_STATES) {
		if (nfa[i].status & FINAL)
			return i;
		i++;
	}
}

nfa_state *read_nfa(FILE *fp_in)
{
	int from = 0, to = 0, sym = 0, reading_states = 0, start_st = 0, final_st = 0;
	size_t len;
	char sym_char, *buf = NULL;
	nfa_state *nfa = (nfa_state *)calloc(1, MAX_STATES * sizeof(nfa_state));

	while ((len = getline(&buf, &len, fp_in)) != -1) {
		/* ignore blank lines and comments */
		if ((strlen(buf) == 1) || (buf[0] == '#')) continue;

		if (!reading_states && strstr(buf, "Start State:")) {
			if (1 != sscanf(buf, "Start State: q%d\n", &start_st)) {
				free(nfa); nfa = NULL;
				goto end;
			}
		} else if (!reading_states && strstr(buf, "Final State")) {
			if (1 != sscanf(buf, "Final State: q%d\n", &final_st)) {
				free(nfa); nfa = NULL;
				goto end;
			}
		} else if (!reading_states && strstr(buf, "State\tSymbol\tState")) {
			reading_states = 1;
		} else if (reading_states) {
			if (strstr(buf, "eps")) {
				if (2 != sscanf(buf, "q%d\teps\tq%d\n", &from, &to)) {
					free(nfa); nfa = NULL;
					goto end;
				}
				sym = eps;
			} else {
				if (3 != sscanf(buf, "q%d\t%c\tq%d\n", &from, &sym_char, &to)) {
					free(nfa); nfa = NULL;
					goto end;
				}
				sym = to_int(sym_char);
			}
			nfa[from].delta[sym] = insert(nfa[from].delta[sym], to);
			nfa[from].status = VALID;
			nfa[to].status = VALID;
		}
	}

	nfa[start_st].status |= START;
	nfa[final_st].status |= FINAL;
end:
	if (buf) free(buf);
	return nfa;
}

dfa_state *read_dfa(FILE *fp_in)
{
	int from = 0, to = 0, sym = 0, reading_states = 0, start = 0;
	size_t len;
	char sym_char, *buf = NULL, *final_buf = NULL;
	dfa_state *dfa = allocate_dfa(MAX_STATES);

	while ((len = getline(&buf, &len, fp_in)) != -1) {
		/* ignore blank lines and comments */
		if ((strlen(buf) == 1) || (buf[0] == '#')) continue;

		if (!reading_states && strstr(buf, "Start State:")) {
			if (1 != sscanf(buf, "Start State: q%d\n", &start)) {
				free(dfa); dfa = NULL;
				goto end;
			}
		} else if (!reading_states && strstr(buf, "Final State(s): ")) {
			final_buf = strdup(buf + strlen("Final State(s): "));
		} else if (!reading_states && strstr(buf, "State\tSymbol\tState")) {
			reading_states = 1;
		} else if (reading_states) {
			if (3 != sscanf(buf, "q%d\t%c\tq%d\n", &from, &sym_char, &to)) {
				free(dfa); dfa = NULL;
				goto end;
			}
			sym = to_int(sym_char);
			dfa[from].delta[sym] = to;
			dfa[from].status = VALID;
			dfa[to].status = VALID;
		}
	}

	if (dfa[start].status & VALID)
		dfa[start].status |= START;

	if (final_buf) {
		char *tmp_ptr = final_buf;
		if (buf) free(buf);
		buf = strtok(tmp_ptr, " q");
		do {
			int st_no = atoi(buf);
			if (dfa[st_no].status & VALID)
				dfa[st_no].status |= FINAL;
		} while ((buf = strtok(NULL, " q")) != NULL);
	}

end:
	if (buf) free(buf);
	if (final_buf) free(final_buf);
	return dfa;
}

void nfa2dot(nfa_state * nfa, FILE *fp)
{
	int i, sym;

	if (!nfa || !fp) return;

	fprintf(fp, "digraph NFA { rankdir=\"LR\"; rank = \"same\"; "
			"node [shape=\"circle\"]; "
			"init [label=\"\" shape=\"point\"] "
			"label = \"NFA\" labelloc = \"t\" size=\"8,8\"\n");
	for (i = 0; i < MAX_STATES; i++) {
		if (nfa[i].status == INVALID) continue;
		if (nfa[i].status & START) {
			fprintf(fp, "init -> q%d [label=\"START\"];\n", i);
		}
		if (nfa[i].status & FINAL) {
			fprintf(fp, "q%d [shape=\"doublecircle\"];\n", i);
		}

		for (sym = 0; sym < num_alphabet; sym++) {
			struct node *node = nfa[i].delta[sym];
			while (node != NULL) {
				if (sym == eps)
					fprintf(fp, "q%d -> q%d [label=\"eps\"];\n", i, node->data);
				else
					fprintf(fp, "q%d -> q%d [label=\"%c\"];\n", i, node->data, to_char(sym));
				node = node->next;
			}
		}
	}
	fprintf(fp, "}\n");
}

void dfa2dot(dfa_state * dfa, char *title, FILE *fp)
{
	int i = 0, sym;

	if (!dfa || !fp) return;

	fprintf(fp, "digraph DFA { rankdir=\"LR\"; rank = \"same\"; "
			"node [shape=\"circle\"]; "
			"init [label=\"\" shape=\"point\"] "
			"label = \"%s\" labelloc = \"t\" size=\"8,8\"\n", title);
	for (i = 0; i < MAX_STATES; i++) {
		if (dfa[i].status == INVALID) continue;
		if (dfa[i].status & START) {
			fprintf(fp, "init -> q%d [label=\"START\"];\n", i);
		} 
		if (dfa[i].status & FINAL) {
			fprintf(fp, "q%d [shape=\"doublecircle\"];\n", i);
		}

		for (sym = 0; sym < num_alphabet - 1; sym++) {
			if (dfa[i].delta[sym] == INVALID_STATE) continue;
			fprintf(fp, "q%d -> q%d [label=\"%c\"];\n", i, dfa[i].delta[sym], to_char(sym));
		}
	}
	fprintf(fp, "}\n");
}
