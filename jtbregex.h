#pragma once
#include "jtbvec.h"
#include "jtbstr.h"



/* note: R"(bla)" is a raw string
 * in C++, where 'bla' is the string. */ 


namespace JTB {

struct RegexMatch {
    const bool match;
    const std::size_t startIndex;
    const std::size_t endIndex;
};


class Regex {
    const Str pattern;

    static const Vec<Str> regAtoms;

    static const Vec<Str> regGroups;

    static const Vec<ConstPair<Str, Str>> regPairs;

    static const Vec<Str> regQuants;

    constexpr RegexMatch innerMatch(Str::const_iterator beg, Str::const_iterator end) const;

public:
    Regex(Str s): pattern {s} {};
    RegexMatch match(const Str&) const;
    constexpr std::size_t patternSize() const;
    static constexpr bool lowerAlphaCharMatch(const char&);
    static constexpr bool upperAlphaCharMatch(const char&);
    static constexpr bool digitCharMatch(const char&);
};

constexpr std::size_t Regex::patternSize() const {
    std::size_t i = 0;
    std::size_t size = 0;
    while (i < pattern.size()) {
	if (pattern.at(i) == '[' && pattern.find(']', i+1)) {
	    ++size;
	    i = pattern.find(']', i+1)+1;
	}
	else {
	    ++size;
	    ++i;
	}
    }

    return size;
}


const Vec<Str> Regex::regAtoms { R"(\\)", R"(\[)", R"(\])", R"(\n)", R"(\t)" };

const Vec<Str> Regex::regGroups  { R"(a-z)", R"(A-Z)", R"(0-9)", R"(\w)", R"(\s)" };

const Vec<ConstPair<Str, Str>> Regex::regPairs { { R"(\()", R"(\))" }, { "{", "}" } };

const Vec<Str> Regex::regQuants { R"(*)", R"(?)", R"(+)", };

constexpr bool Regex::digitCharMatch(const char& c) {
    for (char x = '0'; x <= '9'; x++) {
	if (x == c) {
	    return true;
	}
    }
    return false;
}

constexpr bool Regex::upperAlphaCharMatch(const char& c) {
    for (char x = 'A'; x <= 'Z'; x++) {
	if (x == c) {
	    return true;
	}
    }
    return false;
}

constexpr bool Regex::lowerAlphaCharMatch(const char& c) {
    for (char x = 'a'; x <= 'z'; x++) {
	if (x == c) {
	    return true;
	}
    }
    return false;
}

/* we are not advancing beg or end; if there's no match starting
 * with beg, we simply return false. The caller decides what to do next. <= 02/04/24 09:57:53 */ 
constexpr RegexMatch Regex::innerMatch(const Str::const_iterator beg, const Str::const_iterator end) const {
    if (beg >= end) throw std::runtime_error("Passed bad bounds to Regex::innerMatch");


    /* using mutable copies <= 02/04/24 10:55:27 */ 
    Str::const_iterator index = beg;
    std::size_t pati = 0;
    std::size_t pate = pattern.size();

    while (index < end && pati < pate) {

	/* handling a group <= 02/04/24 10:23:24 */ 
	if (pattern.at(pati) == '[' && pattern.find(']', pati+1) != Str::NPOS) {

	    /* getting the group content <= 02/04/24 10:35:29 */ 
	    Str group = pattern.substrInBounds("[", "]", 1, 1, Str::Bounds::EXC);

	    /* dealing with lowercase alpha <= 02/04/24 10:37:18 */ 
	    if (group.find("a-z") != Str::NPOS && Regex::lowerAlphaCharMatch(*index)) {
		std::cout << "low" << *index << '\n';
		++index;
	    }
	    /* uppercase alpha <= 02/04/24 11:05:17 */ 
	    else if (group.find("A-Z") != Str::NPOS && Regex::upperAlphaCharMatch(*index)) {
		std::cout << "cap" << *index << '\n';
		++index;
	    }
	    /* digits <= 02/04/24 11:05:24 */ 
	    else if (group.find("0-9") != Str::NPOS && Regex::digitCharMatch(*index)) {
		std::cout << "dig" << *index << '\n';
		++index;
	    }
	    /* literals <= 02/04/24 11:22:56 */ 
	    else if (group.remove("a-z").remove("A-Z").remove("0-9").includes(*index)) {
		std::cout << "lit" << *index << '\n';
		++index;
	    }
	    else {
		return RegexMatch(false, 0, 0);
	    }

	    /* finished with the group, so 
	     * we are advancing pat index past the group
	     * closing bracket <= 02/04/24 11:01:04 */ 
	    pati = pattern.find(']', pati+1)+1;
	}

	/* we don't need else here since we advanced
	 * pati if we entered the previous if block <= 02/04/24 11:39:13 */ 
	if (pattern.at(pati) == *index) {
	    std::cout << "zoot" << *index << '\n';
	    ++index;
	    ++pati;
	}
	else { return RegexMatch(false, 0, 0); }

    }

    if (index >= end && pati < pate) return RegexMatch(false, 0, 0);

    return RegexMatch(true, 0, index - beg);
}

RegexMatch Regex::match(const Str& s) const {
    auto stop = s.size() - (*this).patternSize();
    for (auto i = 0; i <= stop; i++) {
	RegexMatch ret = innerMatch(s.const_begin()+i, s.const_end());
	if (ret.match) return RegexMatch(ret.match, i, i + ret.endIndex);
    }

    return RegexMatch(false, 0, 0);
}

std::ostream& operator<<(std::ostream& os, RegexMatch rm) {
    if (rm.match) {
	os << "match!" << " start: " << rm.startIndex << ", end: " << rm.endIndex << '\n';
    }
    else {
	os << "no match!" << '\n';
    }
    return os;
}



}
