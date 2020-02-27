// Original author: Akira1364
// Year: 2020
// License: MIT

#include "pooledmm.hpp"
#include <cstdio>

struct TDataRec {
  uint8_t depth;
  int32_t iterations;
  int32_t check;
};

struct TNode {
  using TNodePool = TNonFreePooledMemManager<TNode>;
  TNode* left;
  TNode* right;

  static inline int32_t check_node(const TNode* const node) noexcept {
    if (node->right != nullptr && node->left != nullptr)
      return 1 + check_node(node->right) + check_node(node->left);
    return 1;
  }

  static inline TNode* make_tree(const int32_t depth, TNodePool* mp) noexcept {
    auto const result = mp->new_item();
    if (depth > 0) {
      result->right = make_tree(depth - 1, mp);
      result->left = make_tree(depth - 1, mp);
    }
    return result;
  }
};

static TDataRec data[9];

static constexpr uint8_t min_depth = 4;

int main(int argc, char* argv[]) {
  const uint8_t max_depth = argc == 2 ? atoi(argv[1]) : 21;

  // Create and destroy a tree of depth `max_depth + 1`.
  TNode::TNodePool pool;
  // `printf` is often faster than `cout`, so I use it instead for this
  // benchmark just to be on the safe side.
  printf("%s%u%s%d\n", "stretch tree of depth ", max_depth + 1, "\t check: ",
         TNode::check_node(TNode::make_tree(max_depth + 1, &pool)));
  pool.clear();

  // Create a "long lived" tree of depth `max_depth`.
  auto const tree = TNode::make_tree(max_depth, &pool);

  // While the tree stays live, create multiple trees. Local data is stored in
  // the `data` variable.
  const uint8_t high_index = (max_depth - min_depth) / 2 + 1;
  #pragma omp parallel for
  for (uint8_t i = 0; i < high_index; ++i) {
    TDataRec* const item = &data[i];
    item->depth = min_depth + i * 2;
    item->iterations = 1 << (max_depth - i * 2);
    item->check = 0;
    TNode::TNodePool ipool;
    for (int32_t J = 1; J <= item->iterations; ++J) {
      item->check += TNode::check_node(TNode::make_tree(item->depth, &ipool));
      ipool.clear();
    }
  }

  // Display the results.
  for (uint8_t i = 0; i < high_index; ++i) {
    const TDataRec* const item = &data[i];
    printf("%d%s%u%s%d\n", item->iterations, "\t trees of depth ", item->depth, "\t check: ", item->check);
  }

  // Check and destroy the long lived tree.
  printf("%s%u%s%d\n", "long lived tree of depth ", max_depth, "\t check: ", TNode::check_node(tree));
  pool.clear();
}
