#ifndef EPAC_PROCUDER_HPP
#define EPAC_PROCUDER_HPP

#include <iostream>

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

namespace ndn {
namespace epac {

class Producer : noncopyable {

 public:
  void run();

 private:
  void onInterest(const InterestFilter &filter, const Interest &interest);

  void onRegisterFailed(const Name &prefix, const std::string &reason);

  void doRegister(const Interest &interest);

  void generateKey();

 private:
  Face m_face;
  KeyChain m_keyChain;

  SecByteBlock *m_key;
  SecByteBlock *m_iv;
};
} // namespace epac
} // namespace ndn

#endif //EPAC_PROCUDER_HPP
