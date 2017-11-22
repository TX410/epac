#ifndef EPAC_CONSUMER_HPP
#define EPAC_CONSUMER_HPP

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/scheduler.hpp>

namespace ndn {
namespace epac {

class Consumer : noncopyable {

 public:
  Consumer();

  void run();

 private:
  void onData(const Interest &interest, const Data &data);

  void onNack(const Interest &interest, const lp::Nack &nack);

  void onTimeout(const Interest &interest);

  void delayedInterest();

 private:
  boost::asio::io_service m_ioService;
  Face m_face;
  Scheduler m_scheduler;
};
} // namespace epac
} // namespace ndn
#endif //EPAC_CONSUMER_HPP
