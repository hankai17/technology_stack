#include "my_sylar/log.hh"
#include "my_sylar/http/http_server.hh"
#include "my_sylar/iomanager.hh"

// 使用quic分支
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer);
    sylar::IPAddress::ptr addr = sylar::IPv4Address::Create("127.0.0.1", 93);
    //server->bind(addr);
    while (!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    sd->addGlobServlet("/*", [](sylar::http::HttpRequest::ptr req,
            sylar::http::HttpResponse::ptr resp,
            sylar::http::HttpSession::ptr session ) {
        std::string str_flow = req->getHeader("Content-Length", "null");
        if (str_flow != "null") {
            int flow_len = atoi(str_flow.c_str());
            int real_flow_len = req->getBody().length();
            if (flow_len != real_flow_len) {
                SYLAR_LOG_ERROR(g_logger) << "req failed";
            } else {
                SYLAR_LOG_ERROR(g_logger) << "req success";
            }
        }
        resp->setBody("Ok:\r\n");
        return 0;
    });
    server->start();
}

int main() {
    sylar::IOManager iom(1, false, "io");
    iom.schedule(run);
    iom.stop();
    return 0;
}
