/*
 *
 * Copyright (c) 2011, 2012 castLabs GmbH
 *
 * http://www.castlabs.com
 *
 */

/*
 * Resource.h
 *
 *  Created on: Sep 9, 2012
 *      Author: Stephan Hesse
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <limits>
#include <vector>
#include <deque>

#include <stdlib.h>

#include <clc/clcPtr.h>
#include <clc/clcError.h>
#include <clc/clcUri.h>

#include <asl/framework/Typedefs.h>
#include <asl/framework/Formats.h>
#include <asl/framework/Block.h>
#include <asl/framework/HttpAgent.h>

using namespace std;

namespace asl {

typedef struct TransactionInfo {

	TransactionInfo(clc::RefPtr<Request> req) : request(req) {}

	clc::RefPtr<Request> request;

} TransactionInfo_t;

typedef std::deque<TransactionInfo> TransactionPool;

class MetaStream;
class Resource : public clc::RefCountedBase<Resource> {

public:

	enum DependencyFlag {
		NONE, PARENT, CHILD, LAST
	};

	enum Kind {
		MP4, //embedded MP4
		MP4A,
		MP4V,
		MP4S,
		MPEGTS,
		MPEGTS_AES128,
		ISMV_AUDIO,
		ISMV_VIDEO,
		FLV,
		RTMP,
		F4V,
		UNKNOWN
	};

	Resource(const clc::URI& uri,
			Kind e,
			MediaTimestamp iDuration,
			MediaTimescale iTimescale,
			Bytes size =-1 ,
			bool isInitialization = false,
			MediaTimestamp iStart = -1,
			MetaStream* pParent = NULL);

	Resource(const clc::URI& uri,
			Kind e = UNKNOWN,
			MediaTimestamp iStart = -1,
			MetaStream* pParent = NULL);

	~Resource();

	const clc::URI & getLocation() const;

	void setRange(Range r) {m_Range = r;}
	const Range & getRange() const {return m_Range;}

	Seconds getDurationInSecs() const;

	MediaTimestamp getScaledDuration() const;
	MediaTimescale getTimescale() const;

	MediaBitrate getAverageBitrate() const;

	clc::RefPtr<Block> getData() const;
	void setData (clc::RefPtr<Block> block) {m_data = block;}

	bool isLoaded() const;
	void setLoadingStats(Seconds dTxTime, Kbps dTxSpeed = -1);

	Kbps getLoadingSpeed() const;
	Seconds getLoadingTime() const;

	bool isInitializationData() const;
	void setInitializationData(bool b);

	Bytes getSize() const;
	void setSize(Bytes b);

	Kind getFormat() const;
	void setFormat(Kind e);

	MediaTimestamp getStartTime() const;

	const MetaStream* getParent() const;
	void setParent(MetaStream* p) {m_parent = p;}

	DependencyFlag getDepsLevel() {return m_depLevel;}
	void setDepsLevel(DependencyFlag flag) {m_depLevel = flag;}

	clc::String& getKey() {return m_key;}
	void setKey(clc::String s) {m_key=s;}

private:

	Bytes m_Size;
	MediaTimestamp m_start;
	MediaTimestamp m_duration;
	MediaTimescale m_timescale;
	Seconds m_txTimeInSecs;
	Kbps m_txSpeed;
	bool m_isLoaded;
	bool m_isInitialization;
	Kind m_Type;
	clc::RefPtr<Block> m_data;
	clc::URI m_Uri;
	Range m_Range;
	MetaStream* m_parent;
	DependencyFlag m_depLevel;
	clc::String m_key;

};

class MetaStream: public std::deque<clc::RefPtr<Resource> >, public clc::RefCountedBase<MetaStream> {

public:

	MetaStream()
	:bandwidth(0)
	,videoWidth(0)
	,videoHeight(0)
	,videoFramerate(0)
	,audioSamplingRate(0)
	,audioChannels(0)
	,audioBitsPerSample(0)
	,audioPacketSize(0)
	,nalUnitLengthFieldSize(0)
	,qualityRanking(0)
	{}

	MediaBitrate bandwidth;
	PixelValue videoWidth;
	PixelValue videoHeight;
	uint16_t audioPacketSize;
	uint16_t audioSamplingRate;
	uint8_t audioChannels;
	uint8_t audioBitsPerSample;
	uint8_t nalUnitLengthFieldSize;
	uint8_t qualityRanking;
	float videoFramerate;

	string audioTag; //for SmoothStreaming
	string id; //for MPEG-DASH representation IDs mainly, and maybe usefull in other cases too
	string dependencyId;
	string fourCC;
	string codecData; //
	string waveFormatEx; //SmoothStreaming

	Seconds getTotalDurationInSecs() const;

	void logInspection();
};

class AdaptationVector: public std::vector<clc::RefPtr<MetaStream> >, public clc::RefCountedBase<AdaptationVector>   {
public:
	AdaptationVector()
	:audio(false)
	,video(false)
	,subtitles(false)
	,isSegmentAligned(false)
	,isEncrypted(false)
	{}

	//"feature" flags
	bool audio;
	bool video;
	bool subtitles;

	/* necessary for some quite weird but possible DASH  */
	bool isSegmentAligned;
	bool isEncrypted;

	uint32_t defaultSampleSize;
	SampleDuration defaultSampleDuration;
	MediaTimescale defaultTimescale;

	bool removeByIndex(size_t i);

	Seconds getMaxDurationInSecs() const;

	void logInspection();
};

class MetaSuperStream: public std::vector<clc::RefPtr<AdaptationVector> >, public clc::RefCountedBase<MetaSuperStream>  {
public:
	MetaSuperStream() {}

	clc::Result getResource(clc::RefPtr<Resource>& res, ResourceCoordinates coord) const;
	clc::Result getMetaStream(clc::RefPtr<MetaStream>& stream, ResourceCoordinates coord) const;
	clc::Result getAdaptationVector(clc::RefPtr<AdaptationVector>& av, ResourceCoordinates coord) const;

	/* video variant selection *//* audio variant selection *//* subtitles */
	clc::Result getMetaStreamCount(ResourceCoordinates& coord, size_t& count);
	clc::Result getBitrateOfMetaStream(ResourceCoordinates coord, MediaBitrate& bitrate);

	Seconds getEstimatedDurationInSecs(bool max=false) const;

	void logInspection();
};

}


#endif /* RESOURCE_H_ */
