

#include "sys.h"
#include "bios.h"


void usbTerminal();

u16 char_buff[G_SCREEN_W * G_SCREEN_H];

int main(void) {


    sysInit();
    gInit(char_buff);
    bi_init();

    gConsPrint("USB COM terminal");
    sysRepaint();

    usbTerminal();

    return 0;

}


void usbTerminal() {


    u8 data[BI_MIN_USB_SIZE + 1];
    u8 tout;


    while (1) {
        
        if (!bi_usb_can_rd())continue;
        tout = bi_usb_rd(data, BI_MIN_USB_SIZE); //read from virtual serial port. you should send 16 or more bytes at tine
        if (tout)continue;
        
        bi_usb_wr(data, BI_MIN_USB_SIZE); //send echo back to serial port
        
        gConsPrint(data);
        sysRepaint();
    }
}
