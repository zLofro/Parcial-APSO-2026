#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

struct msg {
    long type;
    int pid;
};

int listoH1 = 0;
int listoH2 = 0;
int listoH3 = 0;

void listoHijo1(int sig) {
    listoH1 = 1;
}

void listoHijo2(int sig) {
    listoH2 = 1;
}

void listoHijo3(int sig) {
    listoH3 = 1;
}

int main() {
    signal(10, listoHijo1);
    signal(20, listoHijo2);
    signal(30, listoHijo3);

    int pipeInfo[2];

    if (pipe(pipeInfo) == -1) {
        perror("Error al crear la pipe en Padre.\n");
        exit(-1);
    }

    int h1Pid = fork();

    if (h1Pid == 0) {
        close(2);
        dup(pipeInfo[0]);

        execl("Hijo1", "Hijo1", NULL);
        perror("Error en el execl de Hijo2.\n");
        exit(-1);
    } else if (h1Pid == -1) {
        perror("Error al hacer fork del Hijo1.\n");
        exit(-1);
    }

    int h2Pid = fork();

    if (h2Pid == 0) {
        close(2);
        dup(pipeInfo[1]);

        execl("Hijo2", "Hijo2", NULL);
        perror("Error en el execl de Hijo2.\n");
        exit(-1);
    } else if (h2Pid == -1) {
        perror("Error al hacer fork del Hijo2.\n");
        exit(-1);
    }

    int h3Pid = fork();

    if (h3Pid == 0) {
        close(2);
        dup(pipeInfo[1]);

        execl("Hijo3", "Hijo3", NULL);
        perror("Error en el execl de Hijo3.\n");
        exit(-1);
    } else if (h3Pid == -1) {
        perror("Error al hacer fork del Hijo3.\n");
        exit(-1);
    }

    close(pipeInfo[0]);
    close(pipeInfo[1]);

    if(listoH1 == 0) pause();

    if(listoH2 == 0) pause();

    if(listoH3 == 0) pause();

    kill(h1Pid, 11);

    kill(h2Pid, 12);

    key_t queueK;
    int queueId;
    struct msg msg;

    queueK = ftok("./Makefile", 10);
    if (queueK == -1) {
        perror("Error al crear la llave para la cola en Padre.\n");
        exit(-1);
    }

    queueId = msgget(queueK, IPC_CREAT | 0666);
    if (queueId == -1) {
        perror("Error al crear la cola en Padre.\n");
        exit(-1);
    }

    msg.type = 1;
    msg.pid = h2Pid;
    if (msgsnd(queueId, (struct msgbuf*)&msg, sizeof(msg) - sizeof(long), 0) == -1) {
        perror("Error enviando el dato a Hijo1.\n");
        exit(-1);
    }

    msg.type = 2;
    msg.pid = h1Pid;
    if (msgsnd(queueId, (struct msgbuf*)&msg, sizeof(msg) - sizeof(long), 0) == -1) {
        perror("Error enviando el dato a Hijo2.\n");
        exit(-1);
    }

    listoH1 = 0;
    listoH2 = 0;

    if(listoH1 == 0) pause();

    if(listoH2 == 0) pause();

    kill(h3Pid, 13);

    wait(NULL);
    wait(NULL);
    wait(NULL);
}