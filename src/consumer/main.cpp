#include "consumer.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

po::options_description description("Allowed options");

void usage() {
  std::cout << "Usage: producer -p /name/prefix\n" << std::endl;
  std::cout << description << std::endl;
}

int main(int argc, char **argv) {

  description.add_options()
      ("prefix,p", po::value<std::string>(), "ndn domain prefix")
      ("help,h", "display this help and exit");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, description), vm);
  po::notify(vm);

  if (!vm.count("prefix")) {
    usage();
    return 1;
  }

  const std::string &prefix = vm["prefix"].as<std::string>();

  ndn::epac::Consumer consumer(prefix);
  
  try {
    consumer.run();
  } catch (const std::exception &e) {
    std::cerr << "ERROR:" << e.what() << std::endl;
  }

  return 0;
}