#include "cpp-bson-convert.hpp"

#include <gtest/gtest.h>
#include <bsoncxx/json.hpp>

TEST(PrimitiveTypeTest, Deserialization)
{
    struct AllTypes
    {
        bsoncxx::oid id;
        bool boolean;
        int integer;
        double floatingPoint;
        std::string string;
        std::vector<int> intArray;
        std::vector<std::string> stringArray;
        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;

        BSON_DEFINE_TYPE(AllTypes, id, boolean, integer, floatingPoint, string, intArray, stringArray, optionalInt, optionalString)
    };

    AllTypes allTypes;
    allTypes.id = bsoncxx::oid();
    allTypes.boolean = true;
    allTypes.integer = 42;
    allTypes.floatingPoint = 3.14;
    allTypes.string = "Hello, World!";
    allTypes.intArray = {1, 2, 3, 4, 5};
    allTypes.stringArray = {"one", "two", "three", "four", "five"};
    allTypes.optionalInt = 42;

    const auto bson = AllTypes::toBSON(allTypes);
    const auto deserialized = AllTypes::fromBSON(bson);

    ASSERT_EQ(allTypes.id, deserialized.id);
    ASSERT_EQ(allTypes.boolean, deserialized.boolean);
    ASSERT_EQ(allTypes.integer, deserialized.integer);
    ASSERT_EQ(allTypes.floatingPoint, deserialized.floatingPoint);
    ASSERT_EQ(allTypes.string, deserialized.string);
    ASSERT_EQ(allTypes.intArray, deserialized.intArray);
    ASSERT_EQ(allTypes.stringArray, deserialized.stringArray);
    ASSERT_EQ(allTypes.optionalInt, deserialized.optionalInt);
    ASSERT_EQ(allTypes.optionalString, deserialized.optionalString);
}

TEST(PrimitiveTypeTest, Serialization)
{
    struct AllTypes
    {
        bsoncxx::oid id;
        bool boolean;
        int integer;
        double floatingPoint;
        std::string string;
        std::vector<int> intArray;
        std::vector<std::string> stringArray;
        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;

        BSON_DEFINE_TYPE(AllTypes, id, boolean, integer, floatingPoint, string, intArray, stringArray, optionalInt, optionalString)
    };

    AllTypes allTypes;
    allTypes.id = bsoncxx::oid();
    allTypes.boolean = true;
    allTypes.integer = 42;
    allTypes.floatingPoint = 3.14;
    allTypes.string = "Hello, World!";
    allTypes.intArray = {1, 2, 3, 4, 5};
    allTypes.stringArray = {"one", "two", "three", "four", "five"};
    allTypes.optionalInt = 42;

    const auto bson = AllTypes::toBSON(allTypes);
    auto view = bson.view();

    ASSERT_EQ(allTypes.id, bson["id"].get_oid().value);
    ASSERT_EQ(allTypes.boolean, bson["boolean"].get_bool().value);
    ASSERT_EQ(allTypes.integer, bson["integer"].get_int32().value);
    ASSERT_EQ(allTypes.floatingPoint, bson["floatingPoint"].get_double().value);
    ASSERT_EQ(allTypes.string, std::string(bson["string"].get_string().value));
    const auto intArray = bson["intArray"].get_array().value;
    for (size_t i = 0; i < allTypes.intArray.size(); ++i)
    {
        ASSERT_EQ(allTypes.intArray[i], intArray[i].get_int32().value);
    }
    const auto stringArray = bson["stringArray"].get_array().value;
    for (size_t i = 0; i < allTypes.stringArray.size(); ++i)
    {
        ASSERT_EQ(allTypes.stringArray[i], std::string(stringArray[i].get_string().value));
    }
    ASSERT_TRUE(view.find("optionalInt") != view.end());
    ASSERT_EQ(allTypes.optionalInt, bson["optionalInt"].get_int32().value);
    ASSERT_EQ(allTypes.optionalString, std::nullopt);

}

TEST(NestedClassTest, Deserialization)
{
    struct NestedClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        Inner inner;
        std::string name;

        BSON_DEFINE_TYPE(NestedClass, inner, name)
    };

    NestedClass nestedClass;
    nestedClass.inner.x = 42;
    nestedClass.inner.y = 24;
    nestedClass.name = "NestedClass";

    const auto bson = NestedClass::toBSON(nestedClass);
    const auto deserialized = NestedClass::fromBSON(bson);

    ASSERT_EQ(nestedClass.inner.x, deserialized.inner.x);
    ASSERT_EQ(nestedClass.inner.y, deserialized.inner.y);
    ASSERT_EQ(nestedClass.name, deserialized.name);
}

TEST(NestedClassTest, Serialization)
{
    struct NestedClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        Inner inner;
        std::string name;

        BSON_DEFINE_TYPE(NestedClass, inner, name)
    };

    NestedClass nestedClass;
    nestedClass.inner.x = 42;
    nestedClass.inner.y = 24;
    nestedClass.name = "NestedClass";

    const auto bson = NestedClass::toBSON(nestedClass);
    ASSERT_EQ(nestedClass.name, std::string(bson["name"].get_string().value));
    ASSERT_EQ(nestedClass.inner.x, bson["inner"]["x"].get_int32().value);
    ASSERT_EQ(nestedClass.inner.y, bson["inner"]["y"].get_int32().value);

}

TEST(NestedClassArrayTest, Deserialization)
{
    struct NestedClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::vector<Inner> inner;
        std::string name;

        BSON_DEFINE_TYPE(NestedClass, inner, name)
    };

    NestedClass nestedClass;
    nestedClass.inner.push_back({42, 24});
    nestedClass.inner.push_back({24, 42});
    nestedClass.name = "NestedClass";

    const auto bson = NestedClass::toBSON(nestedClass);
    const auto deserialized = NestedClass::fromBSON(bson);

    ASSERT_EQ(nestedClass.inner[0].x, deserialized.inner[0].x);
    ASSERT_EQ(nestedClass.inner[0].y, deserialized.inner[0].y);
    ASSERT_EQ(nestedClass.inner[1].x, deserialized.inner[1].x);
    ASSERT_EQ(nestedClass.inner[1].y, deserialized.inner[1].y);
    ASSERT_EQ(nestedClass.name, deserialized.name);

}

TEST(NestedClassArrayTest, Serialization)
{
    struct NestedClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::vector<Inner> inner;
        std::string name;

        BSON_DEFINE_TYPE(NestedClass, inner, name)
    };

    NestedClass nestedClass;
    nestedClass.inner.push_back({42, 24});
    nestedClass.inner.push_back({24, 42});
    nestedClass.name = "NestedClass";

    const auto bson = NestedClass::toBSON(nestedClass);
    ASSERT_EQ(nestedClass.name, std::string(bson["name"].get_string().value));
    const auto inner = bson["inner"].get_array().value;
    ASSERT_EQ(nestedClass.inner[0].x, inner[0]["x"].get_int32().value);
    ASSERT_EQ(nestedClass.inner[0].y, inner[0]["y"].get_int32().value);
    ASSERT_EQ(nestedClass.inner[1].x, inner[1]["x"].get_int32().value);
    ASSERT_EQ(nestedClass.inner[1].y, inner[1]["y"].get_int32().value);

}

TEST(NestedClassTest, SerializationWithEmptyInner)
{
    struct NestedClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::vector<Inner> inner;
        std::string name;

        BSON_DEFINE_TYPE(NestedClass, inner, name)
    };

    NestedClass nestedClass;
    nestedClass.name = "NestedClass";

    const auto bson = NestedClass::toBSON(nestedClass);
    ASSERT_EQ(nestedClass.name, std::string(bson["name"].get_string().value));
    ASSERT_TRUE(bson["inner"].get_array().value.empty());
}

TEST(PrimitiveTypeTest, SerializationWithEmptyPrimitiveArray)
{
    struct AllTypes
    {
        bsoncxx::oid id;
        bool boolean;
        int integer;
        double floatingPoint;
        std::string string;
        std::vector<int> intArray;
        std::vector<std::string> stringArray;
        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;

        BSON_DEFINE_TYPE(AllTypes, id, boolean, integer, floatingPoint, string, intArray, stringArray, optionalInt, optionalString)
    };

    AllTypes allTypes;
    allTypes.id = bsoncxx::oid();
    allTypes.boolean = true;
    allTypes.integer = 42;
    allTypes.floatingPoint = 3.14;
    allTypes.string = "Hello, World!";
    allTypes.intArray = {};
    allTypes.stringArray = {"one", "two", "three", "four", "five"};
    allTypes.optionalInt = 42;

    const auto bson = AllTypes::toBSON(allTypes);
    auto view = bson.view();

    ASSERT_EQ(allTypes.id, bson["id"].get_oid().value);
    ASSERT_EQ(allTypes.boolean, bson["boolean"].get_bool().value);
    ASSERT_EQ(allTypes.integer, bson["integer"].get_int32().value);
    ASSERT_EQ(allTypes.floatingPoint, bson["floatingPoint"].get_double().value);
    ASSERT_EQ(allTypes.string, std::string(bson["string"].get_string().value));
    ASSERT_TRUE(bson["intArray"].get_array().value.empty());
    const auto stringArray = bson["stringArray"].get_array().value;
    for (size_t i = 0; i < allTypes.stringArray.size(); ++i)
    {
        ASSERT_EQ(allTypes.stringArray[i], std::string(stringArray[i].get_string().value));
    }
    ASSERT_TRUE(view.find("optionalInt") != view.end());
    ASSERT_EQ(allTypes.optionalInt, bson["optionalInt"].get_int32().value);
    ASSERT_EQ(allTypes.optionalString, std::nullopt);
}

TEST(SerializationTest, SerializeMembers)
{
    bsoncxx::builder::basic::document doc{};

    bsoncxx::oid id = bsoncxx::oid();
    bool boolean = true;
    int integer = 42;
    double floatingPoint = 3.14;
    std::string string = "Hello, World!";
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    std::vector<std::string> stringArray = {"one", "two", "three", "four", "five"};
    std::optional<int> optionalInt = 42;
    std::optional<std::string> optionalString = "Optional String";

    serializeMember(doc, "id", id);
    serializeMember(doc, "boolean", boolean);
    serializeMember(doc, "integer", integer);
    serializeMember(doc, "floatingPoint", floatingPoint);
    serializeMember(doc, "string", string);
    serializeMember(doc, "intArray", intArray);
    serializeMember(doc, "stringArray", stringArray);
    serializeMember(doc, "optionalInt", optionalInt);
    serializeMember(doc, "optionalString", optionalString);

    auto view = doc.view();

    ASSERT_EQ(view["id"].get_oid().value, id);
    ASSERT_EQ(view["boolean"].get_bool().value, boolean);
    ASSERT_EQ(view["integer"].get_int32().value, integer);
    ASSERT_EQ(view["floatingPoint"].get_double().value, floatingPoint);
    ASSERT_EQ(view["string"].get_string().value, string);
    const auto intArrayView = view["intArray"].get_array().value;
    for (size_t i = 0; i < intArray.size(); ++i)
    {
        ASSERT_EQ(intArray[i], intArrayView[i].get_int32().value);
    }
    const auto stringArrayView = view["stringArray"].get_array().value;
    for (size_t i = 0; i < stringArray.size(); ++i)
    {
        ASSERT_EQ(stringArray[i], std::string(stringArrayView[i].get_string().value));
    }
    ASSERT_EQ(view["optionalInt"].get_int32().value, optionalInt.value());
    ASSERT_EQ(view["optionalString"].get_string().value, optionalString.value());
}

TEST(DeserializationTest, DeserializeMembers)
{
    bsoncxx::builder::basic::document doc{};

    bsoncxx::oid id = bsoncxx::oid();
    bool boolean = true;
    int integer = 42;
    double floatingPoint = 3.14;
    std::string string = "Hello, World!";
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    std::vector<std::string> stringArray = {"one", "two", "three", "four", "five"};
    std::optional<int> optionalInt = 42;
    std::optional<std::string> optionalString = "Optional String";

    doc.append(bsoncxx::builder::basic::kvp("id", id));
    doc.append(bsoncxx::builder::basic::kvp("boolean", boolean));
    doc.append(bsoncxx::builder::basic::kvp("integer", integer));
    doc.append(bsoncxx::builder::basic::kvp("floatingPoint", floatingPoint));
    doc.append(bsoncxx::builder::basic::kvp("string", string));

    bsoncxx::builder::basic::array intArrayBuilder;
    for (const auto& elem : intArray)
    {
        intArrayBuilder.append(elem);
    }
    doc.append(bsoncxx::builder::basic::kvp("intArray", intArrayBuilder));

    bsoncxx::builder::basic::array stringArrayBuilder;
    for (const auto& elem : stringArray)
    {
        stringArrayBuilder.append(elem);
    }
    doc.append(bsoncxx::builder::basic::kvp("stringArray", stringArrayBuilder));

    if (optionalInt)
    {
        doc.append(bsoncxx::builder::basic::kvp("optionalInt", *optionalInt));
    }
    if (optionalString)
    {
        doc.append(bsoncxx::builder::basic::kvp("optionalString", *optionalString));
    }

    auto view = doc.view();

    bsoncxx::oid deserializedId;
    bool deserializedBoolean;
    int deserializedInteger;
    double deserializedFloatingPoint;
    std::string deserializedString;
    std::vector<int> deserializedIntArray;
    std::vector<std::string> deserializedStringArray;
    std::optional<int> deserializedOptionalInt;
    std::optional<std::string> deserializedOptionalString;

    deserializeMember(deserializedId, view, "id");
    deserializeMember(deserializedBoolean, view, "boolean");
    deserializeMember(deserializedInteger, view, "integer");
    deserializeMember(deserializedFloatingPoint, view, "floatingPoint");
    deserializeMember(deserializedString, view, "string");
    deserializeMember(deserializedIntArray, view, "intArray");
    deserializeMember(deserializedStringArray, view, "stringArray");

    if (view.find("optionalInt") != view.end())
    {
        deserializeMember(deserializedOptionalInt, view, "optionalInt");
    }
    if (view.find("optionalString") != view.end())
    {
        deserializeMember(deserializedOptionalString, view, "optionalString");
    }

    ASSERT_EQ(id, deserializedId);
    ASSERT_EQ(boolean, deserializedBoolean);
    ASSERT_EQ(integer, deserializedInteger);
    ASSERT_EQ(floatingPoint, deserializedFloatingPoint);
    ASSERT_EQ(string, deserializedString);
    ASSERT_EQ(intArray, deserializedIntArray);
    ASSERT_EQ(stringArray, deserializedStringArray);
    ASSERT_EQ(optionalInt, deserializedOptionalInt);
    ASSERT_EQ(optionalString, deserializedOptionalString);
}

TEST(OptionalClassTest, Serialization)
{
    struct OptionalClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;
        std::optional<std::vector<std::string>> optionalStringArray;
        std::optional<Inner> optionalInner;

        BSON_DEFINE_TYPE(OptionalClass, optionalInt, optionalString, optionalStringArray, optionalInner)
    };

    OptionalClass optionalClass;
    optionalClass.optionalInt = 42;
    optionalClass.optionalString = "Optional String";
    optionalClass.optionalStringArray = std::vector<std::string>{"one", "two", "three", "four", "five"};
    optionalClass.optionalInner = OptionalClass::Inner{42, 24};

    const auto bson = OptionalClass::toBSON(optionalClass);
    auto view = bson.view();

    ASSERT_EQ(optionalClass.optionalInt, view["optionalInt"].get_int32().value);
    ASSERT_EQ(optionalClass.optionalString, view["optionalString"].get_string().value);
    const auto optionalVectorString = bson["optionalStringArray"].get_array().value;
    for (size_t i = 0; i < optionalClass.optionalStringArray->size(); ++i)
    {
        ASSERT_EQ(optionalClass.optionalStringArray.value()[i], std::string(optionalVectorString[i].get_string().value));
    }
    const auto optionalInner = bson["optionalInner"].get_document().value;
    ASSERT_EQ(optionalClass.optionalInner->x, optionalInner["x"].get_int32().value);
    ASSERT_EQ(optionalClass.optionalInner->y, optionalInner["y"].get_int32().value);
}

TEST(OptionalClassTest, Deserialization)
{
    struct OptionalClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;
        std::optional<std::vector<std::string>> optionalStringArray;
        std::optional<Inner> optionalInner;

        BSON_DEFINE_TYPE(OptionalClass, optionalInt, optionalString, optionalStringArray, optionalInner)
    };

    OptionalClass optionalClass;
    optionalClass.optionalInt = 42;
    optionalClass.optionalString = "Optional String";
    optionalClass.optionalStringArray = std::vector<std::string>{"one", "two", "three", "four", "five"};
    optionalClass.optionalInner = OptionalClass::Inner{42, 24};

    const auto bson = OptionalClass::toBSON(optionalClass);
    const auto deserialized = OptionalClass::fromBSON(bson);

    ASSERT_EQ(optionalClass.optionalInt, deserialized.optionalInt);
    ASSERT_EQ(optionalClass.optionalString, deserialized.optionalString);
    ASSERT_EQ(optionalClass.optionalStringArray, deserialized.optionalStringArray);
    ASSERT_EQ(optionalClass.optionalInner->x, deserialized.optionalInner->x);
    ASSERT_EQ(optionalClass.optionalInner->y, deserialized.optionalInner->y);
}

TEST(Optional, Serialization)
{
    struct OptionalClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;
        std::optional<std::vector<std::string>> optionalStringArray;
        std::optional<Inner> optionalInner;

        BSON_DEFINE_TYPE(OptionalClass, optionalInt, optionalString, optionalStringArray, optionalInner)
    };

    OptionalClass optionalClass{};
    const auto bson = OptionalClass::toBSON(optionalClass);

    ASSERT_EQ(bson["optionalInt"].type(), bsoncxx::type::k_null);
    ASSERT_EQ(bson["optionalString"].type(), bsoncxx::type::k_null);
    ASSERT_EQ(bson["optionalStringArray"].type(), bsoncxx::type::k_null);
    ASSERT_EQ(bson["optionalInner"].type(), bsoncxx::type::k_null);
}

TEST(Optional, Deserialization)
{
    struct OptionalClass
    {
        struct Inner
        {
            int x;
            int y;

            BSON_DEFINE_TYPE(Inner, x, y)
        };

        std::optional<int> optionalInt;
        std::optional<std::string> optionalString;
        std::optional<std::vector<std::string>> optionalStringArray;
        std::optional<Inner> optionalInner;

        BSON_DEFINE_TYPE(OptionalClass, optionalInt, optionalString, optionalStringArray, optionalInner)
    };

    OptionalClass optionalClass{};
    const auto bson = OptionalClass::toBSON(optionalClass);
    const auto deserialized = OptionalClass::fromBSON(bson);

    ASSERT_EQ(deserialized.optionalInt, std::nullopt);
    ASSERT_EQ(deserialized.optionalString, std::nullopt);
    ASSERT_EQ(deserialized.optionalStringArray, std::nullopt);
    ASSERT_EQ(deserialized.optionalInner, std::nullopt);

}