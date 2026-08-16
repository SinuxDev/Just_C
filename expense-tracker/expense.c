#include "expense.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int str_ieq(const char *a, const char *b) {
    if (!a || !b) {
        return 0;
    }
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

static int contains_comma(const char *s) {
    return s != NULL && strchr(s, ',') != NULL;
}

ExpenseBook *expense_book_create(void) {
    return calloc(1, sizeof(ExpenseBook));
}

void expense_book_free(ExpenseBook *book) {
    if (!book) {
        return;
    }
    free(book->items);
    free(book);
}

int expense_book_add(ExpenseBook *book, double amount, const char *category,
                     const char *note) {
    if (!book || amount < 0.0 || !category || category[0] == '\0') {
        return 0;
    }
    if (contains_comma(category) || contains_comma(note)) {
        return 0;
    }
    if (strlen(category) >= sizeof(((Expense *)0)->category)) {
        return 0;
    }
    if (note && strlen(note) >= sizeof(((Expense *)0)->note)) {
        return 0;
    }

    Expense *grown =
        realloc(book->items, (size_t)(book->count + 1) * sizeof(Expense));
    if (!grown) {
        return 0;
    }
    book->items = grown;

    Expense *item = &book->items[book->count];
    item->amount = amount;
    snprintf(item->category, sizeof(item->category), "%s", category);
    snprintf(item->note, sizeof(item->note), "%s", note ? note : "");
    book->count++;
    return 1;
}

double expense_book_total(const ExpenseBook *book) {
    if (!book) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < book->count; i++) {
        sum += book->items[i].amount;
    }
    return sum;
}

double expense_book_total_for(const ExpenseBook *book, const char *category) {
    if (!book || !category) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < book->count; i++) {
        if (str_ieq(book->items[i].category, category)) {
            sum += book->items[i].amount;
        }
    }
    return sum;
}

int expense_book_save(const ExpenseBook *book, const char *path) {
    if (!book || !path) {
        return 0;
    }

    FILE *file = fopen(path, "w");
    if (!file) {
        return 0;
    }

    fprintf(file, "amount,category,note\n");
    for (int i = 0; i < book->count; i++) {
        fprintf(file, "%.2f,%s,%s\n", book->items[i].amount,
                book->items[i].category, book->items[i].note);
    }
    fclose(file);
    return 1;
}

ExpenseBook *expense_book_load(const char *path) {
    if (!path) {
        return NULL;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    ExpenseBook *book = expense_book_create();
    if (!book) {
        fclose(file);
        return NULL;
    }

    char line[256];
    int first = 1;
    int ok = 1;

    while (ok && fgets(line, (int)sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') {
            continue;
        }
        if (first) {
            first = 0;
            if (strncmp(line, "amount,", 7) == 0) {
                continue;
            }
        }

        char *first_comma = strchr(line, ',');
        if (!first_comma) {
            ok = 0;
            break;
        }
        *first_comma = '\0';

        char *end = NULL;
        double amount = strtod(line, &end);
        if (end == line || *end != '\0') {
            ok = 0;
            break;
        }

        char category[32] = "";
        char note[96] = "";
        char *second_comma = strchr(first_comma + 1, ',');
        if (second_comma) {
            *second_comma = '\0';
            snprintf(category, sizeof(category), "%s", first_comma + 1);
            snprintf(note, sizeof(note), "%s", second_comma + 1);
        } else {
            snprintf(category, sizeof(category), "%s", first_comma + 1);
        }

        if (!expense_book_add(book, amount, category, note)) {
            ok = 0;
        }
    }

    fclose(file);
    if (!ok) {
        expense_book_free(book);
        return NULL;
    }
    return book;
}
