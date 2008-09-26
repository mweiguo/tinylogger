/////////////////////////////////////////////
//  this file include tinylog interface, we can simplify using by some 
//  marco provided at bottom
//
//  author:          mweiguo@gmail.com
//  document author: xunxin03@yahoo.com.cn
//  date:   2008-06
/////////////////////////////////////////////

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <map>

// ========================================================================

struct FmtElement{
    enum KEYWORDTYPE{none, levelno, levelname, pathname, filename, lineno, asctime, funame } Keyword;
    union {
        struct {
            unsigned short align : 1;   // 0 left, 1 right
            unsigned short space : 15;  // -1 for auto justify
        };
        char ch;
    };
};

// ========================================================================
typedef void (*LogMsgHandler)( const char* type, const char* msg );

class tinyLog
{
public:
    enum LEVEL {error=0, warning, info, debug, none};
    static tinyLog& Instance() { static tinyLog t; return t; }
    void level ( LEVEL l ) { _level = l; }
    void show_title ( bool b ) { _showTitle = b; }
    void destination ( LEVEL lev, const char* dest );
    void destination ( char* dest ) { 
        destination(error,dest);
        destination(warning,dest);
        destination(info,dest);
        destination(debug,dest);
    }
    void format ( char* str );
    void log ( const char* fname, const char* filename, int lineno, LEVEL l, const char* format, ... );
    void install_msgHandler ( LogMsgHandler fn ) { _logMsgHandler = fn; }
protected:
    tinyLog ();
    ~tinyLog ();
private:
    /** ƥ��ؼ��֣��ؼ��ְ���(levelno, levelname, pathname, filename, lineno, asctime)������ɹ����عؼ��ֵ��ֽ��������򷵻�0 
     */
    int MatchKeyword ( char* p, FmtElement::KEYWORDTYPE& keyword );
private:
    LogMsgHandler             _logMsgHandler;
    LEVEL                     _level;
    std::ostream*             _out[5];
    std::map<std::string, std::ofstream*>             _outf;
    std::vector<FmtElement>   _FmtManager;
    bool                      _showTitle;
};


#define LOG_INFO( x, ... )             tinyLog::Instance().log ( __FUNCTION__, __FILE__, __LINE__, tinyLog::info,    x, ##__VA_ARGS__ )
#define LOG_WARNING( x, ...)           tinyLog::Instance().log ( __FUNCTION__, __FILE__, __LINE__, tinyLog::warning, x, ##__VA_ARGS__ )
#define LOG_ERROR( x, ...)             tinyLog::Instance().log ( __FUNCTION__, __FILE__, __LINE__, tinyLog::error,   x, ##__VA_ARGS__ )
#define LOG_DEBUG( x, ...)             tinyLog::Instance().log ( __FUNCTION__, __FILE__, __LINE__, tinyLog::debug,   x, ##__VA_ARGS__ )
#define LOG_SHOWTITLE( b )             tinyLog::Instance().show_title ( b )
#define LOG_LEVEL( l )                 tinyLog::Instance().level ( l )
#define LOG_FORMAT( fmt )              tinyLog::Instance().format ( fmt )
#define LOG_ALLDESTINATION( dst )      tinyLog::Instance().destination ( dst )
#define LOG_DESTINATION( level, dst )  tinyLog::Instance().destination ( level, dst )

// ========================================================================

/** \class tinyLog tinyLog.h "tinyLog.h"
 * \brief ����һ��ʹ��log�Ĺ���
 * \details �������¹���
 *  - ���õ���ͬ������豸
 *  - �ı���־���ı���ʽ
 *  - �ṩ��־�ȼ��Զ����˹���
 *
 * \todo 
 *  - �����c/c++���Զ����ɺ����������ƺ�����   
 *  - �ṩ״̬ʽ��¼��ʽ
 *
 * \fn void tinyLog::log ( const char* fname, const char* filename, int lineno, LEVEL l, bool showLevel, const char* format, ... );
 * \brief ���һ����Ϣ 
 * \param fname ��ǰҪ�������Ϣ���ڵĺ���������
 * \param filename ��ǰҪ�������Ϣ���ڵ��ļ�������
 * \param lineno ��ǰҪ�������Ϣ���ڵ��к�
 * \param l ��ǰҪ�������Ϣ�ĵȼ�
 * \param showLevel �����Ϣ��Ϣʱ�Ƿ���Ҫ��ʾ�ȼ�����[INFO], [WARNING]...
 * \param format ��Ϣ����ĸ�ʽ,��printf���﷨��ʽһ��
 * \returns ���صĺ���ִ�д��룬����ȥ���� tinyLog::error_message �������õ������ַ���
 *
 * \fn void tinyLog::destination ( LEVEL lev, const char* dest );
 * \brief ��������豸
 * \details ��ָ���ĵȼ��趨����豸
 * \param lev LEVEL
 * \param dest ������ stdout, stderr, �ֱ�����׼�������׼���������ַ����ᱻ����Ϊ�ļ���
 *
 * \fn void tinyLog::destination ( char* dest )
 * \brief ��������豸
 * \details ��ȫ���ĵȼ��趨Ϊͬ��������豸
 *
 * \fn void tinyLog::format ( char* str );
 * \brief ָ����ʽ���ַ������Ժ����Ϣ���Դ˸�ʽ������� 
 * \param  str %����(keyword)������������������Ҳ�����Ǹ����ֱ�������Ҷ��룬����\%25(asctime) �� \%-25(asctime)�� �ؼ��ֿ�����:
 *  - asctime   ��ǰʱ��
 *  - filename  ��ǰ�ļ���
 *  - lineno    ��ǰ�к�
 *  - funame    ���ں�������
 * 
 * \fn const char* tinyLog::error_message( int errcode );
 * \brief ���ش��������ַ������� 
 *
 * \fn void tinyLog::level ( LEVEL l )
 * \brief ָ����ǰlogger����� ��¼�ȼ����ߵȼ�������ʾ�͵ȼ�����Ϣ������������
 *
 * \fn void tinyLog::install_msgHandler ( LogMsgHandler fn )
 * \brief �����������ע����Ϣ֪ͨ����,��log���е�ʱ�򣬴�ע�ắ�����ᱻ����
 * 
 * fn��ԭ��: typedef void (*LogMsgHandler)( const char* type, const char* msg );
 * type���ᱻ���ó�info, warning, error, debug
 *
 * \enum tinyLog::LEVEL{info, warning, error, none}
 * \brief ö�٣����п���ʹ�õĵȼ� 
 *
 */

/** \mainpage ˵���ֲ�
 *
 * ��˵����Ϊ���²���
 * - \ref purpers
 * - \ref example
 * - \ref reference
 * - \ref style
 * \section purpers Ŀ��
 * - �ܹ��Զ���¼�ļ����ơ��кš��������ơ��������Ƽ���ǰֵ
 * - �ܹ����������ʽ
 * - �ܹ���Բ�ͬ�ĵȼ����ò�ͬ�����Ŀ�꣨���ļ���stdout...��
 * - ���еȼ����˹���
 * \section example ����
 * \include test.cpp 
 * \section style Ԥ����������ʽ
 \verbatim  
 Tue Jun 10 18:19:39 2008 'test.cpp' : 16 : main    :    log message
 [WARNING]   Tue Jun 10 18:19:39 2008 'test.cpp' : 17 : main    :    warn message
 [ERROR]     Tue Jun 10 18:19:39 2008 'test.cpp' : 18 : main    :    error message
 \endverbatim  
 * \section reference �ο�
 * - http://docs.python.org/lib/module-logging.html
 * - http://www.python.org/peps/pep-0282.html
 * - http://www.red-dove.com/python_logging.html
 * - http://torjo.com/log2/index.html
 */

/**
 * \example test.cpp
 *  ��������������������ʹ��tinyLog
 */

/** \file
 * \brief ����ļ����� tinyLog�Ķ���
 * \details tinyLog��һ����־����
 * \author mweiguo@gmail.com
 * \date 2008.06
 * \version v0.1
 */
