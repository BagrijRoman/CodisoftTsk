#ifndef __PROCESSSTATUSSINKINTERFACE_H_INCLUDED
#define __PROCESSSTATUSSINKINTERFACE_H_INCLUDED

#include <string>

class IProcessStatusSink
{
public:
	virtual void HandleProcessStart() = 0;
	virtual void HandleProcessStop() = 0;
	virtual void HandleProcessManuallyStopped() = 0;

	virtual void HandleProcessMonitoringFail(std::string sErrorText) = 0; 
};


#endif // __PROCESSSTATUSSINKINTERFACE_H_INCLUDED

