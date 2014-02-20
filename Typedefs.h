/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************

/*
 * Typedefs.h
 *
 *      Author: Stephan Hesse
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdint.h>
#include <math.h>
#include <vector>
#include <deque>
#include <stack>

#include <asl/framework/Block.h>

namespace asl {

		typedef size_t Bytes;
		typedef float Seconds;
		typedef float Kbps;
		typedef int64_t MediaTimestamp ; //timestamp in timescale units
		typedef uint32_t SampleDuration; //sample duration in timescale units
		typedef int64_t MediaTimestampUs ; //timestamp in microseconds
		typedef uint32_t MediaTimescale ; //timescale
		typedef uint32_t MediaBitrate ; //bitratein bits per second
		typedef uint32_t FourCC ; //fourCC
		typedef uint16_t PixelValue;

		typedef std::deque<clc::RefPtr<Block> > Blocks;
		typedef std::map<uint32_t, size_t> TrackIdToBlockIndexMap;

		typedef struct Resolution {
			Resolution(PixelValue w=0, PixelValue h=0): width(w), height(h) {}
			PixelValue width;
			PixelValue height;
		} Resolution_t;

		typedef struct Error {
			Error(int c, std::string e): code(c), message(e) {}
			int code;
			std::string message;
		} Error_t;

		typedef std::stack<Error> ErrorStack;

		typedef enum OutputType_t {AUDIO=0, VIDEO=1, SUBTITLES=2, NONE=3, LAST=SUBTITLES} OutputType;

		typedef enum SessionType_t { SMOOTHSTREAMING,
							HDS,
							RTMP,
							HLS,
							DASH,
							CFF,
							UNKNOWN } SessionType;

		class ResourceCoordinates {
		public:

			ResourceCoordinates()
			: streamIndex(0)
			, timeIndex(0)
			, adaptationIndex(0)
			, isValid(false)
			{}

			uint8_t streamIndex;
			uint8_t adaptationIndex;
			uint32_t timeIndex;
			bool isValid;

		};
}


#endif /* TYPEDEFS_H_ */
