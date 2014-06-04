/* Author: Gylmar Alexander Moreno
 * Date: 01/28/14
 *
 * My solution is a directed graph implementation with character-based nodes 
 * and edges represented by a reference to a character immediately proceeding .
 * In order to exhaust every possible substring combination we exploit the 
 * ordered nature of the list and branch our search whenever a word is contained 
 * at the front of our input string. We then have two searches, one with this new 
 * substring at the root of the graph (which has a reference to all words in our
 * word set) and the other our original traversal down the path that continues 
 * from where it branches. This process continues recursively until every 
 * possible path the search word can take is exhausted. We sum all the valid 
 * paths (traversals that finish at an 'end' character that points to root), and
 * find out how many combinations compose our search string.
 *
 * Refer to the README for an example of the execution.
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "sol.h"
using namespace std;

Node * root;

//The only input required is the filename
int main(int argc, char *args[]){
    char * curr = (char *) malloc(MAX_LINE_CHARS*sizeof(char));
    char * max1 = (char *) malloc(MAX_LINE_CHARS*sizeof(char));
    char * max2 = (char *) malloc(MAX_LINE_CHARS*sizeof(char));

    //Variables to keep track of our progress/results
    int sub_word_total = 0;
    int curr_len = 0;
    int max1_len = 0;
    int max2_len = 0;

    root = new Node(ACCEPT_STATE_CHAR);  
    strcpy(max1,"");
    strcpy(max2,"");

    char * filename = args[1];
    ifstream wordFile;
    wordFile.open(filename);

    if(!wordFile){
        perror("Error: File not found!");
        exit(-1);
    }

    //Builds graph from text file
    while(!wordFile.eof()){
        wordFile >> curr;
        root->add_word(curr);
    }

    //We iterate through file again now that the tree is built.
    wordFile.clear();
    wordFile.seekg(0, wordFile.beg);
    while(!wordFile.eof()){

        wordFile >> curr;
        curr_len = strlen(curr);

        if(!wordFile) break;  //Avoids repetition of final line

        if(root->num_combinations(curr) > 1){

            sub_word_total++;

            if(curr_len >= max1_len){

                strcpy(max2, max1);
                max2_len = strlen(max2);

                strcpy(max1, curr);
                max1_len = strlen(max1);

            }else if(curr_len <= max1_len && curr_len > max2_len){

                strcpy(max2,curr);
                max2_len = strlen(max2);

            }
        }
    }

    cout << "Longest Concatenated Word : " << max1 << " at " << max1_len << " characters\n";
    cout << "2nd Longest Concatenated  : " << max2 << " at " << max2_len << " characters\n";
    cout << "Total Concatenated Words  : " << sub_word_total << "\n";

    //Finally we tear everything down!
    wordFile.close();
    delete root;

    free(curr);
    free(max1);
    free(max2);

    curr,max1,max2 = NULL;

    return 0;
}

Node::Node(char ch){
    this->letter = ch;
}

Node::~Node(){
    // Run destructor on allocated memory pointed to 
    //  at each vector index
    for(vector<Node *>::iterator it = this->char_list.begin();
            it < this->char_list.end(); it++){
        if ((*it)->letter == ACCEPT_STATE_CHAR){
            continue; // We ignore the root here to delete in main()
        }
        delete (*it);
    } 
}

/* Input: String of variable length.
 *
 * Results: String processed in one of two possible cases
 *      1) empty     -> We create a cycle back to root,
 *                      allowing for searches to branch back
 *                      to root in search for more subwords.
 *      2) not empty -> We create a child node and recurse
 *                      on it with a substring. This essentially
 *                      creates a path and walks on it while there
 *                      are character remaining in the string.
 */
void Node::add_word(const char * word){
    Node * next;
    if(strlen(word)){
        if(next = this->has_char(word[0])){
            next->add_word(word+1);
        }else{
            (this->char_list).push_back(new Node(word[0]));
            (this->char_list.back())->add_word(word+1);
        }
    }else{
        // Point back to root to run subwords
        (this->char_list).push_back(root);
    }
}

/* Input: String of variable length.
 *
 * Process: We traverse the graph from the calling node,
 *          feeding it a substring at every call.
 *          We branch our recursive calls when we come across a reference 
 *          to root so as to begin this new sub-search with every word
 *          at our disposal. 
 *
 * Output:  We sum all the possibilities as we return each
 *          case's return value from the stack. 
 */
int Node::num_combinations(const char * word){
    Node * child;
    int contain_count = 0;

    if (strlen(word) == 0){
        if (this->has_char(ACCEPT_STATE_CHAR)){
            return 1;  //Accept-state termination is a valid combination
        }else{
            return 0;  //We terminate mid-word if we are not in an 
                       // accept state. This is an invalid combination.
        }
    }else{
        //We pass in word as-is to root calls (think of Epsilon 
        //  traversal from NFAs).
        if (this->has_char(ACCEPT_STATE_CHAR)){
            contain_count += root->num_combinations(word);
        }
        if (child = this->has_char(word[0])){
            //Pass substring as we traverse towards our match character Node
            contain_count += child->num_combinations(word+1);
        }
        
        return contain_count;
    }
}

/* Input: Character whose node we hope to find in our
 *        current node's list of children.
 *
 * Output: Child node in vector whose 'letter' attribute
 *          matches the input character.
 */
Node * Node::has_char(char ch){
    for(vector<Node *>::iterator it = this->char_list.begin();
            it < this->char_list.end(); it++){
       if((*it)->letter == ch){
           return *it;
       }
    } 
    return NULL;
}
