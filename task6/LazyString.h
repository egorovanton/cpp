#ifndef LAZYSTRING_H
#define LAZYSTRING_H

#include <memory>
#include <string>
#include <stdexcept>

class LazyString
{
    public:
        LazyString();
        LazyString(const std::string& str);
        LazyString(const LazyString& lazyStr);

        operator std::string();

        const char& operator[] (size_t pos) const;
        const char& at (size_t pos) const;

        size_t length() const;
        size_t size() const;

        LazyString substr(size_t pos, size_t len) const;

        friend std::istream& operator >> (std::istream& input, LazyString& lazyString);
        friend std::ostream& operator << (std::ostream& output, LazyString& lazyString);
    private:
        LazyString (const std::shared_ptr<std::string>& str, size_t pos, size_t len);
        std::shared_ptr<std::string> string;
        size_t start, len;
};

#endif // LAZYSTRING_H
