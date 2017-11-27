#include "consumer.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;

po::options_description description("Allowed options");

void usage() {
  std::cout << "Usage: producer -p /name/prefix\n" << std::endl;
  std::cout << description << std::endl;
}

int main(int argc, char **argv) {

  description.add_options()
      ("data-dir,d", po::value<std::string>(), "directory to save data")
      ("frequency,f", po::value<int>()->default_value(1), "request frequency")
      ("prefix,p", po::value<std::string>(), "ndn domain prefix")
      ("help,h", "display this help and exit");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, description), vm);
  po::notify(vm);

  if (!vm.count("prefix")) {
    usage();
    return 1;
  }

  std::string data_dir;

  if (vm.count("data-dir")) {
    data_dir = boost::filesystem::canonical(vm["data-dir"].as<std::string>()).string();
  }

  const std::string &prefix = vm["prefix"].as<std::string>();
  int frequency = vm["frequency"].as<int>();

  ndn::epac::Consumer consumer(prefix, data_dir, frequency);

  try {
    consumer.run();
  } catch (const std::exception &e) {
    std::cerr << "ERROR:" << e.what() << std::endl;
  }

  return 0;
}