/*
Name: Eungjoe Kang
Student ID: 20210013
Description of the file: 
    Prints the number of newlines, words, and characters
    in the text from standard input to standard ouput
    with line(block) comments replaced by an empty string(a space).
    Prints error message if an unterminated comment is detected.
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>


/* declaration */
void decomment(char input_text[], int end_idx);
int count_word(char text[], int end_idx);


/*
Prints the number of newlines, words, and characters
in the text from standard input to standard ouput
with line(block) comments replaced by an empty string(a space).
Prints error message if an unterminated comment is detected.
*/
int main(void)
{
    /* get input text from stdin */
    int idx = 0, end_idx = 0;
    const int max_input_text_length = 2^31;
    char input_text[max_input_text_length];

    char c = 0;
    while ((c = getchar()) != EOF)
        input_text[idx++] = c;
    
    input_text[idx] = EOF; 
    end_idx = idx; // index of EOF


    /* decomment input text */
    enum DFAstate {NONE, SLASH, STAR, LINE_COMMENT, BLOCK_COMMENT, ERROR};

    int state = NONE;
    int is_not_comm = 1;
    int line_num = 1, comm_start_line_num = 1;

    char decomm_text[max_input_text_length];
    int decomm_idx = 0;
    idx = 0;

    while ((idx <= end_idx)&&(state != ERROR))
    {
        c = input_text[idx];
        idx++;

        is_not_comm = ((state != LINE_COMMENT)&&(state != BLOCK_COMMENT)&&(state != STAR));

        // append non-comment, newline, or EOF to decommented text
        if (is_not_comm) {
            decomm_text[decomm_idx] = c;
            decomm_idx++;
        }
        else if ((c == '\n')||(c == EOF)) {
            decomm_text[decomm_idx] = c;
            decomm_idx++;
        }
        
        if (c == '\n') line_num++;

        // DFA for decommenting
        switch (state)
        {
        case NONE:
            if (c == '/') state = SLASH;
            break;

        case SLASH:
            if (c == '/') {
                state = LINE_COMMENT;
                comm_start_line_num = line_num;
                decomm_idx -= 2; // to eliminate // from decommented text
            }
            else if (c == '*') {
                state = BLOCK_COMMENT;
                comm_start_line_num = line_num;
                decomm_idx -= 2; // to eliminate /* from decommented text
                decomm_text[decomm_idx] = ' '; // replace block comment with space
                decomm_idx++; 
            }
            else state = NONE;
            break;

        case STAR:
            if (c == '/') state = NONE;
            else if (c == '*') state = STAR;
            else state = BLOCK_COMMENT;
            break;

        case LINE_COMMENT:
            if (c == '\n') state = NONE;
            break;

        case BLOCK_COMMENT:
            if (c == '*') state= STAR;
            else if (c == EOF) state = ERROR; // detect unterminated comment
            break;
            
        case ERROR:
            break;

        default:
            assert(0);
            break;
        }
    }
    
    int decomm_end_idx = decomm_idx - 1; // index of EOF

    if (state == ERROR)
        fprintf(stderr, "Error: line %d: undetermined comment\n", comm_start_line_num);
    else {
        int num_word = count_word(decomm_text, decomm_end_idx);
        printf("%d %d %d\n", line_num - 1, num_word, decomm_end_idx);
    }
}


/* count the number of words in text[] until index of end_idx */
int count_word(char text[], int end_idx)
{
    enum DFAstate {OUT,IN};

    int state = OUT;

    int idx = 0, num_word = 0;
    char c = 0;
    
    // DFA for counting words
    while (idx < end_idx)
    {
        c = text[idx];
        idx++;

        switch (state)
        {
        case OUT:
            if (!isspace(c)) {
                state = IN;
                num_word++;
            }
            break;

        case IN:
            if (isspace(c))
                state = OUT;   
            break;
        
        default:
            assert(0);
            break;
        }
    }
    
    return num_word;
}
