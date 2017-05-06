
char inportb (short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %0, %1" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (short _port, char _data)
{
    __asm__ __volatile__ ("outb %0, %1" : : "dN" (_port), "a" (_data));
} 


void outportsm(unsigned short port, unsigned char * data, unsigned long size) {
	asm volatile ("rep outsw" : "+S" (data), "+c" (size) : "d" (port));
}


void inportsm(unsigned short port, unsigned char * data, unsigned long size) {
	asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}