#include "consumer.hpp"

int main(int argc, char **argv) {
  ndn::epac::Consumer consumer;
  try {
    consumer.run();
  } catch (const std::exception &e) {
    std::cerr << "ERROR:" << e.what() << std::endl;
  }

  return 0;
}