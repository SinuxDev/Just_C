#include "expense.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *k_path = "expenses.csv";

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s add <amount> <category> [note...]\n", prog);
    fprintf(stderr, "  %s list\n", prog);
    fprintf(stderr, "  %s total [category]\n", prog);
}

static int join_args(char *out, size_t cap, char **argv, int start, int end) {
    if (!out || cap == 0) {
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

static ExpenseBook *load_or_empty(void) {
    ExpenseBook *book = expense_book_load(k_path);
    if (book) {
        return book;
    }
    return expense_book_create();
}

static int cmd_add(int argc, char **argv) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    double amount = strtod(argv[2], &end);
    if (end == argv[2] || *end != '\0') {
        fprintf(stderr, "Amount must be a number.\n");
        return 1;
    }

    char note[96] = "";
    if (argc > 4 && !join_args(note, sizeof(note), argv, 4, argc)) {
        fprintf(stderr, "Note is too long.\n");
        return 1;
    }

    ExpenseBook *book = load_or_empty();
    if (!book) {
        fprintf(stderr, "Could not open expense book.\n");
        return 1;
    }

    if (!expense_book_add(book, amount, argv[3], note)) {
        fprintf(stderr, "Could not add expense. Check amount, category, and commas.\n");
        expense_book_free(book);
        return 1;
    }

    if (!expense_book_save(book, k_path)) {
        fprintf(stderr, "Could not save '%s'.\n", k_path);
        expense_book_free(book);
        return 1;
    }

    printf("Added %.2f (%s)\n", amount, argv[3]);
    expense_book_free(book);
    return 0;
}

static int cmd_list(void) {
    ExpenseBook *book = load_or_empty();
    if (!book) {
        fprintf(stderr, "Could not open expense book.\n");
        return 1;
    }

    if (book->count == 0) {
        printf("(no expenses yet)\n");
        expense_book_free(book);
        return 0;
    }

    printf("%-4s %-10s %-12s %s\n", "#", "amount", "category", "note");
    for (int i = 0; i < book->count; i++) {
        printf("%-4d %-10.2f %-12s %s\n", i + 1, book->items[i].amount,
               book->items[i].category, book->items[i].note);
    }
    printf("\nTotal: %.2f\n", expense_book_total(book));
    expense_book_free(book);
    return 0;
}

static int cmd_total(int argc, char **argv) {
    ExpenseBook *book = load_or_empty();
    if (!book) {
        fprintf(stderr, "Could not open expense book.\n");
        return 1;
    }

    if (argc >= 3) {
        printf("%.2f\n", expense_book_total_for(book, argv[2]));
    } else {
        printf("%.2f\n", expense_book_total(book));
    }

    expense_book_free(book);
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
    if (strcmp(argv[1], "total") == 0) {
        return cmd_total(argc, argv);
    }

    print_usage(argv[0]);
    return 1;
}
