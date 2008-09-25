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

class tinyLogger
{
 public:
    enum LEVEL {error=0, warning, info, debug, none};
    void level ( LEVEL l ) { _level = l; }
    void destination ( LEVEL lev, const char* dest );
    void destination ( char* dest ) { 
        destination(error,dest);
        destination(warning,dest);
        destination(info,dest);
        destination(debug,dest);
    }
    void format ( char* str );
    void log ( const char* fname, const char* filename, int lineno, LEVEL l, bool showLevel, const char* format, ... );
    void install_msgHandler ( LogMsgHandler fn ) { _logMsgHandler = fn; }
    tinyLogger ();
    ~tinyLogger ();
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
};


#define LOG_INFO( t, x, ... )         t.log ( __FUNCTION__, __FILE__, __LINE__, tinyLogger::info,    true,  x, ##__VA_ARGS__ )
#define LOG_WARNING( t, x, ...)       t.log ( __FUNCTION__, __FILE__, __LINE__, tinyLogger::warning, true,  x, ##__VA_ARGS__ )
#define LOG_ERROR( t, x, ...)         t.log ( __FUNCTION__, __FILE__, __LINE__, tinyLogger::error,   true,  x, ##__VA_ARGS__ )
#define LOG_DEBUG( t, x, ...)         t.log ( __FUNCTION__, __FILE__, __LINE__, tinyLogger::debug,   true,  x, ##__VA_ARGS__ )

// ========================================================================

/** \class tinyLogger tinyLog.h "tinyLog.h"
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
 * \fn void tinyLogger::log ( const char* fname, const char* filename, int lineno, LEVEL l, bool showLevel, const char* format, ... );
 * \brief ���һ����Ϣ 
 * \param fname ��ǰҪ�������Ϣ���ڵĺ���������
 * \param filename ��ǰҪ�������Ϣ���ڵ��ļ�������
 * \param lineno ��ǰҪ�������Ϣ���ڵ��к�
 * \param l ��ǰҪ�������Ϣ�ĵȼ�
 * \param showLevel �����Ϣ��Ϣʱ�Ƿ���Ҫ��ʾ�ȼ�����[INFO], [WARNING]...
 * \param format ��Ϣ����ĸ�ʽ,��printf���﷨��ʽһ��
 * \returns ���صĺ���ִ�д��룬����ȥ���� tinyLogger::error_message �������õ������ַ���
 *
 * \fn void tinyLogger::destination ( LEVEL lev, const char* dest );
 * \brief ��������豸
 * \details ��ָ���ĵȼ��趨����豸
 * \param lev LEVEL
 * \param dest ������ stdout, stderr, �ֱ�����׼�������׼���������ַ����ᱻ����Ϊ�ļ���
 *
 * \fn void tinyLogger::destination ( char* dest )
 * \brief ��������豸
 * \details ��ȫ���ĵȼ��趨Ϊͬ��������豸
 *
 * \fn void tinyLogger::format ( char* str );
 * \brief ָ����ʽ���ַ������Ժ����Ϣ���Դ˸�ʽ������� 
 * \param  str %����(keyword)������������������Ҳ�����Ǹ����ֱ�������Ҷ��룬����\%25(asctime) �� \%-25(asctime)�� �ؼ��ֿ�����:
 *  - asctime   ��ǰʱ��
 *  - filename  ��ǰ�ļ���
 *  - lineno    ��ǰ�к�
 *  - funame    ���ں�������
 * 
 * \fn const char* tinyLogger::error_message( int errcode );
 * \brief ���ش��������ַ������� 
 *
 * \fn void tinyLogger::level ( LEVEL l )
 * \brief ָ����ǰlogger����� ��¼�ȼ����ߵȼ�������ʾ�͵ȼ�����Ϣ������������
 *
 * \fn void tinyLogger::install_msgHandler ( LogMsgHandler fn )
 * \brief �����������ע����Ϣ֪ͨ����,��log���е�ʱ�򣬴�ע�ắ�����ᱻ����
 * 
 * fn��ԭ��: typedef void (*LogMsgHandler)( const char* type, const char* msg );
 * type���ᱻ���ó�info, warning, error, debug
 *
 * \enum tinyLogger::LEVEL{info, warning, error, none}
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
 *  ��������������������ʹ��tinyLogger
 */

/** \file
 * \brief ����ļ����� tinyLogger�Ķ���
 * \details tinyLogger��һ����־����
 * \author mweiguo@gmail.com
 * \date 2008.06
 * \version v0.1
 */
