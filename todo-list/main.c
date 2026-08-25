#include "todo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *k_path = "todos.txt";

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s add <text...>\n", prog);
    fprintf(stderr, "  %s list\n", prog);
    fprintf(stderr, "  %s done <id>\n", prog);
    fprintf(stderr, "  %s delete <id>\n", prog);
}

static int join_args(char *out, size_t cap, char **argv, int start, int end) {
    if (!out || cap == 0 || start >= end) {
        return 0;
    }

    out[0] = '\0';
    for (int i = start; i < end; i++) {
        size_t used = strlen(out);
        if (used > 0) {
            if (used + 2 > cap) {
                return 0;
            }
            out[used] = ' ';
            out[used + 1] = '\0';
            used++;
        }
        size_t arg_len = strlen(argv[i]);
        if (used + arg_len + 1 > cap) {
            return 0;
        }
        memcpy(out + used, argv[i], arg_len + 1);
    }
    return 1;
}

static TodoList *load_or_empty(void) {
    TodoList *list = todo_list_load(k_path);
    if (list) {
        return list;
    }
    return todo_list_create();
}

static int parse_id(const char *text, int *id) {
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (end == text || *end != '\0' || value <= 0) {
        return 0;
    }
    *id = (int)value;
    return 1;
}

static int cmd_add(int argc, char **argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    char text[96];
    if (!join_args(text, sizeof(text), argv, 2, argc)) {
        fprintf(stderr, "Text is too long.\n");
        return 1;
    }

    TodoList *list = load_or_empty();
    if (!list) {
        fprintf(stderr, "Could not open todo list.\n");
        return 1;
    }

    int id = todo_list_add(list, text);
    if (id == 0) {
        fprintf(stderr, "Could not add todo. Avoid empty text and '|' characters.\n");
        todo_list_free(list);
        return 1;
    }

    if (!todo_list_save(list, k_path)) {
        fprintf(stderr, "Could not save '%s'.\n", k_path);
        todo_list_free(list);
        return 1;
    }

    printf("Added #%d %s\n", id, text);
    todo_list_free(list);
    return 0;
}

static int cmd_list(void) {
    TodoList *list = load_or_empty();
    if (!list) {
        fprintf(stderr, "Could not open todo list.\n");
        return 1;
    }

    if (!list->head) {
        printf("(no todos yet)\n");
        todo_list_free(list);
        return 0;
    }

    printf("%-4s %-6s %s\n", "id", "status", "text");
    for (Todo *node = list->head; node; node = node->next) {
        printf("%-4d %-6s %s\n", node->id, node->done ? "done" : "open",
               node->text);
    }

    todo_list_free(list);
    return 0;
}

static int cmd_done(int argc, char **argv) {
    int id = 0;
    if (argc < 3 || !parse_id(argv[2], &id)) {
        print_usage(argv[0]);
        return 1;
    }

    TodoList *list = load_or_empty();
    if (!list) {
        fprintf(stderr, "Could not open todo list.\n");
        return 1;
    }

    if (!todo_list_done(list, id)) {
        fprintf(stderr, "No todo with id %d.\n", id);
        todo_list_free(list);
        return 1;
    }

    if (!todo_list_save(list, k_path)) {
        fprintf(stderr, "Could not save '%s'.\n", k_path);
        todo_list_free(list);
        return 1;
    }

    printf("Marked #%d done\n", id);
    todo_list_free(list);
    return 0;
}

static int cmd_delete(int argc, char **argv) {
    int id = 0;
    if (argc < 3 || !parse_id(argv[2], &id)) {
        print_usage(argv[0]);
        return 1;
    }

    TodoList *list = load_or_empty();
    if (!list) {
        fprintf(stderr, "Could not open todo list.\n");
        return 1;
    }

    if (!todo_list_remove(list, id)) {
        fprintf(stderr, "No todo with id %d.\n", id);
        todo_list_free(list);
        return 1;
    }

    if (!todo_list_save(list, k_path)) {
        fprintf(stderr, "Could not save '%s'.\n", k_path);
        todo_list_free(list);
        return 1;
    }

    printf("Deleted #%d\n", id);
    todo_list_free(list);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        return cmd_add(argc, argv);
    }
    if (strcmp(argv[1], "list") == 0) {
        return cmd_list();
    }
    if (strcmp(argv[1], "done") == 0) {
        return cmd_done(argc, argv);
    }
    if (strcmp(argv[1], "delete") == 0) {
        return cmd_delete(argc, argv);
    }

    print_usage(argv[0]);
    return 1;
}
