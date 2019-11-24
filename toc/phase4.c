#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

int parse_dfa(dfa_state *dfa, int start, char *str)
{
	int i, from = start;

	for (i = 0; i < strlen(str); i++) {
		int sym = to_int(str[i]);
		if (from == INVALID_STATE) {
			return 0;
		}
		from = dfa[from].delta[sym];
	}

	if (dfa[from].status & FINAL)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	int ret = -1, start = 0;
	FILE *fp_log = stderr;
	FILE *fp_in = NULL, *fp_str_in = NULL, *fp_out = stdout;
	char *buf = NULL;
	size_t len;
	dfa_state *dfa = NULL;

	if (argc != 3) {
		fprintf(fp_log, "\nIncorrect syntax."
				"\nsyntax: %s <input file(DFA)> <input file(Set of strings)>\n\n", argv[0]);
		return -1;
	}

	fp_in = fopen(argv[1], "r");
	fp_str_in = fopen(argv[2], "r");
	if (!fp_in || !fp_str_in) {
		fprintf(fp_log, "\nFailed to open in/out file");
		goto error;
	}

	dfa = read_dfa(fp_in);
	if (dfa == NULL) {
		fprintf(fp_log, "\nError: couldn't build dfa from file: %s\n", argv[1]);
		goto error;
	}

	start = find_dfa_start_state(dfa);
	if (start == INVALID_STATE) {
		fprintf(fp_log, "\nError: Invalid start state: %d\n", start);
		goto error;
	}

	while ((len = getline(&buf, &len, fp_str_in)) != -1) {

		/* ignore blank lines */
		if (strlen(buf) == 1) continue;
		if (buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		}

		if (parse_dfa(dfa, start, buf)) {
			fprintf(fp_out, "\x1b[32m%s \t: accepted \x1b[0m\n", buf);
		} else {
			fprintf(fp_out, "\x1b[31m%s \t: rejected \x1b[0m\n", buf);
		}
	}

	ret = 0;

error:
	if (buf)
		free(buf);
	if (fp_in)
		fclose(fp_in);
	if (fp_str_in)
		fclose(fp_str_in);
	return ret;
}
