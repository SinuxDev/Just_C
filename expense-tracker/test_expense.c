#include "expense.h"

#include <math.h>
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

static int nearly_equal(double a, double b) {
    return fabs(a - b) < 0.001;
}

static void test_create_empty_book(void) {
    ExpenseBook *book = expense_book_create();

    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    EXPECT(book->count == 0, "new book has no expenses");
    EXPECT(book->items == NULL, "new book has no items array");
    expense_book_free(book);
}

static void test_add_stores_expense(void) {
    ExpenseBook *book = expense_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    EXPECT(expense_book_add(book, 12.50, "food", "lunch") == 1,
           "add returns success");
    EXPECT(book->count == 1, "book has one expense");
    EXPECT(nearly_equal(book->items[0].amount, 12.50), "amount is stored");
    EXPECT(strcmp(book->items[0].category, "food") == 0, "category is stored");
    EXPECT(strcmp(book->items[0].note, "lunch") == 0, "note is stored");

    expense_book_free(book);
}

static void test_add_rejects_bad_input(void) {
    ExpenseBook *book = expense_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    EXPECT(expense_book_add(book, -1.0, "food", "x") == 0,
           "negative amount is rejected");
    EXPECT(expense_book_add(book, 1.0, "", "x") == 0, "empty category is rejected");
    EXPECT(expense_book_add(book, 1.0, NULL, "x") == 0, "NULL category is rejected");
    EXPECT(expense_book_add(book, 1.0, "food,bad", "x") == 0,
           "comma in category is rejected");
    EXPECT(book->count == 0, "rejected adds do not change the book");

    expense_book_free(book);
}

static void test_totals(void) {
    ExpenseBook *book = expense_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    expense_book_add(book, 10.00, "food", "a");
    expense_book_add(book, 3.50, "transport", "b");
    expense_book_add(book, 2.50, "Food", "c");

    EXPECT(nearly_equal(expense_book_total(book), 16.00), "total sums all amounts");
    EXPECT(nearly_equal(expense_book_total_for(book, "food"), 12.50),
           "category total is case-insensitive");
    EXPECT(nearly_equal(expense_book_total_for(book, "rent"), 0.0),
           "unknown category totals to zero");

    expense_book_free(book);
}

static void test_save_and_load(void) {
    const char *path = "test_expenses.csv";
    ExpenseBook *book = expense_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    expense_book_add(book, 8.25, "coffee", "morning");
    expense_book_add(book, 20.00, "books", "");
    EXPECT(expense_book_save(book, path) == 1, "save succeeds");
    expense_book_free(book);

    ExpenseBook *loaded = expense_book_load(path);
    EXPECT(loaded != NULL, "load returns a book");
    if (loaded) {
        EXPECT(loaded->count == 2, "loaded book has two expenses");
        EXPECT(nearly_equal(loaded->items[0].amount, 8.25), "loaded amount");
        EXPECT(strcmp(loaded->items[0].category, "coffee") == 0, "loaded category");
        EXPECT(strcmp(loaded->items[0].note, "morning") == 0, "loaded note");
        EXPECT(strcmp(loaded->items[1].category, "books") == 0, "second category");
        expense_book_free(loaded);
    }

    EXPECT(expense_book_load("this-file-does-not-exist.csv") == NULL,
           "missing file returns NULL");
    remove(path);
}

int main(void) {
    test_create_empty_book();
    test_add_stores_expense();
    test_add_rejects_bad_input();
    test_totals();
    test_save_and_load();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
