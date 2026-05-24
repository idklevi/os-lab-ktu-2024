#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

sem_t mutex;
sem_t rw_mutex;
sem_t queue;
int read_count = 0;
int data = 0;

void *reader(void *arg) {
    int id = *((int *)arg);

    sem_wait(&queue);
    sem_wait(&mutex);
    read_count++;
    if (read_count == 1)
        sem_wait(&rw_mutex);
    sem_post(&mutex);
    sem_post(&queue);

    printf("Reader %d is READING data=%d\n", id, data);
    sleep(1);
    printf("Reader %d has FINISHED READING\n", id);

    sem_wait(&mutex);
    read_count--;
    if (read_count == 0)
        sem_post(&rw_mutex);
    sem_post(&mutex);

    return NULL;
}

void *writer(void *arg) {
    int id = *((int *)arg);

    sem_wait(&queue);
    sem_wait(&rw_mutex);

    data++;
    printf("Writer %d is WRITING data=%d\n", id, data);
    sleep(2);
    printf("Writer %d has FINISHED WRITING\n", id);

    sem_post(&rw_mutex);
    sem_post(&queue);

    return NULL;
}

int main() {
    int i;
    pthread_t readers[5], writers[5];
    int r_id[5], w_id[2];

    sem_init(&mutex, 0, 1);
    sem_init(&rw_mutex, 0, 1);
    sem_init(&queue, 0, 1);

    for (i = 0; i < 5; i++) {
        r_id[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &r_id[i]);
    }

    for (i = 0; i < 2; i++) {
        w_id[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &w_id[i]);
    }

    for (i = 0; i < 5; i++)
        pthread_join(readers[i], NULL);
    for (i = 0; i < 2; i++)
        pthread_join(writers[i], NULL);

    sem_destroy(&mutex);
    sem_destroy(&rw_mutex);
    sem_destroy(&queue);

    return 0;
}
