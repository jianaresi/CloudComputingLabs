#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <map>
#include <string>

#define CONNECT_TIMEOUT 500 // 非活跃连接500ms断开

namespace swings {

class Buffer;

class HttpResponse {
public:
    static const std::map<int, std::string> statusCode2Message;
    static const std::map<std::string, std::string> suffix2Type;

    HttpResponse(int statusCode, std::string path, bool keepAlive,std::string name,std::string id)
        : statusCode_(statusCode),
          path_(path),
          keepAlive_(keepAlive),
          name_(name),
          id_(id)
    {}

    ~HttpResponse() {}

    Buffer makeResponse();
    void doErrorResponse(Buffer& output, std::string message);
    void doStaticRequest(Buffer& output, long fileSize);
	std::string getname(){return name_;};
    std::string getid(){return id_;};
private:
    std::string __getFileType();

private:
    std::map<std::string, std::string> headers_; // 响应报文头部
    int statusCode_; // 响应状态码
    std::string path_; // 请求资源路径
    std::string name_;
    std::string id_;
    bool keepAlive_; // 长连接
}; // class HttpResponse

} // namespace swings

#endif
