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
#include <stdbool.h>
#include <poll.h>
#include <signal.h>

#define MAX_THREADS 50
#define MAX_PORTS 100
#define LOG_DEBUG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

void print_banner() {
    printf("\n");
    printf("     _ \\               |     ___| \n");
    printf("    |   |  _ \\    __|  __| \\___ \\    __|   _` |  __ \\ \n");
    printf("    ___/  (   |  |     |         |  (     (   |  |   | \n");
    printf("   _|    \\___/  _|    \\__| _____/  \\___| \\__,_| _|  _| \n");
    printf("\n");
    printf("           github.com/byfranke Beta Version  \n");
    printf("\n");
}

void print_help() {
    printf("Usage: portscan <domain or IP> [options]\n");
    printf("Options:\n");
    printf("  -o, --option <option>  Select an option:\n");
    printf("                         1 - Scan common ports\n");
    printf("                         2 - Scan specific ports (comma-separated)\n");
    printf("                         3 - Scan a range of ports\n");
    printf("  -p, --port <ports>     Specify ports to scan (used with option 2, e.g., 80,443,8080)\n");
    printf("  -t, --time <timeout>   Set timeout for port scan (default: 1 second)\n");
    printf("  -h, --help             Show this help message\n");
    printf("  --update               Update the script to the latest version\n");
    printf("\n");
    printf("Examples:\n");
    printf("  portscan example.com -o 1 -t 2\n");
    printf("  portscan example.com -o 2 -p 80,443,8080 -t 1\n");
    printf("  portscan example.com -o 3 -t 1 1-1000\n");
    printf("  portscan example.com --update\n");
}

void update_script() {
    printf("Updating the script...\n");
    if (system("mkdir -p /tmp/portscan_update") != 0) {
        LOG_ERROR("Failed to create update directory");
        return;
    }
    if (system("git clone https://github.com/byfranke/PortScan /tmp/portscan_update") != 0) {
        LOG_ERROR("Failed to clone repository");
        return;
    }
    if (system("sudo bash /tmp/portscan_update/installer.sh") != 0) {
        LOG_ERROR("Failed to run installer script");
        return;
    }
    if (system("rm -rf /tmp/portscan_update") != 0) {
        LOG_ERROR("Failed to clean up update directory");
        return;
    }
    printf("Update completed.\n");
}

bool validate_port(int port) {
    return port >= 0 && port <= 65535;
}

char* resolve_domain(const char* domain) {
    struct addrinfo hints, *res;
    char *ip = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(domain, NULL, &hints, &res) != 0) {
        LOG_ERROR("getaddrinfo failed");
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
    if (!ip) {
        LOG_ERROR("malloc failed");
        freeaddrinfo(res);
        return NULL;
    }

    inet_ntop(res->ai_family, addr, ip, INET6_ADDRSTRLEN);

    freeaddrinfo(res);
    return ip;
}

int check_port(const char *host, int port, int timeout) {
    struct addrinfo hints, *res;
    int sockfd, result;
    char port_str[6];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        LOG_ERROR("getaddrinfo failed for port %d", port);
        return -1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        LOG_ERROR("socket creation failed for port %d", port);
        freeaddrinfo(res);
        return -1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    result = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (result < 0 && errno != EINPROGRESS) {
        LOG_ERROR("connect failed for port %d", port);
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    struct pollfd fds;
    fds.fd = sockfd;
    fds.events = POLLOUT;

    result = poll(&fds, 1, timeout * 1000);
    if (result > 0 && (fds.revents & POLLOUT)) {
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
    free(targs);
    return NULL;
}

void scan_common_ports(const char *host, int timeout) {
    int common_ports[] = {21, 22, 23, 25, 53, 80, 110, 135, 143, 443, 587, 993, 995, 1433, 3306, 3389, 5432, 5900, 8080};
    pthread_t threads[sizeof(common_ports) / sizeof(common_ports[0])];

    for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (!args) {
            LOG_ERROR("malloc failed");
            continue;
        }
        args->host = strdup(host);
        if (!args->host) {
            LOG_ERROR("strdup failed");
            free(args);
            continue;
        }
        args->port = common_ports[i];
        args->timeout = timeout;
        if (pthread_create(&threads[i], NULL, thread_check_port, args) != 0) {
            LOG_ERROR("pthread_create failed");
            free(args->host);
            free(args);
        }
    }

    for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Common port scan completed.\n");
}

void scan_specific_ports(const char *host, const char *ports_str, int timeout) {
    pthread_t threads[MAX_PORTS];
    int ports[MAX_PORTS];
    int port_count = 0;
    char *token;
    char *copy = strdup(ports_str);

    token = strtok(copy, ",");
    while (token != NULL && port_count < MAX_PORTS) {
        int port = atoi(token);
        if (validate_port(port)) {
            ports[port_count++] = port;
        } else {
            LOG_ERROR("Invalid port: %s", token);
        }
        token = strtok(NULL, ",");
    }
    free(copy);

    for (int i = 0; i < port_count; i++) {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (!args) {
            LOG_ERROR("malloc failed");
            continue;
        }
        args->host = strdup(host);
        if (!args->host) {
            LOG_ERROR("strdup failed");
            free(args);
            continue;
        }
        args->port = ports[i];
        args->timeout = timeout;
        if (pthread_create(&threads[i], NULL, thread_check_port, args) != 0) {
            LOG_ERROR("pthread_create failed");
            free(args->host);
            free(args);
        }
    }

    for (int i = 0; i < port_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Specific port scan completed.\n");
}

void scan_port_range(const char *host, int start_port, int end_port, int timeout) {
    pthread_t threads[MAX_THREADS];
    int thread_count = 0;

    for (int port = start_port; port <= end_port;) {
        for (int i = 0; i < MAX_THREADS && port <= end_port; i++) {
            ThreadArgs *args = malloc(sizeof(ThreadArgs));
            if (!args) {
                LOG_ERROR("malloc failed");
                continue;
            }
            args->host = strdup(host);
            if (!args->host) {
                LOG_ERROR("strdup failed");
                free(args);
                continue;
            }
            args->port = port;
            args->timeout = timeout;
            if (pthread_create(&threads[i], NULL, thread_check_port, args) != 0) {
                LOG_ERROR("pthread_create failed");
                free(args->host);
                free(args);
            } else {
                thread_count++;
                port++;
            }
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

void parse_args(int argc, char *argv[], char **host, int *option, int *timeout, int *start_port, int *end_port, char **specific_ports) {
    *timeout = 1; 
    *option = 0;
    *start_port = *end_port = 0;
    *specific_ports = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--option") == 0) {
            if (i + 1 < argc) {
                *option = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
            if (i + 1 < argc) {
                *timeout = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                *specific_ports = argv[++i];
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            exit(0);
        } else if (strcmp(argv[i], "--update") == 0) {
            update_script();
            exit(0);
        } else if (strchr(argv[i], '-') != NULL && isdigit(argv[i][0])) {
            sscanf(argv[i], "%d-%d", start_port, end_port);
        } else {
            *host = argv[i];
        }
    }
}

void handle_signal(int sig) {
    printf("\nReceived signal %d. Exiting...\n", sig);
    exit(0);
}

int main(int argc, char *argv[]) {
    char *host = NULL;
    int option, timeout, start_port, end_port;
    char *specific_ports = NULL;

    signal(SIGINT, handle_signal);

    print_banner();

    if (argc < 2) {
        print_help();
        return 1;
    }

    parse_args(argc, argv, &host, &option, &timeout, &start_port, &end_port, &specific_ports);

    if (!host) {
        printf("Host not specified. Exiting...\n");
        return 1;
    }

    char *resolved_host = resolve_domain(host);
    if (!resolved_host) {
        printf("Failed to resolve host. Exiting...\n");
        return 1;
    }

    if (option == 0) {
        option = 1; // Default to common port scan if no option is specified
    }

    switch (option) {
        case 1:
            scan_common_ports(resolved_host, timeout);
            break;
        case 2:
            if (specific_ports != NULL) {
                scan_specific_ports(resolved_host, specific_ports, timeout);
            } else {
                printf("No ports specified. Use -p or --port to specify ports.\n");
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
