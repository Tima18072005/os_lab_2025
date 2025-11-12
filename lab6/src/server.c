#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>

uint64_t factorial_range(int start, int end) {
    uint64_t result = 1;
    for (int i = start + 1; i <= end; i++) {
        result *= i;
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(server_fd); return 1;
    }
    if (listen(server_fd, 1) < 0) {
        perror("listen"); close(server_fd); return 1;
    }

    printf("Server listening on port %d\n", port);
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept"); close(server_fd); return 1;
    }

    int32_t net_start, net_end;
    if (recv(client_fd, &net_start, sizeof(net_start), 0) <= 0 ||
        recv(client_fd, &net_end, sizeof(net_end), 0) <= 0) {
        perror("recv"); close(client_fd); close(server_fd); return 1;
    }
    int start = ntohl(net_start);
    int end = ntohl(net_end);
    printf("Calculating factorial range: %d to %d\n", start, end);

    uint64_t partial = factorial_range(start, end);
    uint64_t net_partial = htobe64(partial);

    if (send(client_fd, &net_partial, sizeof(net_partial), 0) <= 0) {
        perror("send"); close(client_fd); close(server_fd); return 1;
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
