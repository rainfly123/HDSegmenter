#include "bootstrap.h" 
   

void save_timestamp(struct hash_node * timeStampBuffer, uint32_t fragmentNum, uint64_t timeStamp)
{
    hash_insert(&timeStampBuffer, fragmentNum, timeStamp);
}


// root/cctv1.bootstrap
int write_bootstrap_file(struct hash_node * timeStampBuffer, char *bootstrap, uint32_t first_segment, uint32_t last_segment)
{
    int fd;
    int i;
    int j = 0;
    uint32_t temp, tempn;
    char tmp_file[255];
    
    snprintf(tmp_file, sizeof(tmp_file), "%s.tmp" , bootstrap);
    fd = open(tmp_file, O_WRONLY | O_CREAT | O_TRUNC, 00666);
    if (fd < 0) {
        return -1;
    }
    abst_s bootable;
    afrt_s frtable;
    asrt_s srtable;
    
    bootable.size = htonl(0x00000101);
    strncpy(bootable.type, "abst", 4);
    bootable.version = 0;
    bootable.flag = 0;
    bootable.bootstrapversion = htonl(last_segment);
    bootable.reserved = 0;
    bootable.update = 0;
    bootable.live = 1;
    bootable.profile = 0;
    bootable.timeScale = htonl(0x000003e8);
    bootable.smpteTimeCodeOffset = 0;
    bootable.MovieIdentifier = 0; //0
    bootable.ServerEntryCount = 0; //0
    bootable.QualityEntryCount = 0; //0
    bootable.drm = 0;               //0
    bootable.metadata = 0; //0 
    if (last_segment > 3)
    temp = (uint32_t)hash_search(&timeStampBuffer, (last_segment - 3));
    else
    temp = (uint32_t)hash_search(&timeStampBuffer, (first_segment));
    bootable.currentMediaTime = (uint64_t)htonl(temp) << 32;
    write(fd, &bootable, sizeof(bootable));

    srtable.segmentRunTableCount = 1;
    srtable.size = htonl(0x00000022);
    strncpy(srtable.type, "asrt", 4);
    srtable.version = 0;
    srtable.flag = 0;
    srtable.qualityEntryCount = 0;
    srtable.segmentRunEntryCount = htonl(0x00000002);
    srtable.sentry[0].FirstSegment = htonl(first_segment); 
    srtable.sentry[0].FragmentsPerSegment = htonl(1);
    srtable.sentry[1].FirstSegment = htonl(last_segment - 1);
    srtable.sentry[1].FragmentsPerSegment = htonl(1);
    write(fd, &srtable, sizeof(srtable));
	  
    frtable.fragmentRunTableCount = 1; 
    strncpy(frtable.type, "afrt", 4);
    frtable.version = 0;
    frtable.flag = 0;
    frtable.timeScale = htonl(0x000003e8);
    frtable.QualityEntryCount = 0;
    for (i = first_segment; (i < last_segment); i++)
    {
        frtable.fentry[j].firstFragment = htonl(i);
        temp = (uint32_t)hash_search(&timeStampBuffer, i);
        frtable.fentry[j].firstFragmentTimestamp = (uint64_t)htonl(temp) << 32;
        tempn = (uint32_t)hash_search(&timeStampBuffer, (i + 1));
        frtable.fentry[j].fragmentDuration = htonl(tempn - temp);
        j++;
    }
    
    frtable.FragmentRunEntryCount = htonl(j);
    frtable.size = htonl(22 + 16 * j);
    write(fd, &frtable, (22 + 16 * j));
    close(fd);

    return rename(tmp_file, bootstrap);
}

#ifdef DEBUG
int main(int argc, char **argv)
{
    save_timestamp(1, 0);
    save_timestamp(2, 4000);
    save_timestamp(3, 8000);
    save_timestamp(4, 12000);
    save_timestamp(5, 16000);
    save_timestamp(6, 20000);
    save_timestamp(7, 24000);
    save_timestamp(8, 28000);
    save_timestamp(9, 32000);
    save_timestamp(10, 36000);
    save_timestamp(11, 40000);
    write_bootstrap_file("/root/cctv1.bootstrap", 1, 11);
}
#endif
