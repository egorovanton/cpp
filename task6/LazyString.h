#ifndef LAZYSTRING_H
#define LAZYSTRING_H

#include <iostream>
#include <memory>
#include <string>

class LazyString 
{
private:
    struct CharPointer
    {
        friend class LazyString;
    private:
        size_t pos;
        LazyString* ls;
        
        CharPointer(LazyString* ls, size_t pos);
    public:
        operator char() const;

        CharPointer& operator = (char c);
    };
    
    std::shared_ptr<std::string> string;
    size_t start;
    size_t len;

    LazyString(const std::shared_ptr<std::string> string, size_t start, size_t len);
public:
    LazyString();

    LazyString(const std::string& string);

    size_t size() const;
    size_t length() const;

    CharPointer at(size_t pos);
    const char& at(size_t pos) const;

    CharPointer operator[](size_t pos);
    const char& operator[](size_t pos) const;
    
    LazyString substr(size_t pos = 0, size_t len = std::string::npos) const;
    
    operator std::string();

    friend std::istream& operator >> (std::istream& is, LazyString& ls);
    friend std::ostream& operator << (std::ostream& os, LazyString& ls);
};

#endif // LAZYSTRING_H
