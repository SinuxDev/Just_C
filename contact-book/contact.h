#ifndef CONTACT_H
#define CONTACT_H

typedef struct {
    char name[64];
    char phone[32];
} Contact;

typedef struct {
    Contact *items;
    int count;
} ContactBook;

ContactBook *contact_book_create(void);
void contact_book_free(ContactBook *book);
int contact_book_add(ContactBook *book, const char *name, const char *phone);
ContactBook *contact_book_search(const ContactBook *book, const char *query);
int contact_book_save(const ContactBook *book, const char *path);
ContactBook *contact_book_load(const char *path);

#endif
