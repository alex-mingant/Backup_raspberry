#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Fonction pour obtenir le nom de la commande à partir du fichier cmdline
void getCmdName(int pid) {
    char path[50];
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Erreur lors de l'ouverture du fichier cmdline");
        exit(1);
    }

    char cmdline[256];
    int bytesRead = read(fd, cmdline, sizeof(cmdline) - 1);
    close(fd);

    if (bytesRead > 0) {
        cmdline[bytesRead] = '\0';
        printf("Nom de la commande : %s\n", cmdline);
    } else {
        printf("Nom de la commande : Non disponible\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilisation : %s <PID>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);

    printf("PID : %d\n", pid);

    // Ouvrir le fichier /proc/[PID]/status
    char path[50];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *status_file = fopen(path, "r");
    if (status_file == NULL) {
        perror("Erreur lors de l'ouverture du fichier status");
        return 1;
    }

    char line[256];
    int tgid = -1;
    int ppid = -1;
    int nthreads = 0;
    uid_t uid = -1;
    gid_t gid = -1;

    // Lire le fichier /proc/[PID]/status ligne par ligne
    while (fgets(line, sizeof(line), status_file)) {
        // Rechercher les informations souhaitées
        if (sscanf(line, "Tgid: %d", &tgid) == 1) {
            continue;
        } else if (sscanf(line, "PPid: %d", &ppid) == 1) {
            continue;
        } else if (strstr(line, "Threads:") == line) {
            sscanf(line, "Threads: %d", &nthreads);
        } else if (sscanf(line, "Uid: %u", &uid) == 1) {
            continue;
        } else if (sscanf(line, "Gid: %u", &gid) == 1) {
            continue;
        }
    }

    fclose(status_file);

    if (tgid != -1) {
        printf("Tgid : %d\n", tgid);
        printf("PID du père : %d\n", ppid);
        getCmdName(ppid);
        printf("Nombre de threads fils : %d\n", nthreads);
        printf("UID : %u\n", uid);
        printf("GID : %u\n", gid);
    } else {
        fprintf(stderr, "Informations indisponibles pour le PID %d.\n", pid);
        return 1;
    }

    return 0;
}
