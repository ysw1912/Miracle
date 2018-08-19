#pragma once

#include <string>

class String
{
public:
    String(const char* s) : str(s) {}

    String(const std::string& s) : str(s.c_str()) {}

    const char* c_str() const 
    { return str; }

private:
    const char* str;
};

template <typename To, typename From>
inline To implicit_cast(From f)
{ return f; }
