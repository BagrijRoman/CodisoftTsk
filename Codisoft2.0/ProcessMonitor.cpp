
#include "ProcessMonitor.h"
#include "ProcessStatusSinkInterface.h"
#include <Psapi.h>

#define ProcessStartEvent			1
#define ProcessStopEvent			2
#define ProcessManuallyStoppedEvent 3


�ProcessMonitor::�ProcessMonitor(const std::string &sFilePath, IProcessStatusSink *issProcessStatusSink) :
m_pcProcessStatusSink(issProcessStatusSink),
m_sCommandLine(sFilePath),
m_iProcessID(),
m_hProcessHandle(NULL),
m_hMonitoringThreadHandle(NULL),
m_stProcessStatus(PST__DEFAULT),
m_isManulyStopFlag(false),
m_isMonitoringThreadRunned(false)
{
	InternalInitialize();
}

�ProcessMonitor::�ProcessMonitor(unsigned uiProcessID, IProcessStatusSink *issProcessStatusSink) :
m_pcProcessStatusSink(issProcessStatusSink),
m_sCommandLine(),
m_iProcessID(uiProcessID),
m_hProcessHandle(NULL),
m_hMonitoringThreadHandle(NULL),
m_stProcessStatus(PST_RUNNING),
m_isManulyStopFlag(false),
m_isMonitoringThreadRunned(false)
{
	InternalInitialize();
}

�ProcessMonitor::~�ProcessMonitor()
{
	/* do nothing */
}

bool �ProcessMonitor::StartMonitoring()
{
	assert(!GetIsMonitoringThreadRunnedFlag());
	
	bool bResoult = false;
	SetIsManualyStoppedFlag(false);
	
	HANDLE hThreadHandle = CreateThread(NULL,0,&�ProcessMonitor::ExecuteMonitoringThreadProc,(LPVOID)this,0,NULL);
	
	if (hThreadHandle != NULL)
	{
		SetMonitoringThreadHandle(hThreadHandle);
		SetIsMonitoringThreadRunnedFlag(true);
		bResoult = true;
	}
	else
	{
		HandleMonitorFail("Start Monitoring Thread operation failed");
	}


	return bResoult;
}

bool �ProcessMonitor::StopMonitoring()
{
	assert(GetIsMonitoringThreadRunnedFlag());

	return ProcessManyalyStop();
}

void �ProcessMonitor::InternalInitialize()
{
	�ProcessMonitor::EPROCESSSTATUSTYPE eProcStatus = GetProcessStatus();
	if (eProcStatus == PST_RUNNING)
	{
		GetRunnedProcInfo();
	}
}

bool �ProcessMonitor::CloseHandles()
{
	HANDLE hProcHandle = GetProcessHandle();
	HANDLE hMonitoringThreadHandle = GetMonitoringThreadHandle();
	
	bool bProcessHandleClosed = CloseHandle(hProcHandle);
	bool bMonitoringThreadHandleClosed = CloseHandle(hMonitoringThreadHandle);

	hProcHandle = NULL;
	hMonitoringThreadHandle = NULL;
	SetProcessHandle(hProcHandle);
	SetMonitoringThreadHandle(hMonitoringThreadHandle);

	bool bResoult = bProcessHandleClosed && bMonitoringThreadHandleClosed;

	return bResoult;
}

void �ProcessMonitor::GetRunnedProcInfo()
{
	unsigned uiProcessID = GetProcessID();
	HANDLE hProcessHandle;
	std::string sProcCommadLine;

	hProcessHandle = GetProcHandleByID(uiProcessID);
	sProcCommadLine = GetProcCommandLineByID(uiProcessID);

	SetProcessHandle(hProcessHandle);
	SetCommandLine(sProcCommadLine);
}

bool �ProcessMonitor::ProcessManyalyStop()
{
	SetIsManualyStoppedFlag(true);
	
	HANDLE &hPHandle = GetProcessHandle();
	assert(hPHandle != NULL);
	
	bool bResult = KillProcess(hPHandle, 0);
	
	if (bResult)
	{
		EPROCESSSTATUSTYPE eProcessStatus = PST_STOPPED;
		SetProcStatus(eProcessStatus);
	}
	else
	{
		HandleMonitorFail("Manually Stop process operation failed");
	}
	
	HANDLE hMonitorringThreadHandle = GetMonitoringThreadHandle();
	assert(hMonitorringThreadHandle != NULL);

	WaitForSingleObject(GetMonitoringThreadHandle(), INFINITY);
	CloseHandles();

	return bResult;
}

/*static*/
DWORD WINAPI �ProcessMonitor::ExecuteMonitoringThreadProc(LPVOID lpParameter)
{
	�ProcessMonitor *ppmMonitorInstance = static_cast<�ProcessMonitor *>(lpParameter);

	const unsigned uiThreadResult = ppmMonitorInstance->ExecuteMonitoringThread();

	return uiThreadResult;
}

unsigned �ProcessMonitor::ExecuteMonitoringThread()
{

	while (true)
	{
		if (GetIsManualyStoppedFlag())
		{
			HandleProcEvent(ProcessManuallyStoppedEvent);
			break;
		}

		if (!CheckProcessRunning())
		{
			if (GetProcessHandle() != NULL)
			{
				HandleProcEvent(ProcessStopEvent);
				SetRestartingStatus();
			}
						
			if (!StartCurrentProcess())
			{
				HandleMonitorFail("Start process operation failed");
				break;
			}
			HandleProcEvent(ProcessStartEvent);
		}
		WaitForSingleObject(GetProcessHandle(), INFINITY);
		
	}
	SetIsMonitoringThreadRunnedFlag(false);
	return ERROR_SUCCESS;
}

void �ProcessMonitor::HandleProcEvent(int iEventType)
{
	IProcessStatusSink	*pProcessStatusSink = GetProcessStatusSinkPtr();
	
	switch (iEventType)
	{
		case ProcessStartEvent : 
		{
			pProcessStatusSink->HandleProcessStart();
			break;
		}
		case ProcessStopEvent:
		{
			pProcessStatusSink->HandleProcessStop();
			break;
		}
		case ProcessManuallyStoppedEvent: 
		{ 
			pProcessStatusSink->HandleProcessManuallyStopped(); 
			break;
		}
	}

}

void �ProcessMonitor::HandleMonitorFail(const string &sErrorText)
{
	IProcessStatusSink	*pProcessStatusSink = GetProcessStatusSinkPtr();
	pProcessStatusSink->HandleProcessMonitoringFail(sErrorText);
}

void �ProcessMonitor::SetRestartingStatus()
{
	�ProcessMonitor::EPROCESSSTATUSTYPE eProcStatus = PST_RESTARTING;
	SetProcStatus(eProcStatus);
}

bool �ProcessMonitor::StartCurrentProcess()
{
	bool bResoult = StartProcess(GetCommandLine());
	
	if (bResoult)
	{
		�ProcessMonitor::EPROCESSSTATUSTYPE eProcStatus = PST_RUNNING;
		SetProcStatus(eProcStatus);
	}
	
	return bResoult;
}

bool �ProcessMonitor::CheckProcessRunning()
{
	bool bResoult = false;
	DWORD dwExitCode = 0;

	if (GetProcessHandle()!=NULL)
	{
		if (!GetExitCodeProcess(GetProcessHandle(), &dwExitCode))
		{
			HandleMonitorFail("Get Process Exit code operation failed");
		}
		if (dwExitCode == STILL_ACTIVE)
		{
			bResoult = true;
		}
	}
	return bResoult;
}

bool �ProcessMonitor::StartProcess(const string &strCommandLine)
{
	
	LPSTR CommadLine = const_cast<char *>(strCommandLine.c_str());
	STARTUPINFO si;
	PROCESS_INFORMATION piProcInfo;
	bool bResult = false;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION)); 

 	if (CreateProcess(NULL, CommadLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &piProcInfo))
	{
		bResult = true; 

		SetProcessID(piProcInfo.dwProcessId);
		SetProcessHandle(piProcInfo.hProcess);
	}

	return bResult;
}

bool �ProcessMonitor::KillProcess(HANDLE hProcessHandle, UINT uiExitCode)
{
	bool bResult = TerminateProcess(hProcessHandle, uiExitCode) != 0;

	return bResult;
}

HANDLE �ProcessMonitor::GetProcHandleByID(unsigned uiProcessID)
{
	HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiProcessID);
	return ProcessHandle;
}

string �ProcessMonitor::GetProcCommandLineByID(unsigned uiProcessID)
{
	char path[256] = "";
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, uiProcessID);
	HMODULE mod;

	EnumProcessModules(pHandle, &mod, sizeof(HMODULE), NULL);
	GetModuleFileNameEx(pHandle, mod, path, 255);
	string sLine = path;
	return sLine;
}





