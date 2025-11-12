#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// Вычисление факториала на отрезке (start, end]
uint64_t factorial_range(int start, int end) {
    uint64_t result = 1;
    for (int i = start + 1; i <= end; i++) {
        result *= i;
    }
    return result;
}

// Умножение по модулю с учетом возможного переполнения
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

// Отправка на сервер отрезка вычисления и прием результата
int send_range_and_receive_result(const char *ip, int port, int start, int end, uint64_t *result) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("inet_pton"); close(sock); return -1;
    }
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); close(sock); return -1;
    }
    int32_t net_start = htonl(start);
    int32_t net_end = htonl(end);

    if (send(sock, &net_start, sizeof(net_start), 0) <= 0 ||
        send(sock, &net_end, sizeof(net_end), 0) <= 0) {
        perror("send"); close(sock); return -1;
    }

    uint64_t net_result = 0;
    if (recv(sock, &net_result, sizeof(net_result), 0) <= 0) {
        perror("recv"); close(sock); return -1;
    }

    *result = be64toh(net_result);
    close(sock);
    return 0;
}
