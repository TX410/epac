#include "consumer.hpp"

namespace ndn {
namespace epac {

Consumer::Consumer()
    : m_face(m_ioService), m_scheduler(m_ioService) {

  AutoSeededRandomPool rng;
  InvertibleRSAFunction params;

  params.GenerateRandomWithKeySize(rng, 1024);

  publicKey = new RSA::PrivateKey(params);
  privateKey = new RSA::PrivateKey(params);
}

void Consumer::run() {
//  Interest interest(Name("/youtube/testApp/randomData"));
//  interest.setInterestLifetime(time::seconds(1));
//  interest.setMustBeFresh(true);
//
//  m_face.expressInterest(interest,
//                         bind(&Consumer::onData, this, _1, _2),
//                         bind(&Consumer::onNack, this, _1, _2),
//                         bind(&Consumer::onTimeout, this, _1));
//
//  std::cout << "Sending " << interest << std::endl;

  // Schedule a new event
  m_scheduler.scheduleEvent(time::seconds(2),
                            bind(&Consumer::delayedInterest, this));

  // m_ioService.run() will block until all events finished or m_ioService.stop() is called
  m_ioService.run();

  // Alternatively, m_face.processEvents() can also be called.
  // processEvents will block until the requested data received or timeout occurs.
  // m_face.processEvents();
}

void Consumer::onData(const Interest &interest, const Data &data) {

  ResponseType type = DATA;

  if (interest.getName().at(-2).toUri() == "register") {
    type = REGISTER;
  } else if (interest.getName().at(-1).toUri() == "manifest") {
    type = MANIFEST;
  }

  switch (type) {
    case REGISTER:onSubscribe(interest, data);
      return;
    case MANIFEST:onManifest(interest, data);
      return;
    default:break;
  }

  std::cout << data << std::endl;
}

void Consumer::onNack(const Interest &interest, const lp::Nack &nack) {
  std::cout << "received Nack with reason " << nack.getReason()
            << " for interest " << interest << std::endl;
}

void Consumer::onTimeout(const Interest &interest) {
  std::cout << "Timeout " << interest << std::endl;
}

void Consumer::delayedInterest() {
//  std::cout << "One more Interest, delayed by the scheduler" << std::endl;
//
//  Interest interest(Name("/example/testApp/randomData"));
//  interest.setInterestLifetime(time::milliseconds(1000));
//  interest.setMustBeFresh(true);
//
//  m_face.expressInterest(interest,
//                         bind(&Consumer::onData, this, _1, _2),
//                         bind(&Consumer::onNack, this, _1, _2),
//                         bind(&Consumer::onTimeout, this, _1));
//
//  std::cout << "Sending " << interest << std::endl;
  subscribe();
}

void Consumer::subscribe() {

  std::string derpubkey, pubkey;
  StringSink ss(derpubkey);
  publicKey->DEREncodePublicKey(ss);

  StringSource source(derpubkey, true,
                      new HexEncoder(
                          new StringSink(pubkey)
                      )
  );

  Interest interest(Name("/youtube/register/" + pubkey));

  m_face.expressInterest(interest,
                         bind(&Consumer::onData, this, _1, _2),
                         bind(&Consumer::onNack, this, _1, _2),
                         bind(&Consumer::onTimeout, this, _1));
}

void Consumer::onSubscribe(const Interest &interest, const Data &data) {

  std::cout << data << std::endl;
  const Block &block = data.getContent();

  std::string payload = std::string(reinterpret_cast<const char *>(block.value()), block.value_size());

  std::string hexkey = payload.substr(payload.find("key:") + 4, payload.find(";") - payload.find("key:") - 4);
  std::string hexiv = payload.substr(payload.find("iv:") + 3);

  std::cout << "key:" << hexkey << std::endl;
  std::cout << "iv:" << hexiv << std::endl;

  std::string keystring, ivstring;

  StringSource sskey(hexkey, true,
                     new HexDecoder(
                         new StringSink(keystring)
                     )
  );

  StringSource ssiv(hexiv, true,
                    new HexDecoder(
                        new StringSink(ivstring)
                    )
  );

  m_key = new SecByteBlock(reinterpret_cast<const unsigned char *>(keystring.data()), keystring.size());
  m_iv = new SecByteBlock(reinterpret_cast<const unsigned char * >(ivstring.data()), ivstring.size());

  Interest manifestInterest(Name("/youtube/c4f8a2f4ff6ab8290c4019a0c5183e71/192×144/splits/manifest"));
  manifestInterest.setInterestLifetime(time::milliseconds(1000));
  manifestInterest.setMustBeFresh(true);

  m_face.expressInterest(manifestInterest,
                         bind(&Consumer::onData, this, _1, _2),
                         bind(&Consumer::onNack, this, _1, _2),
                         bind(&Consumer::onTimeout, this, _1));
}

void Consumer::onManifest(const Interest &interest, const Data &data) {

  const Block &block = data.getContent();

  std::string payload = std::string(reinterpret_cast<const char *>(block.value()), block.value_size());

  std::cout << payload << std::endl;
}
}
} // namespace ndn