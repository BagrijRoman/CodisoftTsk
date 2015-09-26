

#include "ProcessManager.h"
#include "stdafx.h"

void CallbackRegistryTestFunc(void *sParametrText);

int main(int argc, char *argv[])
{
	CProcessManager *p_cPprocessManager = new CProcessManager("C:\\Windows\\System32\\calc.exe");  
	void *pvStartText = static_cast<void*>(new string("Call function from start callback registry"));
	void *pvStopText = static_cast<void*>(new string("Call function from stop callback registry"));
	void *pvManuallyStopText = static_cast<void*>(new string("Call function from Manually stop callback registry"));
	void *pvMonitoringFailText = static_cast<void*>(new string("Call function from Monitoring Fail callback registry"));

	p_cPprocessManager->AddProcessStartListener(CallbackRegistryTestFunc,pvStartText);
	p_cPprocessManager->AddProcessStopListener(CallbackRegistryTestFunc,pvStopText);
	p_cPprocessManager->AddProcessManuallyStopListener(CallbackRegistryTestFunc,pvManuallyStopText);
	p_cPprocessManager->AddProcMonitoringFailListener(CallbackRegistryTestFunc,pvMonitoringFailText);

	p_cPprocessManager->StartProcessMonitoring();

	system("pause");

	p_cPprocessManager->StopProcessMonitoring();

	system("pause");

	p_cPprocessManager->StartProcessMonitoring();
	
	system("pause");

	p_cPprocessManager->StopProcessMonitoring();
	
	system("pause");
	
	delete (p_cPprocessManager);
	delete (pvStartText);
	delete (pvStopText);
	delete (pvManuallyStopText);
	delete (pvMonitoringFailText);

	return 0;
}


void CallbackRegistryTestFunc(void *sParametrText)
{
	string *sp = static_cast<string*>(sParametrText);
	string sText = *sp;
	std::cout <<"\n"<< sText << "\n";
}
