#pragma once
#include "jtbstr.h"

namespace JTB {

template <typename T>
JTB::Str Vec<T>::join(std::string token) const {
    std::stringstream retstream;
    for (auto elem : content) {
	try {
	    /* we can't really use a string here, since 
	     * there is no standard conversion from an int to a string. 
	     * a stringstream works because ints have a definition for operator<< <= 01/13/24 21:26:43 */ 
	    retstream << elem;
	    retstream << token;
	} catch (std::runtime_error e) {
	    std::string s = "Couldn't write to stringstream. ";
	    s.append(e.what());
	    throw std::runtime_error(s);
	}
    }
    std::string ret = retstream.str();
    for (auto i = 0; i < token.length(); i++) {
	ret.pop_back();
    }
    return ret;
}

template <typename T>
constexpr std::ostream& operator<<(std::ostream& os, const Vec<T>& vec) {
    os << "[ "; 
    auto size = vec.size();
    if (size == 0) {
	os << "]";
	return os;
    }
    T dummy;
    std::string ss;
    const char* ccs;
    Str js;
    const std::string css;
    char* cs;
    const Str cjs;
    char c;
    const char cc = '\0';
    std::string maybeQuote = 
	typeid(dummy) == typeid(ss) 
	|| typeid(dummy) == typeid(js) 
	|| typeid(dummy) == typeid(c) 
	|| typeid(dummy) == typeid(cc) 
	|| typeid(dummy) == typeid(cjs) 
	|| typeid(dummy) == typeid(css) 
	|| typeid(dummy) == typeid(ccs) 
	|| typeid(dummy) == typeid(cs) 
	? "'" : "";
    if (size - 1 > 0) {
	for (auto i = 0; i < size-1; i++) {
	    os << maybeQuote << vec.at(i) << maybeQuote << ", ";
	}
	os << maybeQuote << vec.at(size-1) << maybeQuote << " ]";
    }
    else { 
	os << maybeQuote << vec.at(size-1) << maybeQuote << " ]";
    }
    return os;
} 

}

