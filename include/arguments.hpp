/*
    Connect Four 2014 (c) 2014-2015 George M. Tzoumas

    This file is part of Connect Four 2014.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// argument parser

#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_

#include <iostream>
#include <cstring>
#include <sstream>
#include <ctype.h>
#include <string_view>
#include <stdexcept>

template<class T>
struct Arg
{
    Arg(std::string name_, T& val_, T def_val_ = T(), bool mandatory_ = false):
           name(std::move(name_)), val(val_), def_val(def_val_), mandatory(mandatory_)
    {
        if ( name.empty()) throw std::range_error("argument name should be nonempty");
    }

    bool operator()(std::string_view value_str) const
    {
        if (value_str.empty())
        {
            if (!mandatory)
            {
                val = def_val;
//                std::cerr << "parsed non-mandatory option " << name <<" using default value " << val << std::endl;
                return true;
            }
            std::cerr << "error: option " << name << " missing mandatory value" << std::endl;
            return false;
        }
        if ( *value_str.begin() != '=')
        {
            std::cerr << "error: option " << name
                      << " should be followed by '=' in order to specify value" << std::endl;
            return false;
        }
        std::istringstream is(&value_str[1]);
        is >> val;
//        std::cerr << "parsed option " << name << " data |" << (data+1) << "| as " << val << std::endl;
        if (!is)
        {
            std::cerr << "error: could not parse value of "
                      << (mandatory ? "mandatory " : "") << "option " << name;
            if (!mandatory) std::cerr << ", assuming default = " << def_val;
            std::cerr << std::endl;
            val = def_val;
            return !mandatory;
        }
        return true;
    }

    std::string name;
    T& val;
    T def_val;
    bool mandatory; // could be templatized
};

inline bool parse_argv(int, char **) { return true; }

// TODO: warn about unknown arguments
template<class Arg, class... Args>
bool parse_argv(int argc, char **argv, const Arg& opt, const Args&... opts)
{
    const auto is_separator = [](const char c) {
        return !(isalnum(c) || c == '_' || c == '-');
    };

    bool res = true;
    for (int i = 1; i < argc; ++i)
    {
//        std::cerr << "parsing arg " << i << " = " << argv[i] << std::endl;
        const auto arg_str = std::string_view(argv[i], strlen(argv[i]));
        const auto asz = arg_str.size();
        const auto osz = opt.name.size();
        if (asz < osz)
            continue;

        const bool has_sep = asz > osz && is_separator(arg_str[osz]);
        const auto value_str = has_sep ? arg_str.substr(osz) : std::string_view();
        std::string opt_str = has_sep ? std::string(argv[i],opt.name.size()) : argv[i];
//        std::cerr << "optStr = |" << optStr << "| valueStr = |" << valueStr << "| opt.name = |" << opt.name << "|" << std::endl;
        if (opt.name == opt_str) res = opt(value_str);
    }
    bool res2 = parse_argv(argc, argv, opts...); // avoid short-circuit
    return res && res2;
}

#endif // _ARGUMENTS_H_

