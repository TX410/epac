#ifndef EPAC_CONSUMER_HPP
#define EPAC_CONSUMER_HPP

#include <queue>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>

using namespace CryptoPP;

namespace ndn {
namespace epac {

enum ResponseType {
  REGISTER,
  MANIFEST,
  DATA
};

class Consumer : noncopyable {

 public:
  Consumer(const std::string &prefix, const std::string &data_dir, int frequency);

  void run();

 private:
  void onData(const Interest &interest, const Data &data);

  void onNack(const Interest &interest, const lp::Nack &nack);

  void onTimeout(const Interest &interest);

  void delayedInterest();

  void subscribe();

  void requestData();

  void onManifest(const Interest &interest, const Data &data);

  void onSubscribe(const Interest &interest, const Data &data);

  bool url_decode(const std::string &in, std::string &out);

 private:
  boost::asio::io_service m_ioService;
  Face m_face;
  Scheduler m_scheduler;

  std::queue<std::string> m_request_queue;

  RSA::PublicKey *publicKey;
  RSA::PrivateKey *privateKey;

  SecByteBlock *m_key;
  SecByteBlock *m_iv;

  const std::string PREFIX;
  const std::string DATA_DIR;
  int m_frequency;
};
} // namespace epac
} // namespace ndn
#endif //EPAC_CONSUMER_HPP
