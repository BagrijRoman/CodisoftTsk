#define _CRT_SECURE_NO_WARNINGS

#include "EventLogger.h"

CEventLogger::CEventLogger() :
m_pLogFile(NULL)
{
	/* do nothing*/
}

CEventLogger::~CEventLogger()
{
	CloseLogFile();
}

void CEventLogger::InitLogFile(const string &sLogFilePath)
{
	OpenLogFile(sLogFilePath);
}

bool CEventLogger::LogMessage(const string &sMessageText)
{
	FILE *pLogFile = GetLogFile();
	assert(pLogFile != NULL);
	bool bResult = false;

	if (EOF != fputs(sMessageText.c_str(), pLogFile))
	{
		bResult = true;
	}
	
	return bResult;
}

void CEventLogger::OpenLogFile(const string &sLogFilePath)
{
	FILE *pLogFile = GetLogFile();
	assert(pLogFile==NULL);
	
	const string &sFullFilePath = sLogFilePath + "\\ProcessLog.txt";
	pLogFile = fopen(sFullFilePath.c_str(), "w+");
	
	SetLogFile(pLogFile);
}

void CEventLogger::CloseLogFile()
{
	FILE *pLogFile = GetLogFile();

	if (pLogFile != NULL)
	{
		fclose(pLogFile);
		SetLogFile(NULL);
	}
}