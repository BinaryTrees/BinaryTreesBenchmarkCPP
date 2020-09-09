// Original author: Akira1364
// Year: 2020
// License: MIT

#include "pooledmm.hpp"
#include <iostream>

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

  static inline TNode* make_tree(const int32_t depth, TNodePool* const mp) noexcept {
    TNode* const result = mp->new_item();
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
  const uint8_t max_depth = argc == 2 ? static_cast<const uint8_t>(atoi(argv[1])) : 10;

  // Create and destroy a tree of depth `max_depth + 1`.
  TNode::TNodePool pool;
  // `cout` can't display `uint8_t` directly...
  std::cout << "stretch tree of depth " << static_cast<uint16_t>(max_depth + 1)
            << "\t check: " << TNode::check_node(TNode::make_tree(max_depth + 1, &pool)) << "\n";
  pool.clear();

  // Create a "long lived" tree of depth `max_depth`.
  TNode* const tree = TNode::make_tree(max_depth, &pool);

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
    for (int32_t j = 1; j <= item->iterations; ++j) {
      item->check += TNode::check_node(TNode::make_tree(item->depth, &ipool));
      ipool.clear();
    }
  }

  // Display the results.
  for (uint8_t i = 0; i < high_index; ++i) {
    const TDataRec* const item = &data[i];
    std::cout << item->iterations << "\t trees of depth " << static_cast<uint16_t>(item->depth)
              << "\t check: " << item->check << "\n";
  }

  // Check and destroy the long lived tree.
  std::cout << "long lived tree of depth " << static_cast<uint16_t>(max_depth)
            << "\t check: " << TNode::check_node(tree) << "\n";
  pool.clear();

  return 0;
}
