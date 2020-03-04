// Original author: Akira1364
// Year: 2020
// License: MIT

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

template <typename T, const size_t initial_size = 64> struct TNonFreePooledMemManager {
  static_assert(std::is_trivially_default_constructible<T>::value &&
                std::is_trivially_destructible<T>::value,
                "T must be trivially default constructible and trivially destructible!");

private:
  // Disable copying, except for the default constructor.
  TNonFreePooledMemManager(const TNonFreePooledMemManager&);
  TNonFreePooledMemManager& operator=(const TNonFreePooledMemManager&);

  size_t cur_size = initial_size;
  T* cur_item = nullptr;
  T* end_item = nullptr;
  std::vector<T*> items;

public:
  inline TNonFreePooledMemManager() noexcept = default;

  inline ~TNonFreePooledMemManager() noexcept { clear(); }

  inline void clear() noexcept {
    if (items.size() > 0) {
      for (T* item : items)
        std::free(item);
      items.clear();
      cur_size = initial_size;
      cur_item = nullptr;
      end_item = nullptr;
    }
  }

  inline T* new_item() noexcept {
    if (cur_item == end_item) {
      cur_size += cur_size;
      cur_item = static_cast<T*>(std::malloc(cur_size * sizeof(T)));
      items.push_back(cur_item);
      end_item = cur_item;
      end_item += cur_size;
    }
    T* const result = cur_item;
    cur_item += 1;
    std::memset(result, 0, sizeof(T));
    return result;
  }

  using TEnumItemsProc = void (*)(T* const);

  // Note that this enumerates *all allocated* items, i.e. a number
  // which is always greater than both `items.size()` and the number
  // of times that `new_item()` has been called.
  inline void enumerate_items(const TEnumItemsProc proc) noexcept {
    if (items.size() > 0) {
      const size_t count = items.size();
      size_t size = initial_size;
      for (size_t i = 0; i < count; ++i) {
        size += size;
        T* const p = items[i];
        const T* last = p;
        last += size;
        if (i == count - 1)
          last = end_item;
        while (p != last) {
          proc(p);
          p += 1;
        }
      }
    }
  }
};
