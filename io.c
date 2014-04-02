#include "io.h"
#include <errno.h>

int io_open(char *file)
{
	if (!file) {
	    return -1;
	}
	int fd = open(file, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
	    return -1;
	}
	return fd;
}

int io_read(int fd, uint8_t *buffer, uint32_t len)
{
    if (!buffer) {
        return -1;
    }
    ssize_t num;
    int total = 0;
    do {
        num = read(fd, buffer, len);
        if (num < 0){
            if ((errno = EINTR) || (errno == EAGAIN))
                continue;
            else 
            	  return total; 
        } else if (num == 0)
            return total;
        len -= num;
        buffer += num;
        total += num;
    }while(len);
    
    return total;
}

int io_close(int fd)
{
    close(fd);
}

#ifdef DEBUG
int main(int argc, char **argv)
{
    int fd = io_open("abc");
    uint8_t buffer[1024];
    int c = io_read(fd, buffer, 1024);
  // printf("%s \n", buffer);
    printf("%d\n", c);
    io_close(fd);
    
}
#endif
