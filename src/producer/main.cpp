#include "producer.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace po = boost::program_options;

po::options_description description("Allowed options");

void usage() {
  std::cout << "Usage: producer -d data_dir -p /name/prefix\n" << std::endl;
  std::cout << description << std::endl;
}

int main(int argc, char **argv) {

  description.add_options()
      ("data-dir,d", po::value<std::string>(), "directory that store data")
      ("prefix,p", po::value<std::string>(), "ndn domain prefix")
      ("help,h", "display this help and exit");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, description), vm);
  po::notify(vm);

  if (!vm.count("prefix") || !vm.count("data-dir")) {
    usage();
    return 1;
  }

  std::string prefix = vm["prefix"].as<std::string>();
  std::string data_dir = vm["data-dir"].as<std::string>();

  boost::filesystem::path data_path = boost::filesystem::canonical(data_dir);

  ndn::epac::Producer producer(prefix, data_path.string());

  try {
    producer.run();
  } catch (const std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }

  return 0;
}