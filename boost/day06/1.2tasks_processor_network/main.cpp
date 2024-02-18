
// Big part of code for this recipe
// in in this header:
#include "tasks_processor_network.hpp"
using namespace tp_network;

// Part of tasks_processor class from
// tasks_processor_network.hpp, that must be defined
// Somewhere in source file
tasks_processor& tasks_processor::get() {
    static tasks_processor proc;
    return proc;
}

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

class authorizer;
typedef boost::shared_ptr<authorizer> authorizer_ptr;

class authorizer: public boost::enable_shared_from_this<authorizer> {   // 封装一个conn 并设置读写业务回调
    tcp_connection_ptr connection_;
    boost::array<char, 512> message_;
    
    explicit authorizer(const tcp_connection_ptr& connection)
        : connection_(connection)
    {}
    
public:
    static void on_connection_accpet(const tcp_connection_ptr& connection) {
        authorizer_ptr auth(new authorizer(connection));
        auth->connection_.async_read(boost::asio::buffer(auth->message_),
            boost::bind(&authorizer::on_data_recieve, auth, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred),
            1
        );
    }

    void on_data_recieve(const boost::system::error_code& error, std::size_t bytes_recieved) {
        if (error) {
            std::cerr << "authorizer.on_data_recieve: error during recieving response: " << error << '\n';
            assert(false);
        }
        if (bytes_recieved == 0) {
            std::cerr << "authorizer.on_data_recieve: zero bytes recieved\n";
            assert(false);
        }
        message_[0] = 'O';
        message_[1] = 'K';
        std::size_t bytes_to_send = 2;
        connection_.async_write(boost::asio::buffer(message_, bytes_to_send),
            boost::bind(&authorizer::on_data_send, shared_from_this(), boost::asio::placeholders::error)
        );
    }

    void on_data_send(const boost::system::error_code& error) {
        if (error) {
            std::cerr << "authorizer.on_data_send: error during sending response: " << error << '\n';
            assert(false);
        }
        connection_.shutdown();
    }
};

bool g_authed = false;

void finish_socket_auth_task(const boost::system::error_code& err,
        std::size_t bytes_transfered, const tcp_connection_ptr& soc, const boost::shared_ptr<std::string>& data)
{
    if (err && err != boost::asio::error::eof) {
        std::cerr << "finish_socket_auth_task: Client error on recieve: " << err.message() << '\n';
        assert(false);
    }

    if (bytes_transfered != 2) {
        std::cerr << "finish_socket_auth_task: wrong bytes count\n";
        assert(false);
    }

    data->resize(bytes_transfered);
    if (*data != "OK") {
        std::cerr << "finish_socket_auth_task: wrong response: " << *data << '\n';
        assert(false);
    }

    g_authed = true;
    soc.shutdown();
    tasks_processor::get().stop();
}

void recieve_auth_task(const boost::system::error_code& err, const tcp_connection_ptr& soc, const boost::shared_ptr<std::string>& data)
{
    if (err) {
        std::cerr << "recieve_auth_task: Client error on recieve: " << err.message() << '\n';
        assert(false);
    }

    soc.async_read(boost::asio::buffer(&(*data)[0], data->size()),  // 写成功 设置读回调
        boost::bind(&finish_socket_auth_task, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, soc, data),
        1
    );
}

const unsigned short g_port_num = 65001;

void send_auth_task() {
    tcp_connection_ptr soc = tasks_processor::get().create_connection("127.0.0.1", g_port_num);
    boost::shared_ptr<std::string> data = boost::make_shared<std::string>("auth_name");

    soc.async_write(boost::asio::buffer(*data),
            boost::bind(&recieve_auth_task, boost::asio::placeholders::error, soc, data)
    );
}

int main() {
    tasks_processor::get().run_after(boost::posix_time::seconds(1), &send_auth_task); // 发送auth_name 收到ok后 则stop
    tasks_processor::get().add_listener(g_port_num, &authorizer::on_connection_accpet);
    assert(!g_authed);

    tasks_processor::get().start();
    assert(g_authed);
    return 0;
}

