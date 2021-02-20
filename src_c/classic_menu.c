#include <stdlib.h>
#include <stdio.h>

#define MENU_ITEM_NAME_SIZE 5

struct MenuItem {
    char name[MENU_ITEM_NAME_SIZE];

    void (*MenuCallback)(struct MenuItem *self);

    struct MenuItem *next;
};

static void menu_print(struct MenuItem *item) {
    fprintf(stdout, "Selected: %s\n", item->name);
    fflush(stdout);
}

enum Select {
    SelectNext, SelectPrev, SelectDo, SelectCancel
};

static enum Select keyboard_select() {
    while (1) {
        switch (fgetc(stdin)) {
            case 'n':
                return SelectNext;
            case 'p':
                return SelectPrev;
            case 'd':
                return SelectDo;
            case '\n':
                continue;
            case 'c':
            default:
                return SelectCancel;
        }
    }
}

static void default_selector(struct MenuItem *item) {
    struct MenuItem *prev = item->next;
    struct MenuItem *curr = item->next;
    enum Select select;

    menu_print(curr);
    while ((select = keyboard_select()) != SelectCancel) {
        switch (select) {
            case SelectPrev: {
                struct MenuItem *tmp = curr;
                curr = prev;
                prev = tmp;
                break;
            }
            case SelectNext: {
                if (0 != curr->next) {
                    prev = curr;
                    curr = curr->next;
                }
                break;
            }
            case SelectDo: {
                if (0 != curr->MenuCallback) {
                    (*curr->MenuCallback)(curr);
                }
                break;
            }
            default:
                break;
        }
        menu_print(curr);
    }
}

static void item1_cb(struct MenuItem *item) {
    fprintf(stdout, "Do item0\n");
    fflush(stdout);
}

struct MenuItem item0 = {"item0", item1_cb};
struct MenuItem item1 = {"item1", default_selector, &item0};


int main() {
    default_selector(&item1);
    return EXIT_SUCCESS;
}

