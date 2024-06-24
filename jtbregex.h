#pragma once
#include "jtbvec.h"
#include "jtbstr.h"

/* note: R"(bla)" is a raw string
 * in C++, where 'bla' is the string. */ 

namespace JTB {

struct RegexMatch {
    bool match;
    long startIndex;
    std::size_t endIndex;
    RegexMatch(bool m, long s, std::size_t e): match(m), startIndex(s), endIndex(e) {  };
    std::size_t length() const {
	return endIndex - startIndex;
    }
    operator bool() const {
	return match;
    }
};

class Regex {
    const Str pattern;

    static const Vec<ConstPair<Str, char>> regAtoms;

    static const Vec<Str> regGroups;

    static const Vec<ConstPair<Str, Str>> regPairs;

    static const Vec<Str> regQuants;

    static RegexMatch innerMatch(Regex, Str::const_iterator&, const Str::const_iterator);

public:
    Regex(Str s): pattern {s} {};
    RegexMatch match(const Str&) const;
    constexpr bool validate() const;
    constexpr std::size_t patternSize() const;
    static constexpr bool lowerAlphaCharMatch(const char&);
    static constexpr bool upperAlphaCharMatch(const char&);
    static constexpr bool digitCharMatch(const char&);
    static bool specialCharMatch(Str, Str::const_iterator&, Vec<ConstPair<std::size_t&, int>>);
};

constexpr bool Regex::validate() const {
    if (pattern.slice(0, -1).find("$") != Str::NPOS) { return false; }
    if (pattern.substr(1).find('^') != Str::NPOS ) { return false; }
    return true;
}

bool Regex::specialCharMatch(Str s, Str::const_iterator& index, Vec<ConstPair<std::size_t&, int>> stuffToInc) {
    for (auto& regAtom : regAtoms) {
	if (s.find(regAtom.first()) != Str::NPOS && *index == regAtom.second()) {
	    ++index;
	    for (auto& incpair : stuffToInc) {
		incpair.first() += incpair.second();
	    }
	    return true;
	}
    }
    return false;
}

constexpr std::size_t Regex::patternSize() const {
    std::size_t i = 0;
    std::size_t size = 0;
    std::size_t groupCount = 0;
    while (i < pattern.size()) {
	if (pattern.at(i) == '(' && pattern.findPairCloser('(', ')', i+1) != Str::NPOS) {
	    ++groupCount;
	    ++i;
	}
	else if (pattern.at(i) == '[' && pattern.find(']', i+1)) {
	    i = pattern.find(']', i+1)+1;

	    if (pattern.at(i) == '+') {
		++size;
		++i;
	    }
	    else if (Str("*?").includes(pattern.at(i))) { 
		++i; 
	    }
	    else {
		++size;
	    }
	}
	else {
	    ++size;
	    ++i;
	}
    }
    if (pattern.at(-1) == '$') --size;

    return size-groupCount;
}


const Vec<ConstPair<Str, char>> Regex::regAtoms { 
    { R"(\\)", '\\' }, 
    { R"(\[)", ']' }, 
    { R"(\])", ']' }, 
    { R"(\n)", '\n' }, 
    { R"(\t)", '\t' },
    { R"(\$)", '$' },
    { R"(\^)", '^' }
};

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

RegexMatch Regex::innerMatch(Regex reg, Str::const_iterator& index, const Str::const_iterator end) {
    if (index >= end) throw std::runtime_error("Passed bad bounds to Regex::innerMatch");

    /* using mutable copies <= 02/04/24 10:55:27 */ 
    auto init = index;
    std::size_t patIndex = 0;
    if (reg.pattern.at(0) == '^') ++patIndex;
    std::size_t patEnd = reg.pattern.size();

    while (index < end && patIndex < patEnd) {

	/* handling groups inside (  ) <= 04/06/24 15:08:44 */ 
	if (reg.pattern.at(patIndex) == '(' && reg.pattern.findPairCloser('(', ')', patIndex+1) != Str::NPOS) {
	    auto closeIndex = reg.pattern.findPairCloser('(', ')', patIndex+1);
	    int patIndexAdjust = 0;

	    /* this is what we use to handle 
	     * group postfix operators like + * and ? <= 04/06/24 15:09:41 */ 
	    struct IterLimits {
		long least;
		long most;
	    } iterlimits { 1, 1 };

	    /* setting iterlimits
	     * by looking one past the group <= 02/04/24 16:10:48 */ 
	    if (reg.pattern.at(closeIndex+1) == '+') {
		iterlimits.most = -1;
		++patIndexAdjust;
	    }
	    else if (reg.pattern.at(closeIndex+1) == '*') {
		iterlimits.least = 0;
		iterlimits.most = -1;
		++patIndexAdjust;
	    }
	    else if (reg.pattern.at(closeIndex+1) == '?') {
		iterlimits.least = 0;
		iterlimits.most = 1;
		++patIndexAdjust;
	    }

	    std::size_t groupMatches = 0;

	    /* matching the group pattern in the (  ) 
	     * as many times as are permitted by iterlimits <= 04/06/24 16:27:40 */ 
	    while (iterlimits.most < 0 || groupMatches < iterlimits.most) {
		std::cout << "looping" << '\n';
		/* we need this because each call to 
		 * innerMatch advances index even if it
		 * ultimately doesn't generate a submatch <= 02/08/24 21:07:10 */ 
		auto reset = index;
		if (!innerMatch(reg.pattern.slice(patIndex+1, closeIndex), index, end)) { 
		    index = reset;
		    /* failure; the number of groupmatches is 
		     * less than the least amount required by 
		     * iterlimits <= 04/06/24 16:39:55 */ 
		    if (iterlimits.least > groupMatches) { 
			std::cout << "failed submatch; reg pattern: " << reg.pattern.slice(patIndex+1, closeIndex) << "; index: " << *index << '\n';
			return RegexMatch(false, 0, 0); 
		    }
		    /* success; groupmatches in excess
		     * of the least amount required by iterlimits; 
		     * end the loop <= 04/06/24 18:20:20 */ 
		    else { break; }
		}
		/* match found for the group; 
		 * increment groupMatches and loop again <= 04/06/24 18:20:57 */ 
		else { ++groupMatches; }
	    }
	    /* setting patIndex beyond the group <= 04/06/24 18:21:50 */ 
	    patIndex = closeIndex+patIndexAdjust+1;
	    std::cout << "successful submatch; patIndex at " << reg.pattern.at(patIndex) << "; index: " << *index << '\n';
	}

	
	/* this is where we deal with character groups 
	 * enclosed in [  ] <= 04/06/24 15:21:44 */ 
	if (patIndex < patEnd && reg.pattern.at(patIndex) == '[' && reg.pattern.find(']', patIndex+1) != Str::NPOS) {
	    std::cout << "starting a group" << '\n';

	    /* getting the group content <= 02/04/24 10:35:29 */ 
	    Str group = reg.pattern.slice(patIndex+1, reg.pattern.find(']', patIndex+1));
	    std::cout << "group is: " << group << "END" << '\n';

	    /* moving patIndex to next reg.pattern element
	     * after the group <= 02/04/24 15:43:39 */ 
	    patIndex = reg.pattern.find(']', patIndex+1)+1;

	    /* using this to handle 
	     * operators like + * ? <= 04/06/24 19:11:37 */ 
	    struct IterLimits {
		long least;
		long most;
	    } iterlimits { 1, 1 };

	    /* setting iterlimits
	     * by looking one past the group <= 02/04/24 16:10:48 */ 
	    if (reg.pattern.at(patIndex) == '+') {
		iterlimits.most = -1;
		++patIndex;
	    }
	    else if (reg.pattern.at(patIndex) == '*') {
		iterlimits.least = 0;
		iterlimits.most = -1;
		++patIndex;
	    }
	    else if (reg.pattern.at(patIndex) == '?') {
		iterlimits.least = 0;
		iterlimits.most = 1;
		++patIndex;
	    }

	    std::cout << "iterlimits: " << iterlimits.most << "; " << iterlimits.least << '\n';

	    std::size_t groupMatchCount = 0;

	    while (iterlimits.most < 0 || groupMatchCount < iterlimits.most) {
		std::cout << "char group looping" << '\n';

		/* lowercase alpha <= 02/04/24 10:37:18 */ 
		if (group.find("a-z") != Str::NPOS && Regex::lowerAlphaCharMatch(*index)) {
		    std::cout << "low" << *index << '\n';
		    ++index;
		    ++groupMatchCount;
		}
		else if (specialCharMatch(group, index, { { groupMatchCount, 1 } })) {
		    std::cout << "specialchar" << *index << '\n';
		}
		else if (group.find("A-Z") != Str::NPOS && Regex::upperAlphaCharMatch(*index)) {
		    std::cout << "cap" << *index << '\n';
		    ++index;
		    ++groupMatchCount;
		}
		/* digits <= 02/04/24 11:05:24 */ 
		else if (group.find("0-9") != Str::NPOS && Regex::digitCharMatch(*index)) {
		    std::cout << "dig" << *index << '\n';
		    ++index;
		    ++groupMatchCount;
		}
		/* literals <= 02/04/24 11:22:56 */ 
		else if (group.remove("a-z").remove("A-Z").remove("0-9").includes(*index)) {
		    std::cout << "lit" << *index << '\n';
		    ++index;
		    ++groupMatchCount;
		}
		else {
		    if (groupMatchCount < iterlimits.least) return RegexMatch(false, 0, 0);
		    else break;
		}
	    }
	}

	if (patIndex < patEnd && specialCharMatch(reg.pattern.substr(patIndex, 2), index, { { patIndex, 2 } })) {
	    std::cout << "specialcharlit" << *index << '\n';
	}
	else if (patIndex < patEnd && reg.pattern.at(patIndex) == *index) {
	    std::cout << "zoot" << *index << '\n';
	    ++index;
	    ++patIndex;
	}

	else { return RegexMatch(false, 0, 0); }

    }

    if (index >= end && patIndex == patEnd - 1 && reg.pattern.at(-1) == '$') { return RegexMatch(true, 0, index - init); }
    else if (index >= end && patIndex < patEnd) return RegexMatch(false, 0, 0);
    else if (reg.pattern.at(-1) == '$') { return RegexMatch(false, 0, 0); }

    return RegexMatch(true, 0, index - init);
}

RegexMatch Regex::match(const Str& s) const {
    if (!(*this).validate()) { Str s {"Invalid Regex pattern: "}; s.push(pattern); throw std::runtime_error(s.c_str()); }
    auto stop = s.size() - (*this).patternSize();
    if (stop < 0) return RegexMatch(false, 0, 0);
    if ((*this).pattern.at(0) == '^') { stop = 0; }
    auto beg = s.const_begin();
    auto end = s.const_end();
    for (auto i = 0; i <= stop; i++) {
	auto index = beg+i;
	if (index >= end) { return RegexMatch(false, 0, 0); }
	RegexMatch ret = innerMatch((*this), index, end);
	if (ret) return RegexMatch(ret, i, i + ret.endIndex);
    }

    return RegexMatch(false, 0, 0);
}

std::ostream& operator<<(std::ostream& os, RegexMatch rm) {
    if (rm) {
	os << "match!" << " start: " << rm.startIndex << ", end: " << rm.endIndex << '\n';
    }
    else {
	os << "no match!" << '\n';
    }
    return os;
}



}
