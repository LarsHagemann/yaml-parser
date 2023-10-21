#include <catch2/catch_all.hpp>
#include <yaml-write.hpp>
#include <yaml-parser.hpp>

using namespace yaml;

TEST_CASE("Test writing of yaml values") {
    const auto object = YamlValue(types::Object{
        { "Server", YamlValue(
            types::List{
                YamlValue(types::Object{
                    { "Name", YamlValue(types::String("Test")) },
                    { "Port", YamlValue(types::UInt16(1234)) }
                }),
                YamlValue(types::Object{
                    { "Name", YamlValue(types::String("Test2")) },
                    { "Port", YamlValue(types::UInt16(1235)) }
                })
            }
        ) }
    });

    std::stringstream stream;
    YamlWrite(stream, object);

    const auto yamlTemplate = YamlNamed("Server", YamlList(YamlObject({
            new YamlNamed("Name", YamlString()),
            new YamlNamed("Port", YamlUInt<16>())
        })
    ));

    const auto yaml = YamlParse(yamlTemplate, stream.str());

    REQUIRE(yaml["Server"].AsList().size() == 2);
    REQUIRE(yaml["Server"].AsList()[0]["Name"].AsString() == "Test");
    REQUIRE(yaml["Server"].AsList()[0]["Port"].AsUInt16() == 1234);
    REQUIRE(yaml["Server"].AsList()[1]["Name"].AsString() == "Test2");
    REQUIRE(yaml["Server"].AsList()[1]["Port"].AsUInt16() == 1235);
}