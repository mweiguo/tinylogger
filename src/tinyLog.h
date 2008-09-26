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
    /** 匹配关键字，关键字包含(levelno, levelname, pathname, filename, lineno, asctime)，如果成功返回关键字的字节数，否则返回0 
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
 * \brief 这是一个使用log的工具
 * \details 具有如下功能
 *  - 设置到不同的输出设备
 *  - 改变日志的文本格式
 *  - 提供日志等级自动过滤功能
 *
 * \todo 
 *  - 如何在c/c++中自动生成函数参数名称和类型   
 *  - 提供状态式记录方式
 *
 * \fn void tinyLog::log ( const char* fname, const char* filename, int lineno, LEVEL l, bool showLevel, const char* format, ... );
 * \brief 输出一个消息 
 * \param fname 当前要输出的消息所在的函数的名称
 * \param filename 当前要输出的消息所在的文件的名称
 * \param lineno 当前要输出的消息所在的行号
 * \param l 当前要输出的消息的等级
 * \param showLevel 输出消息消息时是否需要显示等级，如[INFO], [WARNING]...
 * \param format 消息输出的格式,与printf的语法格式一致
 * \returns 返回的函数执行代码，可以去调用 tinyLog::error_message 函数来得到描述字符串
 *
 * \fn void tinyLog::destination ( LEVEL lev, const char* dest );
 * \brief 设置输出设备
 * \details 对指定的等级设定输出设备
 * \param lev LEVEL
 * \param dest 可以是 stdout, stderr, 分别代表标准输出，标准错误，其他字符串会被解释为文件名
 *
 * \fn void tinyLog::destination ( char* dest )
 * \brief 设置输出设备
 * \details 对全部的等级设定为同样的输出设备
 *
 * \fn void tinyLog::format ( char* str );
 * \brief 指定格式化字符串，以后的信息都以此格式进行输出 
 * \param  str %整数(keyword)，其中整数可以是正也可以是负，分别代表左右对齐，例：\%25(asctime) 或 \%-25(asctime)。 关键字可以是:
 *  - asctime   当前时间
 *  - filename  当前文件名
 *  - lineno    当前行号
 *  - funame    坐在函数名称
 * 
 * \fn const char* tinyLog::error_message( int errcode );
 * \brief 返回错误代码的字符串描述 
 *
 * \fn void tinyLog::level ( LEVEL l )
 * \brief 指定当前logger对象的 记录等级，高等级可以显示低等级的信息，反过来则不行
 *
 * \fn void tinyLog::install_msgHandler ( LogMsgHandler fn )
 * \brief 这个函数用来注册消息通知函数,当log运行的时候，此注册函数将会被调用
 * 
 * fn的原型: typedef void (*LogMsgHandler)( const char* type, const char* msg );
 * type将会被设置成info, warning, error, debug
 *
 * \enum tinyLog::LEVEL{info, warning, error, none}
 * \brief 枚举，所有可以使用的等级 
 *
 */

/** \mainpage 说明手册
 *
 * 本说明分为以下部分
 * - \ref purpers
 * - \ref example
 * - \ref reference
 * - \ref style
 * \section purpers 目标
 * - 能够自动记录文件名称、行号、函数名称、参数名称及当前值
 * - 能够配置输出格式
 * - 能够针对不同的等级设置不同的输出目标（如文件或stdout...）
 * - 具有等级过滤功能
 * \section example 例程
 * \include test.cpp 
 * \section style 预定义的输出样式
 \verbatim  
 Tue Jun 10 18:19:39 2008 'test.cpp' : 16 : main    :    log message
 [WARNING]   Tue Jun 10 18:19:39 2008 'test.cpp' : 17 : main    :    warn message
 [ERROR]     Tue Jun 10 18:19:39 2008 'test.cpp' : 18 : main    :    error message
 \endverbatim  
 * \section reference 参考
 * - http://docs.python.org/lib/module-logging.html
 * - http://www.python.org/peps/pep-0282.html
 * - http://www.red-dove.com/python_logging.html
 * - http://torjo.com/log2/index.html
 */

/**
 * \example test.cpp
 *  下面的例子用来介绍如何使用tinyLog
 */

/** \file
 * \brief 这个文件包括 tinyLog的定义
 * \details tinyLog是一个日志工具
 * \author mweiguo@gmail.com
 * \date 2008.06
 * \version v0.1
 */
