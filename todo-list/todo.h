#ifndef TODO_H
#define TODO_H

typedef struct Todo {
    int id;
    char text[96];
    int done;
    struct Todo *next;
} Todo;

typedef struct {
    Todo *head;
    int next_id;
} TodoList;

TodoList *todo_list_create(void);
void todo_list_free(TodoList *list);
int todo_list_add(TodoList *list, const char *text);
int todo_list_done(TodoList *list, int id);
int todo_list_remove(TodoList *list, int id);
Todo *todo_list_find(const TodoList *list, int id);
int todo_list_count(const TodoList *list);
int todo_list_save(const TodoList *list, const char *path);
TodoList *todo_list_load(const char *path);

#endif
