#include "tinyLog.h"
#include <windows.h>

int main(int argc, char* argv[])
{
    tinyLogger log;
    log.level ( tinyLogger::info );
    log.format ( "%25(asctime)'%(filename)' : %(lineno) : %(funame)\t:\t" );
    log.destination ( "d:\\test.log" );
    log.destination ( tinyLogger::warning, "d:\\warning.log" );
    log.destination ( tinyLogger::error, "d:\\warning.log" );
    for ( int i=0; i<60; i++ ) {
	LOG_INFO    ( log, "log message%10d", 1 );
	LOG_WARNING ( log, "warn message" );
	LOG_ERROR   ( log, "error message" );
	LOG_DEBUG   ( log, "debug message" );
	Sleep ( 1000 );
    }
    return 0;
}

