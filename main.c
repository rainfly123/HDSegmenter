#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "io.h"
#include  "bootstrap.h"
#include  "f4f.h"
#include "f4x.h"
#include "manifest.h"

#define DURATION (6000)
#define NUM_SEGMENT  10

//data's len must equal 9
int flv_read_header(uint8_t *data)
{
    if(*data++ != 'F')
        return -1;
    if(*data++ != 'L')
        return -1;
    if(*data++ != 'V')
        return -1;
    return 0;
        
}

//data's len must equal 11 + 4 = 15
int flv_tag_parse(tag_s *tag, uint8_t *data)
{
   uint8_t * pos = data;
   uint32_t time;

   if (!tag || !data) {
       return -1; 
   }
   
   tag->pre_tag_size = *pos << 24 | *(pos + 1) << 16 | *(pos + 2) << 8 |*(pos + 3);
   pos += 4;
   tag->tagType = *pos++ &0x1f;
   tag->data_size = *pos << 16 | *(pos + 1) << 8 | *(pos + 2);
   pos += 3;
   time = *pos << 16 | *(pos + 1) << 8 | *(pos + 2);
   pos += 3;
   tag->time_stamp = *pos++ << 24 | time;
   tag->streamID = *pos << 16 | *(pos + 1) << 8 | *(pos + 2);
   tag->data = malloc(tag->data_size);
   return 0;
 }

int write_index_file(struct hash_node * timeStampBuffer, uint32_t first_segment, \
                    uint32_t last_segment, uint64_t time)
{
    save_timestamp(timeStampBuffer, last_segment, time);
    write_bootstrap_file(timeStampBuffer, "/videos/cctv2.bootstrap", first_segment, last_segment);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s file [file is a flv file]\n", argv[0]);
        return -1;
    }
    int fd = io_open(argv[1]);
    if (fd < 0) {
        perror("Open file error:");
        return  -1;
    }
    uint8_t data[15];
    int val;
    uint32_t first_segment = 1;
    uint32_t last_segment = 1;
    uint32_t prev_segment_time = 0;
    uint32_t segment_time = 0;
    int remove_file = 0;
    char remove_filename[255];
    struct hash_node * timeStampBuffer;
    hash_initial(&timeStampBuffer);
	  
    val = io_read(fd, data, 9); //flv header
    if (val != 9)
        return -1;
	  
    val = flv_read_header(data);
    if (val < 0) {
        printf("Not FLV file\n");
        return -1;
    }
    tag_s tag_1, tag_2;
    int f4f_fd;
    generate_f4m_file("/videos/cctv2", "cctv2.bootstrap", "cctv2_m", 1023, "cctv2");
    int the_first = 1;
    //find key frame	  
    while (1) 
    {
        val = io_read(fd, data, 15); //flv tag
        if (val != 15)
            return -1;
        memset(&tag_1, 0, sizeof(tag_s));
        flv_tag_parse(&tag_1, data);  
        io_read(fd, tag_1.data, tag_1.data_size);
        if (!IS_VIDEO_TAG(tag_1)) {
            free(tag_1.data);
            continue;
        }
        if (IS_KEY_FRAME(tag_1)) {
            segment_time = tag_1.time_stamp;
            break;
        } else {
            free(tag_1.data);
            continue;
        }
    }

    save_timestamp(timeStampBuffer, last_segment, segment_time);
    f4f_fd = f4f_open("/videos/cctv2", last_segment);
    f4f_write_header(f4f_fd, segment_time, last_segment, DURATION);
    generate_f4x_file("/videos/cctv2", segment_time, last_segment);
    prev_segment_time = segment_time;
    if (f4f_write_tag(f4f_fd, &tag_1, true) < 0) {
            fprintf(stderr, "Warning: Could not write frame of stream\n");
    }
//    free(tag_1.data);

    val = io_read(fd, data, 15); //flv audio tag
    if (val != 15)
        return -1;
    memset(&tag_2, 0, sizeof(tag_s));
    flv_tag_parse(&tag_2, data);  
    io_read(fd, tag_2.data, tag_2.data_size);
    if (f4f_write_tag(f4f_fd, &tag_2, false) < 0) {
            fprintf(stderr, "Warning: Could not write frame of stream\n");
    }

    tag_s tag;
    while (1) 
    {
        val = io_read(fd, data, 15); //flv tag
        if (val != 15)
            return -1;
        memset(&tag, 0, sizeof(tag_s));
        flv_tag_parse(&tag, data);  
        io_read(fd, tag.data, tag.data_size);
        if (IS_VIDEO_TAG(tag)) 
            if (IS_KEY_FRAME(tag)) 
                segment_time = tag.time_stamp;
        if (segment_time - prev_segment_time >= DURATION) {
            f4f_write_tag_pre_size(f4f_fd, &tag);
            f4f_close(f4f_fd);
            if ((int)(last_segment - first_segment) >= NUM_SEGMENT) {
                remove_file = 1;
                first_segment++;
            } else {
                remove_file = 0;
            }

            write_index_file(timeStampBuffer, first_segment, ++last_segment, segment_time);
            if (remove_file) {
                snprintf(remove_filename, sizeof(remove_filename), "%sSeg%u.f4f", "/videos/cctv2", first_segment - 1);
                remove(remove_filename);
                snprintf(remove_filename, sizeof(remove_filename), "%sSeg%u.f4x", "/videos/cctv2", first_segment - 1);
                remove(remove_filename);
            }

    
            if ((f4f_fd = f4f_open("/videos/cctv2", last_segment)) < 0) {
                fprintf(stderr, "Could not open segment \n");
                break;
            }
            generate_f4x_file("/videos/cctv2", segment_time, last_segment);

            // Write a new header at the start of each file
            if (f4f_write_header(f4f_fd, segment_time, last_segment, DURATION) < 0) {
                fprintf(stderr, "Could not write f4f header\n");
                exit(1);
            }
            tag_1.time_stamp = segment_time;
            if (f4f_write_tag(f4f_fd, &tag_1, true) < 0) {
                fprintf(stderr, "Warning: Could not write frame of stream\n");
            }
            tag_2.pre_tag_size = 0x0000003a;
            tag_2.time_stamp = segment_time;
            if (f4f_write_tag(f4f_fd, &tag_2, false) < 0) {
                fprintf(stderr, "Warning: Could not write frame of stream\n");
            }
            prev_segment_time = segment_time;
            tag.pre_tag_size = 0x00000014;
            //sleep(5);
        }
        if (f4f_write_tag(f4f_fd, &tag, false) < 0) {
            fprintf(stderr, "Warning: Could not write frame of stream\n");
        }
        free(tag.data);
    }
}
