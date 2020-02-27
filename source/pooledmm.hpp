#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>

template <typename T> class TNonFreePooledMemManager {
public:
  using TPointerList = std::vector<void*>;
  using TEnumItemsProc = void (*)(T* const);

private:
  size_t cur_size;
  uint8_t* cur_item;
  uint8_t* end_item;
  TPointerList items;

public:
  TNonFreePooledMemManager() {
    cur_size = sizeof(T) * 4;
    cur_item = nullptr;
    end_item = nullptr;
  }

  ~TNonFreePooledMemManager() { clear(); }

  inline void clear() noexcept {
    if (items.size() > 0) {
      for (size_t i = 0; i < items.size(); ++i)
        free(items[i]);
      items.clear();
    }
    cur_size = sizeof(T) * 4;
    cur_item = nullptr;
    end_item = nullptr;
  }

  inline T* new_item() noexcept {
    if (cur_item == end_item) {
      cur_size += cur_size;
      cur_item = static_cast<uint8_t*>(malloc(cur_size));
      items.push_back(cur_item);
      end_item = cur_item;
      end_item += cur_size;
    }
    T* result = static_cast<T*>(static_cast<void*>(cur_item));
    cur_item += sizeof(T);
    memset(result, 0, sizeof(T));
    return result;
  }

  inline void enumerate_items(const TEnumItemsProc proc) noexcept {
    if (items.size() > 0) {
      const size_t count = items.size();
      size_t size = sizeof(T) * 4;
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

