#include "LazyString.h"

LazyString::LazyString() {
    start = 0;
    len = 0;
    string = std::make_shared<std::string>("");
}

LazyString::LazyString(const std::string& str) {
    start = 0;
    len = str.length();
    string = std::make_shared<std::string>(str);
}

LazyString::LazyString(const LazyString& lazyStr) {
    start = lazyStr.start;
    len = lazyStr.length();
    string = lazyStr.string;
}

LazyString::LazyString(const std::shared_ptr<std::string>& str, size_t pos, size_t len) {
    string = str;
    start = pos;
    this->len = len;
}

LazyString::operator std::string() {
    return std::string(string->substr(start, len));
}

char& LazyString::operator [] (size_t pos) {
    return string->at(pos);
}

const char& LazyString::operator [] (size_t pos) const {
    return at(pos);
}

const char& LazyString::at (size_t pos) const {
    if (pos + start > len) {
        throw std::out_of_range("LazyString::at()");
    }
    return string->at(pos + start);
}

size_t LazyString::length() const {
    return len - start;
}

size_t LazyString::size() const {
    return len - start;
}

LazyString LazyString::substr(size_t pos, size_t len) const {
    return LazyString(string, pos, len);
}


std::istream& operator >> (std::istream& input, LazyString& lazyString) {
    input >> *(lazyString.string);
    lazyString.start = 0;
    lazyString.len = lazyString.string->length();
    return input;
}

std::ostream& operator << (std::ostream& output, LazyString& lazyString) {
    output << lazyString.string->substr(lazyString.start, lazyString.len);
    return output;
}



