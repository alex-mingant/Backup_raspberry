#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

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

// Fonction pour obtenir les informations du CPU
void getCPUInfo() {
    FILE *file;
    char line[256];

    // Ouvrir le fichier /proc/cpuinfo en lecture
    file = fopen("/proc/cpuinfo", "r");
    if (file == NULL) {
        perror("Impossible d'ouvrir /proc/cpuinfo");
        exit(1);
    }

    // Lire et afficher chaque ligne du fichier
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    // Fermer le fichier
    fclose(file);
}

// Fonction pour obtenir les informations de la memoire 
void getMemoireInfo() {
    FILE *file;
    char line[256];

    // Ouvrir le fichier /proc/meminfo en lecture
    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("Impossible d'ouvrir /proc/meminfo");
        exit(1);
    }

    // Lire et afficher chaque ligne du fichier
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    // Fermer le fichier
    fclose(file);
}

// Fonction pour obtenir les informations d'un processus par PID
void getProcessInfo(int pid) {
    printf("PID : %d\n", pid);

    // Ouvrir le fichier /proc/[PID]/status
    char path[50];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *status_file = fopen(path, "r");
    if (status_file == NULL) {
        perror("Erreur lors de l'ouverture du fichier status");
        exit(1);
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
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int option;
    int cpu_info = 0;
    int mem_info = 0;
    int process_pid = -1;

    // Gérer les options de la ligne de commande avec getopt
    while ((option = getopt(argc, argv, "cmp:")) != -1) {
        switch (option) {
            case 'c':
                cpu_info = 1;
                break;
            case 'm':
                mem_info = 1;
                break;
            case 'p':
                process_pid = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Utilisation : %s [-c] [-m] [-p <pid>]\n", argv[0]);
                exit(1);
        }
    }

    if (cpu_info) {
        printf("Informations du CPU :\n");
        getCPUInfo();
    }

    if (mem_info) {
        // Ajoutez ici votre code pour obtenir les informations sur la mémoire
        printf("Informations du memoire :\n");
        getMemoireInfo();
    }

    if (process_pid != -1) {
        printf("Informations du processus (PID %d) :\n", process_pid);
        getProcessInfo(process_pid);
    }

    return 0;
}
