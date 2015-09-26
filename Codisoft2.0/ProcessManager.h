#ifndef __INCLUDE_PROCESSMANAGER_H
#define __INCLUDE_PROCESSMANAGER_H

#include "ProcessStatusSinkInterface.h"
#include "EventLogger.h"
#include "ProcessMonitor.h"

class CProcessManager:
	public IProcessStatusSink
{
public:
	typedef std::function<void(void *)> CEventHandlerProcedure;

private:
	class CEventCallbackInfo
	{
	public:
		CEventCallbackInfo(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext) :
			m_fnCallbackProc(fnCallbackProc),
			m_psvCallbackContext(psvCallbackContext)
		{}

		CEventCallbackInfo(CEventHandlerProcedure fnCallbackProc) :
			m_fnCallbackProc(fnCallbackProc),
			m_psvCallbackContext(NULL)
		{}
		
		CEventHandlerProcedure	m_fnCallbackProc;
		void					*m_psvCallbackContext;
	};

	typedef std::list<CEventCallbackInfo> CCallbackRegistry;

public:
	CProcessManager(const string &sFilePath);
	CProcessManager(unsigned uiProcessID);
	~CProcessManager();

public:
	void AddProcessStartListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext);
	
	void AddProcessStopListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext);
	
	void AddProcessManuallyStopListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext);
	
	void AddProcMonitoringFailListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext);
	
	bool StartProcessMonitoring();
	bool StopProcessMonitoring();

	unsigned RetreiveProcessID() const;
	HANDLE RetreiveProcessHandle() const;
	ÑProcessMonitor::EPROCESSSTATUSTYPE GetProcessStatus() const;

private: // IProcessStatusSink
	virtual void HandleProcessStart();
	virtual void HandleProcessStop();
	virtual void HandleProcessManuallyStopped();
	virtual void HandleProcessMonitoringFail(string sErrorText);

private:
	void InitLogger();
	string GetCurrentDirPath();
	void NotifyEventListeners(const CCallbackRegistry &crCallbackRegistry);

private:
	void LogMessage(string sMessageText);

private:
	void InitializeInterfaceMutex();
	void FinalizeInterfaceMutex();

private:
	HANDLE              &GetInterfaceMutex() const { return const_cast<HANDLE &>(m_hInterfaceMutex); }
	CEventLogger		&GetEventLogger() const { return const_cast<CEventLogger &>(m_elEventLogger); }
	ÑProcessMonitor		&GetProcessMonitor() const { return const_cast<ÑProcessMonitor &>(m_pcProcessMonitor); }
	CCallbackRegistry	&GetStartCallbackRegistry() const { return const_cast<CCallbackRegistry &>(m_crStartCallbackRegistry); }
	CCallbackRegistry	&GetStopCallbackRegistry() const { return const_cast<CCallbackRegistry &>(m_crStopCallbackRegistry); }
	CCallbackRegistry	&GetManuallyStopCallbackRegistry() const { return const_cast<CCallbackRegistry &>(m_crManuallyStopCallbackRegistry); }
	CCallbackRegistry	&GetProcMonitoringFailCallbackRegistry() const { return const_cast<CCallbackRegistry &>(m_crProcMonitoringFailCallbackRegistry); }

private:
	HANDLE				m_hInterfaceMutex;
	CEventLogger		m_elEventLogger;
	ÑProcessMonitor     m_pcProcessMonitor;
	CCallbackRegistry	m_crStartCallbackRegistry;
	CCallbackRegistry	m_crStopCallbackRegistry;
	CCallbackRegistry	m_crManuallyStopCallbackRegistry;
	CCallbackRegistry	m_crProcMonitoringFailCallbackRegistry;
};

#endif // __INCLUDE_PROCESSMANAGER_H









