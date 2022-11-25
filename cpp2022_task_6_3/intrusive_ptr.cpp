#include "intrusive_ptr.h"

#include <iostream>

class TDoc : public TRefCounter<TDoc> {
    int value_{0};
public:
    TDoc(int value = 0) : value_{value} {}
    int get_value() const { return value_; }
    void set_value(int value) { value_ = value; }
};

int main()
{
    TIntrusivePtr<TDoc> ptr = nullptr;
    std::cout << ptr.RefCount() << std::endl;
    ptr = MakeIntrusive<TDoc>();
    std::cout << ptr.RefCount() << std::endl;
    TIntrusivePtr<TDoc> ptr2 = ptr;
    std::cout << ptr.RefCount() << std::endl;
    TIntrusivePtr<TDoc> ptr3 = MakeIntrusive<TDoc>();
    std::cout << ptr.RefCount() << " " << ptr3.RefCount() << std::endl;
    ptr3.Reset(ptr2);
    std::cout << ptr.RefCount() << " " << ptr3.RefCount() << std::endl;
    ptr3.Reset();
    std::cout << ptr.RefCount() << " " << ptr3.RefCount() << std::endl;
    ptr3.Reset(std::move(ptr2));
    std::cout << ptr.RefCount() << " " << ptr3.RefCount() << std::endl;
    static_assert(sizeof(TDoc*) == sizeof(ptr));
    
    return 0;
}