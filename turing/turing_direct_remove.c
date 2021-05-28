#include <stdbool.h>
#include <stdio.h>

static int type[] = {0, 0, 0, 'a', 'b', 'b', 'c', 'a', 0, 0};
#define TYPE_SIZE ( sizeof(type)/sizeof(type[0]) )

enum State {
    Q1, Q2, Q3, Q4, Q5, Q6, Q7, Q8, Q9,
    QE
};

void print_type(char symbol);

void machine() {
    enum State state = Q1;
    int type_i = TYPE_SIZE / 2;
    bool running = true;

    while (running) {
        int symbol = type[type_i];
        switch (state) {
            case Q1: {
                if ('a' == symbol || 'b' == symbol || 'c' == symbol) {
                    type_i++;
                } else {
                    type_i--;
                    state = Q2;
                }
                break;
            }
            case Q2: {
                if ('a' == symbol) {
                    type_i--;
                    state = Q3;
                } else if ('b' == symbol) {
                    type_i--;
                    state = Q4;
                } else if ('c' == symbol) {
                    type_i--;
                    state = Q5;
                } else {
                    state = QE;
                }
                break;
            }
            case Q3: {
                if ('a' == symbol || 'b' == symbol || 'c' == symbol) {
                    type_i--;
                } else {
                    type_i++;
                    state = Q6;
                }
                break;
            }
            case Q4: {
                if ('a' == symbol || 'b' == symbol || 'c' == symbol) {
                    type_i--;
                } else {
                    type_i++;
                    state = Q7;
                }
                break;
            }
            case Q5: {
                if ('a' == symbol || 'b' == symbol || 'c' == symbol) {
                    type_i--;
                } else {
                    type_i++;
                    state = Q8;
                }
                break;
            }
            case Q6: {
                if ('a' == symbol) {
                    state = QE;
                } else if ('b' == symbol || 'c' == symbol) {
                    type[type_i] = 0;
                    type_i++;
                    state = Q9;
                } else {
                    state = QE;
                }
                break;
            }
            case Q7: {
                if ('b' == symbol) {
                    state = QE;
                } else if ('a' == symbol || 'c' == symbol) {
                    type[type_i] = 0;
                    type_i++;
                    state = Q9;
                } else {
                    state = QE;
                }
                break;
            }
            case Q8: {
                if ('c' == symbol) {
                    state = QE;
                } else if ('a' == symbol || 'b' == symbol) {
                    type[type_i] = 0;
                    type_i++;
                    state = Q9;
                } else {
                    state = QE;
                }
                break;
            }
            case Q9: {
                if ('a' == symbol || 'b' == symbol || 'c' == symbol) {
                    type[type_i] = 0;
                    type_i++;
                } else {
                    state = QE;
                }
                break;
            }
            default:
            case QE: {
                running = false;
                break;
            }
        }
    }
}

int main() {
    print_type('1');
    machine();
    print_type('2');
    return 0;
}


void print_type(char symbol) {
    int i;

    fprintf(stdout, "%c: ", symbol);
    for (i = 0; i < TYPE_SIZE; ++i) {
        if (type[i] == 0)
            continue;
        fprintf(stdout, "%c", type[i]);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}
