#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <pthread.h>
#include "resource.h"

#pragma comment(lib, "ws2_32.lib")

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
    struct addrinfo hints, *res;
    int status;
    static char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(domain, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
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

    inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);
    freeaddrinfo(res);
    return ipstr;
}

int check_port(char *host, int port) {
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    fd_set myset;
    struct timeval tv;
    int result, valopt;
    int optlen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("Error creating socket");
        return 1;
    }

    unsigned long mode = 1;
    ioctlsocket(sockfd, FIONBIO, &mode);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    result = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            FD_ZERO(&myset);
            FD_SET(sockfd, &myset);
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            if (select(0, NULL, &myset, NULL, &tv) > 0) {
                optlen = sizeof(int);
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &optlen);
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

    closesocket(sockfd);
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
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Exiting...\n");
        return 1;
    }

    char domain[100];
    char *host;
    int choice, port, start_port, end_port, result;
    int i;

    print_banner();

    printf("Address (domain or IP): ");
    scanf("%s", domain);

    host = resolve_domain(domain);
    if (!host) {
        printf("Failed to resolve domain. Exiting...\n");
        WSACleanup();
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

                for (i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
                    ThreadArgs *args = malloc(sizeof(ThreadArgs));
                    args->host = host;
                    args->port = common_ports[i];
                    pthread_create(&threads[i], NULL, thread_check_port, args);
                }

                for (i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
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

                pthread_t threads[100];  // Maximum threads at a time

                for (port = start_port; port <= end_port;) {
                    int thread_count = 0;

                    for (i = 0; i < 50 && port <= end_port; i++) {
                        ThreadArgs *args = malloc(sizeof(ThreadArgs));
                        args->host = host;
                        args->port = port;
                        pthread_create(&threads[i], NULL, thread_check_port, args);
                        thread_count++;
                        port++;
                    }

                    for (i = 0; i < thread_count; i++) {
                        pthread_join(threads[i], NULL);
                    }

                    if (port <= end_port) {
                        Sleep(1000);  // second pause
                    }
                }

                printf("Port scan completed.\n");
                break;
            }
            case 4:
                WSACleanup();
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

    WSACleanup();
    return 0;
}

