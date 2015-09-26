#ifndef __INCLUDE_PROCESSMONITOR_H
#define __INCLUDE_PROCESSMONITOR_H

#include <Windows.h>
#include "stdafx.h"

class IProcessStatusSink;

class ÑProcessMonitor
{
public:
	enum EPROCESSSTATUSTYPE
	{
		PST__MIN,

		PST_INITIAL = PST__MIN,
		PST_RUNNING,
		PST_STOPPED,
		PST_RESTARTING,

		PST__MAX,
		PST__DEFAULT = PST_INITIAL,
	};

public:
	ÑProcessMonitor(const string &sFilePath, IProcessStatusSink *issProcessStatusSink);
	ÑProcessMonitor(unsigned uiProcessID, IProcessStatusSink *issProcessStatusSink);
	~ÑProcessMonitor();

public:
	bool StartMonitoring();
	bool StopMonitoring();

	unsigned RetreiveProcessID() const { return GetProcessID(); }   
	HANDLE RetreiveProcessHandle()  { return GetProcessHandle(); }  
	EPROCESSSTATUSTYPE GetProcessStatus() const  { return GetProcStatus(); } 

private:
	void InternalInitialize();
	void InternalFinalize();

	bool CloseHandles();

	void GetRunnedProcInfo();
	bool ProcessManyalyStop();
	
private:
	static DWORD WINAPI ExecuteMonitoringThreadProc(LPVOID lpParameter);

private:
	unsigned ExecuteMonitoringThread();
	void HandleProcEvent(int iEventType);
	void HandleMonitorFail(const string &sErrorText);

private:
	void         SetRestartingStatus();
	bool		 StartCurrentProcess();
	bool		 CheckProcessRunning();
	bool         StartProcess(const string &strCommandLine);  
	bool         KillProcess(HANDLE hProcessHandle, UINT uiExitCode);  
	HANDLE       GetProcHandleByID(unsigned uiProcessID);
	string		 GetProcCommandLineByID(unsigned uiProcessID);
	


private:
	IProcessStatusSink   *GetProcessStatusSinkPtr() const { return  m_pcProcessStatusSink; }

	string				 &GetCommandLine() const { return const_cast<string &>(m_sCommandLine); }
	void				 SetCommandLine(string &sCommandLine) { m_sCommandLine = sCommandLine; }
	
	unsigned             &GetProcessID() const {return  const_cast<unsigned &>(m_iProcessID); }
	void                 SetProcessID(unsigned iProcessId) { m_iProcessID = iProcessId; }

	void		         SetProcessHandle(HANDLE &hProcessHandle) { m_hProcessHandle = hProcessHandle; }
	HANDLE               &GetProcessHandle() const { return const_cast<HANDLE &>(m_hProcessHandle); }

	void		         SetMonitoringThreadHandle(HANDLE &hThreadHandle) { m_hMonitoringThreadHandle = hThreadHandle; }
	HANDLE               &GetMonitoringThreadHandle() const { return const_cast<HANDLE &>(m_hMonitoringThreadHandle); }
	
	EPROCESSSTATUSTYPE	 &GetProcStatus() const { return const_cast<EPROCESSSTATUSTYPE &>(m_stProcessStatus); }
	void                 SetProcStatus(EPROCESSSTATUSTYPE &eProcSatatus)   { m_stProcessStatus = eProcSatatus; }

	const bool           &GetIsManualyStoppedFlag() const { return m_isManulyStopFlag; }
	void                 SetIsManualyStoppedFlag(const bool &isManualyStoppped) { m_isManulyStopFlag = isManualyStoppped; }
	
	const bool           &GetIsMonitoringThreadRunnedFlag() const { return m_isMonitoringThreadRunned; }
	void                 SetIsMonitoringThreadRunnedFlag(const bool &isMonitoringThreadRunned) { m_isMonitoringThreadRunned = isMonitoringThreadRunned; }

private:
	IProcessStatusSink	*m_pcProcessStatusSink;
	std::string		    m_sCommandLine;
	unsigned		    m_iProcessID;
	HANDLE				m_hProcessHandle;
	HANDLE              m_hMonitoringThreadHandle;
	EPROCESSSTATUSTYPE	m_stProcessStatus;
	bool				m_isManulyStopFlag;
	bool				m_isMonitoringThreadRunned;
	
	
	
};

#endif // __INCLUDE_PROCESSMONITOR_H

