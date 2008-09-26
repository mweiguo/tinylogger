#include "tinyLog.h"
#include <stdexcept>
#include <cstdarg>
// ========================================================================

tinyLog::tinyLog () 
{
    _level       = info;
    _showTitle   = false;
    destination ( "stdout" );
    format ( "%25(asctime)'%(filename)' : %(lineno) : %(funame)\t:\t" );
    _logMsgHandler = 0;
}

// ========================================================================

tinyLog::~tinyLog () 
{
    std::map<std::string,std::ofstream*>::iterator end=_outf.end(), pp;
    for ( pp=_outf.begin(); pp!=end; ++pp ) {
       pp->second->close();
       delete pp->second;
    }
}

// ========================================================================

int tinyLog::MatchKeyword ( char* p, FmtElement::KEYWORDTYPE& keyword )
{
    // none, levelno, levelname, pathname, filename, lineno, asctime
    if ( memcmp ( p, "levelno)",   8 ) == 0 ) {
        keyword = FmtElement::levelno;
        return 7;
    } else if ( memcmp ( p, "levelname)", 10 ) == 0 ) {
        keyword = FmtElement::levelname;
        return 9;
    } else if ( memcmp ( p, "pathname)",  9 ) == 0 ) {
        keyword = FmtElement::pathname;
        return 8;
    } else if ( memcmp ( p, "filename)",  9 ) == 0 ) {
        keyword = FmtElement::filename;
        return 8;
    } else if ( memcmp ( p, "lineno)",    7 ) == 0 ) {
        keyword = FmtElement::lineno;
        return 6;
    } else if ( memcmp ( p, "asctime)",   8 ) == 0 ) {
        keyword = FmtElement::asctime;
        return 7;
    } else if ( memcmp ( p, "funame)",   7 ) == 0 ) {
        keyword = FmtElement::funame;
        return 6;
    }
    return 0;
}

// ========================================================================

void tinyLog::format ( char* str )
{
    // clear first, means reformat
    _FmtManager.clear();
    // unsaved buffer
    std::vector<char> usBuffer;
    // state number
    int state = 0, align = 0, space = -1;
    std::stringstream ss;
    FmtElement::KEYWORDTYPE keyword = FmtElement::none;
    for (  char* p=str; ; p++ ) {
        switch ( state ) {
        case 0:
            if ( *p == '%' )         state = 1;
            else                     state = 5;
            usBuffer.push_back ( *p );
            break;
        case 1:
            if ( *p == '+' ){
                state = 2;
                align = 0;
            } else if ( *p == '-' ){
                state = 46;
                align = 1;
            } else if ( isdigit(*p)){
                state = 3;
                ss << *p;
            } else if ( *p == '(' )    state = 4;
            else                       state = 5;
            usBuffer.push_back ( *p );
            break;
        case 2:
            if ( isdigit(*p) ){
                state = 3;
                ss << *p;
            } else if ( *p == '(' )    state = 4;
            else                       state = 5;
            usBuffer.push_back ( *p );
            break;
        case 46:
            if ( isdigit(*p) ){
                state = 3;
                ss << *p;
            } else if ( *p == '(' )    state = 4;
            else                       state = 5;
            usBuffer.push_back ( *p );
            break;
        case 3:
            if ( *p == '(' ){
                state = 4;
                space = atoi ( ss.str().c_str() );
                ss.str("");
            } else if ( isdigit(*p) ) {
                state = 3;
                ss << *p;
            }else                     state = 5;
            usBuffer.push_back ( *p );
            break;
        case 4:
            {
                int offset = MatchKeyword ( p, keyword );
                if ( offset != 0 ){
                    std::copy ( p, p+offset, std::back_inserter(usBuffer));
                    p += offset;
                    state = 44;
                }else {
                    state = 5;
                    usBuffer.push_back ( *p );
                }
            }
            break;
        case 44:  // finish keyword
            {
                // save command first
                _FmtManager.push_back ( FmtElement() );
                FmtElement& ele = _FmtManager.back();
                ele.Keyword = keyword;
                ele.align   = align;
                ele.space   = space;
                // clear keyword, align and space
                keyword = FmtElement::none;
                align   = 0;
                space   = -1;
                ss.str("");
                usBuffer.clear();
                // read next
                if ( *p == '%' )         state = 1;
                else                     state = 5;
                usBuffer.push_back ( *p );
                break;
            }
        case 5:
            {
                // save command first
                for ( std::vector<char>::iterator pp = usBuffer.begin(); pp!=usBuffer.end(); ++pp ) {
                    _FmtManager.push_back ( FmtElement() );
                    FmtElement& ele = _FmtManager.back();
                    ele.Keyword = FmtElement::none;
                    ele.ch = *pp;
                }
                // clear keyword, align and space
                keyword = FmtElement::none;
                align   = 0;
                space   = -1;
                ss.str("");
                usBuffer.clear();
                // read next
                if ( *p == '%' )         state = 1;
                else                     state = 5;
                usBuffer.push_back ( *p );
                break;
            }
        case 45:
            return;
        }
        if ( *p == 0 )                  state = 45;
    }
}

// ========================================================================

void tinyLog::log ( const char* fname, const char* filename, int lineno, tinyLog::LEVEL l, const char* format, ... )
{
    if ( !_out ) throw std::invalid_argument("invalid destination");
    if ( _level==none || l > _level )
        return;

    if ( _showTitle ) {
	char* szMsgTitle[] = {"[ERROR]","[WARNING]","[INFO]","[DEBUG]"};
	(*_out[l]) << std::left << std::setw(12) << szMsgTitle[l];

	for ( std::vector<FmtElement>::iterator pp=_FmtManager.begin(); pp!=_FmtManager.end(); ++pp ) { 
	    switch ( pp->Keyword ) {
	    case FmtElement::none:
		(*_out[l]) << pp->ch;
		break;
	    case FmtElement::levelno:
		break;
	    case FmtElement::levelname:
		break;
	    case FmtElement::pathname:
		break;
	    case FmtElement::filename:
		if ( pp->space != 32767 )   	   (*_out[l]) << std::setw(pp->space);
		if ( pp->align == 0 )       	   (*_out[l]) << std::left;
		else                        	   (*_out[l]) << std::right;
		(*_out[l]) << filename;
		break;
	    case FmtElement::lineno:
		if ( pp->space != 32767 )   	   (*_out[l]) << std::setw(pp->space);
		if ( pp->align == 0 )       	   (*_out[l]) << std::left;
		else                        	   (*_out[l]) << std::right;
		(*_out[l]) << lineno;
		break;
	    case FmtElement::funame:
		if ( pp->space != 32767 )   	   (*_out[l]) << std::setw(pp->space);
		if ( pp->align == 0 )       	   (*_out[l]) << std::left;
		else                        	   (*_out[l]) << std::right;
		(*_out[l]) << fname;
		break;
	    case FmtElement::asctime: 
		{
		    if ( pp->space != 32767 )        (*_out[l]) << std::setw(pp->space);
		    if ( pp->align == 0 )            (*_out[l]) << std::left;
		    else                    	   (*_out[l]) << std::right;
		    time_t tm;
		    time ( &tm );
		    std::string stm = ctime ( &tm );
		    std::string::size_type pos;
		    if ( std::string::npos != (pos=stm.find_last_of ( '\n' )) )
			stm.replace ( pos, pos+1, "" );
		    (*_out[l]) << stm;
		}
		break;
	    }
	}
    }
    char buffer[512] = {0};
    va_list vl;
    va_start ( vl, format );
    vsprintf ( buffer, format, vl );
    va_end ( vl );
    (*_out[l]) << buffer << std::endl;

    if ( _logMsgHandler ) {
        switch (l) {
        case info:    _logMsgHandler ("info", buffer ); break;
        case warning: _logMsgHandler ("warning", buffer ); break;
        case error:   _logMsgHandler ("error", buffer ); break;
        case debug:   _logMsgHandler ("debug", buffer ); break;
        default:          break;
        }
    }
}

// ========================================================================

void tinyLog::destination ( LEVEL l, const char* dest )
{
    if ( l == none )
        return;
    if ( strcmp ( dest, "stdout" )==0 ){
        _out[l] = &std::cout;
    }else if ( strcmp ( dest, "stderr" )==0 ){
        _out[l] = &std::cerr;
    } else {
        std::ofstream o;
        if ( _outf.find ( dest ) == _outf.end() ) {
            _outf[dest] = new std::ofstream(dest);
        }
        _out[l] = _outf[dest];
    }
}

// ========================================================================
