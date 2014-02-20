/*
 *
 * Copyright (c) 2011, 2012 castLabs GmbH
 *
 * http://www.castlabs.com
 *
 */

/*
 * Manifest.h
 *
 *  Created: 2 juil. 2010
 *  Refactored: 2012-09-09
 *      Author: jb / Stephan Hesse
 */

#ifndef MANIFEST_H_
#define MANIFEST_H_

#include <clc/clcError.h>
#include <clc/clcPtr.h>
#include <clc/clcThreads.h>

#include <asl/framework/Event.h>
#include <asl/framework/Resource.h>
#include <asl/framework/Typedefs.h>
#include <asl/framework/HttpAgent.h>

namespace asl {

class Media: public Request//, public EventSource
{

public:

	Media(clc::URI url);
	virtual ~Media();

	/* is overloaded to parse manifest */
	virtual clc::Result unserialize() = 0;
	clc::RefPtr<MetaSuperStream> getUnserializedObject();

	void setFormat(SessionType e);
	SessionType getFormat();
	bool isLive();
	virtual void update() {}
	clc::mutex& getMutex() {return m_Mutex;}
	MediaTimestampUs getLength() { return m_duration; }
	Seconds getMinBuffer() { return m_minBuffer; }
	Seconds getMaxBuffer() { return m_maxBuffer; }

protected:

	void setLive(bool b);
	void setLength(MediaTimestampUs t) { m_duration=t; }
	void setMinBuffer(Seconds s) { m_minBuffer=s; }
	void setMaxBuffer(Seconds s) { m_maxBuffer=s; }

	SessionType m_type;

private:

	clc::RefPtr<MetaSuperStream> m_superstream;
	MediaTimestampUs m_duration;
	Seconds m_minBuffer;
	Seconds m_maxBuffer;
	bool m_isLive;
	clc::mutex m_Mutex;

};

}

#endif /* MANIFEST_H_ */
