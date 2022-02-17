#ifndef output_hpp
#define output_hpp

#include <stdio.h>

#include <array>
#include <boost/json.hpp>
#include <boost/json/parser.hpp>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

//#include <boost/json/src.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/detail/ostream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using timePoint = std::chrono::time_point<std::chrono::steady_clock>;
namespace pt = boost::property_tree;

struct OutputData {
  std::string _time{}, _countDown{}, _fundingRate{}, _btc{}, _btcPerpetual{},
      _output{}, _space{"; "};
  unsigned long _resolution{1'000'000'000L};
  std::array<std::string, 2> _bidAndOffer{{"", ""}};  // bid, offer of btc
  std::array<std::string, 2> _bidAndOfferPerpetual{
      {"", ""}};  // bid, offer of btc perpetual

  std::string _removableChars{"{}\""};
  timePoint _startTime{std::chrono::high_resolution_clock::now()};
  timePoint _endTime{_startTime};

  std::ofstream _logFile;
  std::string _fileAddress{"/Users/sinamoghimi/Desktop/"};
  std::string _fileName{"example.txt"};

  size_t _notSpamTime{100};  // in millisec

  size_t _logTimeInterval{30},
      _logBufferSize{_logTimeInterval * 1000 / _notSpamTime};  // in seconds
  std::shared_ptr<std::vector<std::string>> _logBuffer{
      std::make_shared<std::vector<std::string>>(_logBufferSize)};
  size_t _logBufferIndex{};

  pt::ptree _root;

  std::string createDate() {
    const std::chrono::time_point<std::chrono::system_clock> now =
        std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream tt;
    tt << std::put_time(std::localtime(&t_c), "%m/%d/%Y %T");
    std::string date{tt.str()};
    std::string nanosec{std::to_string(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count() %
        this->_resolution)};

    nanosec += std::string(9 - nanosec.size(), '0');
    date += "." + nanosec;
    return date;
  }

  void printOutput() {
    system("clear");
    std::cout << this->_output << "\n";
    this->_logBuffer->at(this->_logBufferIndex++) = this->_output;
  }

  void logOutput() {
    this->_endTime = std::chrono::steady_clock::now();
    if ((std::chrono::duration_cast<std::chrono::seconds>(this->_endTime -
                                                          this->_startTime)
             .count()) %
            this->_logTimeInterval ==
        0) {
      this->_logFile.open(this->_fileAddress + this->_fileName,
                          std::ios_base::app);
      for (const std::string& stream : *this->_logBuffer)
        this->_logFile << stream;

      this->_logBufferIndex = (this->_logFile.close(), 0);

      this->_logBuffer =
          std::make_shared<std::vector<std::string>>(this->_logBufferSize);
    }
  }

  void updateInformation(std::istringstream& ss) {
    pt::read_json(ss, this->_root);
    if (this->_root.get<std::string>("stream", "")
            .compare("btcusdt@bookTicker") == 0) {
      _bidAndOffer[0] = this->_root.get<std::string>("data.b", "");
      _bidAndOffer[1] = this->_root.get<std::string>("data.a", "");
    }

    if (_root.get<std::string>("stream", "").compare("btcusdt@markPrice") ==
        0) {
      this->_fundingRate = this->_root.get<std::string>("data.r", "");
    }
  }

  void createOutput() {
    _time = this->createDate();
    this->_btc = this->_bidAndOffer[0] + " " + this->_bidAndOffer[1];
    this->_btcPerpetual =
        this->_bidAndOfferPerpetual[0] + " " + this->_bidAndOfferPerpetual[1];

    this->_output = this->_time + this->_space + this->_btc + "\n";
    //        + this->_space + this->_btcPerpetual + this->_space +
    //        this->_fundingRate + this->_space + this->_countDown;
  }
};

#endif /* output_hpp */
