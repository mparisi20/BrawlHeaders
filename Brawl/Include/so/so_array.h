#pragma once

#include <StaticAssert.h>
#include <so/so_connectable.h>
#include <so/so_general_flag.h>
#include <so/so_null.h>
#include <types.h>
#include <so/so_array_vector_calculator.h>
#include <bitset>

template <class T>
class soSet {
    T* m_elements;
    size_t m_size;

public:
    T* elements() {
        return m_elements;
    }

    u32 size() {
        return m_size;
    }
};

template <class T>
class soArrayFixed : public soNullableInterface {
public:

    virtual bool isNull() const { return false; }
    virtual T& at(s32 index) = 0;
    virtual const T& at(s32 index) const = 0;
    virtual u32 size() const = 0;
    virtual bool isEmpty() const { return this->size() == 0; }
    virtual ~soArrayFixed() { };
};

template <class T>
class soArrayContractible : public soArrayFixed<T> {
public:
    virtual ~soArrayContractible() { }
    virtual void shift() = 0;
    virtual void pop() = 0;
    virtual void clear() = 0;
};

template<typename T>
class soArrayContractibleTable : public soArrayContractible<T>,
                                 public soConnectable<soArrayContractibleTable<T> > {
    T* m_elements;
    s32 m_size;
public:
    soArrayContractibleTable() : m_elements(nullptr), m_size(0) { }
    soArrayContractibleTable(T* elmnts, s32 size) : m_elements(elmnts), m_size(size) {
        if (!elmnts) {
            this->m_size = 0;
        }
    }

    virtual ~soArrayContractibleTable() { }

    soArrayContractibleTable& operator=(const soArrayContractibleTable& other) {
        if (this == &other) {
            return *this;
        }

        this->m_link = other.m_link;
        m_elements = other.m_elements;
        m_size = other.m_size;
        if (other.m_elements == nullptr){
            m_size = 0;
        }
        return *this;
    }

    virtual T& at(s32 index) { return atSub(index); }
    virtual const T& at(s32 index) const { return atSub(index); }

    virtual void shift() {
        if (!this->isEmpty()) {
            if (m_size > 0) {
                if (--m_size <= 0) {
                    m_elements = nullptr;
                } else {
                    m_elements++;
                }
            } else if (this->m_link) {
                this->m_link->shift();
            }
        }
    }

    virtual void pop() {
        if (this->m_link && !this->m_link->isEmpty()) {
            this->m_link->pop();
        } else if (m_size > 0) {
            m_size--;
        }
    }

    virtual void clear() {
        if (this->m_link) {
            this->m_link->clear();
        }
        m_elements = nullptr;
        m_size = 0;
    }

    virtual u32 size() const {
        u32 res = m_size;
        if (!m_elements) {
            res = 0;
        }
        if (this->m_link) {
            return res + this->m_link->size();
        }
        return res;
    }

    bool isNull() const { return false; }

    virtual T& atSub(s32 index) const {
        if (index >= m_size && this->m_link) {
            return this->m_link->at(index - m_size);
        }
        return m_elements[index];
    }
};

template <class T>
class soArray : public soArrayContractible<T> {
public:
    virtual ~soArray() {};

    virtual void unshift(const T&) = 0;
    virtual void push(const T&) = 0;
    virtual void insert(u32, const T&) = 0;
    virtual void erase(u32) = 0;
    virtual u32 capacity() = 0;
    virtual bool isFull() = 0;
    virtual void set(u32 startingIndex, const T& element, u32 numIndicesToSet) = 0;
};

template <class T>
class soArrayNull : public soArray<T> {

public:
    virtual bool isNull() const { return true; }
    virtual T& at(s32 index) {
        static T m_nullElement;
        return m_nullElement;
    };
    virtual const T& at(s32 index) const {
        static T m_nullElement;
        return m_nullElement;
    };
    virtual u32 size() const { return 0; }
    virtual ~soArrayNull() { }
    virtual void shift() { return; }
    virtual void pop() { return; }
    virtual void clear() { return; }
    virtual void unshift(const T&) { return; }
    virtual void push(const T&) { return; }
    virtual void insert(u32, const T&) { return; }
    virtual void erase(u32) { return; }
    virtual u32 capacity() { return 0; }
    virtual bool isFull() { return true; }
    virtual void set(u32 startingIndex, const T& element, u32 numIndicesToSet) { }

    soArrayNull() { }
    soArrayNull(u32 size, u32) { }
    soArrayNull(u32 size, const T& element, u32) { }
};

template <class T, class s> // TODO: Not sure what second template argument is
class soArrayListUnit {
    T m_element;
    int m_4;
    int m_8;
};

template <class T, u32 C>
class soArrayList : public soArray<T> {
    u32 m_a : sizeof(bit_width<C>) + 1;
    u32 m_b : sizeof(bit_width<C>) + 1;
    u32 m_c : sizeof(bit_width<C>) + 1;
    u32 m_size : sizeof(bit_width<C>) + 1;

    soArrayListUnit<T, int> m_units[C];

public:
    virtual bool isNull() const;
    virtual T& at(s32 index);
    virtual const T& at(s32 index) const;
    virtual u32 size() const { return m_size; }
    virtual ~soArrayList() { }
    virtual void shift();
    virtual void pop();
    virtual void clear();
    virtual void unshift(const T&);
    virtual void push(const T&);
    virtual void insert(u32, const T&);
    virtual void erase(u32);
    virtual u32 capacity() { return C; }
    virtual bool isFull();
    virtual void set(u32 startingIndex, const T& element, u32 numIndicesToSet);
};

class soArrayVectorCalcInterface {
public:
    virtual void substitution(u32, u32) = 0;
    virtual void onFull() = 0;
    virtual void offFull() = 0;
    virtual void setTopIndex(u32) = 0;
    virtual void setLastIndex(u32) = 0;
    virtual ~soArrayVectorCalcInterface() { }
};

template <class T>
class soArrayVectorAbstract : public soArray<T>, public soArrayVectorCalcInterface {
public:
    virtual bool isNull() const {
        return false;
    }
    virtual T& at(s32 index) {
        return this->atFastAbstractSub(index);
    }
    virtual const T& at(s32 index) const {
        return this->atFastAbstractSub(index);
    }
    virtual ~soArrayVectorAbstract() { }
    virtual void shift() {
        soArrayVectorCalculator::shift(this, this->isEmpty(), this->capacity(), this->getTopIndex());
        this->setSize(this->size() - 1);
    }
    virtual void pop() {
        soArrayVectorCalculator::pop(this, this->isEmpty(), this->capacity(), this->getLastIndex());
        this->setSize(this->size() - 1);
    }
    virtual void clear() {
        soArrayVectorCalculator::clear(this);
        this->setSize(0);
    }
    virtual void unshift(const T& newElement) {
        u32 topIndex = soArrayVectorCalculator::unshift(this, this->isFull(), this->capacity(), this->getTopIndex(), this->getLastIndex());
        T* element = this->getArrayValueConst(topIndex);
        *element = newElement;
        this->setSize(this->size() + 1);
    }
    virtual void push(const T& newElement) {
        u32 lastIndex = soArrayVectorCalculator::push(this, this->isFull(), this->capacity(), this->getTopIndex(), this->getLastIndex());
        T* element = this->getArrayValueConst(lastIndex);
        *element = newElement;
        this->setSize(this->size() + 1);
    }
    virtual void insert(u32 index, const T& newElement) {
        u32 lastIndex = soArrayVectorCalculator::insert(this, index, this->isFull(), this->size(), this->capacity(), this->getTopIndex(), this->getLastIndex());
        T* element = this->getArrayValueConst(lastIndex);
        *element = newElement;
        this->setSize(this->size() + 1);
    }
    virtual void erase(u32 index) {
        soArrayVectorCalculator::erase(this, index, this->size(), this->capacity(), this->getTopIndex(), this->getLastIndex());
        this->setSize(this->size() - 1);
    }
    virtual u32 capacity() = 0;
    virtual void set(u32 startingIndex, const T& elementToCopy, u32 numIndicesToSet) {
        if (this->size() <= startingIndex + numIndicesToSet) {
            numIndicesToSet = this->size() - startingIndex;
        }
        for (u32 i = 0; i < numIndicesToSet; i++) {
            T& element = this->at(i + startingIndex);
            element = elementToCopy;
        }
    }

    virtual T& atFastAbstractSub(s32 index) const = 0;
    virtual void substitution(u32 subIndex, u32 targetIndex) {
        T* subElement = this->getArrayValueConst(subIndex);
        T* targetElement = this->getArrayValueConst(targetIndex);
        *targetElement = *subElement;
    }
    virtual T* getArrayValueConst(u32) = 0;
    virtual u32 getTopIndex() = 0;
    virtual u32 getLastIndex() = 0;
    virtual void setSize(u32) = 0;
};

template <class T, u32 C>
class soArrayVector : public soArrayVectorAbstract<T> {
    u32 m_topIndex : sizeof(bit_width<C>) + 1;
    u32 m_lastIndex : sizeof(bit_width<C>) + 1;
    u32 m_size : sizeof(bit_width<C>) + 1;
    bool m_isFull : 1;

    T m_elements[C];

public:
    virtual u32 size() const { return m_size; }
    virtual ~soArrayVector() { }
    virtual u32 capacity() { return C; }
    virtual bool isFull() { return m_isFull; }

    virtual T& atFastAbstractSub(s32 index) const {
        int iVar1 = this->m_topIndex + index;
        if (C - 1 < iVar1) {
            iVar1 -= C;
        }
        return this->m_elements[iVar1];

    }
    virtual T* getArrayValueConst(u32 index) {
        return &this->m_elements[index];
    }
    virtual u32 getTopIndex() { return m_topIndex; }
    virtual u32 getLastIndex() { return m_lastIndex; }
    virtual void setSize(u32 size) { m_size = size; }
    virtual void setTopIndex(u32 topIndex) { m_topIndex = topIndex; }
    virtual void setLastIndex(u32 lastIndex) { m_lastIndex = lastIndex; }
    virtual void onFull() { m_isFull = true; }
    virtual void offFull() { m_isFull = false; }

    soArrayVector() {
        m_topIndex = 0;
        m_lastIndex = 0;
        m_size = 0;
        m_isFull = false;
    }
    soArrayVector(u32 size, u32) {
        m_topIndex = 0;
        m_lastIndex = 0;
        m_isFull = false;
        m_size = size;
        soArrayVectorCalculator::postInitialize(this, size, C);
    }
    soArrayVector(u32 size, const T& element, u32) {
        m_topIndex = 0;
        m_lastIndex = 0;
        m_size = 0;
        m_isFull = false;
        size = soArrayVectorCalculator::resize(this, size, this->isEmpty(), this->isFull(), this->capacity(), this->getTopIndex(), this->getLastIndex());
        for (u32 i = 0; i < size; i++) {
            this->push(element);
        }
    }
};

template <class T>
class soArrayVector<T, 0> : public soArrayNull<T> {
public:
    inline soArrayVector() : soArrayNull<T>() { }
    inline soArrayVector(u32 size, u32 unk) : soArrayNull<T>(size, unk) { }
    inline soArrayVector(u32 size, const T& element, u32 unk) : soArrayNull<T>(size, element, unk) { }
};

extern soArrayNull<s32> g_s32ArrayNull;
extern soArrayNull<float> g_floatArrayNull;
extern soArrayNull<soGeneralFlag<s32> > g_s32GeneralFlagArrayNull;
