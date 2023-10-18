#include <catch2/catch_all.hpp>
#include <yaml-parser.hpp>

using namespace yaml;

TEST_CASE("Parses simple types") {
    const auto simpleInt8String = R"(Number: -123)";
    const auto simpleInt16String = R"(Number: -12345)";
    const auto simpleInt32String = R"(Number: -123456789)";
    const auto simpleInt64String = R"(Number: -1234567890123456789)";
    const auto simpleUint8String = R"(Number: 123)";
    const auto simpleUint16String = R"(Number: 12345)";
    const auto simpleUint32String = R"(Number: 123456789)";
    const auto simpleUint64String = R"(Number: 1234567890123456789)";

    const auto simpleFloatString = R"(Number: 123.456)";
    const auto simpleDoubleString = R"(Number: 123.4567890123456789)";

    const auto simpleBoolString = R"(Boolean: true)";

    const auto simpleStringString = R"(String: "Hello, \tWorld!")";

    const auto simpleInt8Template = YamlNamed("Number", YamlInt<8>());
    const auto simpleInt16Template = YamlNamed("Number", YamlInt<16>());
    const auto simpleInt32Template = YamlNamed("Number", YamlInt<32>());
    const auto simpleInt64Template = YamlNamed("Number", YamlInt<64>());
    const auto simpleUint8Template = YamlNamed("Number", YamlUInt<8>());
    const auto simpleUint16Template = YamlNamed("Number", YamlUInt<16>());
    const auto simpleUint32Template = YamlNamed("Number", YamlUInt<32>());
    const auto simpleUint64Template = YamlNamed("Number", YamlUInt<64>());

    const auto simpleFloatTemplate = YamlNamed("Number", YamlFloat());
    const auto simpleDoubleTemplate = YamlNamed("Number", YamlDouble());

    const auto simpleBoolTemplate = YamlNamed("Boolean", YamlBool());

    const auto simpleStringTemplate = YamlNamed("String", YamlString());

    SECTION("Parses simple int8") {
        auto value = YamlParse(simpleInt8Template, simpleInt8String);
        REQUIRE(value["Number"].AsInt8() == -123);
    }
    SECTION("Parses simple int16") {
        auto value = YamlParse(simpleInt16Template, simpleInt16String);
        REQUIRE(value["Number"].AsInt16() == -12345);
    }
    SECTION("Parses simple int32") {
        auto value = YamlParse(simpleInt32Template, simpleInt32String);
        REQUIRE(value["Number"].AsInt32() == -123456789);
    }
    SECTION("Parses simple int64") {
        auto value = YamlParse(simpleInt64Template, simpleInt64String);
        REQUIRE(value["Number"].AsInt64() == -1234567890123456789);
    }
    SECTION("Parses simple uint8") {
        auto value = YamlParse(simpleUint8Template, simpleUint8String);
        REQUIRE(value["Number"].AsUInt8() == 123);
    }
    SECTION("Parses simple uint16") {
        auto value = YamlParse(simpleUint16Template, simpleUint16String);
        REQUIRE(value["Number"].AsUInt16() == 12345);
    }
    SECTION("Parses simple uint32") {
        auto value = YamlParse(simpleUint32Template, simpleUint32String);
        REQUIRE(value["Number"].AsUInt32() == 123456789);
    }
    SECTION("Parses simple uint64") {
        auto value = YamlParse(simpleUint64Template, simpleUint64String);
        REQUIRE(value["Number"].AsUInt64() == 1234567890123456789);
    }

    SECTION("Parses simple float") {
        auto value = YamlParse(simpleFloatTemplate, simpleFloatString);
        REQUIRE(value["Number"].AsFloat() == 123.456f);
    }
    SECTION("Parses simple double") {
        auto value = YamlParse(simpleDoubleTemplate, simpleDoubleString);
        REQUIRE(value["Number"].AsDouble() == 123.4567890123456789);
    }

    SECTION("Parses simple bool") {
        auto value = YamlParse(simpleBoolTemplate, simpleBoolString);
        REQUIRE(value["Boolean"].AsBool() == true);
    }

    SECTION("Parses simple string") {
        auto value = YamlParse(simpleStringTemplate, simpleStringString);
        REQUIRE(value["String"].AsString() == "Hello, \tWorld!");
    }
}

TEST_CASE("Parses lists") {
    const auto simpleListString = R"(
List: 
    - 1
    - 2
    - 3
    - 4
    - 5)";
    const auto simpleListTemplate = YamlNamed("List", YamlList(YamlInt<8>()));

    SECTION("Parses simple list") {
        auto value = YamlParse(simpleListTemplate, simpleListString);
        REQUIRE(value["List"].AsList().size() == 5);
        REQUIRE(value["List"][0].AsInt8() == 1);
        REQUIRE(value["List"][1].AsInt8() == 2);
        REQUIRE(value["List"][2].AsInt8() == 3);
        REQUIRE(value["List"][3].AsInt8() == 4);
        REQUIRE(value["List"][4].AsInt8() == 5);
    }
}

TEST_CASE("Parses objects") {
    const auto simpleObjectString = R"(
Object:
    Item1: 1
    Item2: 2
    Item3: 3
    Item4: 4
    Item5: 5)";

    const auto simpleObjectTemplate = YamlNamed("Object", YamlObject({
        new YamlNamed("Item1", YamlInt<8>()),
        new YamlNamed("Item2", YamlInt<8>()),
        new YamlNamed("Item3", YamlInt<8>()),
        new YamlNamed("Item4", YamlInt<8>()),
        new YamlNamed("Item5", YamlInt<8>())
    }));

    SECTION("Parses simple object") {
        auto value = YamlParse(simpleObjectTemplate, simpleObjectString);
        REQUIRE(value["Object"]["Item1"].AsInt8() == 1);
        REQUIRE(value["Object"]["Item2"].AsInt8() == 2);
        REQUIRE(value["Object"]["Item3"].AsInt8() == 3);
        REQUIRE(value["Object"]["Item4"].AsInt8() == 4);
        REQUIRE(value["Object"]["Item5"].AsInt8() == 5);
    }
}

TEST_CASE("Parses enums") {
    const auto simpleEnumString = R"(
Enum: Value1)";

    const auto simpleEnumTemplate = YamlNamed("Enum", YamlEnum({ "Value1", "Value2", "Value3" }));

    SECTION("Parses simple enum") {
        auto value = YamlParse(simpleEnumTemplate, simpleEnumString);
        REQUIRE(value["Enum"].AsString() == "Value1");
    }
}

TEST_CASE("complex test case") {
    const auto complexString = R"(
RootObject:
    ChildObject1:
        ChildArray:
            - 1
            - 2
            - 3
        ChildString: "Hello, World!"
    ChildObject2:
        ChildArray2:
            - 4
            - 5
            - 6
        ChildNumber: 42
        ChildString2: "Hello, World!"
    )";

    const auto complexTemplate = YamlNamed("RootObject", YamlObject({
        new YamlNamed("ChildObject1", YamlObject({
            new YamlNamed("ChildArray", YamlList(YamlInt<8>())),
            new YamlNamed("ChildString", YamlString())
        })),
        new YamlNamed("ChildObject2", YamlObject({
            new YamlNamed("ChildArray2", YamlList(YamlInt<8>())),
            new YamlNamed("ChildNumber", YamlInt<8>()),
            new YamlNamed("ChildString2", YamlString())
        }))
    }));

    SECTION("Parses complex object") {
        auto value = YamlParse(complexTemplate, complexString);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"].AsList().size() == 3);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"][0].AsInt8() == 1);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"][1].AsInt8() == 2);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"][2].AsInt8() == 3);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildString"].AsString() == "Hello, World!");
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"].AsList().size() == 3);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"][0].AsInt8() == 4);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"][1].AsInt8() == 5);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"][2].AsInt8() == 6);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildNumber"].AsInt8() == 42);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildString2"].AsString() == "Hello, World!");
    }
}

TEST_CASE("Fails if object key is not provided") {
    const auto simpleObjectString = R"(
Object:
    Key1: 1
    )";

    const auto simpleObjectTemplate = YamlNamed("Object", YamlObject({
        new YamlNamed("Key1", YamlInt<8>()),
        new YamlNamed("Key2", YamlInt<8>())
    }));

    SECTION("Fails if object key is not provided") {
        REQUIRE_THROWS(YamlParse(simpleObjectTemplate, simpleObjectString));
    }
}


TEST_CASE("Does not fail, if key is optional") {
    const auto simpleObjectString = R"(
Object:
    Key1: 1
    )";

    const auto simpleObjectTemplate = YamlNamed("Object", YamlObject({
        new YamlNamed("Key1", YamlInt<8>()),
        new YamlNamed("Key2", YamlInt<8>(), true)
    }));

    SECTION("Does not fail, if key is optional") {
        REQUIRE_NOTHROW(YamlParse(simpleObjectTemplate, simpleObjectString));
    }
}

TEST_CASE("Fails if key is provided multiple times") {
    const auto simpleObjectString = R"(
Object:
    Key1: 1
    Key1: 2
    )";

    const auto simpleObjectTemplate = YamlNamed("Object", YamlObject({
        new YamlNamed("Key1", YamlInt<8>()),
        new YamlNamed("Key2", YamlInt<8>())
    }));

    SECTION("Fails if key is provided multiple times") {
        REQUIRE_THROWS(YamlParse(simpleObjectTemplate, simpleObjectString));
    }
}

TEST_CASE("Fails on wrong types") {
    const auto simpleObjectString = R"(
Object:
    Key1: 1
    Key2: "Hello, World!"
    )";

    const auto simpleObjectTemplate = YamlNamed("Object", YamlObject({
        new YamlNamed("Key1", YamlInt<8>()),
        new YamlNamed("Key2", YamlInt<16>())
    }));

    SECTION("Fails on wrong types") {
        REQUIRE_THROWS(YamlParse(simpleObjectTemplate, simpleObjectString));
    }
}

TEST_CASE("Fails on wrong enum values") {
    const auto simpleObjectString = R"(
Object:
    Key1: Value1
    )";

    const auto simpleObjectTemplate = YamlNamed("Object", YamlObject({
        new YamlNamed("Key1", YamlEnum({ "Value2", "Value3" }))
    }));

    SECTION("Fails on wrong enum values") {
        REQUIRE_THROWS(YamlParse(simpleObjectTemplate, simpleObjectString));
    }
}

TEST_CASE("Fails on syntax error") {
    SECTION("Fails on syntax error") {
        REQUIRE_THROWS(YamlParse(YamlNamed("Object", YamlInt<8>({})), "$Object: 1\n"));
    }
    SECTION("Fails on syntax error") {
        REQUIRE_THROWS(YamlParse(YamlNamed("Object", YamlInt<8>({})), "Object = 1\n"));
    }
    SECTION("Fails on wrong root node") {
        REQUIRE_THROWS(YamlParse(YamlNamed("Object", YamlInt<8>({})), "Object_Wrong: 1\n"));
    }
}

TEST_CASE("Comments work") {
    const auto complexString = R"(
# This is a comment. It should be ignored.
RootObject:
    ChildObject1:
        ChildArray:
            - 1
            - 2
            # This is a comment. It should be ignored.
            - 3
        ChildString: "Hello, World!"
    ChildObject2:
        ChildArray2:
            - 4
            - 5
            - 6
        # Another comment.
        ChildNumber: 42
        ChildString2: "Hello, World!"
    )";

    const auto complexTemplate = YamlNamed("RootObject", YamlObject({
        new YamlNamed("ChildObject1", YamlObject({
            new YamlNamed("ChildArray", YamlList(YamlInt<8>())),
            new YamlNamed("ChildString", YamlString())
        })),
        new YamlNamed("ChildObject2", YamlObject({
            new YamlNamed("ChildArray2", YamlList(YamlInt<8>())),
            new YamlNamed("ChildNumber", YamlInt<8>()),
            new YamlNamed("ChildString2", YamlString())
        }))
    }));

    SECTION("Comments work") {
        auto value = YamlParse(complexTemplate, complexString);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"].AsList().size() == 3);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"][0].AsInt8() == 1);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"][1].AsInt8() == 2);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildArray"][2].AsInt8() == 3);
        REQUIRE(value["RootObject"]["ChildObject1"]["ChildString"].AsString() == "Hello, World!");
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"].AsList().size() == 3);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"][0].AsInt8() == 4);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"][1].AsInt8() == 5);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildArray2"][2].AsInt8() == 6);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildNumber"].AsInt8() == 42);
        REQUIRE(value["RootObject"]["ChildObject2"]["ChildString2"].AsString() == "Hello, World!");
    }
}
