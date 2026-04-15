#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock;

void *increment_counter(void *arg) {
        for (int i = 0; i < 10000; i++) {
                pthread_mutex_lock(&lock);
                counter++;
                pthread_mutex_unlock(&lock);
        }
        return NULL;
}

int main() {
        pthread_t t1, t2;

        pthread_mutex_init(&lock, NULL);

        pthread_create(&t1, NULL, increment_counter, NULL);
        pthread_create(&t2, NULL, increment_counter, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        pthread_mutex_destroy(&lock);

        printf("Final counter value: %d\n", counter);

        return 0;
}
