#include "contact.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int contains_comma(const char *s) {
    return s != NULL && strchr(s, ',') != NULL;
}

static int str_icontains(const char *hay, const char *needle) {
    if (!hay || !needle || needle[0] == '\0') {
        return 0;
    }

    for (const char *p = hay; *p; p++) {
        const char *h = p;
        const char *n = needle;
        while (*h && *n) {
            if (tolower((unsigned char)*h) != tolower((unsigned char)*n)) {
                break;
            }
            h++;
            n++;
        }
        if (*n == '\0') {
            return 1;
        }
    }
    return 0;
}

ContactBook *contact_book_create(void) {
    return calloc(1, sizeof(ContactBook));
}

void contact_book_free(ContactBook *book) {
    if (!book) {
        return;
    }
    free(book->items);
    free(book);
}

int contact_book_add(ContactBook *book, const char *name, const char *phone) {
    if (!book || !name || name[0] == '\0' || !phone || phone[0] == '\0') {
        return 0;
    }
    if (contains_comma(name) || contains_comma(phone)) {
        return 0;
    }
    if (strlen(name) >= sizeof(((Contact *)0)->name)) {
        return 0;
    }
    if (strlen(phone) >= sizeof(((Contact *)0)->phone)) {
        return 0;
    }

    Contact *grown =
        realloc(book->items, (size_t)(book->count + 1) * sizeof(Contact));
    if (!grown) {
        return 0;
    }
    book->items = grown;

    Contact *item = &book->items[book->count];
    snprintf(item->name, sizeof(item->name), "%s", name);
    snprintf(item->phone, sizeof(item->phone), "%s", phone);
    book->count++;
    return 1;
}

ContactBook *contact_book_search(const ContactBook *book, const char *query) {
    ContactBook *matches = contact_book_create();
    if (!matches || !book || !query) {
        return matches;
    }

    for (int i = 0; i < book->count; i++) {
        if (str_icontains(book->items[i].name, query) ||
            str_icontains(book->items[i].phone, query)) {
            if (!contact_book_add(matches, book->items[i].name,
                                  book->items[i].phone)) {
                contact_book_free(matches);
                return NULL;
            }
        }
    }
    return matches;
}

int contact_book_save(const ContactBook *book, const char *path) {
    if (!book || !path) {
        return 0;
    }

    FILE *file = fopen(path, "w");
    if (!file) {
        return 0;
    }

    fprintf(file, "name,phone\n");
    for (int i = 0; i < book->count; i++) {
        fprintf(file, "%s,%s\n", book->items[i].name, book->items[i].phone);
    }
    fclose(file);
    return 1;
}

ContactBook *contact_book_load(const char *path) {
    if (!path) {
        return NULL;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return NULL;
    }

    ContactBook *book = contact_book_create();
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
            if (strncmp(line, "name,", 5) == 0) {
                continue;
            }
        }

        char *comma = strrchr(line, ',');
        if (!comma || comma == line) {
            ok = 0;
            break;
        }
        *comma = '\0';
        if (!contact_book_add(book, line, comma + 1)) {
            ok = 0;
        }
    }

    fclose(file);
    if (!ok) {
        contact_book_free(book);
        return NULL;
    }
    return book;
}
