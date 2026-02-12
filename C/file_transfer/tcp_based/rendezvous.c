#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logger.h"

int main(void) {
    int listen_sock, c1, c2;
    struct sockaddr_in server_addr, addr1, addr2;
    socklen_t len = sizeof(struct sockaddr_in);

    // 1. Server setup
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(listen_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        err("Bind failed");
    }
    listen(listen_sock, 2);

    info("VPS: Waiting for Peer1...");
    c1 = accept(listen_sock, (struct sockaddr *) &addr1, &len);
    success("VPS: Peer1 connected form %s:%d", inet_ntoa(addr1.sin_addr), ntohs(addr1.sin_port));

    info("VPS: Waiting for Peer2...");
    c2 = accept(listen_sock, (struct sockaddr *) &addr2, &len);
    success("VPS: Peer2 connected form %s:%d", inet_ntoa(addr2.sin_addr), ntohs(addr2.sin_port));

    // 2. Swaping infos
    info("VPS: Sending peers infos");
    send(c1, &addr2, sizeof(addr2), 0);
    send(c2, &addr1, sizeof(addr1), 0);
    success("VPS: Infos swapped. Closing connections.");

    close(c1);
    close(c2);
    close(listen_sock);

    return 0;
}
