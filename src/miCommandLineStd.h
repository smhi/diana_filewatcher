/*
  libpuTools - Basic types/algorithms/containers

  Copyright (C) 2006-2013 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


/*
  miCommandLineStd -- a simple class to handle command line arguments to
  programs

  Authors: LBS/JSc 06/1999
  Part of puTools
*/

#ifndef _miCommandLineStd_h
#define _miCommandLineStd_h

#include <string>

#include <vector>
#include <map>
#include <sstream>

class miCommandLineStd {
public:

  struct option {
    char flag;
    std::string alias;
    bool hasArg;

    option()
      : flag('\0'), hasArg(false) {}
    option(const char f, const std::string& a, const bool h)
      : flag(f), alias(a), hasArg(h) {}
  };

private:
  std::ostringstream est;
  std::vector<option> opts;
  std::map<char,std::vector<std::string> > args;
  bool err;

  char aliasToFlag(const std::string&) const;
  bool flagLegal(const char) const;
  bool hasArg(const char) const;

  void requiresArgumentError(const char*, const char*);
  void requiresArgumentError(const char*, const char);

  void illegalOptionError(const char*, const char*);
  void illegalOptionError(const char*, const char);

public:

  miCommandLineStd(const std::vector<option>&, const int, char**);

  std::vector<std::string> arg(const char flag)
  { return args[flag]; }

  std::vector<std::string> arg(const std::string& alias)
  { return args[aliasToFlag(alias)]; }

  bool hasFlag(const char flag) const
  { return args.count(flag)>0? true: false; }

  bool hasFlag(const std::string& alias) const
  { return args.count(aliasToFlag(alias))>0? true: false; }

  bool empty() const
  { return args.empty(); }

  bool error() const
  { return err; }

  std::string errStr() {return est.str();}

};

#endif
