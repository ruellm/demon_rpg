#pragma once

#include "common.h"

typedef enum LOG_MODE
{
	LOG_MODE_DEBUG,
	LOG_MODE_FILE
} LOG_MODE;

class Log
{
public:
	Log(void);
	~Log(void);
	static void SetLogFile(const char *szFilename);
	static void Close( );
	static void SetLogMode( LOG_MODE mode ) { m_mode = mode; }

	static void Dbg(const char* szformat, ...);

private:
	static FILE* m_fp;
	static LOG_MODE m_mode;

};

