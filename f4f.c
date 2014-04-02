#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <errno.h>
#include "bootstrap.h"
#include "flv.h"
#include "f4x.h"
#include "f4f.h"

#define FOFFSET 0x2000
///videos/cctv1
//open /video/cctv1Segxxx.f4f
int f4f_open(char *segment, uint32_t number)
{
    char file[255];
    int fd;

    snprintf(file, sizeof(file), "%sSeg%u.f4f", segment, number);
    fd = open(file, O_WRONLY | O_CREAT | O_NONBLOCK, 00666);
    return fd;
  
}

int f4f_write_tag(int fd, tag_s *tag, bool first_tag)
{
    ssize_t cc;
    uint32_t total = 0;
    uint8_t *data = tag->data;
    uint32_t len = tag->data_size;
    ssize_t val;
    uint8_t temp_data[4];
    if (!first_tag) {
        temp_data[0] = tag->pre_tag_size >> 24;
        temp_data[1] = tag->pre_tag_size >> 16;
        temp_data[2] = tag->pre_tag_size >> 8;
        temp_data[3] = tag->pre_tag_size ;
        val = write(fd, &temp_data, sizeof(temp_data));
        if (val != 4) 
            return -1;
    }	  
    val = write(fd, &tag->tagType, sizeof(uint8_t));
    if (val != 1) {
        return -1;
    }
    
    temp_data[0] = tag->data_size >> 16;
    temp_data[1] = tag->data_size >> 8;
    temp_data[2] = tag->data_size;
    val = write(fd, &temp_data, sizeof(temp_data) - 1);
    if (val != 3) {
        return -1;
    }
  
    temp_data[0] = tag->time_stamp >> 16;
    temp_data[1] = tag->time_stamp >> 8;
    temp_data[2] = tag->time_stamp ;
    temp_data[3] = tag->time_stamp >> 24 ;
    val = write(fd, &temp_data, sizeof(temp_data));
    if (val != 4) {
        return -1;
    }
    //streamid = 000000
    memset(temp_data, 0, sizeof(temp_data));
    val = write(fd, &temp_data, sizeof(temp_data) - 1);
    if (val != 3) {
        return -1;
    }
    
    while (len) {
        cc = write(fd, data, len);
        if (cc < 0) 
        {
            if ((errno == EINTR) || (errno == EAGAIN))
                continue;
            if (total)
                return total;
                return cc;	/* write() returns -1 on failure. */
        }
        total += cc;
        data = ((uint8_t *)data) + cc;
        len -= cc;
    }

    return total;    
}


// write afra abst asrt afrt, moof, mfhd no trac ...  
int f4f_write_header(int fd, uint32_t time_stamp, uint32_t current_segment,\
                     uint32_t duration)
{
    afra_s afra_box;
    abst_s abst_box;
    afrt_s afrt_box;
    asrt_s asrt_box;
    ssize_t cc;
    //afra box
    afra_box.size = htonl(0x00000021);
    strncpy(afra_box.type, "afra", 4);
    afra_box.version = 0;
    afra_box.flag = 0;
    afra_box.longID = 1;
    afra_box.longOffset = 0;
    afra_box.globalEntry = 0;
    afra_box.reserved = 0;
    afra_box.timeScale = htonl(0x03e8);
    afra_box.entryCount = htonl(1);
    afra_box.time = (uint64_t)htonl(time_stamp) << 32;
   // afra_box.segment = htonl((uint32_t)FOFFSET);
    afra_box.segment = htonl(0xdb);
    cc = write(fd, &afra_box, 21);
    if (cc != 21)
        return -1;
    cc = write(fd, &afra_box.time, 8);
    if (cc != 8)
       return -1;
    cc = write(fd, &afra_box.segment, 4);
    if (cc != 4)
       return -1;

    abst_box.size = htonl(0x0000006a); //42 + 26 + 38 = 106
    strncpy(abst_box.type, "abst", 4);
    abst_box.version = 0;
    abst_box.flag = 0;
    abst_box.bootstrapversion = 0;
    abst_box.reserved = 0;
    abst_box.update = 0;
    abst_box.live = 1;
    abst_box.profile = 0;
    abst_box.timeScale = htonl(0x000003e8);
    abst_box.smpteTimeCodeOffset = 0;
    abst_box.MovieIdentifier = 0; //0
    abst_box.ServerEntryCount = 0; //0
    abst_box.QualityEntryCount = 0; //0
    abst_box.drm = 0;               //0
    abst_box.metadata = 0; //0 
    abst_box.currentMediaTime = (uint64_t)htonl(time_stamp + 4000) << 32;
    //abst_box.currentMediaTime = 0 ;
    cc = write(fd, &abst_box, sizeof(abst_box));
    if (cc != sizeof(abst_box))
        return -1;
   
    //asrt box
    asrt_box.segmentRunTableCount = 1;
    asrt_box.size = htonl(0x0000001a);
    strncpy(asrt_box.type, "asrt", 4);
    asrt_box.version = 0;
    asrt_box.flag = 0;
    asrt_box.qualityEntryCount = 0;
    asrt_box.segmentRunEntryCount = htonl(0x00000001);
    asrt_box.sentry[0].FirstSegment = htonl(current_segment); 
    asrt_box.sentry[0].FragmentsPerSegment = htonl(1);
    cc = write(fd, &asrt_box, sizeof(asrt_box) - sizeof(segmentEntry_s));
    if (cc != (sizeof(asrt_box) - sizeof(segmentEntry_s)))
        return -1;

    //afrt box
    afrt_box.fragmentRunTableCount = 1; 
    afrt_box.size = htonl(0x00000026);
    strncpy(afrt_box.type, "afrt", 4);
    afrt_box.version = 0;
    afrt_box.flag = 0;
    afrt_box.timeScale = htonl(0x000003e8);
    afrt_box.QualityEntryCount = 0;
    afrt_box.FragmentRunEntryCount = htonl(0x00000001);
    afrt_box.fentry[0].firstFragment = htonl(current_segment) ;
    afrt_box.fentry[0].firstFragmentTimestamp = (uint64_t)htonl(time_stamp) << 32;
    afrt_box.fentry[0].fragmentDuration = htonl(duration);
        
    cc = write(fd, &afrt_box, 38);
    if (cc != 38)
        return -1;

    //write moov tvhd
    moof_s moof_box;
    moof_box.size = htonl(72) ; //moov mfhd traf tfhd traf thhd
    strncpy(moof_box.type, "moof", 4);
    cc = write(fd, &moof_box, 8);
    if (cc != 8)
        return -1;

    mfhd_s mfhd_box;
    mfhd_box.size = htonl(16);
    strncpy(mfhd_box.type, "mfhd", 4);   
    mfhd_box.flags =  0;
    mfhd_box.version =  0;
    mfhd_box.sequence_number = htonl(1);
    cc = write(fd, &mfhd_box, 16);
    if (cc != 16)
        return -1;

    traf_s traf_box;
    traf_box.size = htonl(24); //traf tfhd tfhd
    strncpy(traf_box.type, "traf", 4);   
    cc = write(fd, &traf_box, 8);
    if (cc != 8)
        return -1;

    tfhd_s tfhd_box;
    tfhd_box.size = htonl(16);
    strncpy(tfhd_box.type, "tfhd", 4);   
    tfhd_box.version = 0;
    tfhd_box.flags = htonl(0x010000);
    tfhd_box.trackid = htonl(1);
    cc = write(fd, &tfhd_box, 16);
    if (cc != 16)
        return -1;

    cc = write(fd, &traf_box, 8);
    if (cc != 8)
        return -1;
    tfhd_s tfhd_box1;
    tfhd_box1.size = htonl(16);
    strncpy(tfhd_box1.type, "tfhd", 4);   
    tfhd_box1.version = 0;
    tfhd_box1.flags = htonl(0x010000);
    tfhd_box1.trackid = htonl(2);
    cc = write(fd, &tfhd_box1, 16);
    if (cc != 16)
        return -1;

    //write mdat box
    //lseek(fd, FOFFSET, SEEK_SET);
    uint32_t box_total_size = 0;
    char box_type[4];
    strncpy(box_type, "mdat", 4);
    cc = write(fd, &box_total_size, 4);
    if (cc != 4)
        return -1;
    cc = write(fd, &box_type, 4);
    if (cc != 4)
        return -1;
}


int f4f_close(int fd)
{
    if (fd > 0) {
        close(fd);
        return 0;
    }
    return -1;
}
int f4f_write_tag_pre_size(int fd, tag_s *tag)
{
    if (fd < 0)
        return -1;

    ssize_t val;
    uint8_t temp_data[4];
    temp_data[0] = tag->pre_tag_size >> 24;
    temp_data[1] = tag->pre_tag_size >> 16;
    temp_data[2] = tag->pre_tag_size >> 8;
    temp_data[3] = tag->pre_tag_size ;
    val = write(fd, &temp_data, sizeof(temp_data));
    if (val != 4) 
        return -1;
}


#ifdef DEBUG
int main()
{
   int fd = f4f_open("/root/cctv1", 99);
   f4f_write_header(fd, 4000, 99, 4000);
   tag_s tag;
   tag.tagType = 8;
   tag.data_size = 1;
   tag.time_stamp = 1000;
   f4f_write_tag(fd, &tag);
   f4f_close(fd);
}
#endif
