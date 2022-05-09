// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <string>

#include <boost/program_options/variables_map.hpp>

class Options : public boost::program_options::variables_map {
 public:
  Options(int argc, char** argv);
  std::string getDescription();
 private:
  std::string description_;
};
