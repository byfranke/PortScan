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

#define MAX_THREADS 100

char *ip_address = NULL;
char *host_name = NULL;

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

// main task
typedef struct {
    char *host;
    int port;
} Task;

// task queue
typedef struct {
    Task *tasks[MAX_THREADS];
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} TaskQueue;

void initTaskQueue(TaskQueue *queue) {
    queue->front = -1;
    queue->rear = -1;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}

int enqueueTask(TaskQueue *queue, Task *task) {
    pthread_mutex_lock(&queue->mutex);
    if (queue->rear == MAX_THREADS - 1) {
        pthread_mutex_unlock(&queue->mutex);
        return -1; // full queue
    }

    if (queue->front == -1) {
        queue->front = 0;
    }

    queue->rear++;
    queue->tasks[queue->rear] = task;
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

Task *dequeueTask(TaskQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->front == -1) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    Task *task = queue->tasks[queue->front];
    queue->front++;

    if (queue->front > queue->rear) {
        queue->front = queue->rear = -1;
    }

    pthread_mutex_unlock(&queue->mutex);
    return task;
}

void *thread_check_port(void *args) {
    Task *targs = (Task *)args;
    int result = check_port(targs->host, targs->port);
    if (result == 0) {
        printf("Port %d is open.\n", targs->port);

        // reverse dns lookup
        struct sockaddr_in sa;
        sa.sin_addr.s_addr = inet_addr(targs->host);
        struct hostent *host_info = gethostbyaddr(
            (void *)&sa.sin_addr, 
            sizeof sa.sin_addr, 
            AF_INET
        );

        if (host_info != NULL) {
            if (ip_address == NULL) {
                // changing the state 'ip_address'
                ip_address = strdup(targs->host);
            }
            
            if (host_name == NULL) {
                // changing the state 'host_name'
                host_name = strdup(host_info->h_name);
            }
        } else {
            switch (h_errno) {
                case HOST_NOT_FOUND:
                    printf("Host not found.\n");
                    break;
                case NO_ADDRESS:
                    printf("No IP address found for the given host.\n");
                    break;
                case NO_RECOVERY:
                    printf("A non-recoverable error occurred.\n");
                    break;
                case TRY_AGAIN:
                    printf("Temporary DNS error; try again later.\n");
                    break;
                default:
                    printf("Error resolving host name.\n");
                    break;
            }
        }
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

    TaskQueue taskQueue;
    initTaskQueue(&taskQueue);

    pthread_t threads[MAX_THREADS];

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

                for (int i = 0; i < sizeof(common_ports) / sizeof(common_ports[0]); i++) {
                    Task *task = (Task *)malloc(sizeof(Task));
                    task->host = host;
                    task->port = common_ports[i];
                    enqueueTask(&taskQueue, task);
                }

                int num_threads = sizeof(common_ports) / sizeof(common_ports[0]);
                for (int i = 0; i < num_threads; i++) {
                    pthread_create(&threads[i], NULL, thread_check_port, dequeueTask(&taskQueue));
                }

                for (int i = 0; i < num_threads; i++) {
                    pthread_join(threads[i], NULL);
                }

                if (ip_address != NULL && host_name != NULL) {
                    printf("Reverse DNS lookup...\n");
                    sleep(1);
                    printf("IP Address: %s\n", ip_address);
                    printf("Host Name: %s\n", host_name);
                }

                printf("Port scan completed.\n");
                break;
            }

            case 2: {
                printf("Port number to check: ");
                scanf("%d", &port);
                Task *task = (Task *)malloc(sizeof(Task));
                task->host = host;
                task->port = port;
                enqueueTask(&taskQueue, task);

                pthread_create(&threads[0], NULL, thread_check_port, dequeueTask(&taskQueue));
                pthread_join(threads[0], NULL);

                break;
            }

            case 3: {
                printf("Starting port: ");
                scanf("%d", &start_port);
                printf("Ending port: ");
                scanf("%d", &end_port);

                for (port = start_port; port <= end_port; port++) {
                    Task *task = (Task *)malloc(sizeof(Task));
                    task->host = host;
                    task->port = port;
                    enqueueTask(&taskQueue, task);
                }

                int num_threads = (end_port - start_port) + 1;
                for (int i = 0; i < num_threads; i++) {
                    pthread_create(&threads[i], NULL, thread_check_port, dequeueTask(&taskQueue));
                }

                for (int i = 0; i < num_threads; i++) {
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