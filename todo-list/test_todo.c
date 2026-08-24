#include "todo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_passed = 0;
static int g_failed = 0;

#define EXPECT(cond, msg)                                                      \
    do {                                                                       \
        if (cond) {                                                            \
            g_passed++;                                                        \
        } else {                                                               \
            g_failed++;                                                        \
            fprintf(stderr, "FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__);    \
        }                                                                      \
    } while (0)

static void test_create_empty_list(void) {
    TodoList *list = todo_list_create();

    EXPECT(list != NULL, "create returns a list");
    if (!list) {
        return;
    }

    EXPECT(list->head == NULL, "new list has no nodes");
    EXPECT(todo_list_count(list) == 0, "new list count is zero");
    todo_list_free(list);
}

static void test_add_appends_linked_nodes(void) {
    TodoList *list = todo_list_create();
    EXPECT(list != NULL, "create returns a list");
    if (!list) {
        return;
    }

    EXPECT(todo_list_add(list, "buy milk") == 1, "first id is 1");
    EXPECT(todo_list_add(list, "write code") == 2, "second id is 2");
    EXPECT(todo_list_count(list) == 2, "list has two todos");
    EXPECT(list->head != NULL, "head is not NULL after add");
    EXPECT(list->head->next != NULL, "nodes are linked");
    EXPECT(strcmp(list->head->text, "buy milk") == 0, "first node text");
    EXPECT(strcmp(list->head->next->text, "write code") == 0, "second node text");
    EXPECT(list->head->done == 0, "new todo is not done");
    EXPECT(list->head->next->next == NULL, "list ends after two nodes");

    todo_list_free(list);
}

static void test_add_rejects_bad_input(void) {
    TodoList *list = todo_list_create();
    EXPECT(list != NULL, "create returns a list");
    if (!list) {
        return;
    }

    EXPECT(todo_list_add(list, "") == 0, "empty text is rejected");
    EXPECT(todo_list_add(list, NULL) == 0, "NULL text is rejected");
    EXPECT(todo_list_add(list, "a|b") == 0, "pipe in text is rejected");
    EXPECT(todo_list_count(list) == 0, "rejected adds do not change the list");

    todo_list_free(list);
}

static void test_done_and_remove(void) {
    TodoList *list = todo_list_create();
    EXPECT(list != NULL, "create returns a list");
    if (!list) {
        return;
    }

    todo_list_add(list, "one");
    todo_list_add(list, "two");
    todo_list_add(list, "three");

    EXPECT(todo_list_done(list, 2) == 1, "marking id 2 succeeds");
    EXPECT(todo_list_find(list, 2) != NULL, "id 2 still exists");
    EXPECT(todo_list_find(list, 2)->done == 1, "id 2 is done");
    EXPECT(todo_list_find(list, 1)->done == 0, "id 1 is still open");

    EXPECT(todo_list_remove(list, 1) == 1, "removing head succeeds");
    EXPECT(todo_list_find(list, 1) == NULL, "id 1 is gone");
    EXPECT(todo_list_count(list) == 2, "two todos remain");
    EXPECT(strcmp(list->head->text, "two") == 0, "head is now two");

    EXPECT(todo_list_remove(list, 99) == 0, "missing id is rejected");
    EXPECT(todo_list_done(list, 99) == 0, "missing id cannot be marked done");

    todo_list_free(list);
}

static void test_save_and_load(void) {
    const char *path = "test_todos.txt";
    TodoList *list = todo_list_create();
    EXPECT(list != NULL, "create returns a list");
    if (!list) {
        return;
    }

    todo_list_add(list, "buy milk");
    todo_list_add(list, "write code");
    todo_list_done(list, 2);
    EXPECT(todo_list_save(list, path) == 1, "save succeeds");
    todo_list_free(list);

    TodoList *loaded = todo_list_load(path);
    EXPECT(loaded != NULL, "load returns a list");
    if (loaded) {
        EXPECT(todo_list_count(loaded) == 2, "loaded list has two todos");
        EXPECT(strcmp(loaded->head->text, "buy milk") == 0, "loaded first text");
        EXPECT(loaded->head->done == 0, "loaded first is open");
        EXPECT(strcmp(loaded->head->next->text, "write code") == 0,
               "loaded second text");
        EXPECT(loaded->head->next->done == 1, "loaded second is done");
        EXPECT(todo_list_add(loaded, "new") == 3, "next id continues after load");
        todo_list_free(loaded);
    }

    EXPECT(todo_list_load("this-file-does-not-exist.txt") == NULL,
           "missing file returns NULL");
    remove(path);
}

int main(void) {
    test_create_empty_list();
    test_add_appends_linked_nodes();
    test_add_rejects_bad_input();
    test_done_and_remove();
    test_save_and_load();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
