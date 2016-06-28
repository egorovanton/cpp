#include <memory>
#include <iostream>
#include <atomic>
#include <thread>
#include <string>
#include "LazyString.h"

StringLock::StringLock() {
    readers = 0;
}

StringLock::StringLock(const StringLock& lock) {
    readers = 0;
}                                                      

void StringLock::readLock() {
    this->mutex.lock();
    this->readers++;
    this->mutex.unlock();
}

void StringLock::writeLock() {
    this->mutex.lock();
    while (this->readers.load() > 0)
    {
        std::this_thread::yield();
    }
}

void StringLock::readUnlock() {
    this->readers--;
}

void StringLock::writeUnlock() {
    this->mutex.unlock();
}

LazyString::CharPointer::CharPointer(LazyString* str, size_t pos) {
    this->str = str;
    this->pos = pos;
}

LazyString::CharPointer::operator char() const {
    str->lock->readLock();
    if (this->pos >= str->len) {
        throw std::out_of_range("LazyString: out of range");
    }

    char lchar = (*str->string)[str->start + this->pos];
    str->lock->readUnlock();
    return lchar;
}

LazyString::CharPointer& LazyString::CharPointer::operator = (char c) {
    str->lock->writeLock();
    if (this->pos >= str->len) {
        throw std::out_of_range("LazyString: out of range");
    }

    auto newLock = str->lock;
    if (str->string.use_count() > 1) {
        str->string = std::make_shared<std::string>(str->string->substr(str->start, str->len));
        str->lock = std::make_shared<StringLock>();
        str->start = 0;
    }

    (*str->string)[str->start + this->pos] = c;
    newLock->writeUnlock();
    return *this;
}

LazyString::LazyString(const std::shared_ptr<std::string> string, const std::shared_ptr<StringLock> lock, size_t start, size_t len) {
    this->string = string;
    this->lock   = lock;
    this->start  = 0;
    this->len    = len;
}

LazyString::LazyString() : LazyString(std::string()) {
}

LazyString::LazyString(const std::string& string) {
    this->string = std::make_shared<std::string>(string);
    this->lock   = std::make_shared<StringLock>();
    this->start  = 0;
    this->len    = string.length();
}

size_t LazyString::length() const {
    return size();
}

size_t LazyString::size() const {
    this->lock->readLock();
    size_t len = this->len;
    this->lock->readUnlock();
    return len;
}

LazyString::CharPointer LazyString::operator [] (size_t pos) {
    return at(pos);
}

const char& LazyString::operator [] (size_t pos) const {
    return at(pos);
}

LazyString::CharPointer LazyString::at(size_t pos) {
    return CharPointer(this, pos);
}

const char& LazyString::at(size_t pos) const {
    this->lock->readLock();
    if (pos >= this->len) {
        throw std::out_of_range("LazyString: out of range");
    }

    char& lchar = (*this->string)[this->start + pos];
    this->lock->readUnlock();
    return lchar;
}



LazyString::operator std::string() {
    this->lock->readLock();
    std::string str = std::string(this->string->substr(this->start, this->len));
    this->lock->readUnlock();
    return str;
}

LazyString LazyString::substr(size_t pos, size_t len) const {
    this->lock->readLock();
    if (start >= this->len) {
        throw std::out_of_range("LazyString: out of range");
    }

    size_t lockedStart = this->start;
    size_t lockedLen   = this->len;

    auto newLock      = this->lock;
    auto lockedString = this->string;

    this->lock->readUnlock();
    return LazyString(lockedString, newLock, lockedStart + pos, pos + len > lockedLen ? lockedLen - pos : len);
}

std::istream& operator >> (std::istream &input, LazyString &str) {
    auto string = std::make_shared<std::string>();

    input >> *string;
    str.lock->writeLock();
    std::shared_ptr<StringLock> newLock = str.lock;

    str.string = string;
    str.lock = std::make_shared<StringLock>();
    str.start = 0;
    str.len = string->length();

    newLock->writeUnlock();
    return input;
}

std::ostream& operator << (std::ostream &output, LazyString &str) {
    str.lock->readLock();
    for (size_t i = 0; i < str.len; ++i) {
        output << (*str.string)[str.start + i];
    }
    str.lock->readUnlock();

    return output;
}