#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//movie ID
#define MOVIEID "<id> %s </id>\n"

//bootstrap file's path
#define BOOTSTRAP "<bootstrapInfo\n  profile=\"named\"\n  id=\"bootstrap78379\"\n  url=\"%s\"\n >\n  </bootstrapInfo>\n"

//streamid, bitrate, segment file's path
#define MEDIA "<media\n bootstrapInfoId=\"bootstrap78379\"\n streamId=\"%s\"\n bitrate=\"%d\"\n url=\"%s\"\n>\n </media>\n" 

const char *author = "<!--Author:xiechangcai  Email:xiechc@gmail.com -->\n";

const char *manifest[] = {
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",
	"<manifest xmlns=\"http://ns.adobe.com/f4m/1.0\">\n",
	"<streamType> live </streamType>\n",
        "<duration> 0 </duration>\n",
        "</manifest>\n"
	};
	
///videos/cctv1, /videos/cctv1.bootstrap, cctv1_m, 1023, /videos/cctv1
//generate cctv1.f4m cctv1.bootstrap 
int generate_f4m_file(char *movieID, char *bootstrap, char *streamID, int bitrate, char *segment)
{
    if (!movieID || !bootstrap || !streamID || !segment)
        return -1;

    char file[255];
    char buffer[255];
    int fd;
    int n;
    char *pID;
    pID = strrchr(movieID, '/');
    if (!pID)
       return -1;
    pID++;
    snprintf(file, sizeof(file), "%s.f4m", movieID);
    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 00666);
    if (fd < 0)
        return -1;
    write(fd, manifest[0], strlen(manifest[0]));
    write(fd, manifest[1], strlen(manifest[1]));
    n = snprintf(buffer, sizeof(buffer), MOVIEID, pID);
    write(fd, buffer, n);
    write(fd, manifest[2], strlen(manifest[2]));
    write(fd, manifest[3], strlen(manifest[3]));
    bzero(buffer, sizeof(buffer));
    n = snprintf(buffer, sizeof(buffer), BOOTSTRAP, bootstrap);
    write(fd, buffer, n);
    bzero(buffer, sizeof(buffer));
    n = snprintf(buffer, sizeof(buffer), MEDIA, streamID, bitrate, segment);
    write(fd, buffer, n);
    write(fd, manifest[4], strlen(manifest[4]));
    write(fd, author, strlen(author));
    close(fd);
    return 0;
}

#ifdef DEBUG
int main(int argc, char **argv)
{
printf("iii\n");
generate_f4m_file("/root/cctv1", "/root/cctv1.bootstrap", "cctv1_m", 1023,"/root/cctv1");
}
#endif
