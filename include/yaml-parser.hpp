#ifndef YAML_PARSER_HPP
#define YAML_PARSER_HPP

#include "yaml-types.hpp"

#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <optional>
#include <functional>
#include <stdexcept>
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
#include <format>
#endif
#include <filesystem>
#include <fstream>

namespace yaml {

class YamlError : public std::exception {
private:
    std::string m_error;
public:
    std::string m_Message;
    uint32_t    m_Line;
    uint64_t    m_Column;

    YamlError(uint32_t line, uint64_t column, const std::string& message)
        : m_Line(line), m_Column(column), m_Message(message) {

#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
        m_error = std::format(
            "Yaml error at line {}, column {}: {}",
            m_Line + 1, m_Column, m_Message
        );
#else
        m_error = "Yaml error at line " + std::to_string(m_Line + 1) + ", column " + std::to_string(m_Column) + ": " + m_Message;
#endif
    }

    const char* what() const noexcept override {
        return m_error.c_str();
    }
};

enum class YamlTokenType {
    YAML_IDENTIFIER,
    YAML_COLON,
    YAML_STRING,
    YAML_NUMBER,
    YAML_MINUS,

    YAML_EOF
};

struct YamlToken {
    YamlTokenType Type;
    std::string   Lexeme;
    uint32_t      BeginColumn;
};

class YamlScanner {
private:
    std::string m_Yaml;
    uint64_t    m_Offset = 0u;
    uint64_t    m_LineOffset = 0u;
    uint32_t    m_Line = 0u;
    uint64_t    m_Begin = 0u;
    uint32_t    m_Indentation = 0u;
private:
    bool eof() const {
        return m_Offset >= m_Yaml.size();
    }
    char advance() {
        if (eof()) {
            return '\0';
        }

        if (m_Yaml[m_Offset] == '\n') {
            m_LineOffset = m_Offset + 1;
            m_Line++;
            m_Indentation = 0;
        }
        else {
            m_Indentation++;
        }
        
        m_Offset++;
        return m_Yaml[m_Offset - 1];
    }
    void skipWhitespace() {
        while (!eof() && std::isspace(m_Yaml[m_Offset])) {
            advance();
        }
        if (eof()) {
            m_Indentation = 0;
        }
        m_Begin = m_Offset;
    }
    char current() {
        if (eof()) {
            return '\0';
        }
        return m_Yaml[m_Offset];
    }
    char next() {
        if (m_Offset + 1 >= m_Yaml.size()) {
            return '\0';
        }
        return m_Yaml[m_Offset + 1];
    }
    YamlToken makeToken(YamlTokenType type) {
        YamlToken token;
        token.Type = type;
        token.Lexeme = m_Yaml.substr(m_Begin, m_Offset - m_Begin);
        token.BeginColumn = static_cast<uint32_t>(m_Begin - m_LineOffset);
        m_Begin = m_Offset;
        skipWhitespace();
        return token;
    }
    YamlToken makeString() {
        // "([^"]|\\")*"
        while (current() != '"') {
            if (current() == '\\') {
                advance();
            }
            advance();
        }
        advance();
        return makeToken(YamlTokenType::YAML_STRING);
    }
    YamlToken makeNumber() {
        // [1-9][0-9]*(\.[0-9]*)?([eE][+-]?[0-9]+)?
        while (std::isdigit(current())) {
            advance();
        }
        if (current() == '.') {
            advance();
            while (std::isdigit(current())) {
                advance();
            }
        }
        if (current() == 'e' || current() == 'E') {
            advance();
            if (current() == '+' || current() == '-') {
                advance();
            }
            while (std::isdigit(current())) {
                advance();
            }
        }
        return makeToken(YamlTokenType::YAML_NUMBER);
    }
    YamlToken makeIdentifier() {
        // [a-zA-Z_][a-zA-Z0-9_]*
        while (std::isalnum(current()) || current() == '_') {
            advance();
        }
        return makeToken(YamlTokenType::YAML_IDENTIFIER);
    }
public:
    YamlScanner(const std::string& yaml)
        : m_Yaml(yaml) {}

    uint32_t indentation() const {
        return m_Indentation;
    }
    uint64_t offset() const {
        return m_Offset;
    }
    uint32_t line() const {
        return m_Line;
    }
    uint64_t column() const {
        return m_Offset - m_LineOffset;
    }
    uint64_t columnBegin() const {
        return m_Begin - m_LineOffset;
    }

    YamlToken scan() {
        skipWhitespace();

        if (eof()) {
            return { YamlTokenType::YAML_EOF, "" };
        }

        auto c = advance();
        switch (c) {
        case ':': return makeToken(YamlTokenType::YAML_COLON);
        case '"': return makeString();
        case '#': {
            while (current() != '\n') {
                advance();
            }
            return scan();
        }
        case '-': {
            if (std::isdigit(current())) {
                return makeNumber();
            }
            else {
                return makeToken(YamlTokenType::YAML_MINUS);
            }
        }
        default:
            if (std::isdigit(c) || c == '-') {
                return makeNumber();
            }
            else if (std::isalpha(c)) {
                return makeIdentifier();
            }
            else {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
                throw YamlError(m_Line, m_Offset - m_LineOffset, std::format("Unexpected character '{}'", c));
#else
                throw YamlError(m_Line, m_Offset - m_LineOffset, "Unexpected character '" + std::string(1, c) + "'");
#endif
            }
        }
    }
};

namespace detail {

static YamlToken ExpectTokenType(YamlTokenType type, YamlScanner& scanner) {
    auto token = scanner.scan();
    if (token.Type != type) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
        throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Expected token type {}, got {}", static_cast<int32_t>(type), static_cast<int32_t>(token.Type)));
#else
        throw YamlError(scanner.line(), scanner.columnBegin(), "Expected token type " + std::to_string(static_cast<int32_t>(type)) + ", got " + std::to_string(static_cast<int32_t>(token.Type)));
#endif
    }
    return token;
}

static std::string ToLowerCase(const std::string& in) {
    std::string out;
    out.reserve(in.size());
    for (auto c : in) {
        out += std::tolower(c);
    }
    return out;
}

}

template<typename TValue>
class YamlParser {
public:
    virtual ~YamlParser() = default;

    virtual TValue Parse(YamlScanner& scanner) = 0;
};

class YamlStringParser;
template<uint32_t N>
class YamlIntParser;
template<uint32_t N>
class YamlUIntParser;
class YamlFloatParser;
class YamlDoubleParser;
class YamlBoolParser;

template<typename TValue, typename TParser>
class YamlLeafItem {
public:
    YamlLeafItem() = default;
    virtual ~YamlLeafItem() = default;

    using value_type = TValue;

    YamlValue Parse(YamlScanner& scanner) const {
        return TParser{}.Parse(scanner);
    }
};

template<uint32_t N>
using YamlIntType = std::conditional_t<
    N == 8, int8_t,
    std::conditional_t<
        N == 16, int16_t,
        std::conditional_t<
            N == 32, int32_t,
            std::conditional_t<
                N == 64, int64_t,
                void
            >
        >
    >
>;
template<uint32_t N>
using YamlUIntType = std::conditional_t<
    N == 8, uint8_t,
    std::conditional_t<
        N == 16, uint16_t,
        std::conditional_t<
            N == 32, uint32_t,
            std::conditional_t<
                N == 64, uint64_t,
                void
            >
        >
    >
>;

using YamlString = YamlLeafItem<std::string, YamlStringParser>;
template<uint32_t N>
using YamlInt = YamlLeafItem<YamlIntType<N>, YamlIntParser<N>>;
template<uint32_t N>
using YamlUInt = YamlLeafItem<YamlUIntType<N>, YamlUIntParser<N>>;
using YamlFloat = YamlLeafItem<float, YamlFloatParser>;
using YamlDouble = YamlLeafItem<double, YamlDoubleParser>;
using YamlBool = YamlLeafItem<bool, YamlBoolParser>;

class YamlEnumParser {
public:
    YamlEnumParser(const std::vector<std::string>& values)
        : Values(values) {}

    std::vector<std::string> Values;
    YamlValue Parse(YamlScanner& scanner) const {
        auto token = scanner.scan();
        if (token.Type != YamlTokenType::YAML_IDENTIFIER && token.Type != YamlTokenType::YAML_STRING) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
            throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Expected string, got {}", token.Lexeme));
#else
            throw YamlError(scanner.line(), scanner.columnBegin(), "Expected string, got " + token.Lexeme);
#endif
        }
        const auto lexeme = detail::ToLowerCase(token.Lexeme);
        for (auto& value : Values) {
            if (lexeme == detail::ToLowerCase(value)) {
                return YamlValue(value);
            }
        }
        std::string oneOf = "[";
        for (auto i = 0u; i < Values.size() - 1; ++i) {
            oneOf += Values[i] + ", ";
        }
        oneOf += Values[Values.size() - 1] + "]";
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
        throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Expected one of {}, got {}", oneOf, token.Lexeme));
#else
        throw YamlError(scanner.line(), scanner.columnBegin(), "Expected one of " + oneOf + ", got " + token.Lexeme);
#endif
    }
};

class YamlEnum {
public:
    YamlEnum(std::initializer_list<std::string> values)
        : Values(values) {}

    std::vector<std::string> Values;

    YamlValue Parse(YamlScanner& scanner) const {
        return YamlEnumParser{ Values }.Parse(scanner);
    }
};

class YamlItem;

class YamlListParser {
public:
    YamlListParser(YamlItem* item, uint32_t intendation);

    YamlItem*       Item;
    uint32_t        Intendation;
    YamlValue Parse(YamlScanner& scanner) const;
};

class YamlList {
public:
    YamlList(YamlItem&& item);

    std::unique_ptr<YamlItem> Item;

    YamlValue Parse(YamlScanner& scanner) const;
};

class YamlNamed;

class YamlObjectParser {
public:
    YamlObjectParser(const std::vector<std::unique_ptr<YamlNamed>>* items, uint32_t intendation)
        : Items(std::move(items)), Intendation(intendation) {}

    const std::vector<
        std::unique_ptr<YamlNamed>>* Items;
    uint32_t                Intendation;

    YamlValue Parse(YamlScanner& scanner) const;
};

class YamlObject {
public:
    YamlObject(std::vector<YamlNamed*>&& items);

    std::unique_ptr<std::vector<
        std::unique_ptr<YamlNamed>>> Items;

    YamlValue Parse(YamlScanner& scanner) const {
        return YamlObjectParser{ Items.get(), scanner.indentation() }.Parse(scanner);
    }
};

class YamlItem : public std::variant<
    YamlString,
    YamlInt<8>, YamlInt<16>, YamlInt<32>, YamlInt<64>,
    YamlUInt<8>, YamlUInt<16>, YamlUInt<32>, YamlUInt<64>,
    YamlFloat, YamlDouble,
    YamlBool,
    YamlList,
    YamlEnum,
    YamlObject>
{
public:
    using Parent = std::variant<
        YamlString,
        YamlInt<8>, YamlInt<16>, YamlInt<32>, YamlInt<64>,
        YamlUInt<8>, YamlUInt<16>, YamlUInt<32>, YamlUInt<64>,
        YamlFloat, YamlDouble,
        YamlBool,
        YamlList,
        YamlEnum,
        YamlObject>;

    YamlItem(YamlString&& value) : Parent(std::move(value)) {}
    YamlItem(YamlInt<8>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlInt<16>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlInt<32>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlInt<64>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlUInt<8>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlUInt<16>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlUInt<32>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlUInt<64>&& value) : Parent(std::move(value)) {}
    YamlItem(YamlFloat&& value) : Parent(std::move(value)) {}
    YamlItem(YamlDouble&& value) : Parent(std::move(value)) {}
    YamlItem(YamlBool&& value) : Parent(std::move(value)) {}
    YamlItem(YamlList&& value) : Parent(std::move(value)) {}
    YamlItem(YamlEnum&& value) : Parent(std::move(value)) {}
    YamlItem(YamlObject&& value) : Parent(std::move(value)) {}

    template<typename T>
    const T& As() const {
        return std::get<T>(*this);
    }

    YamlValue Parse(YamlScanner& scanner) const;
};

class YamlNamed {
public:
    YamlNamed(const std::string& name, YamlItem&& item, bool optional = false)
        : Name(name), Item(std::make_unique<YamlItem>(std::forward<YamlItem>(item))), Optional(optional) {} 

    YamlNamed(YamlNamed&& other)
        : Name(std::move(other.Name)), Item(std::move(other.Item)) {}

    std::unique_ptr<YamlItem> Item;
    std::string               Name;
    bool                      Optional = false;
};

class YamlStringParser {
private:
    std::string sanitizeString(const std::string& in) const {
        auto view = std::string_view(in.c_str() + 1, in.size() - 2);
        if (view.size() == 0) {
            return "";
        }
        std::string out;
        out.reserve(view.size());
        for (auto i = 0ull; i < view.size(); ++i) {
            if (view[i] == '\\') {
                switch (view[i + 1]) {
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                case 'v': out += '\v'; break;
                }
                ++i;
            }
            else {
                out += view[i];
            }
        }
        return out;
    }
public:
    std::string Parse(YamlScanner& scanner) const { 
        auto token = detail::ExpectTokenType(YamlTokenType::YAML_STRING, scanner);
        return sanitizeString(token.Lexeme);
    }
};
template<uint32_t N>
class YamlIntParser {
public:
    auto Parse(YamlScanner& scanner) const {
        static_assert(
            N == 8 || N == 16 || N == 32 || N == 64,
            "Invalid integer size"
        );

        auto token = detail::ExpectTokenType(YamlTokenType::YAML_NUMBER, scanner);
        if constexpr (N == 8) {
            return static_cast<int8_t>(std::stoi(token.Lexeme));
        }
        else if constexpr (N == 16) {
            return static_cast<int16_t>(std::stoi(token.Lexeme));
        }
        else if constexpr (N == 32) {
            return static_cast<int32_t>(std::stoi(token.Lexeme));
        }
        else if constexpr (N == 64) {
            return static_cast<int64_t>(std::stoll(token.Lexeme));
        }
    }
};
template<uint32_t N>
class YamlUIntParser {
public:
    auto Parse(YamlScanner& scanner) const {
        static_assert(
            N == 8 || N == 16 || N == 32 || N == 64,
            "Invalid integer size"
        );

        auto token = detail::ExpectTokenType(YamlTokenType::YAML_NUMBER, scanner);
        if constexpr (N == 8) {
            return static_cast<uint8_t>(std::stoul(token.Lexeme));
        }
        else if constexpr (N == 16) {
            return static_cast<uint16_t>(std::stoul(token.Lexeme));
        }
        else if constexpr (N == 32) {
            return static_cast<uint32_t>(std::stoul(token.Lexeme));
        }
        else if constexpr (N == 64) {
            return static_cast<uint64_t>(std::stoull(token.Lexeme));
        }
    }
};
class YamlFloatParser {
public:
    float Parse(YamlScanner& scanner) const {
        auto token = detail::ExpectTokenType(YamlTokenType::YAML_NUMBER, scanner);
        return std::stof(token.Lexeme);
    }
};
class YamlDoubleParser {
public:
    double Parse(YamlScanner& scanner) const {
        auto token = detail::ExpectTokenType(YamlTokenType::YAML_NUMBER, scanner);
        return std::stod(token.Lexeme);
    }
};
class YamlBoolParser {
public:
    bool Parse(YamlScanner& scanner) const {
        auto token = detail::ExpectTokenType(YamlTokenType::YAML_IDENTIFIER, scanner);
        const auto lexeme = detail::ToLowerCase(token.Lexeme);
        if (lexeme == "true") {
            return true;
        }
        else if (lexeme == "false") {
            return false;
        }
        else {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
            throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Expected boolean value, got {}", token.Lexeme));
#else
            throw YamlError(scanner.line(), scanner.columnBegin(), "Expected boolean value, got " + token.Lexeme);
#endif
        }
    }
};

inline YamlValue YamlListParser::Parse(YamlScanner& scanner) const {
    std::vector<YamlValue> value;
    while (scanner.indentation() >= Intendation) {
        detail::ExpectTokenType(YamlTokenType::YAML_MINUS, scanner);
        value.emplace_back(Item->Parse(scanner));
    }
    return value;
}

inline YamlValue YamlObjectParser::Parse(YamlScanner& scanner) const {
    std::unordered_map<std::string, YamlValue> value;
    std::unordered_map<
        std::string, uint32_t> parsedItems;
    while (scanner.indentation() >= Intendation) {
        auto identifier = detail::ExpectTokenType(YamlTokenType::YAML_IDENTIFIER, scanner);
        detail::ExpectTokenType(YamlTokenType::YAML_COLON, scanner);

        bool itemFound = false;
        for (auto i = 0ull; i < Items->size(); ++i) {
           auto& item = (*Items)[i];
           if (item->Name == identifier.Lexeme) {
               value.emplace(
                item->Name,
                item->Item->Parse(scanner));
                auto it = parsedItems.find(item->Name);
                if (it == parsedItems.end()) {
                    parsedItems.emplace(item->Name, 1);
                }
                else {
                    it->second++;
                }
                itemFound = true;
                break;
           }
        }
        if (!itemFound) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
            throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Unexpected identifier {}", identifier.Lexeme));
#else
            throw YamlError(scanner.line(), scanner.columnBegin(), "Unexpected identifier " + identifier.Lexeme);
#endif
        }
    }

    for (auto i = 0ull; i < Items->size(); ++i) {
        auto& item = (*Items)[i];
        auto it = parsedItems.find(item->Name);
        if (it == parsedItems.end() && !item->Optional) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
            throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Missing identifier {}", item->Name));
#else
            throw YamlError(scanner.line(), scanner.columnBegin(), "Missing identifier " + item->Name);
#endif
        }
        else if (it != parsedItems.end() && it->second > 1) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
            throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Duplicate identifier {}", item->Name));
#else
            throw YamlError(scanner.line(), scanner.columnBegin(), "Duplicate identifier " + item->Name);
#endif
        }
    }

    return value;
}

inline YamlValue YamlItem::Parse(YamlScanner& scanner) const {
    return std::visit([&](auto&& arg) -> YamlValue { return arg.Parse(scanner); }, *this);
}

inline YamlObject::YamlObject(std::vector<YamlNamed*>&& items) {
    Items = std::make_unique<std::vector<std::unique_ptr<YamlNamed>>>();
    for (auto i = 0ull; i < items.size(); ++i) {
        Items->emplace_back(std::unique_ptr<YamlNamed>(items[i]));
    }
}

inline YamlListParser::YamlListParser(YamlItem* item, uint32_t intendation)
    : Item(item), Intendation(intendation) {}

inline YamlList::YamlList(YamlItem&& item)
    : Item(std::make_unique<YamlItem>(std::forward<YamlItem>(item))) {}

inline YamlValue YamlList::Parse(YamlScanner& scanner) const {
    return YamlListParser{ Item.get(), scanner.indentation() }.Parse(scanner);
}

static YamlValue YamlParse(const YamlNamed& yamlTemplate, const std::string& yaml) {
    YamlScanner scanner(yaml);
    auto identifier = detail::ExpectTokenType(YamlTokenType::YAML_IDENTIFIER, scanner);
    detail::ExpectTokenType(YamlTokenType::YAML_COLON, scanner);

    if (identifier.Lexeme != yamlTemplate.Name) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
        throw YamlError(scanner.line(), scanner.columnBegin(), std::format("Expected identifier {}, got {}", yamlTemplate.Name, identifier.Lexeme));
#else
        throw YamlError(scanner.line(), scanner.columnBegin(), "Expected identifier " + yamlTemplate.Name + ", got " + identifier.Lexeme);
#endif
    }

    std::unordered_map<std::string, YamlValue> root;
    root.emplace(
        identifier.Lexeme,
        yamlTemplate.Item->Parse(scanner));
    return root;
}

static YamlValue YamlParseFile(const YamlNamed& yamlTemplate, const std::filesystem::path& filepath) {
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
#ifdef YAML_PARSER_ENABLE_FORMAT_LIB
        throw std::runtime_error(std::format("Could not open file {}", filepath.string()));
#else
        throw std::runtime_error("Could not open file " + filepath.string());
#endif
    }
    return YamlParse(yamlTemplate, std::string(
        std::istreambuf_iterator<char>(stream), 
        std::istreambuf_iterator<char>()));
}

}

#endif
