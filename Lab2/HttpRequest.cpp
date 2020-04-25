#include "HttpRequest.h"
#include "mytools.h"

#include <iostream>
#include <cassert>
#include <string>
#include <unistd.h>

using namespace swings;

HttpRequest::HttpRequest(int fd)
    : fd_(fd),
      working_(false),
      timer_(nullptr),
      state_(ExpectRequestLine),
      method_(Invalid),
      version_(Unknown)
{
    assert(fd_ >= 0);
}

HttpRequest::~HttpRequest()
{
    close(fd_);
}

int HttpRequest::read(int* savedErrno)
{
    int ret = inBuff_.readFd(fd_, savedErrno);
    return ret;
}

int HttpRequest::write(int* savedErrno)
{
    int ret = outBuff_.writeFd(fd_, savedErrno);
    return ret;
}

int HttpRequest::parseRequest()
{
    int ok = 1;
    bool hasMore = true;

    while(hasMore) {
    		const char* crlf;
        if(state_ == ExpectRequestLine) {
            // 处理请求行
            const char* crlf = inBuff_.findCRLF();
            if(crlf) {
                ok = __parseRequestLine(inBuff_.peek(), crlf);
                if(ok) {
                    inBuff_.retrieveUntil(crlf + 2);
                    state_ = ExpectHeaders;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        } else if(state_ == ExpectHeaders) {
            // 处理报文头
            crlf = inBuff_.findCRLF();
            //const char* crlf = inBuff_.findCRLF();
            if(crlf) {
                const char* colon = std::find(inBuff_.peek(), crlf, ':');
                if(colon != crlf) {
                    __addHeader(inBuff_.peek(), colon, crlf);
                } else {
                    state_ = ExpectBody;
                    //state_ = GotAll;
                    //hasMore = false;
                }
                inBuff_.retrieveUntil(crlf + 2);
            } else {
                hasMore = false;
            } 
        } else if(state_ == ExpectBody) {
        		std:string tmp=getPath();
        		if(DEBUG)std::cout<<"the path:"<<tmp<<std::endl;
            if(getMethod()=="POST"&&tmp=="./Post_show"){
            	std::string body=inBuff_.retrieveAsString();
            	if(DEBUG)std::cout<<"the body:"<<body<<std::endl;
            	std::string name,id;
            	if(deal(body,name,id)&&ok==1){
            		if(DEBUG)std::cout<<"Name:"<<name<<" ID:"<<id<<std::endl;
            		setnameid(name,id);
            		ok=2;
            	}
            	else ok=3;
            }
            else if(getMethod()=="POST") ok=3;
            state_ = GotAll;
            hasMore = false;
        }
    }

    return ok;
}

bool HttpRequest::__parseRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if(space != end && __setMethod(start, space)) {
        start = space + 1;
        space = std::find(start, end, ' ');
        if(space != end) {
            const char* question = std::find(start, space, '?');
            if(question != space) {
                __setPath(start, question);
                __setQuery(question, space);
            } else {
                __setPath(start, space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
            if(succeed) {
                if(*(end - 1) == '1')
                    __setVersion(HTTP11);
                else if(*(end - 1) == '0')
                    __setVersion(HTTP10);
                else
                    succeed = false;
            } 
        }
    }
    return succeed;
}

bool HttpRequest::__setMethod(const char* start, const char* end)
{
    std::string m(start, end);
    if(m == "GET")
        method_ = Get;
    else if(m == "POST")
        method_ = Post;
    else
        method_ = Invalid;
	method_name_=m;
    return method_ != Invalid;
}

void HttpRequest::__addHeader(const char* start, const char* colon, const char* end)
{
    std::string field(start, colon);
    ++colon;
    while(colon < end && *colon == ' ')
        ++colon;
    std::string value(colon, end);
    while(!value.empty() && value[value.size() - 1] == ' ')
        value.resize(value.size() - 1);

    headers_[field] = value;
}

std::string HttpRequest::getMethod() const
{
    std::string res;
    if(method_ == Get)
        res = "GET";
    else if(method_ == Post)
        res = "POST";
    else 
    	res = method_name_;
    return res;
}

std::string HttpRequest::getHeader(const std::string& field) const
{
    std::string res;
    auto itr = headers_.find(field);
    if(itr != headers_.end())
        res = itr -> second;
    return res;
}

bool HttpRequest::keepAlive() const
{
    std::string connection = getHeader("Connection");
    bool res = connection == "Keep-Alive" || 
               (version_ == HTTP11 && connection != "close");

    return res;
}

void HttpRequest::resetParse()
{
    state_ = ExpectRequestLine; // 报文解析状态
    method_ = Invalid; // HTTP方法
    version_ = Unknown; // HTTP版本
    path_ = ""; // URL路径
    query_ = ""; // URL参数
    headers_.clear(); // 报文头部
}
