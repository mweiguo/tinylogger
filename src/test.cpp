#include "tinyLog.h"
#include <windows.h>

int main(int argc, char* argv[])
{
    LOG_LEVEL ( tinyLog::info );
    LOG_FORMAT ( "%25(asctime)'%(filename)' : %(lineno) : %(funame)\t:\t" );
//     LOG_ALLDESTINATION ( "d:\\test.log" );
//     LOG_DESTINATION ( tinyLog::warning, "d:\\warning.log" );
//     LOG_DESTINATION ( tinyLog::error, "d:\\warning.log" );
    for ( int i=0; i<60; i++ ) {
	LOG_INFO    ( "log message%10d", 1 );
	LOG_WARNING ( "warn message" );
	LOG_ERROR   ( "error message" );
	LOG_DEBUG   ( "debug message" );
	Sleep ( 1000 );
    }
    return 0;
}

