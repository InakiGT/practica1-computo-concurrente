#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BOOKS_NUMBER 4
#define READERS 4
#define WRITERS 4

typedef struct {
    int id;
    char content[10];
    pthread_mutex_t mutex;
    pthread_cond_t cond_read;
    pthread_cond_t cond_write;
    int writing;
    int reading;
} book;

book books[BOOKS_NUMBER];

void * readBook(void * arg) {
    uintptr_t id = (uintptr_t) arg;
    int randomBook = rand() % BOOKS_NUMBER;
    book * cBook = &books[randomBook];

    while( cBook->writing )
        pthread_cond_wait(&cBook->cond_read, &cBook->mutex);

    cBook->reading = 1;
    printf("Reader with id %lu is reading the book with id %d", id, cBook->id);
    printf(" %s \n", cBook->content);
    sleep(10);
    cBook->reading = 0;
    pthread_cond_signal(&cBook->cond_write);

    pthread_exit(NULL);
}

void * writeBook(void * arg) {
    uintptr_t id = (uintptr_t) arg;
    int randomBook = rand() % BOOKS_NUMBER;
    book * cBook = &books[randomBook];

    pthread_mutex_lock(&cBook->mutex);
    while ( cBook->reading )
       pthread_cond_wait(&cBook->cond_write, &cBook->mutex);
    
    cBook->writing = 1;
    printf("Writer with id %lu is writing the book with id %d\n", id, cBook->id);
    strcpy(cBook->content, "Un nuevo.");
    sleep(9);
    cBook->writing = 0;
    pthread_cond_signal(&cBook->cond_read);
    pthread_mutex_unlock(&cBook->mutex);

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    pthread_t readers[READERS], writers[WRITERS];

    for (int i = 0; i < BOOKS_NUMBER; i++) {
        book cBook;
        cBook.id = i;
        cBook.reading = 0;
        cBook.writing = 0;
        strcpy(cBook.content, "Hola mund");
        pthread_mutex_init(&cBook.mutex, NULL);
        pthread_cond_init(&cBook.cond_read, NULL);
        pthread_cond_init(&cBook.cond_write, NULL);

        books[i] = cBook;
    }

    for (int i = 0; i < READERS; i++)
        pthread_create(&readers[i], NULL, readBook, (void *)(uintptr_t)i);
    for (int i = 0; i < WRITERS; i++)
        pthread_create(&writers[i], NULL, writeBook, (void *)(uintptr_t)i);

    for (int i = 0; i < READERS; i++)
        pthread_join(readers[i], NULL);
    for (int i = 0; i < WRITERS; i++)
        pthread_join(writers[i], NULL);
  
    return 0;
}