#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

int next_state = 0;
#define is_operator(ch)        ((ch == '*') || (ch == '?') || (ch == '+') || (ch == '|') || (ch == '.'))
#define is_unary_operator(ch)  ((ch == '*') || (ch == '?') || (ch == '+'))
#define is_binary_operator(ch) ((ch == '|') || (ch == '.'))
#define is_lp(ch)              (ch == '(')
#define is_rp(ch)              (ch == ')')
#define is_once_or_more(ch)    (ch == '+')

/*
 * retrieve last two states and create a repetition over them
 * q1 -> q2
 * q0 -> q1 - > q2 -> q3
 */
void repetition(nfa_state *nfa, stack *state_stack)
{
	int q0, q1, q2, q3;

	q2 = pop(state_stack);
	q1 = pop(state_stack);
	q0 = next_state++;
	q3 = next_state++;

	nfa[q0].delta[eps] = insert(nfa[q0].delta[eps], q1);
	nfa[q0].delta[eps] = insert(nfa[q0].delta[eps], q3);
	nfa[q2].delta[eps] = insert(nfa[q2].delta[eps], q1);
	nfa[q2].delta[eps] = insert(nfa[q2].delta[eps], q3);

	nfa[q0].status = VALID;
	nfa[q3].status = VALID;

	push(state_stack, q0);
	push(state_stack, q3);
}

/*
 * retrieve last four states and create union over them
 * q0 -> q1 and q2 -> q3
 * q4 -> q0 -> q1 -> q5
 * q4 -> q2 -> q3 -> q5
 */
void re_union(nfa_state *nfa, stack *state_stack)
{
	int q0, q1, q2, q3, q4, q5;

	q1 = pop(state_stack);
	q0 = pop(state_stack);
	q3 = pop(state_stack);
	q2 = pop(state_stack);
	q4 = next_state++;
	q5 = next_state++;

	nfa[q4].delta[eps] = insert(nfa[q4].delta[eps], q0);
	nfa[q4].delta[eps] = insert(nfa[q4].delta[eps], q2);

	nfa[q1].delta[eps] = insert(nfa[q1].delta[eps], q5);
	nfa[q3].delta[eps] = insert(nfa[q3].delta[eps], q5);

	nfa[q4].status = VALID;
	nfa[q5].status = VALID;

	push(state_stack, q4);
	push(state_stack, q5);
}

/*
 * retrieve last two states and add an epsilon move
 * q0 -> q1
 */
void zero_or_once(nfa_state *nfa, stack *state_stack)
{
	int q0, q1;

	q1 = pop(state_stack);
	q0 = pop(state_stack);

	nfa[q0].delta[eps] = insert(nfa[q0].delta[eps], q1);

	push(state_stack, q0);
	push(state_stack, q1);
}

/* retrieve last four states and concatenate
 * q0 -> q1 -> q2 -> q3
 * combine by adding an epsilon move from q1 -> q2
 */
void concatenation(nfa_state *nfa, stack *state_stack)
{
	int q0, q1, q2, q3;

	q3 = pop(state_stack);
	q2 = pop(state_stack);
	q1 = pop(state_stack);
	q0 = pop(state_stack);

	nfa[q1].delta[eps] = insert(nfa[q1].delta[eps], q2);

	push(state_stack, q0);
	push(state_stack, q3);
}

/* create two new states and update delta function on symbol */
void character(nfa_state *nfa, stack *state_stack, int sym)
{
	int q0, q1;

	q0 = next_state++;
	q1 = next_state++;

	nfa[q0].delta[sym] = insert(nfa[q0].delta[sym], q1);

	nfa[q0].status = VALID;
	nfa[q1].status = VALID;

	push(state_stack, q0);
	push(state_stack, q1);
}

int num_new_states(char ch)
{
	switch (ch)
	{
		case '*':
		case '?':
			return 0;
		default:
			return 2;
	}
}

int num_old_states(char ch)
{
	switch (ch)
	{
		case '*':
		case '?':
			return 2;
		case '.':
		case '|':
			return 4;
		default:
			return 0;
	}
}



/* call the corresponding thompson construction method based on the operator */
int evaluate(nfa_state *nfa, stack * state_stack, char ch)
{
	if (next_state + num_new_states(ch) > MAX_STATES) {
		printf("\nCannot handle more than %d states\n", MAX_STATES);
		return 0;
	}

	if (num_old_states(ch) > stack_size(state_stack)) {
		printf("\nUnable to handle this regular expression\n");
		return 0;
	}

	switch(ch) {
		case '*': repetition(nfa, state_stack); break;
		case '.': concatenation(nfa, state_stack); break;
		case '|': re_union(nfa, state_stack); break;
		case '?': zero_or_once(nfa, state_stack); break;
		default: character(nfa, state_stack, to_int(ch)); break;
	}

	return 1;
}

nfa_state * build_nfa(char * re)
{
	int i, start, final;
	stack *op_stack = new_stack(strlen(re));
	stack *state_stack = new_stack(4 * strlen(re));
	nfa_state *nfa = (nfa_state *)calloc(1, MAX_STATES * sizeof(nfa_state));

	if (!nfa)
		goto end;

	for (i=0; i < strlen(re) ; i++) {
		/*
		 * if current char is right paranthesis, evaluate until left paranthesis
		 * if current char is a binary operator or left paranthesis,
		 * save them in operator stack for later evaluation
		 * if current char is a symbol or unary operator,
		 * evaluate, create states as required and push them to state_stack.
		 */
		if (is_rp(re[i])) {
			while (!is_lp(top(op_stack))) {
				if (!evaluate(nfa, state_stack, top(op_stack))) {
					free(nfa); nfa = NULL;
					goto end;
				}
				pop(op_stack);
			}
			pop(op_stack);
		} else if (is_binary_operator(re[i]) || is_lp(re[i])) {
			push(op_stack, re[i]);
		} else {
			if (!evaluate(nfa, state_stack, re[i])) {
				free(nfa); nfa = NULL;
				goto end;
			}
		}
	}

	while (!is_empty(op_stack)) {
		if (!evaluate(nfa, state_stack, top(op_stack))) {
			free(nfa); nfa = NULL;
			goto end;
		}
		pop(op_stack);
	}

	/* Once NFA is built,
	 * state_stack contains two states
	 * - start state and final state */
	final = pop(state_stack);
	start = pop(state_stack);

	nfa[start].status |= START;
	nfa[final].status |= FINAL;

end:
	if (op_stack)
		free_stack(op_stack);
	if (state_stack)
		free_stack(state_stack);
	return nfa;
}


char * insert_concatenation_tokens(char *re)
{
	int i, j;
	char *re_concat = calloc(1, 2 * strlen(re));
	if (re_concat == NULL) return NULL;

	for (i = 0, j = 0; i < strlen(re) ; i++, j++) {

		re_concat[j] = re[i];

		if (i == strlen(re) - 1) break;

		/* insert concatenation token only when next char is not an operator */
		if (!is_binary_operator(re[i]) && !is_lp(re[i]) &&
				!is_operator(re[i + 1]) && !is_rp(re[i+1])) {
			re_concat[++j] = '.';
		}
	}

	return re_concat;
}

char * convert_once_or_more_to_zero_or_more(char *re)
{
	int offset = 0, len;
	char *tmp = NULL, *tmp1 = NULL, *tmp2 = NULL, prev_ch;
	char *re_repetition = calloc(1, 2 * strlen(re));
	if (re_repetition == NULL) return NULL;

	tmp = re;

	do {
		tmp1 = strchr(tmp, '+');

		/* Copy string before '+' as is */
		len = strlen(tmp);
		if (tmp1)
			len -= strlen(tmp1);
		memcpy(re_repetition + offset, tmp, len);
		offset += len;

		/* If '+' is not found, return */
		if (tmp1 == NULL)
			break;

		/* If '+' is found, check if prev_char is ')' */
		prev_ch = *(tmp1 - 1);
		re_repetition[offset++] = '.';
		if (is_rp(prev_ch)) {
			*tmp1 = '\0';
			tmp2 = strrchr(tmp, '(');
			if (tmp2 == NULL)
				goto error;
			memcpy(re_repetition + offset, tmp2, strlen(tmp2));
			offset += strlen(tmp2);
		} else {
			re_repetition[offset++] = prev_ch;
		}

		re_repetition[offset++] = '*';
		tmp = tmp1 + 1;

	} while (tmp != NULL);

	return re_repetition;
error:
	if (re_repetition)
		free(re_repetition);
	return NULL;
}

int validate_pattern(char *re)
{
	char *tmp = strdup(re), *tmp2;

	while ((tmp2 = strrchr(tmp, '*')) != NULL) {
		char ch = *(tmp2 - 1);
		if (is_operator(ch) || is_lp(ch))
			goto error;
		*tmp2 = '\0';
	}

	free(tmp);
	return 0;
error:
	free(tmp);
	return 1;
}

char * tokenize(char *re)
{
	char *re_concat = NULL, *re_tokenized = NULL;
	/* Insert explicit concatenation operator */
	re_concat = insert_concatenation_tokens(re);
	if (NULL == re_concat) {
		goto end;
	}

	re_tokenized = convert_once_or_more_to_zero_or_more(re_concat);
	if (NULL == re_tokenized) {
		goto end;
	}

	if (validate_pattern(re_tokenized)) {
		free(re_tokenized);
		re_tokenized = NULL;
		goto end;
	}

end:
	if (re_concat);
	free(re_concat);
	return re_tokenized;
}


int main(int argc, char *argv[])
{
	int ret = -1;
	FILE *fp_log = stderr;
	FILE *fp_in = NULL, *fp_out = NULL, *fp_dot_out = NULL;
	char *re = NULL, *tokenized_re = NULL;
	size_t len;
	nfa_state *nfa = NULL;
	stack *state_stack = NULL;

	if (argc != 3) {
		fprintf(fp_log, "\nIncorrect syntax."
				"\nsyntax: %s <input file(RE)> <output file(NFA)>\n\n", argv[0]);
		return -1;
	}

	fp_in = fopen(argv[1], "r");
	fp_out = fopen(argv[2], "w");
	fp_dot_out = fopen("nfa.dot", "w");
	if (!fp_in || !fp_out) {
		fprintf(fp_log, "\nFailed to open in/out file");
		goto error;
	}

	while ((len = getline(&re, &len, fp_in)) != -1) {
		/* ignore blank lines and comments */
		if ((strlen(re) != 1) && (re[0] != '#'))
			break;
	}

	if (len < 1) {
		fprintf(fp_log, "\nRE not found in the file");
		goto error;
	}

	if (is_operator(re[0])) {
		fprintf(fp_log, "\nRE cannot start with an operator");
		goto error;
	}

	re[len-1] = '\0';

	tokenized_re = tokenize(re);
	if (NULL == tokenized_re) {
		fprintf(fp_log, "\nFailed to tokenize RE");
		goto error;
	}

	nfa = build_nfa(tokenized_re);

	/* Print NFA info */
	fprintf(fp_out, "\n# RE: %s\n", re);

	print_nfa(nfa, fp_out);

	nfa2dot(nfa, fp_dot_out);

	ret = 0;

error:
	if (re)
		free(re);
	if (tokenized_re)
		free(tokenized_re);
	if (fp_in)
		fclose(fp_in);
	if (fp_out)
		fclose(fp_out);
	if (fp_dot_out)
		fclose(fp_dot_out);
	if (nfa)
		delete_nfa(nfa);
	return ret;
}
