
#include "ProcessManager.h"

CProcessManager::CProcessManager(const std::string &sFilePath) :
m_hInterfaceMutex(),
m_pcProcessMonitor(sFilePath, this),
m_elEventLogger(),
m_crStartCallbackRegistry(),
m_crStopCallbackRegistry(),
m_crManuallyStopCallbackRegistry(),
m_crProcMonitoringFailCallbackRegistry()
{
	InitializeInterfaceMutex();
	InitLogger();
}

CProcessManager::CProcessManager(unsigned uiProcessID):
m_hInterfaceMutex(),
m_pcProcessMonitor(uiProcessID, this),
m_elEventLogger(),
m_crStartCallbackRegistry(),
m_crStopCallbackRegistry(),
m_crManuallyStopCallbackRegistry(),
m_crProcMonitoringFailCallbackRegistry()
{
	InitLogger();
}

CProcessManager::~CProcessManager()
{
	FinalizeInterfaceMutex();
}

void CProcessManager::AddProcessStartListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext)
{
	CCallbackRegistry &crStartCallbackRegistry = GetStartCallbackRegistry();
	crStartCallbackRegistry.push_back(CEventCallbackInfo(fnCallbackProc, psvCallbackContext));
}

void CProcessManager::AddProcessStopListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext)
{
	CCallbackRegistry &crStopCallbackRegistry = GetStopCallbackRegistry();
	crStopCallbackRegistry.push_back(CEventCallbackInfo(fnCallbackProc, psvCallbackContext));
}

void CProcessManager::AddProcessManuallyStopListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext)
{
	CCallbackRegistry &crManuallyStopCallbackRegistry = GetManuallyStopCallbackRegistry();
	crManuallyStopCallbackRegistry.push_back(CEventCallbackInfo(fnCallbackProc, psvCallbackContext));
}

void CProcessManager::AddProcMonitoringFailListener(CEventHandlerProcedure fnCallbackProc, void *psvCallbackContext)
{
	CCallbackRegistry &crProcMonitoringFailCallbackRegistry = GetProcMonitoringFailCallbackRegistry();
	crProcMonitoringFailCallbackRegistry.push_back(CEventCallbackInfo(fnCallbackProc, psvCallbackContext));
}

bool CProcessManager::StartProcessMonitoring()
{
	HANDLE &hMutex = GetInterfaceMutex();
	assert(hMutex != 0);
	ÑProcessMonitor &pcProcessMonitor = GetProcessMonitor();
	WaitForSingleObject(hMutex, INFINITE);
	bool bResoult = pcProcessMonitor.StartMonitoring();
	ReleaseMutex(hMutex);
	return bResoult;
}

bool CProcessManager::StopProcessMonitoring()
{
	HANDLE &hMutex = GetInterfaceMutex();
	assert(hMutex != 0);
	ÑProcessMonitor &pcProcessMonitor = GetProcessMonitor();
	WaitForSingleObject(hMutex, INFINITE);
	bool bResoult = pcProcessMonitor.StopMonitoring();
	ReleaseMutex(hMutex);
	return bResoult;
}
	
unsigned CProcessManager::RetreiveProcessID() const
{
	ÑProcessMonitor &pcProcessMonitor = GetProcessMonitor();
	return pcProcessMonitor.RetreiveProcessID();
}

HANDLE CProcessManager::RetreiveProcessHandle() const
{
	ÑProcessMonitor &pcProcessMonitor = GetProcessMonitor();
	return pcProcessMonitor.RetreiveProcessHandle();
}

ÑProcessMonitor::EPROCESSSTATUSTYPE CProcessManager::GetProcessStatus() const
{
	ÑProcessMonitor &pcProcessMonitor = GetProcessMonitor();
	return pcProcessMonitor.GetProcessStatus();
}

/*virtual*/
void CProcessManager::HandleProcessStart()
{
	CCallbackRegistry &crStartCallbackRegistry = GetStartCallbackRegistry();
	NotifyEventListeners(crStartCallbackRegistry);
	LogMessage("Process Started ");
}

/*virtual*/
void CProcessManager::HandleProcessStop()
{
	CCallbackRegistry &crStopCallbackRegistry = GetStopCallbackRegistry();
	NotifyEventListeners(crStopCallbackRegistry);
	LogMessage("Process Stopped ");
}

/*virtual*/
void CProcessManager::HandleProcessManuallyStopped()
{
	CCallbackRegistry &crManuallyStopCallbackRegistry = GetManuallyStopCallbackRegistry();
	NotifyEventListeners(crManuallyStopCallbackRegistry);
	LogMessage("Process Manually Stopped ");
}

/*virtual*/
void CProcessManager::HandleProcessMonitoringFail(std::string sErrorText)
{
	CCallbackRegistry &crProcMonitoringFailCallbackRegistry = GetProcMonitoringFailCallbackRegistry();
	NotifyEventListeners(crProcMonitoringFailCallbackRegistry);
	LogMessage(sErrorText);
}

void CProcessManager::InitLogger()
{
	CEventLogger  &elEventLogger = GetEventLogger();
	std::string sLogFilePath = GetCurrentDirPath();
	elEventLogger.InitLogFile(sLogFilePath);
}

string CProcessManager::GetCurrentDirPath()
{
	char  buffer[MAX_PATH] = "";
	GetCurrentDirectory(sizeof(buffer), buffer);
	string sPath = buffer;
	return sPath;
}

void CProcessManager::NotifyEventListeners(const CCallbackRegistry &crCallbackRegistry)
{
	const CCallbackRegistry::const_iterator itRegistryEnd = crCallbackRegistry.end();

	for (CCallbackRegistry::const_iterator itCurrentInfo = crCallbackRegistry.begin(); itCurrentInfo != itRegistryEnd; ++itCurrentInfo)
	{
		const CEventCallbackInfo &ciCurrentInfo = *itCurrentInfo;

		CEventHandlerProcedure fnCallbackProc = ciCurrentInfo.m_fnCallbackProc;
		void *psvCallbackContext = ciCurrentInfo.m_psvCallbackContext;

		fnCallbackProc(psvCallbackContext);
	}
}

void CProcessManager::LogMessage(string sMessageText)
{
	CEventLogger  &elEventLogger = GetEventLogger();
	string sLogText = sMessageText;
	string sProcId = std::to_string(RetreiveProcessID());
	sLogText.append(" ID = " + sProcId + "\n");
	elEventLogger.LogMessage(sLogText);
}

void CProcessManager::InitializeInterfaceMutex()
{
	HANDLE &hMutex = GetInterfaceMutex();
	hMutex = CreateMutex(NULL,false,NULL);
}

void  CProcessManager::FinalizeInterfaceMutex()
{
	HANDLE &hMutex = GetInterfaceMutex();
	CloseHandle(hMutex);
}