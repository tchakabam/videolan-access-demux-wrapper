/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************

/*
 * Sample.h
 *
 *      Author: Stephan Hesse
 */

#ifndef SAMPLE_H_
#define SAMPLE_H_

#include "Poco/RefCountedObject.h"
#include "Poco/AutoPtr.h"

#include <asl/framework/Formats.h>
#include <asl/framework/Block.h>
#include <asl/framework/Typedefs.h>

using Poco::AutoPtr;
using Poco::RefCountedObject;

namespace asl {

		struct PayloadInfo : public RefCountedObject {

			enum PayloadType { SAMPLE=0, SIDECHANNEL=1, EMPTY=2, END=3 };

			PayloadInfo() :payload(EMPTY), offset(0), data(new Block()) {}
			PayloadInfo(PayloadType type) :payload(type), offset(0), data(new Block()) {}

			PayloadType payload;
			AutoPtr<Block> data;
			uint64_t offset;
		};

		struct DrmInfo {

			int64_t contentUuid;
			int64_t encryptionKeyId;
			AutoPtr<Block> drmInitData;
 
		};

		struct SidechannelInfo : PayloadInfo {

			SidechannelInfo() :PayloadInfo(SIDECHANNEL), timescale(1) {}

			DrmInfo drmData;

			MediaTimescale timescale;
			AudioFormat audio;
			VideoFormat video;

		};

		struct DecryptionInfo {

			AutoPtr<Block> sampleIV;
			AutoPtr<Block> sampleAuxData;

		} ;

		struct SampleInfo : PayloadInfo {

			SampleInfo() :PayloadInfo(SAMPLE), dts(0), pts(0), duration(0), size(0), isFirst(false), isLast(false) {}

			DecryptionInfo cypherData;
			MediaTimestamp dts;            ///< DTS in secs/timescale
			MediaTimestamp pts;            ///< PTS in secs/timescale
			SampleDuration duration;       ///< sample duration in secs/timescale
			Bytes size;

			bool     isFirst;        ///< true, if first sample in fragment
			bool     isLast;         ///< true, if last sample in fragment

		} ;

		static inline SampleInfo* CastToSampleInfo(PayloadInfo* si) {
			if(si->payload == PayloadInfo::SAMPLE) {
				return (SampleInfo*) si;
			} else {
				return NULL;
			}
		}

		static inline SidechannelInfo* CastToSidechannelInfo(PayloadInfo* si) {
			if(si->payload == PayloadInfo::SIDECHANNEL) {
				return (SidechannelInfo*) si;
			} else {
				return NULL;
			}
		}

}

#endif /* SAMPLE_H_ */
