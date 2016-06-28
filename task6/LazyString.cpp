#include <iostream>
#include <memory>
#include <string>
#include "LazyString.h"

LazyString::CharPointer::CharPointer(LazyString* ls, size_t pos) {
    this->ls = ls;
    this->pos = pos;
}

LazyString::CharPointer& LazyString::CharPointer:: operator = (char c) {
    if (this->pos >= ls->len)
        throw std::out_of_range("LazyString: out of range");

    if (ls->string.use_count() > 1) {
        ls->string = std::make_shared<std::string>(ls->string->substr(ls->start, ls->len));
        ls->start = 0;
    }

    (*ls->string)[ls->start + this->pos] = c;
    return *this;
}

LazyString::CharPointer::operator char() const {
    if (this->pos >= ls->len)
        throw std::out_of_range("LazyString: out of range");

    return (*ls->string)[ls->start + this->pos];
}

LazyString::LazyString(const std::shared_ptr<std::string> string, size_t start, size_t len) {
    this->string = string;
    this->start  = start;
    this->len    = len;

}

LazyString::LazyString() {
    start = 0;
    len = 0;
    string = std::make_shared<std::string>("");
}


LazyString::LazyString(const std::string& string) {
    this->start  = 0;
    this->len    = string.length();
    this->string = std::make_shared<std::string>(string);
}

size_t LazyString::size() const {
    return this->len;
}

size_t LazyString::length() const {
    return this->len;
}

LazyString::CharPointer LazyString::at(size_t pos) {
    return CharPointer(this, pos);
}

const char& LazyString::at(size_t pos) const {
    if (pos >= this->len)
        throw std::out_of_range("LazyString: out of range");

    return (*this->string)[this->start + pos];
}

LazyString::CharPointer LazyString::operator [] (size_t pos) {
    return at(pos);
}

const char& LazyString::operator [] (size_t pos) const {
    return at(pos);
}

LazyString LazyString::substr(size_t pos, size_t len) const {
    if (start >= this->len)
        throw std::out_of_range("LazyString: out of range");

    return LazyString(this->string, this->start + pos, pos + len > this->len ? this->len - pos : len);
}

LazyString::operator std::string() {
    return std::string(this->string->substr(this->start, this->len));
}

std::ostream& operator << (std::ostream &output, LazyString &lazyString) {
    output << lazyString.string->substr(lazyString.start, lazyString.len);
    return output;
}

std::istream& operator >> (std::istream &input, LazyString &lazyString) {
    auto string = std::make_shared<std::string>();

    input >> *string;
    lazyString.string = string;
    lazyString.start = 0;
    lazyString.len = string->length();

    return input;
}