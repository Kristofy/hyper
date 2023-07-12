/**
 * @file prefix_array.hpp
 * 
 * @author Osztopáni Kristóf (kristofosztopani@gmail.com)
 * @brief Prefix array 
 * @details
 *  This is a compile time optimizable prefix array implementation.
 *  It is a single header header only library, so you can just include it in your project.
 *  It is a template class, so you can use it with any type that has a merge operation with a proper NullElement.
 *  The merge operation does not have to be commutative, but it has to be associative.
 *  The NullElement has to be the identity element of the merge operation.
 *  The class also implements array ilke access, so you can index into it and even do an update in the assignment
 * 
 *  As for the time complexity:
 *    range query:    O(1).
 *    single update:  O(n).
 *    construction:   O(n).
 * 
 *    SegmentTree::operator[]: O(1) for access O(n) for update.
 *    Range based for loop:    O(n).
 * 
 * As for the space complexity: it is O(n) more precisely n + 1.
 * 
 * It compiles with c++ versions 11 and above
 * Compiles with all major compilers.
 * @version 0.1
 * @date 2023-07-12
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#ifndef HYPER_PREFIX_ARRAY_HPP
#define HYPER_PREFIX_ARRAY_HPP
#include <iterator>

namespace hyper {
namespace data_structures {

namespace operations {
// TODO it will error when included from segment_tree.hpp
template <typename T> T Add(T a, T b) { return a + b; }
template <typename T> T Sub(T a, T b) { return a - b; }

} // namespace operations

template <typename DataType, DataType (*Merge)(DataType, DataType) = operations::Add<DataType>, DataType (*Unmerge)(DataType, DataType) = operations::Sub<DataType>, DataType NullElement = DataType{}>
class PrefixArray {
public:

  template <typename Iter>
  PrefixArray(Iter begin, Iter end) 
    : size(std::distance(begin, end)), 
      pref(new DataType[size + 1]) { 

    pref[0] = NullElement;
    for (size_t i = 1 ; begin != end; ++begin, ++i) {
      pref[i] = Merge(pref[i - 1], *begin);
    }
  }

  PrefixArray(size_t size) 
    : size(size), 
      pref(new DataType[size + 1]) { 

    for (size_t i = 0; i <= size; ++i) { pref[i] = NullElement; }
  }

  ~SegmentTree() {
    delete pref;
  }

  DataType query(size_t l, size_t r) {
    return Unmerge(pref[r + 1], pref[l]);
  }

  //TODO: Test this
  void update(unsigned pos, DataType val) {
    for (size_t i = pos + 2; i <= size; ++i) {
      auto original = Unmerge(pref[i], pref[i - 1]);
      pref[i - 1] = Merge(pref[i - 1], val);
      val = original;
    }
    pref[size] = Merge(pref[size - 1], val);
  }

// #pragma region Iterators
//   using const_iterator = const DataType*;
//   using const_value_type = const DataType;
//   const_iterator begin() const { return tree + tree_offset; }
//   const_iterator end() const { return tree + tree_offset + size; }
//   const_value_type operator[](size_t pos) const { return tree[pos + tree_offset]; }

//   struct ValueType {
//     ValueType(SegmentTree& st, size_t pos) : st(st), pos(pos) {}
//     ValueType& operator=(const DataType& val) { st.update(pos, val); return *this; }
//     operator const_value_type() const { return const_cast<const SegmentTree&>(st)[pos]; }
//     private:
//       SegmentTree& st;
//       const size_t pos;
//   };

//   struct Iterator {
//     Iterator(SegmentTree& st) : st(st), pos(0) {}

//     ValueType operator*() const { ValueType(st, pos); }
//     ValueType operator->() const { ValueType(st, pos); }

//     Iterator& operator++() { ++pos; return *this; }
//     Iterator& operator--() { --pos; return *this; }

//     Iterator operator++(int) { Iterator tmp = *this; ++pos; return tmp; }
//     Iterator operator--(int) { Iterator tmp = *this; --pos; return tmp; }

//     Iterator& operator+=(size_t n) { pos += n; return *this; }
//     Iterator& operator-=(size_t n) { pos -= n; return *this; }

//     private:
//       SegmentTree& st;
//       size_t pos;
//   };

//   using iterator = Iterator;
//   using reference = ValueType;

//   iterator begin() { return Iterator(*this); }
//   iterator end() { return Iterator(*this) + size; }
//   reference operator[](size_t pos) { return ValueType(*this, pos); }

// #pragma endregion
private:
  const size_t size;
  DataType* pref;
};

} // namespace data_structures
} // namespace hyper

#endif // HYPER_PREFIX_ARRAY_HPP
