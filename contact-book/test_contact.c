#include "contact.h"

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

static void test_create_empty_book(void) {
    ContactBook *book = contact_book_create();

    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    EXPECT(book->count == 0, "new book has no contacts");
    EXPECT(book->items == NULL, "new book has no items array");
    contact_book_free(book);
}

static void test_add_stores_contact(void) {
    ContactBook *book = contact_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    EXPECT(contact_book_add(book, "Ada Lovelace", "0912345678") == 1,
           "add returns success");
    EXPECT(book->count == 1, "book has one contact");
    EXPECT(strcmp(book->items[0].name, "Ada Lovelace") == 0, "name is stored");
    EXPECT(strcmp(book->items[0].phone, "0912345678") == 0, "phone is stored");

    contact_book_free(book);
}

static void test_add_rejects_bad_input(void) {
    ContactBook *book = contact_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    EXPECT(contact_book_add(book, "", "123") == 0, "empty name is rejected");
    EXPECT(contact_book_add(book, NULL, "123") == 0, "NULL name is rejected");
    EXPECT(contact_book_add(book, "Ada", "") == 0, "empty phone is rejected");
    EXPECT(contact_book_add(book, "Ada,Bad", "123") == 0,
           "comma in name is rejected");
    EXPECT(book->count == 0, "rejected adds do not change the book");

    contact_book_free(book);
}

static void test_search_by_name_or_phone(void) {
    ContactBook *book = contact_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    contact_book_add(book, "Ada Lovelace", "0912345678");
    contact_book_add(book, "Alan Turing", "0999888777");
    contact_book_add(book, "Grace Hopper", "0911111111");

    ContactBook *by_name = contact_book_search(book, "ada");
    EXPECT(by_name != NULL, "search returns a book");
    if (by_name) {
        EXPECT(by_name->count == 1, "name search finds one contact");
        EXPECT(strcmp(by_name->items[0].name, "Ada Lovelace") == 0,
               "search is case-insensitive");
        contact_book_free(by_name);
    }

    ContactBook *by_phone = contact_book_search(book, "09998");
    EXPECT(by_phone != NULL, "phone search returns a book");
    if (by_phone) {
        EXPECT(by_phone->count == 1, "phone search finds one contact");
        EXPECT(strcmp(by_phone->items[0].name, "Alan Turing") == 0,
               "phone substring matches");
        contact_book_free(by_phone);
    }

    ContactBook *none = contact_book_search(book, "xyz");
    EXPECT(none != NULL, "no-match search still returns a book");
    if (none) {
        EXPECT(none->count == 0, "unknown query finds nothing");
        contact_book_free(none);
    }

    contact_book_free(book);
}

static void test_save_and_load(void) {
    const char *path = "test_contacts.csv";
    ContactBook *book = contact_book_create();
    EXPECT(book != NULL, "create returns a book");
    if (!book) {
        return;
    }

    contact_book_add(book, "Ada Lovelace", "0912345678");
    contact_book_add(book, "Alan Turing", "0999888777");
    EXPECT(contact_book_save(book, path) == 1, "save succeeds");
    contact_book_free(book);

    ContactBook *loaded = contact_book_load(path);
    EXPECT(loaded != NULL, "load returns a book");
    if (loaded) {
        EXPECT(loaded->count == 2, "loaded book has two contacts");
        EXPECT(strcmp(loaded->items[0].name, "Ada Lovelace") == 0, "loaded name");
        EXPECT(strcmp(loaded->items[0].phone, "0912345678") == 0, "loaded phone");
        EXPECT(strcmp(loaded->items[1].name, "Alan Turing") == 0, "second name");
        contact_book_free(loaded);
    }

    EXPECT(contact_book_load("this-file-does-not-exist.csv") == NULL,
           "missing file returns NULL");
    remove(path);
}

int main(void) {
    test_create_empty_book();
    test_add_stores_contact();
    test_add_rejects_bad_input();
    test_search_by_name_or_phone();
    test_save_and_load();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
