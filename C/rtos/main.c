#include "mac.h"

int main(void) {
    uint8_t str[8];// payload
    uint8_t speed_value = 120;

    str[0] = speed_value;
    str[1] = 0x00;
    str[2] = 0x00;
    str[3] = 0x00;

    uint32_t mac = generate_mac(str, 4);

    str[4] = (mac >> 24) & 0xFF;
    str[5] = (mac >> 16) & 0xFF;
    str[6] = (mac >> 8) & 0xFF;
    str[7] = mac & 0xFF;

    printf("Sending CAN Frame: Data[%d] | MAC[%08X]\n", str[0], mac);

    return 0;
}
