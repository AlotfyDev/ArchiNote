#ifndef __RAIISTYLEPOINTER_HPP__
#define __RAIISTYLEPOINTER_HPP__



// Simple wrapper for raw pointer management (RAII-style, manual control)
template <typename T>
class RawPtrWrapper {
private:
    T* ptr;  // Raw pointer inside

public:
    // Constructor: Allocate and manage
    explicit RawPtrWrapper(T* p = nullptr) : ptr(p) {}

    // Destructor: Manual delete (full control)
    ~RawPtrWrapper() {
        if (ptr) {
            delete ptr;  // Explicit delete call
            ptr = nullptr;
        }
    }

    // No copy (to avoid double-delete)
    RawPtrWrapper(const RawPtrWrapper&) = delete;
    RawPtrWrapper& operator=(const RawPtrWrapper&) = delete;

    // Move for transfer (control lifecycle)
    RawPtrWrapper(RawPtrWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    RawPtrWrapper& operator=(RawPtrWrapper&& other) noexcept {
        if (this != &other) {
            if (ptr) delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Accessors: Get raw for use, but no ownership transfer
    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    // Manual release (if needed, full control)
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // Check validity
    bool valid() const { return ptr != nullptr; }
};





















#endif // __RAIISTYLEPOINTER_HPP