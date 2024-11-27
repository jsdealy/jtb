#pragma once
#include <cstring>
#include <stdexcept>
#include <string>
#include <iterator>
#include <iostream>
#include <functional>
#include <sstream>
#include <vector>
#include <error.h>
#include "jtbextras.h"
#include "jtbvec_base.h"


namespace JTB {


class Str {
    std::string content;
public:
    class iterator {
	std::string::iterator i;
	constexpr std::string::iterator& expose() { return i; }
    public:
	iterator() = default;
	iterator(std::string::iterator&& input): i(input) {}
	iterator(std::string::iterator& input): i(input) {}
	iterator& operator=(iterator& input) { i = input.expose(); return *this; }
	iterator& operator=(std::string::iterator&& input) { i = input; return *this; }
	iterator operator++(int) { iterator j = *this; ++i; return j; }
	iterator& operator++() { ++i; return *this; }
	constexpr auto operator<=>(iterator& rhs) const { return i <=> rhs.expose(); }
	iterator operator+(int j) const { iterator k = *this; k.expose() += j; return k;}
	iterator operator-(int j) const { iterator k = *this; k.expose() -= j; return k;}
	int operator-(iterator& j) const { return i - j.expose();}
	iterator& operator+=(int j) { i += j; return *this;}
	iterator& operator-=(int j) { i -= j; return *this;}
	char& operator*() { return *i; };
    };

    class const_iterator {
	std::string::const_iterator i;
	constexpr std::string::const_iterator expose() const { return i; }
    public:
	const_iterator() = default;
	const_iterator(std::string::const_iterator&& input): i(input) {}
	const_iterator(std::string::const_iterator& input): i(input) {}
	const_iterator& operator=(const_iterator& input) { i = input.expose(); return *this; }
	const_iterator& operator=(std::string::const_iterator&& input) { i = input; return *this; }
	const_iterator operator++(int) { const_iterator j = *this; ++i; return j; }
	const_iterator& operator++() { ++i; return *this; }
	constexpr auto operator<=>(const const_iterator& rhs) const { return i <=> rhs.expose(); }
	constexpr bool operator==(const const_iterator& rhs) const { return i == rhs.expose(); }
	const_iterator operator+(int j) const { auto k = i + j; return k;}
	const_iterator operator-(int j) const { auto k = i - j; return k;}
	int operator-(const_iterator& j) const { return i - j.expose();}
	int operator-(const const_iterator& j) const { return i - j.expose();}
	const_iterator& operator+=(int j) { i += j; return *this;}
	const_iterator& operator-=(int j) { i -= j; return *this;}
	const char& operator*() { return *i; };
    };

    /* constructors <- 01/12/24 20:15:28 */ 
    Str(const std::string& s): content(s) {}
    Str(const std::string&& s): content(s) {}
    Str(const char* s): content(s) {}
    Str(const char& c) { content.push_back(c); }
    Str(char&& c) { content.push_back(c); }
    Str() = default;

    /* methods returning iterators <- 01/12/24 20:15:08 */ 
    iterator begin() { iterator i = content.begin(); return i; }
    iterator end() { iterator i = content.end(); return i; }
    const_iterator const_begin() const { const_iterator i = std::string::const_iterator(content.begin()); return i; }
    const_iterator const_end() const { const_iterator i = std::string::const_iterator(content.end()); return i; }

    /* Returns a const ref to the char at the front of the string. <- 01/12/24 20:15:22 */ 
    constexpr const char& front() const { return content.front(); }

    /* Returns a const ref to the char at the back of the string. <- 01/18/24 18:55:29 */ 
    constexpr const char& back() const { return content.back(); }

    /* Const range-checked access to the chars. Can be negative. <- 01/20/24 13:35:47 */ 
    constexpr const char& at(int i) const { if (i >= 0) { return content.at(i); } else { return content.at(content.size()+i); } }

    constexpr char& operator[](size_t i) { return content[i]; }

    /* assignment operators <- 01/12/24 20:13:54 */ 
    Str& operator=(const Str& s) { content = s.stdstr(); return *this; }
    Str& operator=(std::string s) { content = s; return *this; }
    Str& operator=(const char* s) { content = s; return *this; }
    

    /* addition/concatenation operators <- 01/12/24 20:14:06 */ 
    Str& operator+=(const Str& s) { content += s.stdstr(); return *this; }
    Str operator+(Str& s) const { return (*this).concat(s); }
    Str operator+(const char* s) const { return (*this).concat(s); }

    /* left-side concatenation <- 06/14/24 11:10:44 */ 
    friend Str operator+(const char*, const Str&); 
    friend Str operator+(const std::string&, const Str&); 

    /* Exposes the inner std::string. <- 01/12/24 20:17:11 */ 
    constexpr const std::string& stdstr() const { return content; }

    /* Exposes the inner c string. <- 01/20/24 13:36:48 */ 
    constexpr const char* c_str() const { return content.c_str(); }

    /* stream writing operator <- 01/12/24 20:17:54 */ 
    friend std::ostream& operator<<(std::ostream&, const JTB::Str&);

    /* stream writing operator <- 01/12/24 20:17:54 */ 
    friend JTB::Str& operator<<(JTB::Str&, const char*);
    friend JTB::Str& operator<<(JTB::Str&, const int&);

    /* stream writing operator <- 01/12/24 20:17:54 */ 
    friend JTB::Str& operator<<(JTB::Str&, const JTB::Str&);

    /* stream writing operator <- 01/12/24 20:17:54 */ 
    friend std::istream& operator>>(std::istream&, JTB::Str&);

    /* stream writing operator <- 01/12/24 20:17:54 */ 
    friend std::iostream& operator>>(std::iostream&, JTB::Str&);

    /* Returns the size of the string. <- 01/20/24 13:37:01 */ 
    constexpr std::size_t size() const { return content.size(); };

    /* comparison operators <- 01/18/24 11:37:29 */ 
    constexpr bool operator==(const Str& s2) const { return content == s2.stdstr(); }
    constexpr bool operator==(const std::string& s2) const { return content == s2; }
    constexpr bool operator==(const char* s2) const { return content == s2; }

    enum { NPOS = std::string::npos };

    enum class Bounds { INC, EXC };

    /* Fun stuff begins */
    
    /* Repeat this string n times. <- 01/18/24 18:31:57 */ 
    Str repeat(int n) { Str s = *this; Str k = s; for (int i = 0; i < n-1; ++i) k += s; return k;}

    /* Returns true if s is a substring of this string. <- 01/18/24 18:32:14 */ 
    bool includes(const Str s) const;

    /* True if the string has size 0. <- 06/14/24 23:07:12 */ 
    constexpr bool isEmpty() const { return (*this).size() == 0; }

    /* Returns true if c is one of the chars in this string. <- 01/18/24 18:32:26 */ 
    constexpr bool includes(const char c) const;

    /* Read everything from an istream and push it onto the end of the string.
     * Returns a reference to the updated string. <- 01/18/24 18:32:26 */ 
    Str& absorb(std::istream& istream);

    /* Read a line from an istream and push it onto the end of the string.
     * Returns a reference to the updated string. <- 01/18/24 18:32:26 */ 
    Str& absorbLine(std::istream& istream);

    /* Trims copies of s from the front and back of the string.
     * Defaults to removing spaces and tabs. <- 01/18/24 18:33:40 */ 
    Str trim(std::string = " \t") const;

    /* Returns true if this string starts with s. <- 01/18/24 18:34:59 */ 
    bool startsWith(Str) const;

    /* Returns a substring starting at start.
     * Length n is optional. <- 01/20/24 13:44:39 */ 
    Str substr(std::size_t, long = NPOS) const;

    /* Returns an all-lowercase version of the string. */ 
    Str lower() const;

    /* Returns an all-uppercase version of the string. */ 
    Str upper() const;

    /* Returns true of the string ends with substring
     * s. <- 01/20/24 13:45:27 */ 
    bool endsWith(Str) const;

    /* Returns the location of the first occurrence
     * in the string of one of the 
     * chars in the argument string. <- 01/20/24 18:25:23 */ 
    std::size_t findFirstCharFromString(Str) const;

    enum class SplitMode { CHUNKINC, CHUNKEXC, LISTINC, LISTEXC };

    enum class __ExtractedSplitMode { CHUNK, LIST };

    /* Split the string into a Vec<Str>. 
     * Two main modes (LIST, CHUNK) with 
     * two submodes (INC, EXC) each.
     * Default SplitMode is LISTEXC.
     * LIST treats each char in the 
     * argument as a splitting pattern.
     * CHUNK treats the entire argument
     * as the splitting pattern. 
     * EXC leaves splitting patterns
     * out of the resulting Vec. 
     * INC includes them as ends of
     * the strings in the Vec.
     * Using "" as the splitting pattern
     * results in a Vec containing
     * the chars in the string.
     * <- 01/20/24 18:29:11 */ 
    Vec<Str> split(Str = "", SplitMode = SplitMode::LISTEXC) const;

    /* Returns the start index of the 
     * first occurrence of s within the 
     * terminal segment of the string 
     * starting at pos.
     * pos can be negative to set
     * from the back. 
     * If s does not occur in the substr
     * determined by pos,
     * then return is Str::NPOS. <- 02/03/24 22:02:09 */ 
    std::size_t find(const Str, int = 0) const;

    constexpr std::size_t find(const char, int = 0) const;

    /* Returns true iff s is found 
     * as a substring. Optional starting index. <- 04/06/24 19:19:51 */ 
    bool boolFind(const Str, int = 0) const;
    /* Returns true iff c is found. Optional starting index. <- 04/06/24 19:19:51 */ 
    constexpr bool boolFind(const char, int = 0) const;

    /* Returns a substring starting at zero
     * and ending at the first occurrence of the 
     * supplied char. Offset is optional. <- 03/23/24 23:20:32 */ 
    Str sliceAtChar(const char, int = 0) const;

    std::size_t count(Str, int = 0) const;

    std::size_t rfind(Str, int = -1 ) const;

    /* Finds the nth occurrence of s starting
     * at pos. n should be 1 if looking for 
     * the first occurrence. Setting n to 0 
     * guarantees a failure (NPOS). <- 02/03/24 23:01:04 */ 
    std::size_t findNth(Str, std::size_t = 0, std::size_t = 0) const;

    std::size_t rfindNth(Str, std::size_t = 0, int = -1) const;

    /* Returns the appropriate index of 
     * close, skipping nested pairs. Search
     * starts at start. <- 02/04/24 20:18:15 */ 
    std::size_t findPairCloser(const Str, const Str, const std::size_t = 0) const;

    /* Finds the nth occurence of open
     * then finds the mth occurrence of close
     * after that point. If m is negative, 
     * then close is looked for from the back
     * of the string. Bounds can be INC or EXC
     * to include open and close in the 
     * returned substring or not. <- 02/03/24 22:59:18 */ 
    Str substrInBounds(Str, Str, int = 1, int = -1, Bounds = Bounds::INC) const;

    /* Finds the first occurence of open and the 
     * first occurence of close. Substr 
     * is in between. Bounds can be INC or EXC.<- 07/12/24 22:32:44 */ 
    Str substrInBounds(Str, Str, Bounds) const;

    /* Returns a slice of the string starting at
     * start and going up to but not including
     * end. end can be negative to count
     * from the back of the string. <- 02/04/24 17:09:09 */ 
    Str slice(long, long) const;

    /* Pops n chars off the back of the 
     * string and returns them (as a string). <- 01/18/24 18:45:22 */ 
    Str pop(std::size_t n = 1); 

    Str& clear();

    /* Pushing s onto the back of the string. <- 01/18/24 18:42:17 */ 
    Str& push(Str s) { content.append(s.stdstr()); return *this; }

    /* Pushing s onto the back of the string. <- 01/18/24 18:42:17 */ 
    Str& push(const char* s) { content.append(s); return *this; }

    /* Pushing size-sized initial seg of s onto the back of the string. <- 01/18/24 18:42:17 */ 
    Str& push(const char* s, std::size_t size) { content.append(s,size); return *this; }

    /* Pushing c onto the back of the string. <- 01/18/24 18:42:17 */ 
    Str& push(const char c) { content.push_back(c); return *this; }

    /* Returns a copy of the string 
     * concatenated with s. <- 01/20/24 22:54:49 */ 
    Str concat(Str s) const { auto str = content; str.append(s.stdstr()); return str; }
    Str concat(const char* s) const { auto str = content; str.append(s); return str; }

    /* Pushing s onto the front of the string. <- 01/20/24 19:42:15 */ 
    Str& frontPush(Str);


    /* Pops n chars off the front
     * of the string and returns them 
     * (as a string). <- 01/20/24 20:38:10 */ 
    Str frontPop(std::size_t n = 1);

    /* Fills from start with n copies of 
     * s. If n == Str::NPOS, then fill 
     * proceeds to and ceases at the 
     *
     * end of the string. Otherwise
     * the fill is determined by the 
     *
     * number of copies and will expand
     * the string to accommodate. <- 01/20/24 22:07:14 */ 
    Str& fill(Str, std::size_t = 0, std::size_t = Str::NPOS);

    /* Pops size chars from the 
     * string starting at start;
     * returns the chars as a string. 
     * If there are less than 
     * size many chars remaining
     * in the string at start,
     * pops the remaining chars
     * and returns them.
     * <- 01/20/24 23:06:12 */ 
    Str popAt(std::size_t, std::size_t = 1);

    /* Inserts s into the string at
     * the provided location, then 
     * returns a reference to the 
     * updated string. <- 01/21/24 12:30:56 */ 
    Str& insert(Str, std::size_t);

    /* Returns a string based on the string 
     * but with all occurrences of s removed. <- 02/04/24 17:10:41 */ 
    Str remove(Str) const;

    void forEach(std::function<void(const char)> callback) const {
	for (std::size_t i = 0; i < content.size(); ++i) {
	    callback(content[i]);
	}
    };

    Str map(std::function<Str(const char)> callback) const {
	Str ret {};
	for (std::size_t i = 0; i < content.size(); ++i) {
	    ret.push(callback(content[i]));
	}
	return ret;
    };

    template <typename X>
    X reduce(std::function<X(X startRet, const char next)> callback, X startRet) const {
	for (std::size_t i = 0; i < content.size(); ++i) {
	    startRet = callback(startRet, content[i]);
	}
	return startRet;
    };

    bool every(std::function<bool(const char)> callback) const {
	for (std::size_t i = 0; i < content.size(); ++i) {
	    if (!callback(content[i])) return false;
	}
	return true;
    };

    bool any(std::function<bool(const char)> callback) const {
	for (std::size_t i = 0; i < content.size(); ++i) {
	    if (callback(content[i])) return true;
	}
	return false;
    };

    Str filter(std::function<bool(const char)> callback) const {
	Str ret {};
	for (std::size_t i = 0; i < content.size(); ++i) {
	    if (callback(content[i])) ret.push(content[i]);
	}
	return ret;
    };

};


inline std::size_t Str::findPairCloser(const Str open, const Str close, std::size_t start) const {
    std::size_t nestCount = 0;
    auto i = start;
    while (nestCount > 0 || content.at(i) != close) {
	if (content.at(i) == open) ++nestCount;
	else if (content.at(i) == close) --nestCount;
	++i;
    }
    return i >= content.size() ? Str::NPOS : i;
}


inline Str Str::slice(long start, long end) const {
    if (start < 0) throw std::out_of_range("JTB::Str::slice error: `start` is less than zero.");
    long size = content.size();
    if (end > size) end = size;
    if (end >= 0 && end <= start) return "";
    if (end < 0 && size + end <= start) return "";
    if (end >= 0) {
	return (*this).substr(start, end-start);
    }
    else {
	return (*this).substr(start, (size+end)-start);
    }
}

inline Str Str::remove(Str s) const {
    auto size = content.size();
    auto ssize = s.size();
    Str ret = "";
    auto start = 0;
    auto sstart = (*this).find(s);
    if (sstart == Str::NPOS) return *this;
    while (start < size && sstart != Str::NPOS) {
	if (sstart != start) ret.push((*this).substr(start, sstart - start));
	start = sstart + ssize;
	sstart = (*this).find(s, sstart + ssize);
    }
    if (start < size) {
	ret.push((*this).substr(start));
    }
    return ret;
}

inline Str& Str::insert(Str s, std::size_t i) {
    Str tail = (*this).substr(i);
    (*this).fill(s, i, 1);
    auto ssize = s.size();
    if (content.size() <= i+ssize) (*this).push(tail);
    else (*this).fill(s, i+ssize, 1);
    return (*this);
}

inline Str Str::popAt(std::size_t start, std::size_t size) {
    Str ret = (*this).substr(start, size);
    (*this) = (*this).substr(0, start).concat(start+size >= content.size() ? "" : (*this).substr(start+size));
    return ret;
}

inline Str& Str::fill(Str s, std::size_t start, std::size_t copies) {
    auto contentsize = content.size();
    if (start >= contentsize) throw std::out_of_range("JTB::Str::fill error. Start is past the string size.");
    auto ssize = s.size();
    if (copies == Str::NPOS) {
	for (std::size_t i = start, j = 0; i < contentsize; j++, i++) {
	    content[i] = s[j%ssize];
	}
    }
    else {
	(*this) = (*this).substr(0, start).push(s.repeat(copies));
    }
    return (*this);
}

inline Str Str::pop(std::size_t n) { 
    auto contentsize = content.size();
    if (n > contentsize) { 
	Str ret = (*this);
	content.clear(); 
	return ret;
    }    
    Str ret = content.substr(contentsize-n); 
    content = content.substr(0, contentsize-n);
    return ret; 
}

inline Str& Str::clear() {
    content.clear();
    return (*this);
}

inline Str Str::frontPop(std::size_t n) {
    auto contentsize = content.size();
    if (n > contentsize) { 
	Str ret = *this;
	content.clear(); 
	return ret;
    }    
    Str ret = (*this).substr(0, n);
    for (auto i = 0; i < contentsize-n; i++) {
	content[i] = (*this).at(i+n);
    }
    (*this).pop(n);
    return ret;
}

inline Str& Str::frontPush(Str s) {
    (*this).push(s);
    int contentsize = content.size();
    int pushsize = s.size();
    for (auto i = contentsize-1; i > pushsize-1; i--) {
	(*this)[i] = (*this).at(i-pushsize);
    }
    for (auto i = pushsize-1; i >= 0; i--) {
	(*this)[i] = s.at(i);
    } 
    return (*this);
}

inline bool Str::boolFind(const Str s, int pos) const {
    return (*this).find(s,pos) != NPOS;
}

inline constexpr bool Str::boolFind(const char s, int pos) const {
    return (*this).find(s,pos) != NPOS;
}

inline std::size_t Str::find(const Str s, int pos) const {
    auto size = content.size();
    if (pos >= 0 && pos >= size) return Str::NPOS;
    if (pos < 0 && (pos * -1) > size) return Str::NPOS;
    auto searchsize = s.size();
    if (searchsize > size) return Str::NPOS;
    for (auto i = pos >= 0 ? pos : size + pos; i <= size - searchsize; i++) {
	auto j = i;
	auto matches = 0;
	while (matches < searchsize && j < size && content[j] == s.at(matches)) { 
	    j++; 
	    matches++;
	}
	if (matches == searchsize) return i;
    }
    return Str::NPOS;
}

inline constexpr std::size_t Str::find(const char c, int pos) const {
    if (content.empty()) return Str::NPOS;
    auto size = content.size();
    if (pos >= 0 && pos >= size) return Str::NPOS;
    if (pos < 0 && (pos * -1) > size) return Str::NPOS;
    for (auto i = pos >= 0 ? pos : size + pos; i < size; i++) {
	if (content[i] == c) return i;
    }
    return Str::NPOS;
}

inline Str Str::sliceAtChar(const char c, int offset) const {
    return (*this).slice(0,(*this).find(c)+offset);
}

inline std::size_t Str::count(const Str s, int pos) const {
    auto size = content.size();
    if (pos >= 0 && pos >= size) return 0;
    if (pos < 0 && (pos * -1) > size) return 0;
    auto searchsize = s.size();
    if (searchsize > size) return 0;
    std::size_t count = 0;
    for (auto i = pos >= 0 ? pos : size + pos; i <= size - searchsize; ++i) {
	auto j = i;
	auto matches = 0;
	while (matches < searchsize && j < size && content[j] == s.at(matches)) { 
	    j++; 
	    matches++;
	}
	if (matches == searchsize) { 
	    ++count; 
	    i += searchsize-1;
	}
    }
    return count;
}

inline std::size_t Str::rfind(Str s, int pos) const {
    auto size = content.size();
    auto searchsize = s.size();
    for (int i = pos < 0 ? size + pos : pos; i >= 0 && i >= searchsize - 1; i--) {
	int j = i;
	int matches = searchsize-1;
	while (matches >= 0 && j >= 0 && content[j] == s[matches]) { 
	    j--; 
	    matches--;
	}
	if (matches < 0) return j+1;
    }
    return Str::NPOS;
}

inline std::size_t Str::findNth(Str s, std::size_t n, std::size_t pos) const {
    if (n <= 0) return Str::NPOS;
    std::size_t count = 0;
    std::size_t ret = 0;
    while (count < n && ret != Str::NPOS) {
	ret = (*this).find(s, pos);
	pos = ret + 1;
	++count;
    }
    return ret;
}

inline std::size_t Str::rfindNth(Str s, std::size_t n, int pos) const {
    if (n <= 0) return Str::NPOS;
    std::size_t count = 0;
    std::size_t ret = 0;
    while (count < n && ret != Str::NPOS) {
	ret = (*this).rfind(s, pos);
	pos = ret - 1;
	count++;
    }
    return ret;
}

inline Str Str::substrInBounds(Str open, Str close, int n, int m, Bounds bounds) const {
    auto openpos = (*this).findNth(open, n);
    if (openpos == Str::NPOS) return "";
    std::size_t openSize {open.size()};
    openpos = openpos + openSize - 1;
    auto closepos = 0;
    if (m > 0) { closepos = (*this).findNth(close, m, openpos+1); }
    else { closepos = (*this).rfindNth(close, m * -1); }
    if (closepos == Str::NPOS || closepos <= openpos) return "";
    if (openpos != Str::NPOS && closepos != Str::NPOS) {
	if (bounds == Bounds::INC) return content.substr(openpos, (closepos+1) - openpos);
	if (bounds == Bounds::EXC) return content.substr(openpos+1, (closepos-1) - openpos);
    }
    return "";
}

inline Str Str::substrInBounds(Str open, Str close, Bounds bounds) const {
    return (*this).substrInBounds(open, close, 1, 1, bounds);
}

inline std::size_t Str::findFirstCharFromString(Str s) const {
    auto size = s.size();
    if (size <= 0) return Str::NPOS;
    std::size_t min = content.find(s.at(0));
    if (size == 1) return min;
    for (auto i = 1; i < size; i++) {
	auto x = content.find(s.at(i));
	min = x < min ? x : min;
    }
    return min;
}

inline Str::Bounds getBound(Str::SplitMode m) {
    if (m == Str::SplitMode::LISTEXC || m == Str::SplitMode::CHUNKEXC) {
	return Str::Bounds::EXC;
    }
    else return Str::Bounds::INC;
}

inline Str::__ExtractedSplitMode getMode(Str::SplitMode m) {
    if (m == Str::SplitMode::LISTINC || m == Str::SplitMode::LISTEXC) {
	return  Str::__ExtractedSplitMode::LIST;
    }
    else return Str::__ExtractedSplitMode::CHUNK;
}

inline Vec<Str> Str::split(Str separator, Str::SplitMode mode) const {
    Vec<Str> ret;
    auto contentsize = content.size();
    if (contentsize == 0) return ret;
    auto sep = separator.stdstr();
    auto sepsize = separator.size();

    if (getMode(mode) == Str::__ExtractedSplitMode::CHUNK && sepsize > contentsize) return ret;

    if (separator == "") {
	for (auto c : content) {
	    ret.push(c);
	}
    }
    else if (sepsize == 1) {
	std::size_t start = 0;
	std::size_t n = content.find(sep);

	while (start < contentsize && n != std::string::npos) {
	    /* pushing substring if it is non-empty <- 01/20/24 18:00:21 */ 
	    auto substrToPush = (*this).substr(start, getBound(mode) == Str::Bounds::EXC ? n-start : (n+1)-start);
	    if (substrToPush.size() > 0) ret.push(substrToPush);
	    start = n+1;
	    n = content.find(sep, n+1);
	}

	if (start < contentsize) {
	    ret.push((*this).substr(start));
	}
    }
    else {
	std::size_t start = 0;
	std::size_t n = 0;

	if (getMode(mode) == Str::__ExtractedSplitMode::CHUNK) { 
	    n = content.find(sep); 
	}
	else if (getMode(mode) == Str::__ExtractedSplitMode::LIST){ 
	    n = (*this).findFirstCharFromString(sep);
	}

	while (start < contentsize && n != Str::NPOS) {
	    auto inc = getMode(mode) == Str::__ExtractedSplitMode::LIST ? 1 : sepsize;

	    /* pushing substring into vec <- 01/20/24 17:13:35 */ 
	    auto substrToPush = (*this).substr(start, getBound(mode) == Str::Bounds::EXC ? n-start : (n+inc)-start);
	    if (substrToPush.size() > 0) ret.push(substrToPush);

	    /* setting next startpoint <- 01/20/24 17:20:16 */ 
	    start = n+inc;

	    /* checking bounds <- 01/20/24 17:20:23 */ 
	    if (n+inc >= contentsize) break;

	    /* finding next splitpoint <- 01/20/24 17:13:42 */ 
	    if (getMode(mode) == Str::__ExtractedSplitMode::CHUNK) { 
		n = (*this).find(sep, n+inc); 
	    }
	    else if (getMode(mode) == Str::__ExtractedSplitMode::LIST) {
		auto substrpos = (*this).substr(n+inc).findFirstCharFromString(sep);
		if (substrpos == Str::NPOS) break;
		n = n+inc+substrpos;
	    }

	}

	if (start < contentsize) {
	    ret.push((*this).substr(start));
	}
    }

    return ret;
}

inline Str Str::lower() const {
    std::string ret; 
    for (auto c : content) {
	ret.push_back(tolower(c));
    }
    return ret;
}

inline Str Str::upper() const {
    std::string ret; 
    for (auto c : content) {
	ret.push_back(toupper(c));
    }
    return ret;
}

inline Str Str::substr(std::size_t start, long n) const {
    if (n == Str::NPOS) {
	if (start > content.size()-1) throw std::out_of_range("JTB::Str::substr error. Start position out of range.");
	return content.substr(start);
    }
    else if (n < 0) {
	int size = content.size()+n-start;
	if (size < 0) throw std::out_of_range("JTB::Str::substr error. Bad use of negative n.");
	else return content.substr(start, size);
    }
    else  {
	if (start > content.size()-1) throw std::out_of_range("JTB::Str::substr error. Start position out of range.");
	return content.substr(start, n);
    }
}

inline bool Str::endsWith(Str s) const {
    if (s.size() > content.size()) return false;
    if (content.substr(content.size()-s.size()) == s.stdstr()) return true;
    return false;
}

inline bool Str::startsWith(Str s) const {
    if (content.substr(0, s.size()) == s.stdstr()) {
	return true;
    } 
    return false;
}

inline Str operator+(const char* c_str, const Str& s) {
    return Str(c_str).concat(s);
} 

inline Str operator+(const std::string& stdstr, const Str& s) {
    return Str(stdstr).concat(s);
} 

inline std::ostream& operator<<(std::ostream& os, const JTB::Str& s) {
    os << s.stdstr(); return os;
}

inline JTB::Str& operator<<(JTB::Str& s, const JTB::Str& rhs) {
    s.push(rhs); return s;
}

inline JTB::Str& operator<<(JTB::Str& s, const char* cstr) {
    s.push(cstr); return s;
}

inline JTB::Str& operator<<(JTB::Str& s, const int& i) {
    s.push(std::to_string(i)); return s;
}

inline std::iostream& operator>>(std::iostream& is, JTB::Str& s) {
    #ifdef READ_LINE_TO_JTB_STR
    std::getline(is, s.content);
    #else
    is >> s.content; 
    #endif
    return is;
}

inline std::istream& operator>>(std::istream& is, JTB::Str& s) {
    is >> s.content; return is;
}

inline bool Str::includes(const Str s) const {
    return content.find(s.stdstr()) != std::string::npos; 
}

inline Str& Str::absorb(std::istream& istream) {
    std::string linebuffer {};
    while (istream.good()) {
	std::getline(istream, linebuffer);
	(*this).push(linebuffer);
    }
    return (*this);
}

inline Str& Str::absorbLine(std::istream& istream) {
    std::string linebuffer {};
    std::getline(istream, linebuffer);
    (*this).push(linebuffer);
    return (*this);
}

inline constexpr bool Str::includes(const char c) const {
    return content.find(c) != std::string::npos; 
}

inline Str Str::trim(std::string stuffToTrim) const { 
    std::string::size_type front = 0;
    std::string::size_type back = content.size();
    while (stuffToTrim.find(content.at(front)) != std::string::npos) front++;
    while (stuffToTrim.find(content.at(back-1)) != std::string::npos) back--;
    return content.substr(front, back - front);
}

}
