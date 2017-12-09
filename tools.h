//
// Created by Andy on 2017/12/7.
//

#ifndef C0_COMPILER_TOOLS_H
#define C0_COMPILER_TOOLS_H

#include "string"
#include "sstream"
namespace tools {
    template < typename T > std::string toString(const T& n )
    {
        std::ostringstream stream ;
        stream << n ;
        return stream.str() ;
    }

    int strToInt(std::string str);
    char strToChar(std::string str);
}

#endif //C0_COMPILER_TOOLS_H
