#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/sys_sysheader.h>

int
main(int argc, char **argv)
{
    pid_t pid;
    int pipefd;
    long ptraceresult;

    pipefd = atoi(argv[1]);
    read(pipefd, &pid, sizeof(pid));

    ptraceresult = ptrace(PT_ATTACHEXC, pid, NULL, 0);
    if (ptraceresult < 0) {
        perror("ptrace");
        return 1;
    }
    waitpid(pid, NULL, 0);

    printf("pid: %d\n", pid);

    close(pipefd);
    ptrace(PT_CONTINUE, pid, NULL, 0);
    ptrace(PT_DETACH, pid, NULL, 0);
    return 0;
}

