dot nfa.dot -Tps -O
dot dfa.dot -Tps -O
dot mindfa.dot -Tps -O

evince nfa.dot.ps dfa.dot.ps mindfa.dot.ps
