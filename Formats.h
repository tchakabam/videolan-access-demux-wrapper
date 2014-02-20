/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************

/*
 * Formats.h
 *
 *      Author: Stephan Hesse
 */

#ifndef FORMATS_H_
#define FORMATS_H_

#include <asl/framework/Typedefs.h>

namespace asl {

		typedef class GenericFormat {

		public:
#ifdef __ANDROID__
			char type;
#endif
			MediaBitrate bitrate;
			FourCC fourCC;
			int profile; // fourcc specific profile, -1 if undefined

			bool isEmpty;

			GenericFormat () 
				: bitrate (0)
				  , fourCC (0)
				  , profile (-1)
				  , isEmpty(true)
			{
#ifdef __ANDROID__
				type = 0;
#endif
			}

			virtual ~GenericFormat ()
			{}

			virtual bool specificIsEqual (const GenericFormat& other) const 
			{
				return true;
			}
		} GenericFormat_t;

		typedef class VideoFormat : public GenericFormat {

		public:

			Resolution screenResolution;
			uint8_t nalUnitLengthFieldSize;

			VideoFormat()
				: GenericFormat ()
				, nalUnitLengthFieldSize (4)
			{
#ifdef __ANDROID__
				type = 'v';
#endif
			}

			virtual bool specificIsEqual (const GenericFormat& other) const 
			{
#ifdef __ANDROID__
				const VideoFormat * pOther = (other.type == type) ? static_cast<const VideoFormat *> (&other) : 0;
#else
				const VideoFormat * pOther = dynamic_cast<const VideoFormat *> (&other);
#endif
				return pOther 
					&& screenResolution.width == pOther->screenResolution.width
					&& screenResolution.height == pOther->screenResolution.height
					&& nalUnitLengthFieldSize == pOther->nalUnitLengthFieldSize;
			}
		} VideoFormat_t;

		typedef class AudioFormat : public GenericFormat {

		public:

			enum Framing {
				PLAIN = 0, ///< No framing, one frame per read()
				ADTS,
				LATM
			};

			uint16_t numChannels;
			uint16_t samplingRate;
			uint16_t bitsPerSample;

			Framing framing;

			AudioFormat()
				: GenericFormat()
				  , numChannels (0)
				  , samplingRate (0)
				  , bitsPerSample(16)
				  , framing (PLAIN)
			{
#ifdef __ANDROID__
				type = 'a';
#endif
			}

			virtual bool specificIsEqual (const GenericFormat& other) const 
			{
#ifdef __ANDROID__
				const AudioFormat * pOther = (other.type == type) ? static_cast<const AudioFormat *> (&other) : 0;
#else
				const AudioFormat * pOther = dynamic_cast<const AudioFormat *> (&other);
#endif
				return pOther 
					&& numChannels  == pOther->numChannels
					&& samplingRate == pOther->samplingRate
					&& bitsPerSample == pOther->bitsPerSample
					&& framing  == pOther->framing;
			}
		} AudioFormat_t ;

}

#endif /* FORMATS_H_ */
