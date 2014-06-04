#ifndef SOL_H
#define SOL_H

#define MAX_LINE_CHARS 80
#define ACCEPT_STATE_CHAR '\n' //Character must be unique to root only

#include <vector>
#include <string.h>

using namespace std;

class Node{
    public:
        Node(char ch);
        ~Node();

        int num_combinations(const char * word);
        void add_word(const char * word);

    private:
        char letter;
        vector<struct Node *> char_list;

        Node * has_char(char ch);
};


#endif
