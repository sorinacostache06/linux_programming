#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUF_SIZE 100

int isprime(int r)
{
    sleep(10);
    int i = 3;
    if (r < 2) 
        return -1;
    if (r == 2) 
        return 0;
    if (r % 2 == 0) 
        return -1;
    while (i * i <= r) {
        if (r % i == 0) 
            return -1;
        i += 2;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int pfd[2], n, r, childPid, status;
    char buff[BUF_SIZE];
    char str1[BUF_SIZE];

    if (argc < 2) {
        printf("error: no arguments\n");
        return -1;
    } else if (argc > 2) {
        printf("error: too many arguments\n");
        return -1;
    }

    n = atoi(argv[1]);
    if (n < 0 || n > 10) {
        printf("error: argument not on range\n");
        return -1;
    }

    if (pipe(pfd) == -1) {
        printf("error: pipe can not be initialized\n");
        return -1;
    }
    
    for (int i = 0; i < n; i++) {
        switch (childPid = fork()) {
            case -1: 
                printf("Error number is : %d\n", errno);
                printf("Error description is : %s\n",strerror(errno));
                return -1;
            case 0:
                if (close(pfd[0]) == -1) 
                    printf("error: can not close read pipe\n");
                time_t t;
                srand((int)time(&t) % getpid());
                r = rand()%1000 + 100;
                while (isprime(r))
                    r = rand()%1000 + 100; 
               
                char str_prime[10], str_pid[10];
                sprintf(str_prime, "%d", r);
                sprintf(str_pid, "%d", getpid());

                char message[100];
                strcat(message, "PID: ");
                strcat(message, str_pid);
                strcat(message, " generate prime number: ");
                strcat(message, str_prime);
                strcat(message, "\n");
                int len = strlen(message);
                if (write(pfd[1], message, len) != len)
                    printf("error pipe: can not write\n");
                
                if (close(pfd[1]) == -1)
                    printf("error: can not close write pipe\n");
                 _exit(EXIT_SUCCESS);
            default:
                while (waitpid(childPid, &status, 0) == -1) {
                    status = -1;
                    break;
                }
                if (read(pfd[0], buff, BUF_SIZE) == -1)
                    printf("error: parent can not read from pipe\n");
                else printf("%s\n", buff);
                break;
        }
    }

    if (close(pfd[1]) == -1)
        printf("error: can not close write pipe\n");
   
    if (close(pfd[0]) == -1)
        printf("error: can not close read pipe\n");
    
    _exit(EXIT_SUCCESS);
}
