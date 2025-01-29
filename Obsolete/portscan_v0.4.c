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
#include <ctype.h>

#define MAX_THREADS 50

void print_banner() {
    printf("\n");
    printf("     _ \\               |     ___| \n");
    printf("    |   |  _ \\    __|  __| \\___ \\    __|   _` |  __ \\ \n");
    printf("    ___/  (   |  |     |         |  (     (   |  |   | \n");
    printf("   _|    \\___/  _|    \\__| _____/  \\___| \\__,_| _|  _| \n");
    printf("                   github.com/byfranke V0.4    \n");
    printf("\n");
}

int validate_port(int port) {
    return port >= 0 && port <= 65535;
}

char* resolve_domain(const char* domain) {
    struct addrinfo hints, *res;
    char *ip = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return NULL;
    }

    void *addr;
    if (res->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        addr = &(ipv4->sin_addr);
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    ip = malloc(INET6_ADDRSTRLEN);
    inet_ntop(res->ai_family, addr, ip, INET6_ADDRSTRLEN);

    freeaddrinfo(res);
    return ip;
}

int check_port(char *host, int port, int timeout) {
    struct addrinfo hints, *res;
    int sockfd, result;
    char port_str[6];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
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
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (select(sockfd + 1, NULL, &myset, NULL, &tv) > 0) {
        int so_error;
        socklen_t len = sizeof(so_error);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) {
            close(sockfd);
            freeaddrinfo(res);
            return 0;
        }
    }

    close(sockfd);
    freeaddrinfo(res);
    return -1;
}

typedef struct {
    char *host;
    int port;
    int timeout;
} ThreadArgs;

void *thread_check_port(void *args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int result = check_port(targs->host, targs->port, targs->timeout);
    if (result == 0) {
        printf("Port %d is open.\n", targs->port);
    }
    free(targs->host);
    free(args);
    return NULL;
}

void scan_common_ports(char *host, int timeout) {
    int common_ports[] = {21, 22, 23, 25, 53, 80, 110, 135, 143, 443, 587, 993, 995, 1433, 3306, 3389, 5432, 5900, 8080};
    pthread_t threads[sizeof(common_ports) / sizeof(common_ports[0])];

    for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->host = strdup(host);
        args->port = common_ports[i];
        args->timeout = timeout;
        pthread_create(&threads[i], NULL, thread_check_port, args);
    }

    for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Common port scan completed.\n");
}

void scan_port_range(char *host, int start_port, int end_port, int timeout) {
    pthread_t threads[MAX_THREADS];
    int thread_count = 0;

    for (int port = start_port; port <= end_port;) {
        for (int i = 0; i < MAX_THREADS && port <= end_port; i++) {
            ThreadArgs *args = malloc(sizeof(ThreadArgs));
            args->host = strdup(host);
            args->port = port;
            args->timeout = timeout;
            pthread_create(&threads[i], NULL, thread_check_port, args);
            thread_count++;
            port++;
        }

        for (int i = 0; i < thread_count; i++) {
            pthread_join(threads[i], NULL);
        }

        thread_count = 0;
        if (port <= end_port) {
            sleep(1);
        }
    }

    printf("Range port scan completed.\n");
}

void parse_args(int argc, char *argv[], char **host, int *option, int *timeout, int *start_port, int *end_port) {
    *timeout = 1; 
    *option = 0;
    *start_port = *end_port = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            *option = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            *timeout = atoi(argv[++i]);
        } else if (strchr(argv[i], '-') != NULL && isdigit(argv[i][0])) {
            sscanf(argv[i], "%d-%d", start_port, end_port);
        } else {
            *host = argv[i];
        }
    }
}

int main(int argc, char *argv[]) {
    char *host = NULL;
    int option, timeout, start_port, end_port;

    print_banner();

    if (argc < 2) {
        printf("Usage: %s <domain or IP> -o <option> -t <timeout> [range]\n", argv[0]);
        return 1;
    }

    parse_args(argc, argv, &host, &option, &timeout, &start_port, &end_port);

    if (!host) {
        printf("Host not specified. Exiting...\n");
        return 1;
    }

    char *resolved_host = resolve_domain(host);
    if (!resolved_host) {
        printf("Failed to resolve host. Exiting...\n");
        return 1;
    }

    switch (option) {
        case 1:
            scan_common_ports(resolved_host, timeout);
            break;
        case 2:
            if (start_port > 0 && validate_port(start_port)) {
                if (check_port(resolved_host, start_port, timeout) == 0) {
                    printf("Port %d is open.\n", start_port);
                } else {
                    printf("Port %d is closed or unreachable.\n", start_port);
                }
            } else {
                printf("Invalid port specified.\n");
            }
            break;
        case 3:
            if (validate_port(start_port) && validate_port(end_port) && start_port <= end_port) {
                scan_port_range(resolved_host, start_port, end_port, timeout);
            } else {
                printf("Invalid range specified.\n");
            }
            break;
        default:
            printf("Invalid option specified.\n");
            break;
    }

    free(resolved_host);
    return 0;
}
