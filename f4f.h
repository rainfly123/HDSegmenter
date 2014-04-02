#include <stdint.h>
#include "flv.h"

#ifndef __F4F_H__
#define __F4F_H__

typedef unsigned int bool;
#define true 1
#define false 0

typedef struct 
{
    uint32_t size;
    char type[4];
}moof_s;

typedef struct 
{
    uint32_t size;
    char type[4];
    uint32_t flags:24;
    uint32_t version:8;
    uint32_t sequence_number; //always equal fragment number
}mfhd_s;

typedef struct 
{
    uint32_t size;
    char type[4];
}traf_s;

typedef struct 
{
    uint32_t size;
    char type[4];
    uint32_t flags:24;   //3 has base_data_offset and sample_desciption_index
                        //0x010000 no include
    uint32_t version:8; //0
    uint32_t trackid;  //1
    uint64_t base_data_offset; 
    uint32_t sample_desciption_index; //
}tfhd_s;

int f4f_open(char *segment, uint32_t number);
int f4f_write_header(int fd, uint32_t time_stamp, uint32_t current_segment,\
                     uint32_t duration);
int f4f_write_tag(int fd, tag_s *tag, bool first_tag);
int f4f_close(int fd);
int f4f_write_tag_pre_size(int fd, tag_s *tag);

#endif
