#ifndef web_socket_session_hpp
#define web_socket_session_hpp

#include <array>
#include <boost/beast/core/detail/ostream.hpp>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "output.hpp"

using namespace std::chrono_literals;

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;             // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;        // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;        // from <boost/asio/ip/tcp.hpp>

// Class to connect to a websocket endpoint and receive data continuously
// counterparty closes or stopped manually
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
 private:
  net::io_context& _ioContext;
  ssl::context& _sslContext;
  tcp::resolver _tcpResolver;
  std::shared_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>>
      _webSocketStream;
  beast::flat_buffer _buffer;
  std::string _host;
  std::string _hostAndPort;
  char const* _port;
  std::string _target;
  tcp::endpoint _tcpEndPoint;
  bool _terminate = false;
  OutputData _output;

  size_t _notSpamTime{100};  // in millisec
  std::chrono::duration<unsigned, std::milli> _sleepTime{_notSpamTime};

  void closeConnection() {
    _webSocketStream->close(boost::beast::websocket::close_code::normal);
  }

  void clearBuffer() { _buffer.clear(); }

  void getData() {
    _webSocketStream->read(_buffer);
    if (_buffer.size() > 0) {
      std::string dataString{beast::buffers_to_string(_buffer.data())};
      std::istringstream ss(dataString);
      _output.updateInformation(ss);
      _output.createOutput();
      _output.printOutput();
      _output.logOutput();
    }
    clearBuffer();
  }

  void sleep() { std::this_thread::sleep_for(_sleepTime); }

  void initiateConnection() {
    // Look up the domain name
    auto const tcpResolverResults{_tcpResolver.resolve(_host, _port)};

    // Connect to the ip address we get from the lookup table.
    _tcpEndPoint =
        net::connect(get_lowest_layer(*_webSocketStream), tcpResolverResults);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(
            _webSocketStream->next_layer().native_handle(), _host.c_str()))
      throw beast::system_error(
          beast::error_code(static_cast<int>(::ERR_get_error()),
                            net::error::get_ssl_category()),
          "Failed to set SNI Hostname");

    // Update the _host string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    _hostAndPort = _host + ":" + std::to_string(_tcpEndPoint.port());

    // Perform the SSL handshake
    _webSocketStream->next_layer().handshake(ssl::stream_base::client);

    // Set a decorator to change the User-Agent of the handshake
    _webSocketStream->set_option(
        websocket::stream_base::decorator([](websocket::request_type& req) {
          req.set(http::field::user_agent,
                  std::string(BOOST_BEAST_VERSION_STRING) +
                      " websocket-client-coro");
        }));

    // Perform the websocket handshake
    _webSocketStream->handshake(_hostAndPort, _target);
  }

  void connectAndReceive() {
    initiateConnection();
    // Read Messages
    try {
      while (true)  // expect to read continuously until a connection failure
        ((!_terminate) ? getData() : closeConnection()), sleep();
    } catch (beast::system_error const& se) {
      if (se.code() == websocket::error::closed) {
        std::cout << "socket was closed." << std::endl;
      } else {
        std::cout << "exception: " << se.code() << ", " << se.code().message()
                  << ", " << se.what();
      }
    } catch (std::exception& ex) {
      std::cout << "exception: " << ex.what();
    }
  }

 public:
  // Resolver and socket require an io_context
  explicit WebSocketSession(net::io_context& ioc, ssl::context& ctx,
                            std::string host, char const* port,
                            std::string target)
      : _ioContext{ioc},
        _sslContext{ctx},
        _tcpResolver{_ioContext},
        _host{host},
        _port{port},
        _target{target},
        _hostAndPort{} {}

  void Stop() { _terminate = true; }

  void Start() {
    while (!_terminate) {
      _webSocketStream =
          std::make_shared<websocket::stream<beast::ssl_stream<tcp::socket>>>(
              _ioContext, _sslContext);

      try {
        connectAndReceive();  // initalizes websocket and polls continuously for
                              // messages until socket is closed or an exception
                              // occurs
      } catch (beast::system_error const& se) {
        std::cout << "exception: " << se.code() << ", " << se.code().message()
                  << std::endl;
        std::this_thread::sleep_for(
            100ms);  // just wait a little while to not spam
      }

      _webSocketStream.reset();
    }

    // if we get to here process has been terminated
  }
};

#endif /* web_socket_session_h */
