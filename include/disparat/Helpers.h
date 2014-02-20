/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************

/*
 * Helpers.h
 *
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include <asl/framework/Typedefs.h>

//MACROS

#define ONE_MHZ_TIMESCALE 1000000L
#define TEN_MHZ_TIMESCALE 10000000L

// AudioTags used in SmoothStreaming manifests
#define AUDIOTAG_AAC 255
#define AUDIOTAG_EC3 127
#define AUDIOTAG_AC3 63
#define AUDIOTAG_DTS 31

/* write FourCC 32-bit unsigned from 4 x 8 bit chars  */
#define FOURCC(a,b,c,d) ( (uint32_t) ((((d)&0xff)<<24) | (((c)&0xff)<<16) | (((b)&0xff)<<8) | ((a)&0xff)) )
#define FOURCC_FROM_SEQ(s) ( (uint32_t) ((((s[3])&0xff)<<24) | (((s[2])&0xff)<<16) | (((s[1])&0xff)<<8) | ((s[0]&0xff))) )

//GLOBAL FUNCTIONS

namespace asl {

	char* HexStringToBytes(std::string data);

	float ReadFixpoint16FromInt(int i);

	/* return: number of converted NALUs */
	int ToH264AnnexB(unsigned char * pBuf, size_t iLen);

	MediaTimestamp ToMicroseconds(MediaTimestamp in, MediaTimescale scale);

	MediaTimestamp ConvertScale (MediaTimestamp t, MediaTimescale fromScale, MediaTimescale toScale);

	std::string FourCCToString (uint32_t ui_fourcc);

	bool ParseRange(const std::string& s, size_t* from, size_t* to);

}

#endif /* HELPERS_H_ */
