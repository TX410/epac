#include "producer.hpp"

namespace ndn {
namespace epac {

void Producer::run() {
  m_face.setInterestFilter("/example/testApp",
                           bind(&Producer::onInterest, this, _1, _2),
                           RegisterPrefixSuccessCallback(),
                           bind(&Producer::onRegisterFailed, this, _1, _2));

  generateKey();
}

void Producer::onInterest(const InterestFilter &filter, const Interest &interest) {
  std::cout << "I:" << interest << std::endl;

  Name dataName(interest.getName());
  dataName.append("testapp").appendVersion();

  static const std::string content = "HELLO KITTY";

  // Create Data packet
  shared_ptr<Data> data = make_shared<Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(time::seconds(10));
  data->setContent(reinterpret_cast<const uint8_t *>(content.c_str()), content.size());

  // Sign Data packet with default identity
  m_keyChain.sign(*data);
  // m_keyChain.sign(data, <identityName>);
  // m_keyChain.sign(data, <certificate>);

  // Return Data packet to the requester
  std::cout << ">> D: " << *data << std::endl;
  m_face.put(*data);
}

void Producer::onRegisterFailed(const Name &prefix, const std::string &reason) {
  std::cerr << "ERROR: Failed to register prefix \""
            << prefix << "\" in local hub's daemon (" << reason << ")"
            << std::endl;
  m_face.shutdown();
}

void Producer::generateKey() {

  AutoSeededRandomPool rng;

  m_key = new SecByteBlock(AES::DEFAULT_KEYLENGTH);
  rng.GenerateBlock(*m_key, m_key->size());

  m_iv = new SecByteBlock(AES::BLOCKSIZE);
  rng.GenerateBlock(*m_iv, m_iv->size());
}

void Producer::doRegister(const Interest &interest) {

  Name dataName(interest.getName());

  std::string keystring = std::string(reinterpret_cast<const char *>(m_key->data()), m_key->size());
  std::string ivstring = std::string(reinterpret_cast<const char *>(m_iv->data()), m_iv->size());

  std::string hexkey, hexiv;
  StringSource sskey(keystring, true,
                     new HexEncoder(
                         new StringSink(hexkey)
                     )
  );

  StringSource ssiv(ivstring, true,
                    new HexEncoder(
                        new StringSink(hexiv)
                    )
  );

  std::string payload = "key:" + hexkey + ";iv:" + hexiv;

  shared_ptr<Data> data = make_shared<Data>();
  data->setName(dataName);

  data->setContent(reinterpret_cast<const uint8_t *>(payload.c_str()), payload.size());

  m_keyChain.sign(*data);

  m_face.put(*data);
}
} // namespace epac
} // namespace ndn