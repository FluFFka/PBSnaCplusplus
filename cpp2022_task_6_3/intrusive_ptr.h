#include <cstddef>
#include <utility>

template <typename T>
class TBasePtr {
protected:
    T* ptr_ = nullptr;
public:
    using element_type = T;
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    T* Get() const {
        return ptr_;
    }
    explicit operator bool() const {
        return ptr_ != nullptr;
    }
};

template <typename T, typename U>
bool operator==(const TBasePtr<T>& lhs, const TBasePtr<U>& rhs) {
    return lhs.Get() == rhs.Get();
}

template <typename T, typename U>
bool operator!=(const TBasePtr<T>& lhs, const TBasePtr<U>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
bool operator==(const TBasePtr<T>& lhs, std::nullptr_t) {
    return !lhs;
}

template<typename T>
bool operator==(std::nullptr_t, const TBasePtr<T>& rhs) {
    return !rhs;
}

template<typename T>
bool operator!=(const TBasePtr<T>& lhs, std::nullptr_t) {
    return (bool)lhs;
}

template<typename T>
bool operator!=(std::nullptr_t, const TBasePtr<T>& rhs) {
    return (bool)rhs;
}


template <typename T>
class TRefCounter {
    int ref_num_{0};
public:
    int RefCount() const {
        return ref_num_;
    }
    void AddRef() {
        ref_num_++;
    }
    void ReleaseRef() {
        ref_num_--;
    }
};


template <typename T>
class TIntrusivePtr : public TBasePtr<T> {
public:
    TIntrusivePtr() {
        this->ptr_ = nullptr;
    }
    TIntrusivePtr(T* p) {
        this->ptr_ = p;
        if (this->ptr_) {
            this->ptr_->AddRef();
        }
    }
    TIntrusivePtr(const TIntrusivePtr& rhs) {
        this->ptr_ = rhs.Get();
        if (this->ptr_ != nullptr) {
            this->ptr_->AddRef();
        }
    }
    TIntrusivePtr(TIntrusivePtr&& rhs) {
        this->ptr_ = rhs.Get();
        rhs->ptr_ = nullptr;
    }

    void Release() {
        if (this->ptr_ != nullptr) {
            this->ptr_->ReleaseRef();
            if (this->ptr_->RefCount() == 0) {
                delete this->ptr_;
            }
            this->ptr_ = nullptr;
        }
    }

    TIntrusivePtr& operator=(const TIntrusivePtr& rhs) {
        Release();
        this->ptr_ = rhs.Get();
        if (this->ptr_ != nullptr) {
            this->ptr_->AddRef();
        }
        return *this;
    }
    TIntrusivePtr& operator=(TIntrusivePtr&& rhs) {
        this->ptr_ = rhs.Get();
        rhs.ptr_ = nullptr;
        return *this;
    }

    void Reset() {
        Release();
    }
    void Reset(T* p) {
        Release();
        this->ptr_ = p;
        if (this->ptr_) {
            this->ptr_->AddRef();
        }
    }
    void Reset(const TIntrusivePtr& p) {
        Release();
        this->ptr_ = p.Get();
        if (this->ptr_) {
            this->ptr_->AddRef();
        }
    }
    void Reset(TIntrusivePtr&& p) {
        this->ptr_ = p.Get();
        p.ptr_ = nullptr;
    }

    int RefCount() {
        if (this->ptr_) {
            return this->ptr_->RefCount();
        }
        return 0;
    }

    ~TIntrusivePtr() {
        Release();
    }
};


template <typename T, typename... Ts>
TIntrusivePtr<T> MakeIntrusive(Ts&&... params) {
    return TIntrusivePtr<T>(
        new T(std::forward<Ts>(params)...)
    );
}