#include "producer.hpp"

int main(int argc, char **argv) {
  ndn::epac::Producer producer;
  try {
    producer.run();
  } catch (const std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}