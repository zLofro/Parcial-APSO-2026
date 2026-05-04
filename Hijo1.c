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

int avisaP = 0;
int avisaH2 = 0;

void avisaPadre(int sig) {
    avisaP = 1;
}
void avisaHijo2(int sig) {
    avisaH2 = 1;
}

void handleAlarm(int sig) {
    printf("¡¡Tiempo excedido!! Has perdido.\n");
    exit(1);
}

int main() {
    printf("Hola, soy el hijo Hijo1 y mi PID es %d.\n", getpid());

    signal(11, avisaPadre);
    signal(12, avisaHijo2);
    signal(14, handleAlarm); // 14 = SIGALRM no se por que el editor me marca SIGALRM como indefinido

    int rPipe = dup(2);
    close(2);
    open("/dev/tty", O_WRONLY);

    key_t queueK;
    int queueId;
    struct msg msg;

    queueK = ftok("./Makefile", 10);
    if (queueK == -1) {
        perror("Error al crear la llave para la cola en Hijo1.\n");
        exit(-1);
    }

    queueId = msgget(queueK, IPC_CREAT | 0666);
    if (queueId == -1) {
        perror("Error al crear la cola en Hijo1.\n");
        exit(-1);
    }

    kill(getppid(), 10);

    if(avisaP == 0) pause();

    avisaP = 0;

    if (msgrcv(queueId, (struct msgbuf*)&msg, sizeof(msg) - sizeof(long), 1, 0) == -1) {
        perror("Error leyendo el dato de la cola en Hijo1.\n");
        exit(-1);
    }

    int h2Pid = msg.pid;

    printf("Leido PID del hijo2: %d\n", h2Pid);

    kill(getppid(), 10);

    if(avisaH2 == 0) pause();

    int values;

    if (read(rPipe, &values, sizeof(values)) == -1) {
        perror("Error al leer los valores en Hijo1.\n");
        exit(-1);
    }

    int value;
    int vals[values];
    for (int i = 0; i < values; i++) {
        if (read(rPipe, &value, sizeof(value)) == -1) {
            perror("Error al leer un valor en Hijo1.\n");
            exit(-1);
        }

        vals[i] = value;

        printf("Leido valor de la pipe: %d\n", value);
    }

    alarm(5);
    int num;
    printf("Introduce un número.\n");
    scanf("%d", &num);

    int i = 0;
    int found = 0;
    while(found == 0 && i < values) {
        if (vals[i] == num) found = 1;

        i++;
    }

    if (found == 1) {
        printf("¡¡Has ganado!!\n");
    } else {
        printf("¡¡Has perdido!!\n");
    }
}