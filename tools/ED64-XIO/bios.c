

#include "bios.h"



#define REG_BASE        0x1F800000
#define REG_FPG_CFG     0x0000
#define REG_USB_CFG     0x0004
#define REG_TIMER       0x000C
#define REG_BOOT_CFG    0x0010
#define REG_IOM_VER     0x0014
#define REG_I2C_CMD     0x0018
#define REG_I2C_DAT     0x001C

#define REG_FPG_DAT     0x0200
#define REG_USB_DAT     0x0400

#define REG_SYS_CFG     0x8000
#define REG_KEY         0x8004
#define REG_DMA_STA     0x8008
#define REG_DMA_ADDR    0x8008
#define REG_DMA_LEN     0x800C
#define REG_RTC_SET     0x8010
#define REG_RAM_CFG     0x8018
#define REG_IOM_CFG     0x801C
#define REG_SDIO        0x8020
#define REG_MCN_VER     0x8040
#define REG_SDIO_ARD    0x8200
#define REG_IOM_DAT     0x8400
#define REG_DD_TBL      0x8800
#define REG_SD_CMD_RD   (REG_SDIO + 0x00*4)
#define REG_SD_CMD_WR   (REG_SDIO + 0x01*4)
#define REG_SD_DAT_RD   (REG_SDIO + 0x02*4)
#define REG_SD_DAT_WR   (REG_SDIO + 0x03*4)
#define REG_SD_STATUS   (REG_SDIO + 0x04*4)

#define DMA_STA_BUSY    0x0001
#define DMA_STA_ERROR   0x0002
#define DMA_STA_LOCK    0x0080

#define SD_CFG_BITLEN   0x000F
#define SD_CFG_SPD      0x0010
#define SD_STA_BUSY     0x0080

#define CFG_BROM_ON     0x0001
#define CFG_REGS_OFF    0x0002
#define CFG_SWAP_ON     0x0004

#define FPG_CFG_NCFG    0x0001
#define FPG_STA_CDON    0x0001
#define FPG_STA_NSTAT   0x0002

#define I2C_CMD_DAT     0x10
#define I2C_CMD_STA     0x20
#define I2C_CMD_END     0x30

#define IOM_CFG_SS      0x0001
#define IOM_CFG_RST     0x0002
#define IOM_CFG_ACT     0x0080
#define IOM_STA_CDN     0x0001

#define USB_LE_CFG      0x8000
#define USB_LE_CTR      0x4000

#define USB_CFG_ACT     0x0200
#define USB_CFG_RD      0x0400
#define USB_CFG_WR      0x0000

#define USB_STA_ACT     0x0200
#define USB_STA_RXF     0x0400
#define USB_STA_TXE     0x0800
#define USB_STA_PWR     0x1000
#define USB_STA_BSY     0x2000

#define USB_CMD_RD_NOP  (USB_LE_CFG | USB_LE_CTR | USB_CFG_RD)
#define USB_CMD_RD      (USB_LE_CFG | USB_LE_CTR | USB_CFG_RD | USB_CFG_ACT)
#define USB_CMD_WR_NOP  (USB_LE_CFG | USB_LE_CTR | USB_CFG_WR)
#define USB_CMD_WR      (USB_LE_CFG | USB_LE_CTR | USB_CFG_WR | USB_CFG_ACT)

#define REG_LAT 0x04
#define REG_PWD 0x04

#define ROM_LAT 0x40
#define ROM_PWD 0x12

#define REG_ADDR(reg)   (KSEG1 | REG_BASE | (reg))

u32 bi_reg_rd(u16 reg);
void bi_reg_wr(u16 reg, u32 val);
void bi_usb_init();
u8 bi_usb_busy();


void bi_init() {

    IO_WRITE(PI_BSD_DOM1_LAT_REG, 0x04);
    IO_WRITE(PI_BSD_DOM1_PWD_REG, 0x0C);
    
    bi_reg_wr(REG_KEY, 0xAA55);
    bi_reg_wr(REG_SYS_CFG, 0);
    bi_usb_init();

    bi_set_save_type(SAVE_OFF);
}

void bi_reg_wr(u16 reg, u32 val) {

    sysPI_wr(&val, REG_ADDR(reg), 4);
}

u32 bi_reg_rd(u16 reg) {

    u32 val;
    sysPI_rd(&val, REG_ADDR(reg), 4);
    return val;
}


//****************************************************************************** usb section
//******************************************************************************
//******************************************************************************

void bi_usb_init() {

    u8 buff[512];
    u8 resp;
    bi_reg_wr(REG_USB_CFG, USB_CMD_RD_NOP); //turn off usb r/w activity

    //flush fifo buffer
    while (bi_usb_can_rd()) {
        resp = bi_usb_rd(buff, 512);
        if (resp)break;
    }
}

u8 bi_usb_can_rd() {

    u32 status = bi_reg_rd(REG_USB_CFG) & (USB_STA_PWR | USB_STA_RXF);
    if (status == USB_STA_PWR)return 1;
    return 0;
}

u8 bi_usb_can_wr() {

    u32 status = bi_reg_rd(REG_USB_CFG) & (USB_STA_PWR | USB_STA_TXE);
    if (status == USB_STA_PWR)return 1;
    return 0;
}

u8 bi_usb_busy() {

    u32 tout = 0;

    while ((bi_reg_rd(REG_USB_CFG) & USB_STA_ACT) != 0) {

        if (tout++ != 8192)continue;
        bi_reg_wr(REG_USB_CFG, USB_CMD_RD_NOP);
        return BI_ERR_USB_TOUT;
    }

    return 0;
}


u8 bi_usb_rd(void *dst, u32 len) {

    u8 resp = 0;
    u16 blen, baddr;

    while (len) {

        blen = 512;
        if (blen > len)blen = len;
        baddr = 512 - blen;

        bi_reg_wr(REG_USB_CFG, USB_CMD_RD | baddr);
        resp = bi_usb_busy();
        if (resp)break;

        sysPI_rd(dst, REG_ADDR(REG_USB_DAT + baddr), blen);

        dst += blen;
        len -= blen;
    }

    return resp;
}

u8 bi_usb_wr(void *src, u32 len) {

    u8 resp = 0;
    u16 blen, baddr;

    bi_reg_wr(REG_USB_CFG, USB_CMD_WR_NOP);

    while (len) {

        blen = 512;
        if (blen > len)blen = len;
        baddr = 512 - blen;

        sysPI_wr(src, REG_ADDR(REG_USB_DAT + baddr), blen);
        src += 512;

        bi_reg_wr(REG_USB_CFG, USB_CMD_WR | baddr);

        resp = bi_usb_busy();
        if (resp)break;

        len -= blen;
    }

    return resp;
}

void bi_set_save_type(u8 type) {

    bi_reg_wr(REG_RAM_CFG, type);
}