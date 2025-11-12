#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct {
    char ip[64];
    int port;
} Server;

uint64_t mod_mult(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;
    a %= mod;
    while (b) {
        if (b & 1) result = (result + a) % mod;
        a = (a << 1) % mod;
        b >>= 1;
    }
    return result;
}

int parse_server_line(const char* line, Server *srv) {
    char copy[128];
    strncpy(copy, line, sizeof(copy));
    copy[sizeof(copy)-1] = '\0';
    char *colon = strchr(copy, ':');
    if (!colon) return -1;
    *colon = 0;
    strncpy(srv->ip, copy, sizeof(srv->ip));
    srv->port = atoi(colon+1);
    return 0;
}

int send_range_and_receive(Server *srv, int start, int end, uint64_t *result) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(srv->port);
    if (inet_pton(AF_INET, srv->ip, &addr.sin_addr) <= 0) {
        perror("inet_pton"); close(sock); return -1;
    }
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect"); close(sock); return -1;
    }
    int32_t net_start = htonl(start);
    int32_t net_end = htonl(end);

    if (send(sock, &net_start, sizeof(net_start), 0) <= 0 || 
        send(sock, &net_end, sizeof(net_end), 0) <= 0) {
        perror("send"); close(sock); return -1;
    }

    uint64_t net_res = 0;
    if (recv(sock, &net_res, sizeof(net_res), 0) <= 0) {
        perror("recv"); close(sock); return -1;
    }
    *result = be64toh(net_res);
    close(sock);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <k> <mod> <servers_file>\n", argv[0]);
        return 1;
    }
    int k = atoi(argv[1]);
    uint64_t mod = strtoull(argv[2], NULL, 10);
    const char *filename = argv[3];

    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); return 1; }
    Server servers[16];
    int count = 0;
    char line[128];
    while (fgets(line, sizeof(line), f) && count < 16) {
        line[strcspn(line, "\r\n")] = 0;
        if (parse_server_line(line, &servers[count]) == 0) count++;
    }
    fclose(f);
    if (count == 0) {
        fprintf(stderr, "No servers loaded\n");
        return 1;
    }

    // Делим диапазон 0..k для count серверов
    int starts[16];
    int ends[16];
    starts[0] = 0;
    for (int i = 0; i < count; i++) {
        ends[i] = (i == count - 1) ? k : (k * (i + 1)) / count;
        if (i > 0) starts[i] = ends[i-1];
    }

    uint64_t final_res = 1;
    for (int i = 0; i < count; i++) {
        uint64_t part_res = 0;
        if (send_range_and_receive(&servers[i], starts[i], ends[i], &part_res) != 0) {
            fprintf(stderr, "Failed to get result from server %s:%d\n", servers[i].ip, servers[i].port);
            return 1;
        }
        final_res = mod_mult(final_res, part_res, mod);
    }

    printf("Factorial mod %llu = %llu\n", mod, final_res);
    return 0;
}
