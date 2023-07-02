#pragma once


// Extends the implemented uniary -, binary + and binary * operators
template <typename Base, typename SubType>
class AritmeticExtension {
  public:

  Base operator-(const Base& other) const {
    return static_cast<const Base&>(*this) + (-other);
  }

  Base& operator*=(const Base& other) {
    return static_cast<Base&>(*this) = static_cast<Base&>(*this) * other;
  }

  Base& operator+=(const Base& other) {
    return static_cast<Base&>(*this) = static_cast<Base&>(*this) + other;
  }

  Base& operator-=(const Base& other) {
    return static_cast<Base&>(*this) = static_cast<Base&>(*this) - other;
  }

  Base& operator+(const SubType& other) const {
    return static_cast<const Base&>(*this) + Base{other};
  }

  Base& operator*(const SubType& other) const {
    return static_cast<const Base&>(*this) * Base{other};
  }

  Base& operator-(const SubType& other) const {
    return static_cast<const Base&>(*this) - Base{other};
  }

  Base& operator+=(const SubType& other) {
    return static_cast<Base&>(*this) += Base{other};
  }

  Base& operator-=(const SubType& other) {
    return static_cast<Base&>(*this) -= Base{other};
  }

  Base& operator*=(const SubType& other) {
    return static_cast<Base&>(*this) *= Base{other};
  }

};


// Vector extension
// The vector must have the following methods:
// 1. operator+(const VectorType& other) const
// 2. operator*(const ScalarType& scalar) const
// 3. operator-()

template <typename VectorType, typename ScalarType>
class VectorExtension {
  public:
    VectorType operator-(const VectorType& other) const {
      return static_cast<const VectorType&>(*this) + (-other);
    }

    VectorType& operator+=(const VectorType& other) {
      return static_cast<VectorType&>(*this) = static_cast<VectorType&>(*this) + other;
    }

    VectorType& operator*=(const ScalarType& scalar) {
      return static_cast<VectorType&>(*this) = static_cast<VectorType&>(*this) * scalar;
    }

    VectorType& operator-=(const VectorType& other) {
      return static_cast<VectorType&>(*this) = static_cast<VectorType&>(*this) - other;
    }
};
