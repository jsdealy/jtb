namespace JTB {

template <typename... Types> struct Tup;

/* base case <= 01/20/24 11:36:38 */ 
template <> struct Tup<> {};

/* tuple <X, Y, Z> inherits from 
 * tuple <Y, Z>, etc. <= 01/20/24 11:36:23 */ 
template <typename T, typename... Types>
struct Tup<T, Types...> : Tup<Types...> {
    T item;
    typedef Tup<Types...> Inherited;

    Tup(T t, Types... pack): item {t}, Tup<Types...>(pack...) { };

    T yield() { return item; }
    /* by casting *this to the base type,
     * we expose the otherwise shadowed 
     * item in the base struct <= 01/20/24 11:39:14 */ 
    Inherited& right() { return (*this); }
};

/* using static functions inside otherwise
 * empty structs is necessary because you cannot
 * partially specialize function templates <= 01/20/24 11:34:09 */ 
template <int N, typename T, typename... Types>
struct Getter;

/* base case for "sideways" recursion <= 01/20/24 11:34:38 */ 
template <typename T, typename... Types>
struct Getter<0, T, Types...> {
    static T top(Tup<T, Types...>& tup) { return tup.yield(); }
};

/* "sideways" recursive definition <= 01/20/24 11:34:48 */ 
template <int N, typename T, typename... Types>
struct Getter {
    static auto top(Tup<T, Types...>& tup) { return Getter<N-1, Types...>::top(tup.right()); }
};

/* user-facing function <= 01/20/24 11:35:01 */ 
template <int N, typename T, typename... Types>
auto get(Tup<T, Types...>& tup) {
    return Getter<N, T, Types...>::top(tup);
}

/* Allows creation of a JTB::Tup without 
 * explicit template parameters. Just
 * do `auto //whatever = JTB::make_tup(//blabla)`.
 * <= 01/20/24 12:01:10 */ 
template <typename... Types>
Tup<Types...> make_tup(Types... args) {
    return Tup<Types...>(args...);
}

}
