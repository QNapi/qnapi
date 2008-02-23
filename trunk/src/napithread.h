#ifndef __NAPITHREAD__H__
#define __NAPITHREAD__H__

#include <QThread>

class NapiThread : public QThread
{
Q_OBJECT
	public:
		NapiThread() : abort(false)
		{
			setTerminationEnabled(true);
		}
		void requestAbort() { abort = true; }
	protected:
		bool abort;
};

#endif
