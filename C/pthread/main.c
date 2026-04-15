#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *print_message(void *prt) {
        char *message = (char*)prt;
        printf("%s is starting...\n", message);

        sleep(1);

        printf("%s is finished.\n", message);
        return NULL;
}

int main() {
        pthread_t thread1, thread2;
        char *msg1 = "Thread 1";
        char *msg2 = "Thread 2";

        pthread_create(&thread1, NULL, print_message, (void*)msg1);
        pthread_create(&thread2, NULL, print_message, (void*)msg2);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

        printf("Back in main: All threads have completed.\n");

        return 0;
}
