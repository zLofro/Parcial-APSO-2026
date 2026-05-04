#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

struct msg {
    long type;
    int start;
    int end;
};

int avisaP = 0;

void avisaPadre(int sig) {
    avisaP = 1;
}

int main() {
    printf("Hola, soy el hijo Hijo3 y mi PID es %d.\n", getpid());

    signal(13, avisaPadre);

    key_t queueK;
    int queueId;
    struct msg msg;

    queueK = ftok("./Makefile", 10);
    if (queueK == -1) {
        perror("Error al crear la llave para la cola en Hijo3.\n");
        exit(-1);
    }

    queueId = msgget(queueK, IPC_CREAT | 0666);
    if (queueId == -1) {
        perror("Error al crear la cola en Hijo3.\n");
        exit(-1);
    }

    kill(getppid(), 30);

    if (avisaP == 0) pause();

    int start;
    int end;
    printf("Introduce los valores inicial y final.\n");
    scanf("%d", &start);
    scanf("%d", &end);

    msg.type = 3;
    msg.start = start;
    msg.end = end;

    if (msgsnd(queueId, (struct msgbuf*)&msg, sizeof(msg) - sizeof(long), 0) == -1) {
        perror("Error enviando el dato en Hijo3.\n");
        exit(-1);
    }
}