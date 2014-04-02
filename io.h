#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef __IO_H__
#define __IO_H__

int io_open(char *file);
int io_read(int fd, uint8_t *buffer, uint32_t len);
int io_close(int fd);

#endif
