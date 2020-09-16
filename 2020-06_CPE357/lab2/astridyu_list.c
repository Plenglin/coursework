#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct listelement {
    struct listelement *prev, *next;
    char text[1000];
};

struct listelement *head = NULL;
struct listelement *tail = NULL;

void set_listelement_text(struct listelement *node, char *text) {
    strncpy(node->text, text, 999);
    node->text[999] = '\0';
}

struct listelement *create_listelement(struct listelement *prev, char *text) {
    struct listelement *node = malloc(sizeof(struct listelement));
    node->prev = prev;
    node->next = NULL;
    set_listelement_text(node, text);

    if (prev != NULL) {
        prev->next = node;
    }
    return node;
}

struct listelement *delete_listelement(struct listelement *node) {
    struct listelement *next = node->next;
    struct listelement *prev = node->prev;

    if (next != NULL) {
        next->prev = prev;
    }
    if (prev != NULL) {
        prev->next = next;
    }

    free(node);

    return prev;
}

void append_to_list(char *text) {
    if (head == NULL && tail == NULL) {
        head = create_listelement(NULL, text);
        tail = head;
    } else {
        tail = create_listelement(tail, text);
    }
}

int delete_from_list() {
    if (head == tail) {
        if (head == NULL) {  // This is an empty list.
            return 1;
        } else {  // This is a single-element list.
            delete_listelement(head);
            head = NULL;
            tail = NULL;
        }
    } else {
        tail = delete_listelement(tail);
    }
    return 0;
}

void print_list(struct listelement *node) {
    putchar('[');
    if (node != NULL) {
        printf("%s", node->text);
        node = node->next;
        while (node != NULL) {
            printf(", %s", node->text);
            node = node->next;
        }
    }
    printf("]\n");
}

int main() {
    char line[1000];
    while (1) {
        printf("1 push string\n");
        printf("2 print list\n");
        printf("3 delete item\n");
        printf("4 end program\n");

        int command;
        scanf("%d", &command);

        switch (command) {
            case 1: 
                printf("Enter text:\n");
                scanf("%s", line);
                append_to_list(line);
                printf("Pushed %s to list\n", line);
                break;
            case 2: 
                print_list(head);
                break;
            case 3: 
                if (delete_from_list()) {
                    printf("List was empty. Nothing was done.\n");
                } else {
                    printf("Deleted from the list.\n");
                }
                break;
            case 4:
                printf("Goodbye!\n");
                return 0;
            default: 
                printf("Invalid command.\n");
        }
        printf("\n");
    }
    return 0;
}