#include "producer.hpp"

namespace ndn {
namespace epac {

void Producer::run() {

  generateKey();

  m_face.setInterestFilter("/example/testApp",
                           bind(&Producer::onInterest, this, _1, _2),
                           RegisterPrefixSuccessCallback(),
                           bind(&Producer::onRegisterFailed, this, _1, _2));

  m_face.processEvents();
}

void Producer::onInterest(const InterestFilter &filter, const Interest &interest) {
  std::cout << "I:" << interest << std::endl;

  RequestType type = DATA;

  /**
   * for register:
   *        /youtube/register/{pubkey hex}
   * for data:
   *        /youtube/{dataname}
   */
  if (interest.getName().at(-2).toUri() == "register")
    type = REGISTER;

  switch (type) {
    case REGISTER:doSubscribe(interest);
      break;
    case DATA: doData(interest);
      break;
  }
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

void Producer::doSubscribe(const Interest &interest) {

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
void Producer::doData(const Interest &interest) {

  std::string url = interest.getName().getSubName(1).toUri();

  std::string filename;

  decodeUrl(DATADIR + url, filename);

  std::string payload;

  FileSource file(filename.c_str(), true,
                  new StringSink(payload)
  );

  shared_ptr<Data> data = make_shared<Data>();
  data->setName(interest.getName());

  data->setContent(reinterpret_cast<const uint8_t *>(payload.c_str()), payload.size());

  m_keyChain.sign(*data);

  m_face.put(*data);
}

bool Producer::decodeUrl(const std::string &in, std::string &out) {

  out.reserve(in.size());

  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }
  return true;
}
} // namespace epac
} // namespace ndn