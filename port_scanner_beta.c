#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define MAX_THREADS 50

// Função para exibir o banner do programa
void print_banner() {
    printf("\n");
    printf("     _ \\               |     ___| \n");
    printf("    |   |  _ \\    __|  __| \\___ \\    __|   _` |  __ \\ \n");
    printf("    ___/  (   |  |     |         |  (     (   |  |   | \n");
    printf("   _|    \\___/  _|    \\__| _____/  \\___| \\__,_| _|  _| \n");
    printf("              github.com/byfranke v0.2         \n");
    printf("\n");
}

// Função para validar se uma porta está no intervalo válido (0-65535)
int validate_port(int port) {
    return port >= 0 && port <= 65535;
}

// Função para validar a escolha do menu
int validate_choice(int choice) {
    return choice >= 1 && choice <= 4;
}

// Função para resolver um domínio para um endereço IP (suporta IPv4 e IPv6)
char* resolve_domain(const char* domain) {
    struct addrinfo hints, *res;
    char *ip = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return NULL;
    }

    void *addr;
    if (res->ai_family == AF_INET) { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        addr = &(ipv4->sin_addr);
    } else { // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    ip = malloc(INET6_ADDRSTRLEN);
    inet_ntop(res->ai_family, addr, ip, INET6_ADDRSTRLEN);

    freeaddrinfo(res);
    return ip;
}

// Função para verificar se uma porta está aberta
int check_port(char *host, int port) {
    struct addrinfo hints, *res;
    int sockfd, result;
    char port_str[6];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("Error creating socket");
        freeaddrinfo(res);
        return -1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    result = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (result < 0 && errno != EINPROGRESS) {
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    fd_set myset;
    FD_ZERO(&myset);
    FD_SET(sockfd, &myset);

    struct timeval tv;
    tv.tv_sec = 1; // timeout de 1 segundo
    tv.tv_usec = 0;

    if (select(sockfd + 1, NULL, &myset, NULL, &tv) > 0) {
        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) {
            close(sockfd);
            freeaddrinfo(res);
            return 0; // Porta aberta
        }
    }

    close(sockfd);
    freeaddrinfo(res);
    return -1; // Porta fechada ou erro
}

// Função para exibir o menu de opções
void print_menu() {
    printf("\nChoose an option:\n");
    printf("[1] Common ports (e.g., 80, 443, 22)\n");
    printf("[2] Specific port\n");
    printf("[3] Range of ports\n");
    printf("[4] Exit\n");
}

// Estrutura para passar argumentos para as threads
typedef struct {
    char *host;
    int port;
} ThreadArgs;

// Função executada por cada thread para verificar uma porta
void *thread_check_port(void *args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int result = check_port(targs->host, targs->port);
    if (result == 0) {
        printf("Port %d is open.\n", targs->port);
    }
    free(targs->host); // Libera a memória alocada para o host
    free(args); // Libera a memória alocada para os argumentos
    return NULL;
}

// Função para escanear portas comuns
void scan_common_ports(char *host) {
    int common_ports[] = {21, 22, 23, 25, 53, 80, 110, 135, 143, 443, 587, 993, 995, 1433, 3306, 3389, 5432, 5900, 8080};
    pthread_t threads[sizeof(common_ports) / sizeof(common_ports[0])];

    for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->host = strdup(host); // Aloca e copia o host
        args->port = common_ports[i];
        pthread_create(&threads[i], NULL, thread_check_port, args);
    }

    for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Port scan completed.\n");
}

// Função principal
int main() {
    char domain[100];
    char *host;
    int choice, port, start_port, end_port;

    print_banner();

    printf("Address (domain or IP): ");
    scanf("%s", domain);

    host = resolve_domain(domain);
    if (!host) {
        printf("Failed to resolve domain. Exiting...\n");
        return 1;
    }

    while (1) {
        print_menu();
        printf("Option: ");
        while (scanf("%d", &choice) != 1 || !validate_choice(choice)) {
            printf("Invalid choice. Please choose a valid option (1, 2, 3, or 4): ");
            while (getchar() != '\n'); // Limpa o buffer de entrada
        }

        switch (choice) {
            case 1:
                scan_common_ports(host);
                break;

            case 2:
                printf("Port number to check: ");
                while (scanf("%d", &port) != 1 || !validate_port(port)) {
                    printf("Invalid port. Please enter a valid port (0-65535): ");
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                }
                if (check_port(host, port) == 0) {
                    printf("Port %d is open.\n", port);
                } else {
                    printf("Port %d is closed or unreachable.\n", port);
                }
                break;

            case 3:
                printf("Starting port: ");
                while (scanf("%d", &start_port) != 1 || !validate_port(start_port)) {
                    printf("Invalid port. Please enter a valid port (0-65535): ");
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                }
                printf("Ending port: ");
                while (scanf("%d", &end_port) != 1 || !validate_port(end_port) || end_port < start_port) {
                    printf("Invalid port. Please enter a valid port (0-65535) greater than or equal to the starting port: ");
                    while (getchar() != '\n'); // Limpa o buffer de entrada
                }

                pthread_t threads[MAX_THREADS];
                int thread_count = 0;

                for (port = start_port; port <= end_port;) {
                    for (int i = 0; i < MAX_THREADS && port <= end_port; i++) {
                        ThreadArgs *args = malloc(sizeof(ThreadArgs));
                        args->host = strdup(host); // Aloca e copia o host
                        args->port = port;
                        pthread_create(&threads[i], NULL, thread_check_port, args);
                        thread_count++;
                        port++;
                    }

                    for (int i = 0; i < thread_count; i++) {
                        pthread_join(threads[i], NULL);
                    }

                    thread_count = 0;
                    if (port <= end_port) {
                        sleep(1); // Pausa de 1 segundo
                    }
                }

                printf("Port scan completed.\n");
                break;

            case 4:
                free(host); // Libera a memória alocada para o host
                exit(0);

            default:
                printf("Invalid choice. Please choose a valid option (1, 2, 3, or 4).\n");
        }

        char exit_choice;
        printf("\nTo continue (Y/N)? ");
        scanf(" %c", &exit_choice);
        if (exit_choice == 'n' || exit_choice == 'N') {
            free(host); // Libera a memória alocada para o host
            break;
        }
    }

    return 0;
}
