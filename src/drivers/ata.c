#include <stdarg.h>
#include "main.h"
#include "heap.h"
#include "ata.h"

ide_channel_regs_t ide_channels[2];
ide_device_t ide_devices[4];
unsigned char ide_buf[2048] = {0};
unsigned char ide_irq_invoked = 0;
unsigned char atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void ide_detect()
{
}

void ata_io_wait(unsigned short bus)
{
    inportb(bus + ATA_REG_ALTSTATUS);
    inportb(bus + ATA_REG_ALTSTATUS);
    inportb(bus + ATA_REG_ALTSTATUS);
    inportb(bus + ATA_REG_ALTSTATUS);
}

int ata_wait(unsigned short bus, int advanced)
{
    unsigned char status = 0;

    ata_io_wait(bus);

    while ((status = inportb(bus + ATA_REG_STATUS)) & ATA_SR_BSY)
        ;

    if (advanced)
    {
        status = inportb(bus + ATA_REG_STATUS);
        if (status & ATA_SR_ERR)
            return 1;
        if (status & ATA_SR_DF)
            return 1;
        if (!(status & ATA_SR_DRQ))
            return 1;
    }

    return 0;
}

void ata_select(unsigned short bus)
{
    outportb(bus + ATA_REG_HDDEVSEL, 0xA0);
}

void ata_wait_ready(unsigned short bus)
{
    while (inportb(bus + ATA_REG_STATUS) & ATA_SR_BSY)
        ;
}

void ide_init(unsigned short bus)
{

    puts("initializing IDE device on bus ");

    char buf1[255];

    itoa(bus, 16, buf1);
    puts(buf1);
    puts("\n");

    outportb(bus + 1, 1);
    outportb(bus + 0x306, 0);

    ata_select(bus);
    ata_io_wait(bus);

    outportb(bus + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    ata_io_wait(bus);

    int status = inportb(bus + ATA_REG_COMMAND);

    ata_wait_ready(bus);

    ata_identify_t device;
    unsigned short *buf = (unsigned short *)&device;

    for (int i = 0; i < 256; ++i)
    {
        buf[i] = inportb(bus);
    }

    unsigned char *ptr = (unsigned char *)&device.model;
    for (int i = 0; i < 39; i += 2)
    {
        unsigned char tmp = ptr[i + 1];
        ptr[i + 1] = ptr[i];
        ptr[i] = tmp;
    }

  

    outportb(bus + ATA_REG_CONTROL, 0x02);
}

void ide_read_sector(unsigned short bus, unsigned char slave, unsigned int lba, unsigned char *buf)
{
    outportb(bus + ATA_REG_CONTROL, 0x02);

    ata_wait_ready(bus);

    outportb(bus + ATA_REG_HDDEVSEL, 0xe0 | slave << 4 |
                                         (lba & 0x0f000000) >> 24);
    outportb(bus + ATA_REG_FEATURES, 0x00);
    outportb(bus + ATA_REG_SECCOUNT0, 1);
    outportb(bus + ATA_REG_LBA0, (lba & 0x000000ff) >> 0);
    outportb(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >> 8);
    outportb(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
    outportb(bus + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    if (ata_wait(bus, 1))
    {
        puts("ide read error");
    }

    int size = 256;
    outportsm(bus, buf, size);
    ata_wait(bus, 0);
}

void ide_write_sector(unsigned short bus, unsigned char slave, unsigned int lba, unsigned char *buf)
{
    outportb(bus + ATA_REG_CONTROL, 0x02);

    ata_wait_ready(bus);

    outportb(bus + ATA_REG_HDDEVSEL, 0xe0 | slave << 4 |
                                         (lba & 0x0f000000) >> 24);
    ata_wait(bus, 0);
    outportb(bus + ATA_REG_FEATURES, 0x00);
    outportb(bus + ATA_REG_SECCOUNT0, 0x01);
    outportb(bus + ATA_REG_LBA0, (lba & 0x000000ff) >> 0);
    outportb(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >> 8);
    outportb(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
    outportb(bus + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    ata_wait(bus, 0);
    int size = 256;
    inportsm(bus, buf, size);
    outportb(bus + 0x07, ATA_CMD_CACHE_FLUSH);
    ata_wait(bus, 0);
}

int ide_cmp(unsigned int *ptr1, unsigned int *ptr2, unsigned long size)
{
    unsigned int i = 0;
    while (i < size)
    {
        if (*ptr1 != *ptr2)
            return 1;
        ptr1++;
        ptr2++;
        i += 4;
    }
    return 0;
}

void ide_write_sector_retry(unsigned short bus, unsigned char slave, unsigned int lba, unsigned char *buf)
{
    unsigned char *read_buf = kmalloc(512);
    asm volatile("cli");
    do
    {
        ide_write_sector(bus, slave, lba, buf);
        ide_read_sector(bus, slave, lba, read_buf);
    } while (ide_cmp((unsigned int *)buf, (unsigned int *)read_buf, 512));
    asm volatile("sti");
}