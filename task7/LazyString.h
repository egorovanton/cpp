#ifndef LAZYSTRING_H
#define LAZYSTRING_H

#include <atomic>
#include <string>
#include <iostream>
#include <memory>
#include <mutex>

using namespace std;

class LazyLock
{
    private:
        std::atomic<int> readers;
        mutex mutex;
    public:
        LazyLock();
        LazyLock(const LazyLock& lock);
        void readLock();
        void readUnlock();
        void writeLock();
        void writeUnlock();
};

class LazyString
{
    private:
        struct LazyChar
        {
            friend class LazyString;
        private:
            const size_t pos;
            LazyString* ls;
            
            LazyChar(LazyString* ls, size_t pos);
        public:
            
            operator char() const;
            LazyChar& operator=(char c);
        };
        
        size_t len;
        size_t start;

        std::shared_ptr<std::string> string;
        std::shared_ptr<LazyLock> lock;
        
        LazyString( const std::shared_ptr<std::string> string,
                    const std::shared_ptr<LazyLock> lock, size_t start,
                    size_t len);
    public:
        LazyString();
        LazyString(const std::string& string);
        size_t length() const;
        size_t size() const;

        LazyChar at(size_t pos);
        const char& at(size_t pos) const;

        operator std::string();

        LazyChar operator [] (size_t pos);
        const char& operator [] (size_t pos) const;
        
        LazyString substr(size_t pos = 0, size_t len = std::string::npos) const;

        friend std::istream& operator >> (std::istream& is, LazyString& ls);        
        friend std::ostream& operator << (std::ostream& os, LazyString& ls);
};

#endif // LAZYSTRING_H