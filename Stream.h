/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************

/*
 * Stream.h
 *
 */

#ifndef STREAM_H_
#define STREAM_H_

#include <string>

#include "Poco/AutoPtr.h"

#include <disparat/Typedefs.h>
#include <disparat/Formats.h>
#include <disparat/Block.h>
#include <disparat/Sample.h>

using Poco::AutoPtr;

namespace disparat {

	class Stream {

	public:

		virtual ~Stream() {}

		virtual int getType(SessionType& type) = 0;

		virtual int open(std::string url) = 0;

		virtual int close() = 0;

		virtual bool isOpen() = 0;

		/* read / flush samples */ /////////////////////////////////////
		virtual int read(OutputType eStream, clc::RefPtr<PayloadInfo>& rSampleInfo) = 0;

		virtual int flush(MediaTimestampUs maxDelta) = 0;

		/* multimedia accessors */ //////////////////////
		virtual int seek(MediaTimestampUs iTimeUs) = 0;

		/* absolute media time value (us) */
		virtual int getPos(MediaTimestampUs& pos) = 0;

		/* media duration */
		virtual int getDuration(MediaTimestampUs& len) = 0;

		virtual bool hasComponent(OutputType eStream) = 0;

		virtual bool isEncrpyted() = 0;

	};
}

#endif
