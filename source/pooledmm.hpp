// Original author: Akira1364
// Year: 2020
// License: MIT

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

template <typename T, const size_t initial_size = 32> struct TNonFreePooledMemManager {
  static_assert(std::is_trivially_default_constructible<T>::value &&
                std::is_trivially_destructible<T>::value,
                "T must be trivially default constructible and trivially destructible!");

private:
  // Disable copying, except for the default constructor.
  TNonFreePooledMemManager(const TNonFreePooledMemManager&);
  TNonFreePooledMemManager& operator=(const TNonFreePooledMemManager&);
  
  size_t cur_size = sizeof(T) * initial_size;
  // The only reason these are `uint8_t*` is because C++ annoyingly warns on `void*` arithmetic,
  // despite the fact that every compiler I'm aware of treats it completely identically to
  // `uint8_t*` arithmetic as far as "default increment/decrement size in bytes" (which is 1 byte).
  uint8_t* cur_item = nullptr;
  uint8_t* end_item = nullptr;
  std::vector<void*> items;

public:
  inline TNonFreePooledMemManager() noexcept = default;

  inline ~TNonFreePooledMemManager() noexcept {
    clear();
  }

  inline void clear() noexcept {
    if (items.size() > 0) {
      for (size_t i = 0; i < items.size(); ++i)
        free(items[i]);
      items.clear();
    }
    cur_size = sizeof(T) * initial_size;
    cur_item = nullptr;
    end_item = nullptr;
  }

  inline T* new_item() noexcept {
    if (cur_item == end_item) {
      cur_size += cur_size;
      cur_item = static_cast<uint8_t*>(malloc(cur_size));
      // No explicit cast necessary here, which is pretty
      // inconsistent if you ask me...
      items.push_back(cur_item);
      end_item = cur_item;
      end_item += cur_size;
    }
    T* result = static_cast<T*>(static_cast<void*>(cur_item));
    cur_item += sizeof(T);
    memset(result, 0, sizeof(T));
    return result;
  }

  using TEnumItemsProc = void (*)(T* const);

  // Note that this enumerates *all allocated* items, i.e. a number
  // which is always greater than both `items.size()` and the number
  // of times that `new_item()` has been called.
  inline void enumerate_items(const TEnumItemsProc proc) noexcept {
    if (items.size() > 0) {
      const size_t count = items.size();
      size_t size = sizeof(T) * initial_size;
      for (size_t i = 0; i < count; ++i) {
        size += size;
        uint8_t* p = static_cast<uint8_t*>(items[i]);
        const uint8_t* last = p;
        last += size;
        if (i == count - 1)
          last = end_item;
        while (p != last) {
          proc(static_cast<T* const>(static_cast<void*>(p)));
          p += sizeof(T);
        }
      }
    }
  }
};
