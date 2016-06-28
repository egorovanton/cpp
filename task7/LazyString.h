#ifndef LAZYSTRING_H
#define LAZYSTRING_H

#include <memory>
#include <iostream>
#include <mutex>
#include <atomic>
#include <string>

class StringLock {
    private:
        std::atomic<int> readers;
        std::mutex mutex;
    public:
        StringLock();
        StringLock(const StringLock& lock);
        
        void readLock();
        void writeLock();
        
        void readUnlock();
        void writeUnlock();
};

class LazyString {
    private:
        size_t start;
        size_t len;
        
        std::shared_ptr<std::string> string;
        std::shared_ptr<StringLock> lock;

        LazyString(const std::shared_ptr<std::string> string, const std::shared_ptr<StringLock> lock, size_t start, size_t len);
        
        struct CharPointer {
            friend class LazyString;
            private:
                size_t pos;
                LazyString* str;
                CharPointer(LazyString* str, size_t pos);
            public:
                operator char() const;
                CharPointer& operator = (char c);
        };

    public:
        size_t length() const;
        size_t size() const;

        LazyString();
        LazyString(const std::string& string);

        operator std::string();
        LazyString substr(size_t pos = 0, size_t len = std::string::npos) const;

        CharPointer operator [] (size_t pos);
        const char& operator [] (size_t pos) const;        
        CharPointer at(size_t pos);
        const char& at(size_t pos) const;        

        friend std::istream& operator >> (std::istream& input, LazyString& str);
        friend std::ostream& operator << (std::ostream& output, LazyString& str);
};

#endif