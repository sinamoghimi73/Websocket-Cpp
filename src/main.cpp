#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "root_certificates.hpp"
#include "web_socket_session.hpp"

int main() {
  auto host{"stream.binance.com"};
  auto port{"9443"};
  auto target{
      "/stream?streams=btcusdt@bookTicker/btcusdt@markPrice@1s"};  // bitcoin
                                                                   // full order
                                                                   // book
                                                                   // endpoint,
                                                                   // 100ms
                                                                   // updates

  // The io_context is required for all I/O
  net::io_context ioc;

  // The SSL context is required, and holds certificates
  ssl::context ctx{ssl::context::tlsv12_client};
  ctx.set_verify_mode(ssl::verify_none);

  // This holds the root certificate used for verification
  load_root_certificates(ctx);

  // Run the I/O service. The call will return when
  // the socket is closed.
  ioc.run();

  std::string inputStr{"r"};
  std::thread t;

  std::shared_ptr<WebSocketSession> websocketSession;

  while (inputStr != "x") {
    //        std::cout << "Press 'r' to run. Press 's' to stop. Press 'x' to
    //        Exit." << std::endl; std::cin >> inputStr; std::cout <<
    //        websocketSession.use_count() << "\n";

    if (inputStr == "r") {                      // run
      if (websocketSession.use_count() == 0) {  // should not run twice
        t = std::thread([&websocketSession, &ioc, &ctx, host, port, target] {
          websocketSession =
              std::make_shared<WebSocketSession>(ioc, ctx, host, port, target);
          websocketSession->Start();  // within a thread so doesn't block
        });
      }
      std::this_thread::sleep_for(100ms);
    } else if (inputStr == "s" or inputStr == "x") {  // stop & exit
      if (websocketSession.use_count()) {             // running
        websocketSession->Stop();
        t.join();
        t.~thread();
        websocketSession.reset();
      }
    }
  }

  return EXIT_SUCCESS;
}
