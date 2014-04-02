#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#ifndef __F4X_H__
#define __F4X_H__

//only one framgment in each segment
#pragma pack(1)
typedef struct {
	 uint32_t size;          //0x31 == 49
	 char type[4];          //afra
	 uint32_t version:8;    //0
	 uint32_t flag:24;      //0
	 uint8_t reserved:5;    //0
	 uint8_t globalEntry:1; //1
	 uint8_t longOffset:1;  //1
	 uint8_t longID:1;     //1
	 uint32_t timeScale;     //03E8
	 uint32_t entryCount;    //0
	 uint32_t globalEntryCount; //?   ----1
	 uint64_t time;           //?
	 uint32_t segment;        //?
	 uint32_t fragment;       //?
	 uint64_t afraOffset;     //? ---- 0
	 uint64_t offsetFromAfra;  //0
   } afra_s;

int generate_f4x_file(char *segmentName, uint32_t time, uint32_t segment);

#endif
