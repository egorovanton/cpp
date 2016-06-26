#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "LazyString.h"

LazyLock::LazyLock() {
    readers = 0;
}

LazyLock::LazyLock(const LazyLock& lock) {
    readers = 0;
}                                                       

void LazyLock::readLock() {
    this->mutex.lock();
    this->readers++;
    this->mutex.unlock();
}

void LazyLock::readUnlock() {
    this->readers--;
}

void LazyLock::writeLock() {
    this->mutex.lock();
    while (this->readers.load() > 0) {
        std::this_thread::yield();
    }
}

void LazyLock::writeUnlock() {
    this->mutex.unlock();
}

LazyString::LazyChar::LazyChar(LazyString* ls, size_t pos) {
    this->ls  = ls;
    this->pos = pos;
}

LazyString::LazyChar::operator char() const {
    ls->lock->readLock();
    if (this->pos >= ls->len)
        throw std::out_of_range("LazyString: out of range");

    char l_c = (*ls->string)[ls->start + this->pos];
    ls->lock->readUnlock();
    return l_c;
}

LazyString::LazyChar& LazyString::LazyChar::operator=(char c) {
    ls->lock->writeLock();
    if (this->pos >= ls->len)
        throw std::out_of_range("LazyString: out of range");
    std::shared_ptr<LazyLock> l_lock = ls->lock;

    if (ls->string.useCount() > 1)
    {
        ls->string = std::make_shared<std::string>(ls->string->substr(
            ls->start, ls->len));
        ls->lock = std::make_shared<LazyLock>();
        ls->start = 0;
    }

    (*ls->string)[ls->start + this->pos] = c;
    l_lock->writeUnlock();
    return *this;
}

LazyString::LazyString( const std::shared_ptr<std::string> string, 
                        const std::shared_ptr<LazyLock> lock,
                        size_t start, size_t len )
{
    this->string = string,
    this->lock   = lock;
    this->start  = start;
    this->len    = len;
}

LazyString::LazyString() {
    LazyString("");
}

LazyString::LazyString(const std::string& string) {
    this->string = std::make_shared<std::string>(string);
    this->lock = std::make_shared<LazyLock>();
    start = 0;
    this->len = string.length();
}

size_t LazyString::length() const {
    this->lock->readLock();
    size_t llen = this->len;
    this->lock->readUnlock();
    return llen;
}

size_t LazyString::size() const {
    return length();
}

LazyString::LazyChar LazyString::at(size_t pos) {
    return LazyChar(this, pos);
}

const char& LazyString::at(size_t pos) const {
    this->lock->readLock();
    if (pos >= this->len)
        throw std::out_of_range("LazyString: out of range");

    char& lchar = (*this->string)[this->start + pos];
    this->lock->readUnlock();
    return lchar;
}

LazyString::LazyChar LazyString::operator[](size_t pos) {
    return at(pos);
}

const char& LazyString::operator[](size_t pos) const {
    return at(pos);
}

LazyString LazyString::substr(size_t pos, size_t len) const {
    this->lock->readLock();
    if (start >= this->len)
        throw std::out_of_range("LazyString: out of range");

    std::shared_ptr<std::string> lazyString = this->string;
    std::shared_ptr<LazyLock> l_lock = this->lock;
    size_t l_start = this->start;
    size_t l_len = this->len;

    this->lock->readUnlock();
    return LazyString(lazyString, l_lock, l_start + pos, pos + len > l_len ? l_len - pos : len);
}

LazyString::operator std::string() {
    this->lock->readLock();
    std::string lazyString = std::string(this->string->substr(this->start,
                                                            this->len));
    this->lock->readUnlock();
    return lazyString;
}

std::istream& operator>>(std::istream &input, LazyString &lazyString) {
    auto string = std::make_shared<std::string>();

    input >> *string;
    lazyString.lock->writeLock();
    std::shared_ptr<LazyLock> l_lock = lazyString.lock;

    lazyString.string = string;
    lazyString.lock = std::make_shared<LazyLock>();
    lazyString.start = 0;
    lazyString.len = string->length();

    l_lock->write_unlock();
    return input;
}

std::ostream& operator<<(std::ostream &output, LazyString &lazyString) {
    lazyString.lock->readLock();
    output << lazyString.string->substr(lazyString.start, lazyString.len);
    lazyString.lock->readUnlock();

    return output;
}