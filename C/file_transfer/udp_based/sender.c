#include <stdio.h>
#include <sys/socket.h>

#include "utils.h"

int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    return 0;
}
