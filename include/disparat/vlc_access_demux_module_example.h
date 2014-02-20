/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************
 *
 * Copyright (C) 2001-2008 the VideoLAN team
 * $Id: ccd416fa773702e61b7ddd419941efddff88163c $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>

#include <vlc_demux.h>
#include <vlc_charset.h>                           /* EnsureUTF8 */
#include <vlc_url.h>
#include <vlc_block.h>
#include <vlc_es_out.h>

#include "poco/AutoPtr.h"

#include <disparat/Stream.h>
#include <disparat/Helpers.h>

using Poco::AutoPtr;

//Config

#define PREBUFFER_ENABLE 0
#define DEBUG_PTS_DELAY 3000000
#define SLEEP_ON_EMPTY_READ

using namespace disparat;

//
// Local prototypes
//

//demux
static int  Open ( vlc_object_t * );
static void Close( vlc_object_t * );
  
vlc_module_begin ()
	set_shortname( N_("CASL") )
	set_description( N_("CastLabs Adaptive Streaming Library SDK Demo plugin") )
	set_capability( "access_demux", 120 )
	set_category( CAT_INPUT )
	set_subcategory( SUBCAT_INPUT_ACCESS )
	set_callbacks( Open, Close )
	add_shortcut("http")
	add_shortcut("https")
	add_shortcut("dash")
	add_shortcut("hls")
	add_shortcut("hds")
	add_shortcut("hss")
	add_shortcut("ssm")
vlc_module_end ()

struct demux_sys_t {

	/* status flags */
	bool b_audio;
	bool b_video;
	bool b_h264_init;
	bool b_audio_init;

	vlc_url_t url;
	
	/* vlc format stuff */
	es_format_t audio_fmt;
	es_format_t video_fmt;

	/* vlc elementary streams */
	es_out_id_t *p_audio_es;
	es_out_id_t *p_video_es;

	/* timestamps and sidechannel info */
	MediaTimestampUs videoPts;
	MediaTimestampUs audioPts;
	SidechannelInfo* audio;
	SidechannelInfo* video;

	Stream* p_Client;
};


static int   Demux   ( demux_t * );
static int   Control ( demux_t *, int, va_list );

static bool InitH264(demux_t *p_demux, unsigned char* p_data, size_t i_len);
static bool InitAudio(demux_t *p_demux, unsigned char* p_data, size_t i_len);

static int  Open ( vlc_object_t * obj) {

	demux_t		*p_demux = (demux_t*)obj;
	demux_sys_t	*p_sys = NULL;

	// Set exported functions
	p_demux->pf_demux = Demux;
	p_demux->pf_control = Control;
	p_demux->s = NULL;
	p_demux->p_sys = p_sys = (demux_sys_t*) malloc( sizeof( demux_sys_t ) );

	/* Setup system */
	p_sys->b_h264_init = false;
	p_sys->b_audio_init = false;
	p_sys->p_audio_es = 0;
	p_sys->p_video_es = 0;
	p_sys->audio = 0;
	p_sys->video = 0;
	p_sys->videoPts = 0;
	//p_sys->p_Client = StreamImpl();

	/* Parse URL */
  	vlc_UrlParse( &p_sys->url, p_demux->psz_location, 0 );
	msg_Info(p_demux, "Opening URL: %s:%d%s", p_sys->url.psz_host, p_sys->url.i_port, p_sys->url.psz_path );
	msg_Dbg(p_demux,"Location: %s", p_demux->psz_location); 

	std::string url ( p_demux->psz_location );

	/* Open session */
	if ( ! p_sys->p_Client->open( url ) ) {

		return VLC_EGENERIC;	
	} 

	return VLC_SUCCESS;
}

static bool InitH264(demux_t *p_demux, unsigned char* p_data, size_t i_len) {

	demux_sys_t *p_sys = p_demux->p_sys;

	if( ! p_sys->video ) return false;

	if(p_data == NULL || i_len == 0) {
		msg_Err(p_demux, "No SPS/PPS found in initialization data!");
		return false;
	} else {
		msg_Dbg(p_demux, "Reading SPS/PPS from @%u of length %d", p_data, i_len);
	}

	if( ! p_sys->b_h264_init ) {

		block_t* p_block;
		MediaTimestampUs pos = 0; 
		VideoFormat& vf = p_demux->p_sys->video->video;

		msg_Info(p_demux, "Getting video format info: %d x %d", vf.screenResolution.width, vf.screenResolution.height);
		msg_Dbg(p_demux, "Init'ing H264 decoder ...");

		es_format_Init( &p_sys->video_fmt, VIDEO_ES, VLC_CODEC_H264 );		
		p_sys->video_fmt.p_extra = p_data;
		p_sys->video_fmt.i_extra = i_len;
/*
		p_sys->video_fmt.video.i_width = vf.screenResolution.width;
		p_sys->video_fmt.video.i_height = vf.screenResolution.height;
		p_sys->video_fmt.video.i_visible_width = vf.screenResolution.width;
		p_sys->video_fmt.video.i_visible_height = vf.screenResolution.height;
*/
		p_sys->p_video_es = es_out_Add(p_demux->out, &p_sys->video_fmt);

		p_sys->b_h264_init = true;

		/* (re-)init H264 decoder */
		msg_Info(p_demux, "Pushing SPS/PPS block of size %d to decoder ...", i_len);

		//p_demux->p_sys->p_Client->getPos(pos);

		p_block = block_Alloc(i_len);
		block_Init(p_block, p_data, i_len);
		p_block->i_pts = VLC_TS_0 + pos;
		p_block->i_dts = VLC_TS_0 + pos;
		es_out_Send(p_demux->out, p_demux->p_sys->p_video_es, p_block);

		return true;
	}
	
	/* VIDEO ES */
	
/* //Doesn't really work well
//trying to replace VLC ES outs with ones of the _adapted_ format !!!
	//es_out_Del( p_demux->out, p_sys->p_video_es );
	//p_sys->p_video_es = es_out_Add(p_demux->out, &p_sys->video_fmt);
 	//es_out_Control( p_demux->out, ES_OUT_SET_ES_FMT, p_sys->p_video_es
*/
	

#if 0
	msg_Dbg(p_demux, "SPS/PPS delimiter: %u %u %u %u", *((uint8_t*)p_demux->p_sys->video_fmt.p_extra+0), 
								*((uint8_t*)p_demux->p_sys->video_fmt.p_extra+1),
								*((uint8_t*)p_demux->p_sys->video_fmt.p_extra+2),
								*((uint8_t*)p_demux->p_sys->video_fmt.p_extra+3));

	for(int i=0; i<i_len;i++) {
		msg_Dbg(p_demux, "SPS/PPS delimiter at %i: %u",i, *((uint8_t*)p_demux->p_sys->video_fmt.p_extra+i));	
	}
#endif

	//es_out_Control( p_demux->out, ES_OUT_RESTART_ES, p_sys->p_video_es);

	return true;

}


static bool InitAudio(demux_t *p_demux, unsigned char* p_data, size_t i_len) {

	demux_sys_t* p_sys = p_demux->p_sys;

	if( ! p_sys->b_audio_init && p_sys->audio ) {

		char szFourCC[4]; 
		vlc_fourcc_t fourCC = 0; //uint32_t
		vlc_fourcc_to_char( p_sys->audio->audio.fourCC, szFourCC);

		msg_Info(p_demux, "Found audio 4CC = %s", szFourCC);

		if( p_sys->audio->audio.fourCC == VLC_CODEC_MP4A ) {
			fourCC = VLC_CODEC_MP4A;
		} else if ( p_sys->audio->audio.fourCC == VLC_CODEC_EAC3 ) {
			fourCC = VLC_CODEC_EAC3;
		} 

		if( ! fourCC ) {
			msg_Err(p_demux, "Non-supported or invalid 4CC = %s", szFourCC);
			return false;
		}

		msg_Info(p_demux, "Init audio decoder @ %d Hz ...", (int) p_sys->audio->audio.samplingRate);

		/* AUDIO ES */
		es_format_Init( &p_sys->audio_fmt, AUDIO_ES , fourCC );
	 	p_sys->audio_fmt.audio.i_channels = 2;
	 	p_sys->audio_fmt.audio.i_rate = p_sys->audio->audio.samplingRate;
		p_sys->audio_fmt.audio.i_bitspersample = 16;
		p_sys->audio_fmt.i_extra = i_len;
		p_sys->audio_fmt.p_extra = p_data;
		/* add elementary stream to chain */	
		p_sys->p_audio_es = es_out_Add(p_demux->out, &p_sys->audio_fmt);
		p_sys->b_audio_init = true;

		return true;
	}

	return false;
}

static void Close( vlc_object_t * obj)
{
	demux_t* p_demux = (demux_t*)obj;

	if( p_demux->p_sys->p_Client ) {

		p_demux->p_sys->p_Client->close();
		free(p_demux->p_sys->p_Client);
	}

	free(p_demux->p_sys);
}


inline static int DemuxES(demux_t *p_demux,
					es_out_id_t * p_es_out,
					OutputType eStream) {

	demux_sys_t* p_sys = p_demux->p_sys;
	block_t * p_block;
	AutoPtr<PayloadInfo> sample;
	MediaTimestampUs pos;
	SampleInfo* si = 0;
	SidechannelInfo* sci = 0;
	MediaTimestampUs* pts(0);
	MediaTimestampUs pcr = 0;

	//msg_Dbg(p_demux, "Reading from stream object ...");

	p_sys->p_Client->read(eStream, sample);

	if( sample->payload == PayloadInfo::EMPTY ) {

		return 0; //no bytes read

	} else if( sample->payload == PayloadInfo::SAMPLE ) {
		
		si = CastToSampleInfo( sample.get() );	 
		
	} else if( sample->payload == PayloadInfo::SIDECHANNEL ) {

		if(eStream == VIDEO) {

			p_sys->video = CastToSidechannelInfo( sample.get() );
			InitH264(p_demux, p_sys->video->data->data(), p_sys->video->data->length());
	
		} else if(eStream == AUDIO) {
	
			p_sys->audio = CastToSidechannelInfo( sample.get() );	
			InitAudio(p_demux, p_sys->audio->data->data(), p_sys->audio->data->length());
		}
	}  

	switch(eStream) {
	case VIDEO: sci = p_sys->video; pts = &p_sys->videoPts; break;
	case AUDIO: sci = p_sys->audio; pts = &p_sys->audioPts; break;
	}

	if( ! sci ) {
		msg_Err(p_demux, "No sidechannel data present!");
		return 0;
	}

	if( ! si ) {
		return 0;	
	}

	//msg_Info(p_demux, "Allocating block...");

	p_block = block_Alloc( si->size );
	block_Init(p_block, ( si->data->data() + si->offset ), si->size);
	p_block->i_pts = VLC_TS_0 + ToMicroseconds(si->pts, sci->timescale);
	p_block->i_dts = VLC_TS_0 + ToMicroseconds(si->dts, sci->timescale);
	p_block->i_nb_samples = 1;

	//msg_Dbg(p_demux, "DTS=%lld, PTS=%lld", ToMicroseconds(si->dts, sci->timescale), ToMicroseconds(si->pts, sci->timescale));
	//msg_Dbg(p_demux, "Pushing sample of %d bytes with DTS=%lld, PTS=%lld", si->size, p_block->i_dts, p_block->i_pts);

	*pts = p_block->i_pts; //update PTS counter
	pcr = std::min(p_sys->videoPts, p_sys->audioPts ); //determine current PCR value

	es_out_Control( p_demux->out, ES_OUT_SET_PCR, pcr);
	es_out_Send( p_demux->out, p_es_out, p_block );

	return si->size;
}

static int Demux( demux_t *p_demux)
{

	int i = 0;
	bool bSelected = true;

	//msg_Info(p_demux, "Calling Demux now !!!!!!!!!!!!");
#if 1
	if(p_demux->p_sys->p_audio_es) {
		es_out_Control( p_demux->out, ES_OUT_GET_ES_STATE, p_demux->p_sys->p_audio_es, &bSelected);
	}

	if( bSelected ) {
		
		//msg_Dbg(p_demux, "Audio track selected");
		i += DemuxES(p_demux, p_demux->p_sys->p_audio_es, AUDIO);
	}
#endif
	if(p_demux->p_sys->p_video_es) {
		es_out_Control( p_demux->out, ES_OUT_GET_ES_STATE, p_demux->p_sys->p_video_es, &bSelected);
	}

	if( bSelected ) {

		//msg_Dbg(p_demux, "Video track selected");
		i += DemuxES(p_demux, p_demux->p_sys->p_video_es, VIDEO);
	}

	if (i > 0) {
		//msg_Dbg(p_demux, "Done reading %d total audio/video bytes", i);
	} else {
#ifdef SLEEP_ON_EMPTY_READ
		//msg_Dbg(p_demux, "Had an empty read, blocking just a tiny while...");
		usleep(500000);
#endif
	}

	return 1; 

}

static int   Control ( demux_t *p_demux, int i_query, va_list args)
{

	demux_sys_t *p_sys = p_demux->p_sys;
	double f, *pf;
	int64_t i64, *pi64;
	bool *pb_bool;
	VideoFormat vf;
	MediaTimestampUs dur = -1;
	MediaTimestampUs pos = 0;

	switch( i_query )
	{
	case DEMUX_GET_POSITION:
			//return VLC_EGENERIC;
			pf = (double*)va_arg( args, double * );
			p_demux->p_sys->p_Client->getPos(pos);
			p_demux->p_sys->p_Client->getDuration(dur);
			*pf = double(pos) / double(dur);
            		return VLC_SUCCESS;

        case DEMUX_SET_POSITION:
            		f = (double)va_arg( args, double );
			msg_Dbg(p_demux, "Seeking to %f percent of media length", f*100);
			p_demux->p_sys->p_Client->getDuration(dur);
			p_demux->p_sys->p_Client->seek( (MediaTimestampUs) (f * double(dur)) );
	    		return VLC_SUCCESS;

        case DEMUX_GET_TIME:
			pi64 = (int64_t*)va_arg( args, int64_t * );
			if (pi64) {
				p_demux->p_sys->p_Client->getPos(*pi64);
				//*pi64 = std::min(p_sys->videoPts, p_sys->audioPts );
				return VLC_SUCCESS;
			}
			else return VLC_EGENERIC;

        case DEMUX_SET_TIME:
			i64 = (int64_t)va_arg( args, int64_t );
			p_demux->p_sys->p_Client->seek(i64);
			return VLC_SUCCESS;

        case DEMUX_GET_LENGTH:
			pi64 = (int64_t*)va_arg( args, int64_t * );
			if (pi64) {
				p_demux->p_sys->p_Client->getDuration(*pi64);
				//msg_Dbg(p_demux, "Total length: %lld", *pi64);
				return VLC_SUCCESS;
			}
			else 
			return VLC_EGENERIC;

        case DEMUX_GET_FPS:
			return VLC_EGENERIC;

        case DEMUX_GET_META:
        {
			return VLC_EGENERIC;
        }

        case DEMUX_GET_TITLE_INFO:
        {
			return VLC_EGENERIC;
        }

        case DEMUX_SET_TITLE:
        {
			return VLC_EGENERIC;
        }

        case DEMUX_SET_SEEKPOINT:
        {			
			return VLC_EGENERIC;
        }

        case DEMUX_GET_PTS_DELAY:
		        //return VLC_EGENERIC;
			pi64 = (int64_t*)va_arg( args, int64_t * );
			*pi64 = DEBUG_PTS_DELAY;
			return VLC_SUCCESS;

	/* Special for access_demux */
	case DEMUX_CAN_PAUSE:
	case DEMUX_CAN_SEEK:
	case DEMUX_CAN_CONTROL_PACE:
	{
			pb_bool = (bool*)va_arg( args, bool * );
			*pb_bool = true;
			return VLC_SUCCESS;
	}

        case DEMUX_SET_NEXT_DEMUX_TIME:	
        case DEMUX_SET_GROUP:
        case DEMUX_HAS_UNSUPPORTED_META:
        case DEMUX_GET_ATTACHMENTS:
        case DEMUX_CAN_RECORD:
            return VLC_EGENERIC;

        default:
            msg_Warn( p_demux, "control query %u unimplemented", i_query );
            return VLC_EGENERIC;
    }
}
