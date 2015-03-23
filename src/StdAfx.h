#pragma once

#include <winsock2.h>
//#include <windows.h>
#include <ws2tcpip.h>
#include <conio.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <exception>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <ulterius.h>
#include <ulterius_def.h>

#include "version_config.h"

#include "ultracomm.h"
#include "options.h"
#include "listener.h"
#include "exit_codes.h"

namespace po = boost::program_options;


