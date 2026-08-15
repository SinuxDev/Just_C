#ifndef EXPENSE_H
#define EXPENSE_H

typedef struct {
    double amount;
    char category[32];
    char note[96];
} Expense;

typedef struct {
    Expense *items;
    int count;
} ExpenseBook;

ExpenseBook *expense_book_create(void);
void expense_book_free(ExpenseBook *book);
int expense_book_add(ExpenseBook *book, double amount, const char *category,
                     const char *note);
double expense_book_total(const ExpenseBook *book);
double expense_book_total_for(const ExpenseBook *book, const char *category);
int expense_book_save(const ExpenseBook *book, const char *path);
ExpenseBook *expense_book_load(const char *path);

#endif
