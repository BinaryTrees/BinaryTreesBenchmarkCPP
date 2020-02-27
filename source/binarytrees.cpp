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

  static inline int32_t checkNode(const TNode* const node) noexcept {
    if (node->right != nullptr && node->left != nullptr)
      return 1 + checkNode(node->right) + checkNode(node->left);
    return 1;
  }

  static inline TNode* makeTree(const int32_t depth, TNodePool* mp) noexcept {
    auto const result = mp->new_item();
    if (depth > 0) {
      result->right = makeTree(depth - 1, mp);
      result->left = makeTree(depth - 1, mp);
    }
    return result;
  }
};

static TDataRec data[9];

static constexpr uint8_t mindepth = 4;

int main(int argc, char* argv[]) {
  const uint8_t maxdepth = argc == 2 ? atoi(argv[1]) : 21;

  // Create and destroy a tree of depth MaxDepth + 1.
  TNode::TNodePool pool;
  // `printf` is often faster than `cout`, so I use it instead for this
  // benchmark just to be on the safe side.
  printf("%s%u%s%d\n", "stretch tree of depth ", maxdepth + 1, "\t check: ",
         TNode::checkNode(TNode::makeTree(maxdepth + 1, &pool)));
  pool.clear();

  // Create a "long lived" tree of depth MaxDepth.
  auto const tree = TNode::makeTree(maxdepth, &pool);

  // While the tree stays live, create multiple trees. Local data is stored in
  // the "Data" variable.
  const uint8_t highindex = (maxdepth - mindepth) / 2 + 1;
  #pragma omp parallel for
  for (uint8_t i = 0; i < highindex; ++i) {
    TDataRec* const item = &data[i];
    item->depth = mindepth + i * 2;
    item->iterations = 1 << (maxdepth - i * 2);
    item->check = 0;
    TNode::TNodePool ipool;
    for (int32_t J = 1; J <= item->iterations; ++J) {
      item->check += TNode::checkNode(TNode::makeTree(item->depth, &ipool));
      ipool.clear();
    }
  }

  // Display the results.
  for (uint8_t i = 0; i < highindex; ++i) {
    const TDataRec* const item = &data[i];
    printf("%d%s%u%s%d\n", item->iterations, "\t trees of depth ", item->depth, "\t check: ", item->check);
  }

  // Check and destroy the long lived tree.
  printf("%s%u%s%d\n", "long lived tree of depth ", maxdepth, "\t check: ", TNode::checkNode(tree));
  pool.clear();
}
