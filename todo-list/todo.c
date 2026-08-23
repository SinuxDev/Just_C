#include "todo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TodoList *todo_list_create(void) {
    TodoList *list = calloc(1, sizeof(TodoList));
    if (list) {
        list->next_id = 1;
    }
    return list;
}

void todo_list_free(TodoList *list) {
    if (!list) {
        return;
    }

    Todo *node = list->head;
    while (node) {
        Todo *next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

int todo_list_add(TodoList *list, const char *text) {
    if (!list || !text || text[0] == '\0') {
        return 0;
    }
    if (strchr(text, '|') != NULL) {
        return 0;
    }
    if (strlen(text) >= sizeof(((Todo *)0)->text)) {
        return 0;
    }

    Todo *node = calloc(1, sizeof(Todo));
    if (!node) {
        return 0;
    }

    node->id = list->next_id;
    snprintf(node->text, sizeof(node->text), "%s", text);

    if (!list->head) {
        list->head = node;
    } else {
        Todo *tail = list->head;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = node;
    }

    list->next_id++;
    return node->id;
}

Todo *todo_list_find(const TodoList *list, int id) {
    if (!list) {
        return NULL;
    }

    for (Todo *node = list->head; node; node = node->next) {
        if (node->id == id) {
            return node;
        }
    }
    return NULL;
}

int todo_list_done(TodoList *list, int id) {
    Todo *node = todo_list_find(list, id);
    if (!node) {
        return 0;
    }
    node->done = 1;
    return 1;
}

int todo_list_remove(TodoList *list, int id) {
    if (!list || !list->head) {
        return 0;
    }

    Todo *node = list->head;
    Todo *prev = NULL;
    while (node) {
        if (node->id == id) {
            if (prev) {
                prev->next = node->next;
            } else {
                list->head = node->next;
            }
            free(node);
            return 1;
        }
        prev = node;
        node = node->next;
    }
    return 0;
}

int todo_list_count(const TodoList *list) {
    if (!list) {
        return 0;
    }

    int count = 0;
    for (Todo *node = list->head; node; node = node->next) {
        count++;
    }
    return count;
}

int todo_list_save(const TodoList *list, const char *path) {
    if (!list || !path) {
        return 0;
    }

    FILE *file = fopen(path, "w");
    if (!file) {
        return 0;
    }

    for (Todo *node = list->head; node; node = node->next) {
        fprintf(file, "%d|%d|%s\n", node->id, node->done, node->text);
    }
    fclose(file);
    return 1;
}

TodoList *todo_list_load(const char *path) {
    if (!path) {
        return NULL;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    TodoList *list = todo_list_create();
    if (!list) {
        fclose(file);
        return NULL;
    }

    char line[128];
    int ok = 1;
    int max_id = 0;

    while (ok && fgets(line, (int)sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }

        char *first = strchr(line, '|');
        char *second = first ? strchr(first + 1, '|') : NULL;
        if (!first || !second) {
            ok = 0;
            break;
        }

        *first = '\0';
        *second = '\0';

        int id = atoi(line);
        int done = atoi(first + 1);
        const char *text = second + 1;
        if (id <= 0 || (done != 0 && done != 1) || text[0] == '\0') {
            ok = 0;
            break;
        }

        int new_id = todo_list_add(list, text);
        if (new_id == 0) {
            ok = 0;
            break;
        }

        Todo *node = todo_list_find(list, new_id);
        node->id = id;
        node->done = done;
        if (id > max_id) {
            max_id = id;
        }
    }

    fclose(file);
    if (!ok) {
        todo_list_free(list);
        return NULL;
    }

    list->next_id = max_id + 1;
    return list;
}
