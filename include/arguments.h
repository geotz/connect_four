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

#include<iostream>
#include<cstring>
#include<sstream>
#include<ctype.h>

template<class T>
struct Arg {
    Arg(const std::string& aName, T& aVal, T aDefVal = T(), bool aMandatory = false):
           name(aName), val(aVal), def_val(aDefVal), mandatory(aMandatory) { }
    bool operator()(const char *data) const {
        if (*data == 0)
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
        if ( *data != '=')
        {
            std::cerr << "error: option " << name << " should be followed by '=' in order to specify value" << std::endl;
            return false;
        }
        std::istringstream is(data+1);
        is >> val;
//        std::cerr << "parsed option " << name << " data |" << (data+1) << "| as " << val << std::endl;
        if (!is)
        {
            std::cerr << "error: could not parse value of " << (mandatory ? "mandatory " : "") << "option " << name;
            if (!mandatory) std::cerr << ", assuming default = " << def_val;
            std::cerr << std::endl;
            val = def_val;
            return !mandatory;
        }
        return true;
    }
    const std::string name;
    T& val;
    T def_val;
    bool mandatory; // could be templatized
};

// inlined to avoid multiple definitions
inline bool parse_argv(int, char **) { return true; }

// TODO: warn about unknown arguments
template<class A, class... Aargs>
bool parse_argv(int argc, char **argv, const A& opt, const Aargs&... opts)
{
    const auto is_separator = [](const char c) {
        return !(isalnum(c) || c == '_' || c == '-');
    };

    bool res = true;
    for (int i = 1; i < argc; ++i)
    {
//        std::cerr << "parsing arg " << i << " = " << argv[i] << std::endl;
        const size_t len = strlen(argv[i]);
        if (len < opt.name.size()) continue;
        const bool bw = is_separator(argv[i][opt.name.size()]);
        const char *data = bw ? argv[i]+opt.name.size() : argv[i]+len;
        std::string sarg = bw ? std::string(argv[i],opt.name.size()) : argv[i];
//        std::cerr << "sarg = |" << sarg << "| data = |" << data << "| opt.name = |" << opt.name << "|" << std::endl;
        if (opt.name == sarg) res = opt(data);
    }
    bool res2 = parse_argv(argc, argv, opts...); // avoid short-circuit
    return res && res2;
}

#endif // _ARGUMENTS_H_

