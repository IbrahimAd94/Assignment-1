#include "linked_list.h"
#include <stdio.h>    // For printf and fprintf
#include <stdlib.h>   // For malloc and free

void list_init(Node** head, size_t size) {
    *head = NULL;
    (void)size; // Suppress unused parameter warning
}

void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        // The list is empty, new node becomes the head
        *head = new_node;
    } else {
        // Traverse to the end of the list and add the new node
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        fprintf(stderr, "Error: prev_node is NULL in list_insert_after.\n");
        return;
    }
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in list_insert_after.\n");
        return;
    }
    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (next_node == NULL) {
        fprintf(stderr, "Error: next_node is NULL in list_insert_before.\n");
        return;
    }
    if (*head == next_node) {
        // Insert at the start
        Node* new_node = (Node*)malloc(sizeof(Node));
        if (new_node == NULL) {
            fprintf(stderr, "Error: Memory allocation failed in list_insert_before.\n");
            return;
        }
        new_node->data = data;
        new_node->next = *head;
        *head = new_node;
        return;
    }
    Node* current = *head;
    while (current != NULL && current->next != next_node) {
        current = current->next;
    }
    if (current == NULL) {
        fprintf(stderr, "Error: next_node not found in list_insert_before.\n");
        return;
    }
    list_insert_after(current, data);
}

void list_delete(Node** head, uint16_t data) {
    Node* current = *head;
    Node* prev = NULL;
    while (current != NULL && current->data != data) {
        prev = current;
        current = current->next;
    }
    if (current == NULL) {
        fprintf(stderr, "Error: Data %u not found in list_delete.\n", data);
        return;
    }
    if (prev == NULL) {
        // Deleting the head node
        *head = current->next;
    } else {
        prev->next = current->next;
    }
    free(current);
}

Node* list_search(Node** head, uint16_t data) {
    Node* current = *head;
    while (current != NULL) {
        if (current->data == data) {
            return current; // Data found
        }
        current = current->next;
    }
    return NULL; // Data not found
}

void list_display(Node** head) {
    Node* current = *head;
    printf("[");
    while (current != NULL) {
        printf("%u", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    if (*head == NULL) {
        // list is empty
        printf("[]");
        return;
    }

    if (start_node == NULL) {
        // If start_node is NULL then start from the head
        start_node = *head;
    }

    Node* current = start_node;
    printf("[");
    int first = 1; // To handle commas 

    while (current != NULL) {
        if (!first) {
            printf(", ");
        }
        printf("%u", current->data);
        if (current == end_node) {
            break;
        }
        current = current->next;
        first = 0;
    }
    printf("]");
}

int list_count_nodes(Node** head) {
    int count = 0;
    Node* current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void list_cleanup(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    *head = NULL;
}
