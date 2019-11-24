gcc -g phase1.c -o re2nfa
./re2nfa p1.in p1.out

gcc -g phase2.c -o nfa2dfa
./nfa2dfa p1.out p2.out

gcc -g phase3.c -o dfa2mindfa
./dfa2mindfa p2.out p3.out

gcc -g phase4.c -o parser
./parser p3.out p4.in

#cat p1.out p2.out p3.out
