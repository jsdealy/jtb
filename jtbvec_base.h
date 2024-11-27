#pragma once
#include <stdexcept>
#include <vector>
#include <sstream>
#include "jtbextras.h"
#include <iostream>
#include <functional>




namespace JTB {

class Str;

template<typename T>
class Vec {
    std::vector<T> content;

    enum class SortStage { INIT, PROC };

    void sortInner(std::function<float(const T&, const T&)> callback, std::size_t start = 0, std::size_t end = 0, SortStage stage = SortStage::INIT) {
	if (stage == SortStage::INIT) { end = content.size(); }
	/* using original start/end important for the recursion <- 06/14/24 20:03:42 */ 
	std::size_t origstart { start };
	std::size_t origend { end };
	if (start >= end) return;
	/* pivot is a T <- 06/14/24 15:03:37 */ 
	T pivot { content.at(end-1) };
	/* swap is an index <- 06/14/24 15:03:43 */ 
	std::size_t swap { start + 1 };
	while (swap <= end-1) {
	    /* if callback is neg, switch start and swap items and advance swap position, start 
	     * does not advance <- 06/14/24 15:08:38 */ 
	    if (callback(content.at(start), pivot) < 0) { 
		T dummy = content.at(swap);
		content.at(swap) = content.at(start);
		swap++;
		content.at(start) = dummy;
	    }
	    else {
		/* callback is nonneg, we advance start <- 06/14/24 15:25:14 */ 
		start++;
		/* if start >= swap, start is on new item (or pivot), advance swap to one past start <- 06/14/24 15:25:38 */ 
		if (start >= swap) swap = start + 1;
		/* if start < swap, start is on an already swapped item, switch it with new item (or pivot) at swap 
		 * and advance swap <- 06/14/24 15:26:41 */ 
		else {
		    T dummy = content.at(swap);
		    content.at(swap) = content.at(start);
		    swap++;
		    content.at(start) = dummy;
		}
	    }
	}

	/* sort the items between 0 and start-1 <- 06/14/24 15:36:58 */ 
	sortInner(callback, origstart, start, SortStage::PROC);
	/* sort the items between start+1 and end-1 <- 06/14/24 15:37:28 */ 
	sortInner(callback, start+1, origend, SortStage::PROC);
    }

public:

    Vec(T& x) { std::vector<T> s(x); content = s; }
    Vec(const T& x) { std::vector<T> s(x); content = s; }
    Vec(std::vector<T>& x): content(x) {};
    Vec(std::vector<T>&& x): content(x) {};
    Vec(const std::vector<T>& x): content(x) {};
    Vec(const std::vector<T>&& x): content(x) {};
    Vec(std::initializer_list<T> list): content(list) {}
    Vec(T&& x) { std::vector<T> s; s.push_back(std::move(x)); content = s; }
    Vec() = default;

    /* Returns the size of the Vec<T>. <= 01/18/24 18:20:35 */ 
    auto size() const { return content.size(); }

    /* cheap iterators <= 02/09/24 19:04:15 */ 
    using const_iterator = typename std::vector<T>::const_iterator;
    const_iterator begin() const { return content.begin(); }
    const_iterator end() const { return content.end(); }
    using iterator = typename std::vector<T>::iterator;
    iterator begin() { return content.begin(); }
    iterator end() { return content.end(); }

    T& operator[](std::size_t i) { if (i >= content.size()) throw std::out_of_range("Indexing out of range."); return content[i]; }

    /* Const access to Vec<T> contents. <= 01/18/24 20:46:17 */ 
    const T& at(std::size_t i) const { return content.at(i); }

    /* Pushes a copy of x onto the back of the Vec<T> <= 01/18/24 18:15:08 */ 
    Vec& push(const T& x) { content.push_back(x); return *this; }

    /* Pushes a copy of x onto the back of the Vec<T>. 
     * Returns the Vec<T>.
     * <= 01/18/24 18:15:21 */ 
    Vec& push(const T&& x) { content.push_back(x); return *this; }

    /* Pops the element at the back of the Vec<T> and returns it. <= 01/18/24 18:15:44 */ 
    T pop() { if (content.size() <= 0) throw std::out_of_range("Called pop on empty Vec."); T x { content.back() }; content.pop_back(); return x; }

    bool operator==(const Vec<T>&) const;

    Vec<T>& operator=(const Vec<T>&);

    template <typename U>
    friend constexpr std::ostream& operator<<(std::ostream&, const Vec<U>&); 

    template <typename U>
    friend constexpr std::istream& operator>>(std::istream&, Vec<U>&); 

    template <typename U>
    friend constexpr std::iostream& operator>>(std::iostream&, Vec<U>&); 

    /* Returns a Vec<X> by calling X (*callback)(T) on 
     * the elements of the Vec<T>. <= 01/18/24 18:26:18 */ 
    template <typename X>
    Vec<X> map(std::function<X(T)> callback) const { 
	Vec<X> ret;
	for (auto& elem : content) {
	    ret.push(callback(elem));
	}
	return ret;
    }

    /* Returns a Vec<T> for which 
     * the passed callback
     * returns true. <= 01/18/24 21:21:39 */ 
    Vec<T> filter(std::function<bool(T)>) const;


    /* Returns an accumulated X from the Vec<T>
     * by passing elements sequentially
     * to the callback. The accumulated
     * results are passed as the first
     * arg. The next element is 
     * passed as the second. start 
     * is the initial value. <= 01/18/24 21:28:07 */ 
    template <typename X>
    X reduce(std::function<X(T,T)> callback, T start) const {
	for (T elem : content){
	    start = callback(start, elem);
	}
	return start;
    } 

    /* Returns an accumulated X built
     * using the elements of the Vec<T>.
     * start is the starting accumulation
     * value, startIndex is the index
     * to begin the accumulation. <= 01/18/24 22:36:29 */ 
    template <typename X>
    X reduce(std::function<X(T,T,std::size_t,Vec<T>)> callback, X start, std::size_t startIndex) const {
	for (auto i = startIndex; i < content.size(); i++) {
	    start = callback(start, content[i], i, *this);
	}
	return start;
    } 

    /* Fill the Vec<T> with val
     * starting at index. <= 01/19/24 09:58:20 */ 
    Vec<T>& fill(T val, std::size_t index = 0) {
	auto size = content.size();
	if (index >= size) throw std::out_of_range("Fill is indexing out of range.");
	for (auto i = index; i < size; i++) {
	    content[i] = val;
	}
	return *this;
    }

    /* Fill the Vec<T> with size 
     * copies of val starting 
     * at index. If size is greater
     * than the length of the Vec<T>,
     * expand it as needed to complete 
     * the fill.
     * * <= 01/19/24 10:53:59 */ 
    Vec<T>& fill(T val, std::size_t size, std::size_t index) {

	if (content.size() < size) {
	    fill(val);
	    while (content.size() < size - 1) {
		content.push(val);
	    }
	}
	else {
	    for (auto i = 0; i < size; i++) {
		content[i] = val;
		content.max_size();
	    }
	}
    }

    template <typename X>
    Vec<JTB::ConstPair<T, X>> zip(Vec<X>& stuffToZip) const {
	Vec<ConstPair<T, X>> ret;
	for (auto i = 0; i < stuffToZip.size() && i < content.size(); i++) {
	    ret.push({content[i], stuffToZip[i]});
	}
	return ret;
    }

    Str join(std::string = " ") const;

    bool every(std::function<bool(T)> callback) const {
	for (auto& elem : content) {
	    if (!callback(elem)) return false;
	}
	return true;
    }
    
    bool includes(const T& x) const {
	for (auto& elem : content) {
	    if (elem == x) return true;
	}
	return false;
    }

    bool any(std::function<bool(T)> callback) const {
	for (auto& elem : content) {
	    if (callback(elem)) return true;
	}
	return false;
    }

    Vec<T>& operator+=(const Vec<T>& rhs) {
	for (auto& elem : rhs) {
	    content.push_back(elem);
	}
	return *this;
    }

    Vec<T>& operator+=(const Vec<T>&& rhs) {
	for (auto& elem : rhs) {
	    content.push_back(elem);
	}
	return *this;
    }

    Vec<T> operator+(const Vec<T>& rhs) const {
	Vec<T> ret = *this;
	for (auto& elem : rhs) {
	    ret.push(elem);
	}
	return ret;
    }

    Vec<T>& operator+=(std::initializer_list<T> rhs) {
	for (auto& elem : rhs) {
	    content.push_back(elem);
	}
	return *this;
    }

    Vec<T>& operator-=(const Vec<T>& rhs) {
	std::vector<T> ret; 
	for (auto& elem : content) {
	    if (!rhs.includes(elem)) {
		ret.push_back(elem);
	    }
	}
	content = ret;
	return *this;
    }

    Vec<T>& operator-=(const Vec<T>&& rhs) {
	std::vector<T> ret; 
	for (auto& elem : content) {
	    if (!rhs.includes(elem)) {
		ret.push_back(elem);
	    }
	}
	content = ret;
	return *this;
    }

    Vec<T>& operator-=(std::initializer_list<T> rhs) {
	std::vector<T> ret; 
	Vec<T> temp {rhs};
	for (auto& elem : content) {
	    if (!temp.includes(elem)) {
		ret.push_back(elem);
	    }
	}
	content = ret;
	return *this;
    }

    Vec<T> operator-(const Vec<T>& rhs) const {
	Vec<T> ret;
	for (auto& elem : content) {
	    if (!rhs.includes(elem)) {
		ret.push(elem);
	    }
	}
	return ret;
    }

    Vec<T> operator-(const Vec<T> rhs) const {
	Vec<T> ret;
	for (auto& elem : content) {
	    if (!rhs.includes(elem)) {
		ret.push(elem);
	    }
	}
	return ret;
    }

    
    void forEach(std::function<void(T)> callback) const {
	for (auto& elem : content) {
	    callback(elem);
	}
    }


    /* Quicksort in place. If callback(a,b) is negative, b 
     * will come before a in the sort. <- 06/14/24 15:40:12 */ 
    void sort(std::function<float(const T&, const T&)> callback) {
	sortInner(callback);
    }



    /**
     * @brief Splices a Vec
     *
     * This method takes a starting index and a count.
     * The count can be positive or negative. If negative
     * it counts back from the end of the Vec. 
     *
     * @param start The starting index.
     * @param count The second integer.
     */
    Vec<T> splice(std::size_t start, int count) const {
	Vec<T> ret;
	std::size_t trueEnd = count < 0 ? content.size() + count : start + count;
	for (auto i = start; i < trueEnd; i++) {
	    ret.push(content.at(i));
	}
	return ret;
    }

    Vec<T>& reverse() {
	for (std::size_t i = 0, j = content.size() - 1; i < j; i++, j--) {
	    T dummy = content[i];
	    content[i] = content[j];
	    content[j] = dummy;
	}
	return *this;
    }

    T& at(std::size_t i) {
	return content.at(i);
    }

    struct FindObject {
	T element;
	std::size_t index;
    };

    FindObject find(std::function<bool(T)> callback) const {
	FindObject ret;
	for (auto i = 0; i < content.size(); i++) {
	    if (callback(content.at(i))) {
		ret.element = content.at(i);
		ret.index = i;
	    }
	}
	return ret;
    }

    T frontPop() {
	if (content.size() <= 1) return content.at(0);
	T ret = content.at(0);
	for (auto i = 0; i < content.size() - 1; i++) {
	    content.at(i) = content.at(i+1);
	}
	return ret;
    }

    Vec<T>& frontPush(T elem) {
	T dummy = content.back();
	content.push_back(dummy);
	for (auto i = content.size()-1; i > 0; i--) {
	    content.at(i) = content.at(i-1);
	}
	content.at(0) = elem;
	return *this;
    }

};

template <typename T>
inline bool Vec<T>::operator==(const Vec<T>& rhs) const {
    if (content.size() != rhs.size()) return false;
    auto size = content.size();
    for (auto i = 0; i < size; i++) {
	if (content.at(i) != rhs.at(i)) return false;
    }
    return true;
}

template <typename T>
inline Vec<T>& Vec<T>::operator=(const Vec<T>& rhs) {
    content.clear();
    auto size = rhs.size();
    for (auto i = 0; i < size; i++) {
	content.push_back(rhs.at(i));
    }
    return *this;
}

/**
* @brief Filters a Vec<T>
*
* This takes a callback returning a bool and taking a T.
* It returns a Vec<T> containing elems of calling 
* Vec<T> for which the callback returns true.
*/
template <typename T>
inline Vec<T> Vec<T>::filter(std::function<bool(T)> callback) const {
    Vec<T> ret;
    for (auto& elem : content) {
	if (callback(elem)) ret.push(elem);
    }
    return ret;
}

}

