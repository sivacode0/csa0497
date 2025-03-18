#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>  // Include for wait() function

#define SHM_SIZE 1024

int main() {
    int shm_id;
    key_t key = 1234;
    char *shared_memory;

    // Create shared memory segment
    shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    // Attach shared memory
    shared_memory = (char *)shmat(shm_id, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Fork a child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process: Write to shared memory
        printf("Child: Writing to shared memory...\n");
        strcpy(shared_memory, "Hello from Child Process!");
        printf("Child: Data written.\n");

        // Detach shared memory (Child does not need to remove the segment)
        shmdt(shared_memory);
    } else {
        // Parent process: Wait and read from shared memory
        wait(NULL);
        printf("Parent: Reading from shared memory...\n");
        printf("Parent: Data received: %s\n", shared_memory);

        // Detach and remove shared memory
        shmdt(shared_memory);
        shmctl(shm_id, IPC_RMID, NULL);
    }

    return 0;
}
