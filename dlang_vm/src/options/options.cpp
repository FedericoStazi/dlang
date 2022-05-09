// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "options.h"

#include <limits>
#include <string>
#include <vector>

#include <boost/program_options/parsers.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

Options::Options(int argc, char** argv)
    : boost::program_options::variables_map() {
  // Optional command line arguments
  boost::program_options::options_description
      optionsDescription("Usage: dlang_vm [options] [<file>]");
  optionsDescription.add_options()
      ("help", "Display this list of options")
      ("verbosity",
          boost::program_options::value<std::string>()
              ->default_value("output"),
          "One of:\n"
            "\t  - quiet:      no output is produced\n"
            "\t  - output:     program output to stdout\n"
            "\t  - time:       timing statistics to stderr\n"
            "\t  - statistics: all statistics to stderr\n"
            "\t  - debug:      output to stdout, debug to stderr")
      ("jit-threshold",
          boost::program_options::value<size_t>()
              ->default_value(0),
          "The value of the threshold for jit compilation")
      ("jit-policy",
          boost::program_options::value<std::string>()
              ->default_value("no"),
          "One of:\n"
            "\t  - no\n"
            "\t  - tracing\n"
            "\t  - individual (group jit)\n"
            "\t  - block (group jit)\n"
            "\t  - function (group jit)")
      ("memory",
          boost::program_options::value<std::string>()
              ->default_value("none"),
          "One of:\n"
            "\t  - none\n"
            "\t  - amortized\n"
            "\t  - mark-and-sweep")
      ("optimizations",
          boost::program_options::value<std::string>()
              ->default_value(""),
          "A list of:\n"
            "\t  - redundant-checks\n"
            "\t  - unused-writes");


  // Required positional argument (bytecode file)
  boost::program_options::positional_options_description
      positionalOptionsDescription;
  positionalOptionsDescription.add("file", 1);

  // Hidden option (required by boost to have "file" as a positional argument)
  boost::program_options::options_description allOptions;
  allOptions.add_options()("file", "");
  allOptions.add(optionsDescription);

  // Convert options description into a string
  std::stringstream ss;
  ss << optionsDescription;
  description_ = ss.str();

  // Parse arguments
  boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv)
          .options(allOptions)
          .positional(positionalOptionsDescription)
          .run(),
      *this);
  boost::program_options::notify(*this);
}

std::string Options::getDescription() {
  return description_;
}
