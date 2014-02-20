/*
 *
 * Copyright (c) 2011, 2012 castLabs GmbH
 *
 * http://www.castlabs.com
 *
 */

/*
 * DRMManifest.h
 *
 *  Created on: Sep 9, 2012
 *      Author: ubuntu
 */

#ifndef DRMMANIFEST_H_
#define DRMMANIFEST_H_

#include <asl/framework/Media.h>

namespace asl {

	class ProtectedMedia : public Media {
	public:

		ProtectedMedia(clc::URI uri)
		: m_bIsEncrypted(false)
		, m_pDrmHeader(0)
		, Media(uri)
		{}

		~ProtectedMedia() {
			if ( m_pDrmHeader )
			{
				free( m_pDrmHeader );
			}
		}

		// DRM Info
		bool isEncrypted() {
			return m_bIsEncrypted;
		}

		char* getDrmHeader()
		{
			return m_pDrmHeader;
		}
/*
		DRMType getDrmType()
		{
			return m_drmType;
		}
*/
	protected:

		void setDrmHeader(char* drmHeader)
		{
			this->m_pDrmHeader = drmHeader;
		}
/*
		void setDrmType(DRMType drmType)
		{
			this->m_drmType = drmType;
		}
*/
		void setEncrypted(bool isEncrypted)
		{
			this->m_bIsEncrypted = isEncrypted;
		}

	private:
		// Protection Info
		//DRMType m_drmType;
		// the drm type read from <protectionheader systemid>
		bool m_bIsEncrypted;
		// true if manifest contains protection tag
		char* m_pDrmHeader;
		// content of the protectionheader tag
	};

}

#endif /* DRMMANIFEST_H_ */
