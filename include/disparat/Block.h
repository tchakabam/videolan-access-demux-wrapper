/*****************************************************************************
 * VideoLAN plugin: access_demux module wrapper
 * 
 * Authors: Stephan Hesse <disparat@gmail.com>
 *****************************************************************************

/*
 * Buffer.h
 *
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include <vector>

#include <disparat/DataSink.h>

#include "Poco/RefCountedObject.h"
#include "Poco/AutoPtr.h"

namespace disparat {

	class Block: public Poco::RefCountedObject, public DataSink {
	public:

		Block(size_t size=0, bool keep=false) : m_Data( new std::vector<unsigned char>() ) {
			m_Data->resize(size);
			m_Keep = keep;
		}

		~Block() {
			if(!m_Keep) {
				delete m_Data;
			}
		}

		size_t write(unsigned char* data, size_t len) {

			//m_Data->resize(len);
			//memcpy(m_Data->data(), data, len);

			for(size_t i=0;i<len;i++) {
				m_Data->push_back(data[i]);
			}

			return len;
		}

		ptrdiff_t flush() { m_Data->clear(); return 0; }

		unsigned char* data() const {return m_Data->data();}
		size_t length() const { return m_Data->size(); }

		bool isKeepingData() const {return m_Keep;}
		void setKeepingData(bool b) {m_Keep = b;}

	private:
		std::vector<unsigned char>* m_Data;
		bool m_Keep;
	};

}

#endif /* BUFFER_H_ */
