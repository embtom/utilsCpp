/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal
 * (https://github.com/embtom)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _ENUM_REFLECT_H_
#define _ENUM_REFLECT_H_

#pragma once
#include <type_traits>
#include <map>
#include <regex>
#include <ostream>



template<typename Enum, class Enable = void>
class EnumReflectBase;

template<typename Enum>
class EnumReflectBase<Enum, typename std::enable_if_t<std::is_enum<Enum>::value>>
{
public:
    static void reflect( const char* enumsInfo )
    {
        if (m_string2enum.size() != 0) {
            return;
        }

        if ((enumsInfo == nullptr) || (*enumsInfo == 0)) {
            return;
        }

        // Should be called once per each enumeration.
        std::string senumsInfo(enumsInfo);
        std::regex re("^([a-zA-Z_][a-zA-Z0-9_]+) *=? *([^,]*)(,|$) *");     // C++ identifier to optional " = <value>"
        std::smatch sm;

        for (int value = 0; regex_search(senumsInfo, sm, re); senumsInfo = sm.suffix(), value++)
        {
            std::string enumName = sm[1].str();
            std::string enumValue = sm[2].str();

            if (enumValue.length() != 0) {
                if(enumValue.compare(0,2,"0b") == 0) {
                    value = std::stoi(enumValue.substr(2), nullptr, 2);
                }
                else {
                    value = std::stoi(enumValue,nullptr,0);
                }
            }

            Enum enumEntry = static_cast<Enum>(value);
            m_enum2string[enumEntry] = enumName;
            m_string2enum[enumName] = enumEntry;
        }
    }

    static bool toString(Enum E, std::string& rName)
    {
        auto it = m_enum2string.find(E);
        if (it == m_enum2string.end()) {
            return false;
        }

        rName = it->second;
        return true;
    }
private:
    static std::map<std::string, Enum> m_string2enum;
    static std::map<Enum, std::string> m_enum2string;
};

template <typename Enum>
 std::map<std::string, Enum> EnumReflectBase<Enum, typename std::enable_if_t<std::is_enum<Enum>::value>>::m_string2enum;

template <typename Enum>
 std::map<Enum, std::string> EnumReflectBase<Enum, typename std::enable_if_t<std::is_enum<Enum>::value>>::m_enum2string;


template <typename Enum>
class EnumReflect
{
    public:
        static const char* getEnums() { return ""; }
};

template<typename Enum, std::enable_if_t<std::is_enum<Enum>::value, int> = 0>
std::string EnumToString(Enum E)
{
    EnumReflect<Enum>::reflect(EnumReflect<Enum>::getEnums());

    std::string name;
    if( EnumReflect<Enum>::toString(E, name))
        return name;
    else
        return "";
}


template<typename Enum, std::enable_if_t<std::is_enum<Enum>::value, int> = 0>
std::ostream &operator <<(std::ostream &out, const Enum value)
{
    out << EnumToString(value);
    return out;
}

template <typename ... T>
constexpr size_t countLength(T ... args)
{
    null(args...); //kill warnings
    return sizeof...(args);
}

#define DECLARE_ENUM(name, basetype, ...)                                         \
    enum class name : basetype { __VA_ARGS__ };                                    \
    template <>                                                         \
    class EnumReflect<name>: public EnumReflectBase<name> {             \
        public:                                                         \
            static const char* getEnums() { return #__VA_ARGS__; }      \
    };


#endif /* _ENUM_REFLECT_H_ */