#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

typedef struct {
    char ip[64];
    int port;
} Server;

int parse_server_line(const char *line, Server *server) {
    char copy[128];
    strncpy(copy, line, sizeof(copy));
    copy[sizeof(copy)-1] = '\0';
    char *colon = strchr(copy, ':');
    if (!colon) return -1;
    *colon = 0;
    strncpy(server->ip, copy, sizeof(server->ip));
    server->port = atoi(colon + 1);
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

    FILE *fp = fopen(filename, "r");
    if (!fp) { perror("fopen"); return 1; }

    Server servers[16];
    int count = 0;
    char line[128];

    while (fgets(line, sizeof(line), fp) && count < 16) {
        line[strcspn(line, "\r\n")] = 0;
        if (parse_server_line(line, &servers[count]) == 0)
            count++;
    }
    fclose(fp);

    if (count == 0) {
        fprintf(stderr, "No servers found\n");
        return 1;
    }

    int starts[16], ends[16];
    starts[0] = 0;
    for (int i = 0; i < count; i++) {
        ends[i] = (i == count -1) ? k : (k * (i + 1)) / count;
        if (i > 0) starts[i] = ends[i-1];
    }

    uint64_t final_result = 1;
    for (int i = 0; i < count; i++) {
        uint64_t partial = 0;
        if (send_range_and_receive_result(servers[i].ip, servers[i].port, starts[i], ends[i], &partial) != 0) {
            fprintf(stderr, "Failed to get result from server %s:%d\n", servers[i].ip, servers[i].port);
            return 1;
        }
        final_result = mod_mult(final_result, partial, mod);
    }

    printf("Factorial mod %llu = %llu\n", mod, final_result);
    return 0;
}
