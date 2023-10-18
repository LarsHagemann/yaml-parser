## C++ YAML Parser

This is a simple C++ yaml parser that let's you define the expected structure before parsing. 
This is not a strict yaml implementation. 

## Issues

- Strings have to be quoted with `"`
- Lists only support the `- ` Syntax and not the `[]` Syntax

## Usage

This is a basic usage example. For more exampels, have a look at the tests. 

```C++
#include <iostream>
#include <yaml-parser.hpp>

using namespace yaml;

int main(void) {
    const auto yaml = R"(
Server:
    AllowedClients:
        Ips:
            - "127.0.0.1"
            - "168.0.0.1"
            - "localhost"
    NumberOfCores: 4
    CorsPolicy: 
        - "https://example.com"
        - "https://example.org"    
    )";

    const auto serverTemplate = YamlNamed("Server", YamlObject({
        new YamlNamed("AllowedClients", YamlObject({
            new YamlNamed("Ips", YamlList(YamlString()))
        })),
        new YamlNamed("NumberOfCores", YamlInt<8>()),
        new YamlNamed("CorsPolicy", YamlList(YamlString()))
    }));

    // The following line throws on errors
    auto value = YamlParse(serverTemplate, yaml);

    for (const auto& client : value["Server"]["AllowedClients"]["Ips"].AsList()) {
        std::cout << client.AsString() << '\n';
    }
}
```

## Tests

The tests can be build with the provided CMake file. 
