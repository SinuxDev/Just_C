#include "contact.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *k_path = "contacts.csv";

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s add <name...> <phone>\n", prog);
    fprintf(stderr, "  %s list\n", prog);
    fprintf(stderr, "  %s search <query...>\n", prog);
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

static ContactBook *load_or_empty(void) {
    ContactBook *book = contact_book_load(k_path);
    if (book) {
        return book;
    }
    return contact_book_create();
}

static void print_contacts(const ContactBook *book, const char *empty_msg) {
    if (!book || book->count == 0) {
        printf("%s\n", empty_msg);
        return;
    }

    printf("%-4s %-24s %s\n", "#", "name", "phone");
    for (int i = 0; i < book->count; i++) {
        printf("%-4d %-24s %s\n", i + 1, book->items[i].name,
               book->items[i].phone);
    }
}

static int cmd_add(int argc, char **argv) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    char name[64];
    if (!join_args(name, sizeof(name), argv, 2, argc - 1)) {
        fprintf(stderr, "Name is too long.\n");
        return 1;
    }

    const char *phone = argv[argc - 1];
    ContactBook *book = load_or_empty();
    if (!book) {
        fprintf(stderr, "Could not open contact book.\n");
        return 1;
    }

    if (!contact_book_add(book, name, phone)) {
        fprintf(stderr, "Could not add contact. Check name, phone, and commas.\n");
        contact_book_free(book);
        return 1;
    }

    if (!contact_book_save(book, k_path)) {
        fprintf(stderr, "Could not save '%s'.\n", k_path);
        contact_book_free(book);
        return 1;
    }

    printf("Added %s (%s)\n", name, phone);
    contact_book_free(book);
    return 0;
}

static int cmd_list(void) {
    ContactBook *book = load_or_empty();
    if (!book) {
        fprintf(stderr, "Could not open contact book.\n");
        return 1;
    }

    print_contacts(book, "(no contacts yet)");
    contact_book_free(book);
    return 0;
}

static int cmd_search(int argc, char **argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    char query[64];
    if (!join_args(query, sizeof(query), argv, 2, argc)) {
        fprintf(stderr, "Query is too long.\n");
        return 1;
    }

    ContactBook *book = load_or_empty();
    if (!book) {
        fprintf(stderr, "Could not open contact book.\n");
        return 1;
    }

    ContactBook *matches = contact_book_search(book, query);
    contact_book_free(book);
    if (!matches) {
        fprintf(stderr, "Search failed.\n");
        return 1;
    }

    print_contacts(matches, "(no matches)");
    contact_book_free(matches);
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
    if (strcmp(argv[1], "search") == 0) {
        return cmd_search(argc, argv);
    }

    print_usage(argv[0]);
    return 1;
}
