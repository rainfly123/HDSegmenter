#include "f4x.h"   

// /root/cctv, 3000, 2
// generate /root/cctvSeg2.f4x
int generate_f4x_file(char *segmentName, uint32_t time, uint32_t segment)
{
    if (!segmentName) {
        return -1;
    }
    char path[255];
    int fd;
    afra_s box;
    
    snprintf(path, sizeof(path), "%sSeg%d.f4x", segmentName, segment);
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 00666);
    if (fd < 0)
        return -1;
        
    box.size = htonl(0x00000039);
    strncpy(box.type, "afra", 4);
    box.version = 0;
    box.flag = 0;
    box.longID = 1;
    box.longOffset = 1;
    box.globalEntry = 1;
    box.reserved = 0;
    box.timeScale = htonl(0x03e8);
    box.entryCount = 0;
    box.globalEntryCount = htonl(1);
    box.time = (uint64_t)htonl(time) << 32;
    box.segment = htonl(segment);
    box.fragment = htonl(segment);
    box.afraOffset = 0;
    box.offsetFromAfra = 0;     
    write(fd, &box, 0x39);
    close(fd);
}

#ifdef DEBUG
int main(int argc, char **argv)
{
    generate_f4x_file("/root/cctv", 3000, 2);
}
#endif
