#pragma once

#include <iostream>

// In the internal representation the value is expected to be in the range (-Mod, Mod)
template <typename DataType, DataType Mod, typename ExtendedType = DataType>
class ModInt : public AritmeticExtension<ModInt<DataType, Mod, ExtendedType>, DataType> {
  public:
    explicit ModInt() : value{} {}
    explicit ModInt(DataType value) :value{value % Mod} {}

    ModInt operator-() const {
      return ModInt{value - Mod};
    }

    ModInt operator+(const ModInt& other) const {
      DataType result = value + other.value;
      if(result >= Mod) result -= Mod;

      return ModInt{result};
    }

    ModInt operator*(const ModInt& other) const {
      ExtendedType result = static_cast<ExtendedType>(value) * static_cast<ExtendedType>(other.value);
      if(result >= Mod) result %= static_cast<ExtendedType>(Mod);

      return ModInt{static_cast<DataType>(result)};
    }

    [[nodiscard]] DataType GetValue() const { return (value < 0 ? value + Mod : value); }

  private:
    DataType value;
};

template <typename DataType, DataType Mod, typename ExtendedType>
std::ostream& operator<<(std::ostream& os, const ModInt<DataType, Mod, ExtendedType>& mod_int) {
  os << mod_int.GetValue();
  return os;
}
