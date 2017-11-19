#include "consumer.hpp"

namespace ndn {
namespace epac {

Consumer::Consumer()
    : m_face(m_ioService), m_scheduler(m_ioService) {

}

void Consumer::run() {
  Interest interest(Name("/example/testApp/randomData"));
  interest.setInterestLifetime(time::seconds(1));
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&Consumer::onData, this, _1, _2),
                         bind(&Consumer::onNack, this, _1, _2),
                         bind(&Consumer::onTimeout, this, _1));

  std::cout << "Sending " << interest << std::endl;

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
  std::cout << "One more Interest, delayed by the scheduler" << std::endl;

  Interest interest(Name("/example/testApp/randomData"));
  interest.setInterestLifetime(time::milliseconds(1000));
  interest.setMustBeFresh(true);

  m_face.expressInterest(interest,
                         bind(&Consumer::onData, this, _1, _2),
                         bind(&Consumer::onNack, this, _1, _2),
                         bind(&Consumer::onTimeout, this, _1));

  std::cout << "Sending " << interest << std::endl;
}
}
} // namespace ndn