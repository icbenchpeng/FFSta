#pragma once

#include <unordered_map>

namespace fsta {

template< typename T, typename Less = std::less<T>, typename Hash = std::hash<T>, typename Eq = std::equal_to<T> >
struct AliasMap {
  typedef typename std::unordered_map<T, T, Hash, Eq> Map;
  void operator()(T const & i0, T const & i1) {
	T j0 = get(i0); T j1 = get(i1); Less less;
	if (less(j0, j1)) set(j1, j0); else set(j0, j1);
  }
  T operator[](T const & i) const { return get(i); }
  struct const_iterator : public Map::const_iterator {
	typedef Map::const_iterator Super;
    const_iterator(Map::const_iterator i, const AliasMap* am) : Map::const_iterator(i), map(am) {}
    typedef std::pair<T, T> value_type;
    const value_type operator*() const {
      value_type ret = Super::operator*();
      ret.second = map->get(ret.second);
      return ret;
    }
  private:
    const AliasMap* map;
  };
  const_iterator begin() const { return const_iterator(table.begin(), this); }
  const_iterator end() const { return const_iterator(table.end(), this); }
  std::vector<T> keys() const {
	std::vector<T> ret;
	for (auto & k : table) { ret.push_back(k.first); }
	std::sort(ret.begin(), ret.end(), Less());
	return ret;
  }
private:
  void set(T const & i0, T const & i1) { table[i0] = i1; }
  T get(T const & i) const {
    auto it = table.find(i);
    if (it == table.end()) return i;
    T ret = get(it->second);
    it->second = ret;
    return ret;
  }
  mutable Map table;
};

} // end namespace fsta
