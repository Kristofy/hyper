#pragma once

#include <memory>
#include <functional>

template <typename T, typename U>
concept Function = requires(T fn, U param) {
  { fn(param, param) } -> std::convertible_to<U>;
};

template <typename DataType = int, Function<DataType> auto Merge = std::plus<DataType>{}, DataType NullElement = DataType{}>
class SegmentTree {
public:

  template <typename Iter>
  SegmentTree(Iter begin, Iter end) :size(std::distance(begin, end)), tree_offset(1 << (32 - __builtin_clz(size - 1))) {
    build(begin, end);
  }

  DataType query(DataType l, DataType r) {
    DataType res = NullElement;
    for (l += tree_offset, r += tree_offset; l < r; l >>= 1, r >>= 1) {
      if (l & 1) res = Merge(res, tree[l++]);
      if (r & 1) res = Merge(res, tree[--r]);
    }
    return res;
  }

  void update(int pos, DataType val) {
    for (tree[pos += tree_offset] = val ; pos > 1; pos >>= 1) {
      tree[pos >> 1] = Merge(tree[pos], tree[pos ^ 1]);
    }
  }

private:

  template <typename Iter>
  void build(Iter begin, Iter end) {
    tree = std::make_unique<DataType[]>(tree_offset * 2);
    for (int i = 0 ; begin != end; ++begin, ++i) {
      tree[i + tree_offset] = *begin;
    }

    for (int i = tree_offset + size; i < static_cast<int>(tree_offset) * 2; ++i) {
      tree[i] = NullElement;
    }

    for (int i = (size + tree_offset - 1) / 2; i > 0; --i) {
      tree[i] = Merge(tree[i << 1], tree[i << 1 | 1]);
    }
  }


  std::unique_ptr<DataType[]> tree;
  const std::size_t size;
  const std::size_t tree_offset;
};
