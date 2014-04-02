#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "hash.h"

#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#pragma pack(1)
typedef struct {
	 uint32_t size;          //   42 + 34 + 182 = 257
	 char type[4];          //abst
	 uint32_t version:8;    //0
	 uint32_t flag:24;      //0
	 uint32_t bootstrapversion; //1 ??
	 uint8_t  reserved:4;     //0
	 uint8_t  update:1;       //0
	 uint8_t  live:1;         //0
	 uint8_t  profile:2;     //0
	 uint32_t timeScale;     //03E8
	 uint64_t currentMediaTime;           //?
	 uint64_t smpteTimeCodeOffset;  //0
	 uint8_t MovieIdentifier; //0
	 uint8_t ServerEntryCount; //0
	 uint8_t QualityEntryCount; //0
	 uint8_t drm;               //0
	 uint8_t metadata; //0 
   } abst_s;
   
typedef struct {
    uint32_t FirstSegment;
    uint32_t FragmentsPerSegment;
}segmentEntry_s;

#pragma pack(1)
typedef struct {
	 uint8_t segmentRunTableCount;//1
	 uint32_t size;          //34
	 char type[4];          //asrt
	 uint32_t version:8;    //0
	 uint32_t flag:24;      //0
	 uint8_t qualityEntryCount; //0
	 uint32_t segmentRunEntryCount; // 2 the minimum and the maximum
	 //f4f file segmentRunEntryCount =1
	 segmentEntry_s sentry[2];
}asrt_s;
 
typedef struct {
    uint32_t firstFragment;
    uint64_t firstFragmentTimestamp;
    uint32_t fragmentDuration;
    //uint8_t DiscontinuityIndicator;
}fragmentEntry_s;

#pragma pack(1)
typedef struct {
	 uint8_t fragmentRunTableCount; //1
	 uint32_t size;          //182
	 char type[4];          //asrt
	 uint32_t version:8;    //0
	 uint32_t flag:24;      //0
	 uint32_t timeScale; //03e8
	 uint8_t QualityEntryCount; //0
	 uint32_t FragmentRunEntryCount; // 10  10 fragments
	 //f4f file FragmentRunEntryCount =1 
	 fragmentEntry_s fentry[10];
}afrt_s  ;

   
void save_timestamp(struct hash_node * timeStampBuffer, uint32_t fragmentNum, uint64_t timeStamp);

// root/cctv1.bootstrap  last_segment - first_segment = 10
int write_bootstrap_file(struct hash_node * timeStampBuffer, char *bootstrap, uint32_t first_segment, uint32_t last_segment);

#endif
