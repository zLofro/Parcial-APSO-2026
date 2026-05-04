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
    int pid;
};

struct msgh3 {
    long type;
    int start;
    int end;
};

int avisaP = 0;

void avisaPadre(int sig) {
    avisaP = 1;
}

int main() {
    printf("Hola, soy el hijo Hijo2 y mi PID es %d.\n", getpid());

    srand(getpid());
    signal(12, avisaPadre);

    int wPipe = dup(2);
    close(2);
    open("/dev/tty", O_WRONLY);

    key_t queueK;
    int queueId;
    struct msg msg;
    struct msgh3 msgh3;

    queueK = ftok("./Makefile", 10);
    if (queueK == -1) {
        perror("Error al crear la llave para la cola en Hijo2.\n");
        exit(-1);
    }

    queueId = msgget(queueK, IPC_CREAT | 0666);
    if (queueId == -1) {
        perror("Error al crear la cola en Hijo2.\n");
        exit(-1);
    }

    sleep(1);

    kill(getppid(), 20);

    if (avisaP == 0) pause();

    if (msgrcv(queueId, (struct msgbuf*)&msg, sizeof(msg) - sizeof(long), 2, 0) == -1) {
        perror("Error leyendo el dato de la cola en Hijo2.\n");
        exit(-1);
    }

    int h1Pid = msg.pid;

    printf("Leido PID del hijo1: %d\n", h1Pid);

    kill(getppid(), 20);

    if (msgrcv(queueId, (struct msgbuf*)&msgh3, sizeof(msgh3) - sizeof(long), 3, 0) == -1) {
        perror("Error leyendo el dato de la cola en Hijo2.\n");
        exit(-1);
    }

    int start = msgh3.start;
    int end = msgh3.end;

    int values = end - start;

    if (write(wPipe, &values, sizeof(values)) == -1) {
        perror("Error al enviar el valor por la pipe desde hijo2.\n");
        exit(-1);
    }
    for (int i = start; i < end; i++) {
        int r = start + rand() % end;
        if (write(wPipe, &r, sizeof(r)) == -1) {
            perror("Error al enviar el valor de i por la pipe desde hijo2.\n");
            exit(-1);
        }
    }

    kill(h1Pid, 12); // ENVIA SEÑAL 12
}