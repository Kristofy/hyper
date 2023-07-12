/**
 * @file segment_tree.hpp
 * 
 * @author Osztopáni Kristóf (kristofosztopani@gmail.com)
 * @brief Segment tree 
 * @details
 *  This is a compile time optimizable iterative segment tree implementation.
 *  It is a single header header only library, so you can just include it in your project.
 *  It is a template class, so you can use it with any type that has a merge operation with a proper NullElement.
 *  The merge operation does not have to be commutative, but it has to be associative.
 *  The NullElement has to be the identity element of the merge operation.
 *  The class also implements array ilke access, so you can index into it and even do an update in the assignment
 * 
 *  As for the time complexity:
 *    range query:    O(log n).
 *    single update:  O(log n).
 *    construction:   O(n).
 * 
 *    SegmentTree::operator[]: O(1) for access O(log n) for update.
 *    Range based for loop:    O(n).
 * 
 * As for the space complexity: it is O(n) more precisely 2 * 2 ^ ceil(log2(n)).
 * 
 * It compiles with c++ versions 11 and above
 * Compiles with gc and clang but not with msvc (because of the __builtin_clz function).
 * @version 0.1
 * @date 2023-07-12
 * 
 * @copyright Copyright (c) 2023
 * 
*/

#ifndef HYPER_SEGMENT_TREE_HPP
#define HYPER_SEGMENT_TREE_HPP
#include <iterator>

namespace hyper {
namespace data_structures {

namespace operations {

template <typename T> T Add(T a, T b) { return a + b; }

} // namespace operations

template <typename DataType, DataType (*Merge)(DataType, DataType) = operations::Add<DataType>, DataType NullElement = DataType{}>
class SegmentTree {
public:

  template <typename Iter>
  SegmentTree(Iter begin, Iter end) 
    : size(std::distance(begin, end)), 
      tree_offset(1 << (32 - __builtin_clz(size - 1))),
      tree(new DataType[tree_offset * 2]) { 

    for (size_t i = 0 ; begin != end; ++begin, ++i) {
      tree[i + tree_offset] = *begin;
    }

    for (size_t i = tree_offset + size; i < tree_offset * 2; ++i) {
      tree[i] = NullElement;
    }

    for (size_t i = (size + tree_offset - 1) / 2; i > 0; --i) {
      tree[i] = Merge(tree[i << 1], tree[i << 1 | 1]);
    }
  }

  SegmentTree(size_t size) 
    : size(size),
      tree_offset(1 << (32 - __builtin_clz(size - 1))),
      tree(new DataType[tree_offset * 2]) {

    for (size_t i = tree_offset; i < tree_offset * 2; ++i) {
      tree[i] = NullElement;
    }
  }

  ~SegmentTree() {
    delete tree;
  }

  DataType query(size_t l, size_t r) {
    DataType left = NullElement;
    DataType right = NullElement;
    for (l += tree_offset, r += tree_offset; l < r; l >>= 1, r >>= 1) {
      if (l & 1) left = Merge(tree[l++], left);
      if (r & 1) right = Merge(right, tree[--r]);
    }

    return Merge(left, right);
  }

  void update(size_t pos, DataType val) {
    tree[pos += tree_offset] = val;
    while(pos >>= 1) {
      tree[pos] = Merge(tree[pos << 1], tree[pos << 1 | 1]);
    }
  }

#pragma region Iterators
  using const_iterator = const DataType*;
  using const_value_type = const DataType;
  const_iterator begin() const { return tree + tree_offset; }
  const_iterator end() const { return tree + tree_offset + size; }
  const_value_type operator[](size_t pos) const { return tree[pos + tree_offset]; }

  struct ValueType {
    ValueType(SegmentTree& st, size_t pos) : st(st), pos(pos) {}
    ValueType& operator=(const DataType& val) { st.update(pos, val); return *this; }
    operator const_value_type() const { return const_cast<const SegmentTree&>(st)[pos]; }
    private:
      SegmentTree& st;
      const size_t pos;
  };

  struct Iterator {
    Iterator(SegmentTree& st) : st(st), pos(0) {}

    ValueType operator*() const { ValueType(st, pos); }
    ValueType operator->() const { ValueType(st, pos); }

    Iterator& operator++() { ++pos; return *this; }
    Iterator& operator--() { --pos; return *this; }

    Iterator operator++(int) { Iterator tmp = *this; ++pos; return tmp; }
    Iterator operator--(int) { Iterator tmp = *this; --pos; return tmp; }

    Iterator& operator+=(size_t n) { pos += n; return *this; }
    Iterator& operator-=(size_t n) { pos -= n; return *this; }

    private:
      SegmentTree& st;
      size_t pos;
  };

  using iterator = Iterator;
  using reference = ValueType;

  iterator begin() { return Iterator(*this); }
  iterator end() { return Iterator(*this) + size; }
  reference operator[](size_t pos) { return ValueType(*this, pos); }

#pragma endregion
private:
  const size_t size;
  const size_t tree_offset;
  DataType* tree;
};

} // namespace data_structures
} // namespace hyper

#endif // HYPER_SEGMENT_TREE_HPP
