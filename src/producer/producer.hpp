#ifndef EPAC_PROCUDER_HPP
#define EPAC_PROCUDER_HPP

#include <iostream>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>

using namespace CryptoPP;

namespace ndn {
namespace epac {

enum RequestType {
  REGISTER,
  DATA
};

class Producer : noncopyable {

 public:
  Producer(const std::string &prefix, const std::string &data_dir);

  void run();

 private:
  void onInterest(const InterestFilter &filter, const Interest &interest);

  void onRegisterFailed(const Name &prefix, const std::string &reason);

  void doSubscribe(const Interest &interest);

  void doData(const Interest &interest);

  void generateKey();

  bool decodeUrl(const std::string &in, std::string &out);

 private:
  Face m_face;
  KeyChain m_keyChain;

  SecByteBlock *m_key;
  SecByteBlock *m_iv;

  const std::string PREFIX;
  const std::string DATADIR;
};
} // namespace epac
} // namespace ndn

#endif //EPAC_PROCUDER_HPP
