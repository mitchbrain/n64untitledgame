/* 
 * File:   bios.h
 * Author: krik
 *
 * Created on September 22, 2013, 2:34 PM
 */

#ifndef BIOS_H
#define	BIOS_H

#include "sys.h"

#define BI_ERR_I2C_CMD     0xB0
#define BI_ERR_I2C_TOUT    0xB1
#define BI_ERR_USB_TOUT    0xB2
#define BI_ERR_MCN_CFG     0xB3
#define BI_ERR_IOM_CFG     0xB4

#define BI_ROM_ADDR    (KSEG1 | 0x10000000)
#define BI_RAM_ADDR    (KSEG1 | 0x08000000)

#define BI_ROM_SLEN     0x20000
#define BI_RAM_SLEN     0x100

#define BI_DISK_SPD_LO  0
#define BI_DISK_SPD_HI  1

#define BI_BCFG_BOOTMOD 1
#define BI_BCFG_SD_INIT 2
#define BI_BCFG_SD_TYPE 4
#define BI_BCFG_GAMEMOD 8
#define BI_BCFG_CICLOCK 0x8000

#define BI_DD_TBL_SIZE  2048
#define BI_DD_PGE_SIZE  0x8000


#define BI_MIN_USB_SIZE 16

#define SAVE_OFF        0
#define SAVE_EEP4K      1
#define SAVE_EEP16K     2
#define SAVE_SRM32K     3
#define SAVE_SRM96K     4
#define SAVE_FLASH      5
#define SAVE_SRM128K    6 
#define SAVE_MPAK       8
#define SAVE_DD64       16

void bi_init();

u8 bi_usb_can_rd();
u8 bi_usb_can_wr();
u8 bi_usb_rd(void *dst, u32 len);
u8 bi_usb_wr(void *src, u32 len);


void bi_set_save_type(u8 type);



#endif	/* BIOS_H */

