#ifndef YAML_TYPES_HPP
#define YAML_TYPES_HPP

#include <variant>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace yaml {

class YamlValue {
private:
    std::variant<
        bool,
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        uint8_t,
        uint16_t,
        uint32_t,
        uint64_t,
        float,
        double,
        std::string,
        std::vector<YamlValue>,
        std::unordered_map<std::string, YamlValue>
    > m_Value;
    YamlValue() = default;
public:
    template<typename T>
    YamlValue(const T& value) {
        Set(value);
    }
    
    template<typename T>
    void Set(const T& value) {
        if constexpr (std::is_same_v<T, bool>) {
            m_Value.emplace<0>(value);
        }
        else if constexpr (std::is_same_v<T, int8_t>) {
            m_Value.emplace<1>(value);
        }
        else if constexpr (std::is_same_v<T, int16_t>) {
            m_Value.emplace<2>(value);
        }
        else if constexpr (std::is_same_v<T, int32_t>) {
            m_Value.emplace<3>(value);
        }
        else if constexpr (std::is_same_v<T, int64_t>) {
            m_Value.emplace<4>(value);
        }
        else if constexpr (std::is_same_v<T, uint8_t>) {
            m_Value.emplace<5>(value);
        }
        else if constexpr (std::is_same_v<T, uint16_t>) {
            m_Value.emplace<6>(value);
        }
        else if constexpr (std::is_same_v<T, uint32_t>) {
            m_Value.emplace<7>(value);
        }
        else if constexpr (std::is_same_v<T, uint64_t>) {
            m_Value.emplace<8>(value);
        }
        else if constexpr (std::is_same_v<T, float>) {
            m_Value.emplace<9>(value);
        }
        else if constexpr (std::is_same_v<T, double>) {
            m_Value.emplace<10>(value);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            m_Value.emplace<11>(value);
        }
        else if constexpr (std::is_same_v<T, std::vector<YamlValue>>) {
            m_Value.emplace<12>(value);
        }
        else if constexpr (std::is_same_v<T, std::unordered_map<std::string, YamlValue>>) {
            m_Value.emplace<13>(value);
        }
        else {
            throw std::runtime_error("Invalid type");
        }
    }

    bool AsBool() const {
        return std::get<bool>(m_Value);
    }
    int8_t AsInt8() const {
        return std::get<int8_t>(m_Value);
    }
    int16_t AsInt16() const {
        return std::get<int16_t>(m_Value);
    }
    int32_t AsInt32() const {
        return std::get<int32_t>(m_Value);
    }
    int64_t AsInt64() const {
        return std::get<int64_t>(m_Value);
    }
    uint8_t AsUInt8() const {
        return std::get<uint8_t>(m_Value);
    }
    uint16_t AsUInt16() const {
        return std::get<uint16_t>(m_Value);
    }
    uint32_t AsUInt32() const {
        return std::get<uint32_t>(m_Value);
    }
    uint64_t AsUInt64() const {
        return std::get<uint64_t>(m_Value);
    }
    float AsFloat() const {
        return std::get<float>(m_Value);
    }
    double AsDouble() const {
        return std::get<double>(m_Value);
    }
    const std::string& AsString() const {
        return std::get<std::string>(m_Value);
    }
    const std::vector<YamlValue>& AsList() const {
        return std::get<std::vector<YamlValue>>(m_Value);
    }
    const std::unordered_map<std::string, YamlValue>& AsObject() const {
        return std::get<std::unordered_map<std::string, YamlValue>>(m_Value);
    }

    const YamlValue& operator[](const std::string& key) const {
        return AsObject().at(key);
    }
    const YamlValue& operator[](size_t index) const {
        return AsList().at(index);
    }

    bool HasKey(const std::string& key) const {
        return AsObject().find(key) != AsObject().end();
    }
};

}

#endif