#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __FLV_H__
#define __FLV_H__

typedef struct {
    uint32_t pre_tag_size;
    uint8_t tagType;
    uint32_t data_size ;//////*human readable, 
    uint32_t time_stamp ;  ///human readable
    uint32_t streamID ;
    uint8_t *data;
} tag_s;

#define IS_VIDEO_TAG(Tag) ((Tag.tagType)  == 9 )
#define IS_KEY_FRAME(Tag) ((Tag.data[0] & 0xf0) == 0x10)
#endif

