#include "Log.h"

FILE* Log::m_fp = NULL;
LOG_MODE Log::m_mode = LOG_MODE_DEBUG;

Log::Log(void)
{
}


Log::~Log(void)
{
}

void Log::SetLogFile(const char *szFilename)
{
	if (m_mode == LOG_MODE_FILE){
		if( m_fp ) 
			fclose( m_fp );
		m_fp = fopen( szFilename, "w");
	}
}

void Log::Close()
{
	if (m_mode == LOG_MODE_FILE){
		if( m_fp ) {
			fclose( m_fp );
		}
	}
}

void Log::Dbg(const char* szformat, ...)
{
	va_list pArg;
	va_start(pArg, szformat);
	char buf[1000];
	int len = vsprintf (buf, szformat, pArg);
	va_end(pArg);

	if(m_mode == LOG_MODE_DEBUG) {

#ifdef WIN32
		::OutputDebugStringA(buf);
#else
		fprintf(STDOUT, "%s", buf);
#endif
	} else if (m_mode == LOG_MODE_FILE){
		if( m_fp == NULL ) return;
		fprintf(m_fp, "%s", buf);
	}
}
