/*

Modern Bson Serialization/Deserialization library for C++ (17+)
version 1.2.2
https://github.com/sertaceker/cpp-bson-convert

If you encounter any issues, please submit a ticket at https://github.com/sertaceker/cpp-bson-convert/issues

MIT License

Copyright (c) 2024 sertaceker

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef CPP_BSON_CONVERT_HPP
#define CPP_BSON_CONVERT_HPP

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <bsoncxx/v_noabi/bsoncxx/document/view.hpp>
#include <bsoncxx/v_noabi/bsoncxx/oid.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/v_noabi/bsoncxx/types.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/basic/array.hpp>
#include <vector>



#pragma region typetraits

    template <typename T>
    struct is_std_vector : std::false_type
    {
    };

    template <typename U>
    struct is_std_vector<std::vector<U>> : std::true_type
    {
    };

    template <typename T>
    inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

    template <typename T>
    inline constexpr bool is_primitive_v = std::is_same_v<T, std::string> || std::is_arithmetic_v<T> || std::is_same_v<T, bsoncxx::v_noabi::oid>;

    template <class>
    inline constexpr bool always_false_v = false;

#pragma endregion

#pragma region deserialize methods

    /**
     * @brief Deserialize a BSON element to a C++ type
     * @tparam T C++ type to deserialize to
     * @tparam Element BSON element type
     * @param element BSON element to deserialize
     * @return Deserialized C++ type
     */
    template <typename T, typename Element>
    T get(const Element& element)
    {
        if constexpr (std::__is_optional_v<T>)
        {
            if (element && element.type() != bsoncxx::v_noabi::type::k_null)
            {
                return get<typename T::value_type>(element);
            }
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            return element.get_bool().value;
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            return element.get_int32().value;
        }
        else if constexpr (std::is_same_v<T, int32_t>)
        {
            return element.get_int32().value;
        }
        else if constexpr (std::is_same_v<T, int64_t>)
        {
            return element.get_int64().value;
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return element.get_double().value;
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            auto str = element.get_string().value;
            return std::string(str);
        }
        else if constexpr (std::is_same_v<T, std::chrono::time_point<std::chrono::system_clock>>)
        {
            return element.get_date();
        }
        else if constexpr (std::is_same_v<T, short>)
        {
            return static_cast<unsigned short>(element.get_int32().value);
        }
        else if constexpr (std::is_same_v<T, unsigned short>)
        {
            return static_cast<unsigned short>(element.get_int32().value);
        }
        else if constexpr (is_std_vector_v<T>)
        {
            std::vector<typename T::value_type> vec;
            vec.reserve(element.get_array().value.length());
            for (const auto& el : element.get_array().value)
            {
                vec.emplace_back(get<typename T::value_type>(el));
            }
            return vec;
        }
        else if constexpr (std::is_same_v<T, bsoncxx::v_noabi::oid>)
        {
            return element.get_oid().value;
        }
        else if constexpr (std::is_class_v<T>)
        {
            return T::fromBSON(element.get_document().view());
        }
        else
        {
            static_assert(always_false_v<T>, "Unsupported type");
        }

        return T{};
    }

    /**
     * @brief Deserialize a member from a BSON document
     * @tparam T Type of the member
     * @param member Member to deserialize
     * @param doc BSON document to deserialize from
     * @param key Key of the member in the BSON document
     */
    template <typename T>
    void deserializeMember(T& member, const bsoncxx::v_noabi::document::view& doc, const std::string& key)
    {
        auto it = doc.find(key);
        if (it != doc.end())
        {
            member = get<T>(*it);
        }
    }

#define EXPAND(x) x
#define EVAL(...)  EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...)  EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...)  EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...)  EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...)   EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...)   EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...)   EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...)    EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...)    EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...)    EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...)    __VA_ARGS__

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()

#define DESERIALIZE_MEMBER_FROM_DOC(member, doc, name) deserializeMember(instance.member, doc, name);
#define RECURSE_FROM_BSON_INDIRECT() RECURSE_FROM_BSON
#define RECURSE_FROM_BSON() BSON_FROM_BSON_1

#define BSON_FROM_BSON_1(class_name, x, ...)      \
DESERIALIZE_MEMBER_FROM_DOC(x, doc, #x)                          \
__VA_OPT__(OBSTRUCT(RECURSE_FROM_BSON)()(class_name, __VA_ARGS__))

#define BSON_DEFINE_FROM_BSON(class_name, ...)           \
static class_name fromBSON(const bsoncxx::document::view& doc) { \
class_name instance{}; \
EVAL(BSON_FROM_BSON_1(class_name, __VA_ARGS__)) \
return instance;                                        \
}

#pragma endregion

#pragma region serialize methods

    /**
     * @brief Serialize a primitive member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const T& value)
    {
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, value));
    }

    /**
     * @brief Serialize a time_point member to a BSON document
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    inline void serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::chrono::system_clock::time_point& value)
    {
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, bsoncxx::v_noabi::types::b_date{value}));
    }

    /**
     * @brief Serialize an optional member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::optional<T>& value)
    {
        if (value.has_value())
        {
            doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, value.value()));
            return;
        }

        // if the field is oid, we don't add it at all so that mongodb sets it automatically
        if (typeid(T) != typeid(bsoncxx::oid))
        {
            doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, bsoncxx::v_noabi::types::b_null{}));
        }
    }

    /**
     * @brief Serialize a primitive vector member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::vector<T>& value)
    {
        bsoncxx::v_noabi::builder::basic::array arr;
        for (const auto& el : value)
        {
            arr.append(el);
        }
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, arr));
    }

    /**
     * @brief Serialize a non primitive vector member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<!is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::vector<T>& value)
    {
        bsoncxx::v_noabi::builder::basic::array arr;
        for (const auto& el : value)
        {
            arr.append(T::toBSON(el).view());
        }
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, arr));
    }

    /**
     * @brief Serialize a primitive optional vector member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::optional<std::vector<T>>& value)
    {
        if (value.has_value())
        {
            bsoncxx::v_noabi::builder::basic::array arr;
            for (const auto& el : value.value())
            {
                arr.append(el);
            }
            doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, arr));
            return;
        }

        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, bsoncxx::v_noabi::types::b_null{}));
    }

    /**
     * @brief Serialize a non primitive optional vector member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<!is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::optional<std::vector<T>>& value)
    {
        if (value.has_value())
        {
            bsoncxx::v_noabi::builder::basic::array arr;
            for (const auto& el : value.value())
            {
                arr.append(T::toBSON(el).view());
            }
            doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, arr));
            return;
        }
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, bsoncxx::v_noabi::types::b_null{}));
    }

    /**
     * @brief Serialize a class member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<!is_primitive_v<T> && !is_std_vector_v<T> && !std::__is_optional_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const T& value)
    {
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, T::toBSON(value).view()));
    }

    /**
     * @brief Serialize an optional member to a BSON document
     * @tparam T Type of the member
     * @param doc BSON document to serialize to
     * @param key Key of the member in the BSON document
     * @param value Value of the member
     */
    template <typename T>
    std::enable_if_t<!is_primitive_v<T>> serializeMember(bsoncxx::v_noabi::builder::basic::document& doc, const std::string& key, const std::optional<T>& value)
    {
        if (value.has_value())
        {
            serializeMember(doc, key, value.value());
            return;
        }
        doc.append(bsoncxx::v_noabi::builder::basic::kvp(key, bsoncxx::v_noabi::types::b_null{}));
    }


#define APPEND_MEMBER_TO_DOC(member, doc, name) serializeMember(doc, name, obj.member);

#define RECURSE_TO_BSON_INDIRECT() RECURSE_TO_BSON
#define RECURSE_TO_BSON() BSON_TO_BSON_1

#define BSON_TO_BSON_1(class_name, x, ...)       \
APPEND_MEMBER_TO_DOC(x, doc, #x)                \
__VA_OPT__(OBSTRUCT(RECURSE_TO_BSON)()(class_name, __VA_ARGS__))

#define BSON_DEFINE_TO_BSON(class_name, ...)           \
static bsoncxx::document::value toBSON(const class_name& obj) { \
bsoncxx::v_noabi::builder::basic::document doc{}; \
EVAL(BSON_TO_BSON_1(class_name, __VA_ARGS__)) \
return doc.extract(); \
}

#define BSON_DEFINE_TYPE(class_name, ...)           \
BSON_DEFINE_FROM_BSON(class_name, __VA_ARGS__) \
BSON_DEFINE_TO_BSON(class_name, __VA_ARGS__)

#pragma endregion


#endif //CPP_BSON_CONVERT_HPP
