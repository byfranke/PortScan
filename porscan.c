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

void print_banner() {
    printf("\n");
    printf("     _ \\               |     ___| \n");
    printf("    |   |  _ \\    __|  __| \\___ \\    __|   _` |  __ \\ \n");
    printf("    ___/  (   |  |     |         |  (     (   |  |   | \n");
    printf("   _|    \\___/  _|    \\__| _____/  \\___| \\__,_| _|  _| \n");
    printf("              github.com/byfranke v0.2         \n");
    printf("\n");
}

char* resolve_domain(const char* domain) {
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(domain)) == NULL) {
        herror("gethostbyname");
        return NULL;
    }

    addr_list = (struct in_addr **) he->h_addr_list;
    if (addr_list[0] != NULL) {
        return inet_ntoa(*addr_list[0]);
    }

    return NULL;
}

int check_port(char *host, int port) {
    int sockfd, result, valopt;
    struct sockaddr_in server_addr;
    fd_set myset;
    struct timeval tv;
    socklen_t lon;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    result = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0) {
        if (errno == EINPROGRESS) {
            tv.tv_sec = 1; // second timeout
            tv.tv_usec = 0;
            FD_ZERO(&myset);
            FD_SET(sockfd, &myset);
            if (select(sockfd+1, NULL, &myset, NULL, &tv) > 0) {
                lon = sizeof(int);
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
                if (valopt) {
                    result = -1;
                } else {
                    result = 0;
                }
            } else {
                result = -1;
            }
        } else {
            result = -1;
        }
    } else {
        result = 0;
    }

    close(sockfd);
    return result;
}

void print_menu() {
    printf("\nChoose an option:\n");
    printf("[1] Common ports (e.g., 80, 443, 22)\n");
    printf("[2] Specific port\n");
    printf("[3] Range of ports\n");
    printf("[4] Exit\n");
}

typedef struct {
    char *host;
    int port;
} ThreadArgs;

void *thread_check_port(void *args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int result = check_port(targs->host, targs->port);
    if (result == 0) {
        printf("Port %d is open.\n", targs->port);
    }
    free(args);
    return NULL;
}


int main() {
    char domain[100];
    char *host;
    int choice, port, start_port, end_port, result;

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
        scanf("%d", &choice);

        switch (choice) {
        case 1: {
            int common_ports[] = {21, 22, 23, 25, 53, 80, 110, 135, 143, 443, 587, 993, 995, 1433, 3306, 3389, 5432, 5900, 8080};
            char *services[] = {
                "FTP", "SSH", "Telnet", "SMTP", "DNS", "HTTP", "POP3", "MS RPC", 
                "IMAP", "HTTPS", "SMTP (TLS)", "IMAPS", "POP3S", "Microsoft SQL Server", 
                "MySQL", "RDP", "PostgreSQL", "VNC", "HTTP Proxy"
            };

            pthread_t threads[sizeof(common_ports) / sizeof(common_ports[0])];

            for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
                ThreadArgs *args = malloc(sizeof(ThreadArgs));
                args->host = host;
                args->port = common_ports[i];
                pthread_create(&threads[i], NULL, thread_check_port, args);
            }

            for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
                pthread_join(threads[i], NULL);
            }

            printf("Port scan completed.\n");
            break;
        }

            case 2: {
                printf("Port number to check: ");
                scanf("%d", &port);
                result = check_port(host, port);
                if (result == 0) {
                    printf("Port %d is open.\n", port);
                } else {
                    printf("Port %d is closed.\n", port);
                }
                break;
            }
            case 3: {
            printf("Starting port: ");
            scanf("%d", &start_port);
            printf("Ending port: ");
            scanf("%d", &end_port);

            pthread_t threads[end_port - start_port + 1];

            int thread_count = 0;
            for (port = start_port; port <= end_port; port++) {
                ThreadArgs *args = malloc(sizeof(ThreadArgs));
                args->host = host;
                args->port = port;
                pthread_create(&threads[thread_count], NULL, thread_check_port, args);
                thread_count++;
            }

            for (int i = 0; i < thread_count; i++) {
                pthread_join(threads[i], NULL);
            }

            printf("Port scan completed.\n");
            break;
        }
            case 4:
                exit(0);
            default:
                printf("Invalid choice. Please choose a valid option (1, 2, 3, or 4).\n");
        }

        char exit_choice;
        printf("\nTo continue (Y/N)? ");
        scanf(" %c", &exit_choice);
        if (exit_choice == 'n' || exit_choice == 'N') {
            break;
        }
    }

    return 0;
}
