# cpp-bson-convert

`cpp-bson-convert` is a modern BSON serialization/deserialization library for C++ (17+). It provides utilities for serializing and deserializing C++ objects to and from BSON using the MongoDB C++ driver. This library simplifies the process of converting complex C++ objects, including nested objects and collections, to BSON format and vice versa.

# Table of Contents

* [Installation](#installation)
* [Important Notes](#important-notes)
* [Usage](#usage)
    * [Defining BSON Serialization and Deserialization](#defining-bson-serialization-and-deserialization)
    * [Nested Objects](#nested-objects)
    * [Manual Serialization and Deserialization](#manual-serialization-and-deserialization)
* [Examples](#examples)
* [License](#license)
* [Contact](#contact)

## Installation

This is a header-only library. So actually, all you need is to add the `cpp-bson-convert.hpp` file in the `src` folder to your project and start using it with `#include`.

But this library needs the `mongocxx` library, of course. So, you also need to add `mongocxx` to your project before using it.

To include this library in your project, add the following to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.14)

project(myProject)

add_executable(myProject main.cpp)

find_package(mongocxx CONFIG REQUIRED)

target_link_libraries(myProject PRIVATE mongo::bsoncxx_static mongo::mongocxx_static)
```

## Usage
### Defining BSON Serialization and Deserialization
To serialize and deserialize a C++ object to and from BSON, use the BSON_DEFINE_TYPE macro. This macro generates the necessary functions for converting the object.

> [!IMPORTANT]
> The use of the `BSON_DEFINE_TYPE` macro only allows the serialization and deserialization of the properties you specify. Anything not included in this macro cannot be serialized or deserialized.

> [!IMPORTANT]
> If you want to use the `BSON_DEFINE_TYPE` macro, your classes must have default constructors.

```cpp
struct MyClass {
    int id;
    std::string name;

    BSON_DEFINE_TYPE(MyClass, id, name)
};
```

After that, you can serialize and deserialize the object using the toBson and fromBson functions.

```cpp
MyClass obj;
obj.id = 1;
obj.name = "Test";

auto bson = MyClass::toBSON(obj);
auto deserializedObj = MyClass::fromBSON(bson);
```


### Nested Objects
If your object contains nested objects, you can use the BSON_DEFINE_TYPE macro for the nested objects as well.

```cpp
struct OuterClass {
    struct InnerClass {
        int x;
        int y;

        BSON_DEFINE_TYPE(InnerClass, x, y)
    };

    InnerClass inner;
    std::string name;

    BSON_DEFINE_TYPE(OuterClass, inner, name)
};
```

### Manual Serialization and Deserialization
If you prefer not to use the BSON_DEFINE_TYPE macro, you can manually serialize and deserialize members using the serializeMember and deserializeMember functions.

```cpp
bsoncxx::builder::basic::document doc{};

int id = 42;
std::string name = "Example";

serializeMember(doc, "id", id);
serializeMember(doc, "name", name);

auto view = doc.view();

int deserializedId;
std::string deserializedName;

deserializeMember(deserializedId, view, "id");
deserializeMember(deserializedName, view, "name");
```

## License
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

## Contact
If you have problems regarding the library, please open an [issue on GitHub](https://github.com/sertaceker/cpp-bson-convert/issues). Please describe your request, issue, or question in as much detail as possible and also include the version of your compiler and operating system, as well as the version of the library you are using. Before opening a new issue, please confirm that the topic is not already exists in closed issues.