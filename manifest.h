#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef __MANIFEST_H__
#define __MANIFEST_H__
int generate_f4m_file(char *movieID, char *bootstrap, char *streamID, int bitrate, char *segment);
#endif
