CFLAGS=-Wall -g
CC=g++

trie:
	$(CC) $(CFLAGS) src/trie.hpp driver_programs/trie_main.cpp -o trie

aho:
	$(CC) $(CFLAGS) src/trie.hpp src/aho_corasick.hpp driver_programs/aho_corasick_main.cpp -o aho

clean:
	rm trie.exe & rm trie & rm aho.exe & rm aho
