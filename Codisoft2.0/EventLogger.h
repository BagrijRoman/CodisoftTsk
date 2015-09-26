#ifndef __INCLUDE_EVENTLOGGER_H
#define __INCLUDE_EVENTLOGGER_H

#include "stdafx.h"

class CEventLogger
{
public:
	CEventLogger();
	~CEventLogger();

public:
	void InitLogFile(const string &sLogFilePath);
	bool LogMessage(const string &sMessageText);

private:
	void OpenLogFile(const string &sLogFilePath);
	void CloseLogFile();
   
private:
	void SetLogFile(FILE *pLogFile) { m_pLogFile = pLogFile; }
	FILE *GetLogFile() const { return m_pLogFile; }

private:
	FILE	*m_pLogFile;
};


#endif // __INCLUDE_EVENTLOGGER_H

