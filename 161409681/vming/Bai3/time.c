#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct timeval start, end;
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        gettimeofday(&start, NULL);

        execl("/bin/sh", "sh", "-c", argv[1], (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, &status, 0);

        gettimeofday(&end, NULL);

        double elapsed = ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec))/1000000;
        printf("Thời gian thực thi: %f\n", elapsed);
    }

    return 0;
}
