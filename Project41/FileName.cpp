#include <iostream>
using namespace std;

template<typename T>
class CustomUniquePtr {
private:
    T* ptr;

public:
    explicit CustomUniquePtr(T* p = nullptr) : ptr(p) {}

    ~CustomUniquePtr() {
        delete ptr;
    }

    CustomUniquePtr(const CustomUniquePtr&) = delete;
    CustomUniquePtr& operator=(const CustomUniquePtr&) = delete;

    CustomUniquePtr(CustomUniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    CustomUniquePtr& operator=(CustomUniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T& operator*() const { return *ptr; }

    T* operator->() const { return ptr; }

    T* get() const { return ptr; }
};

template<typename T>
class CustomSharedPtr {
private:
    T* ptr;
    size_t* refCount;

public:
    explicit CustomSharedPtr(T* p = nullptr) : ptr(p) {
        refCount = new size_t(1);
    }

    CustomSharedPtr(const CustomSharedPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        ++(*refCount);
    }

    CustomSharedPtr& operator=(const CustomSharedPtr& other) {
        if (this != &other) {
            if (--(*refCount) == 0) {
                delete ptr;
                delete refCount;
            }

            ptr = other.ptr;
            refCount = other.refCount;
            ++(*refCount);
        }
        return *this;
    }

    ~CustomSharedPtr() {
        if (--(*refCount) == 0) {
            delete ptr;
            delete refCount;
        }
    }

    T& operator*() const { return *ptr; }

    T* operator->() const { return ptr; }

    T* get() const { return ptr; }

    size_t use_count() const { return *refCount; }
};

int main() {
    {
        CustomUniquePtr<int> uptr(new int(10));
        cout << "CustomUniquePtr value: " << *uptr << endl;
    } 

    {
        CustomSharedPtr<int> sptr1(new int(20));
        cout << "CustomSharedPtr value: " << *sptr1 << ", Ref count: " << sptr1.use_count() << endl;

        {
            CustomSharedPtr<int> sptr2 = sptr1;
            cout << "After copy, Ref count: " << sptr1.use_count() << endl;
        } 

        cout << "After sptr2 out of scope, Ref count: " << sptr1.use_count() << endl;
    } 

    return 0;
}
