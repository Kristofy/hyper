#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace hyper::script {

  
  template <typename T>
  class Vector {
    public:
      Vector() = default;
      Vector(const std::vector<T>& vec) : vec(vec) {}
      Vector(std::vector<T>&& vec) : vec(std::move(vec)) {}
      Vector(const std::initializer_list<T>& list) : vec(list) {}
      Vector(Vector&& other) : vec(std::move(other.vec)) {}

      Vector operator+(const Vector& other) const {
        std::vector<T> result = this->vec;
        result.insert(result.end(), other.vec.cbegin(), other.vec.cend());
        return Vector{result};
      }

      template <typename Fn, typename U = decltype(std::declval<Fn>()(std::declval<T>()))>
      Vector<U> Map(Fn fn) const {
        std::vector<U> result;
        result.reserve(this->vec.size());

        std::transform(this->vec.cbegin(), this->vec.cend(), std::back_inserter(result), fn);
        
        return Vector<U>{result};
      }

      template <typename Fn>
      Vector Filter(Fn fn) const {
        std::vector<T> result;
        result.reserve(this->vec.size());

        std::copy_if(this->vec.cbegin(), this->vec.cend(), std::back_inserter(result), fn);

        return Vector{result};
      }

      typename std::vector<T>::value_type operator[](std::size_t index) const {
        return vec[index];
      }

      typename std::vector<T>::value_type& operator[](std::size_t index) {
        return vec[index];
      }

      void Add(const T& item) {
        vec.push_back(item);
      }

      void Add(T&& item) {
        vec.push_back(std::move(item));
      }

      std::size_t Size() const {
        return vec.size();
      }

    private:
      std::vector<T> vec;
  };




  class String {
    public:
      String() = default;
      String(const char* str) : str(str) {}
      String(const std::string& str) : str(str) {}

      String operator+(const String& other) const {
        return String{this->str + other.str};
      }

      String& operator+=(const String& other) {
        this->str += other.str;
        return *this;
      }

      String replace(const String& old, const String& updated) const {
        std::string result = this->str;

        std::size_t pos = result.find(old.str);
        while(pos != std::string::npos) {
          result.replace(pos, old.str.size(), updated.str);
          pos = result.find(old.str, pos + updated.str.size());
        }

        return String{result};
      }

      String operator*(int times) const {
        std::string result;
        for(int i = 0; i < times; i++) {
          result += this->str;
        }
        return String{result};
      }

      String& operator*=(int times) {
        this->str = std::move((*this * times).str);
        return *this;
      }

      String LTrim() const {
        std::string result = this->str;
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
          return !std::isspace(ch);
        }));
        return String{result};
      }

      String RTrim() const {
        std::string result = this->str;
        result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
          return !std::isspace(ch);
        }).base(), result.end());
        return String{result};
      }

      String Trim() const {
        return this->LTrim().RTrim();
      }

      int ToInt() const {
        return std::stoi(this->str);
      }

      char operator[](std::size_t index) const {
        return str[index];
      }

      char& operator[](std::size_t index) {
        return str[index];
      }

      auto Slice(std::size_t start, std::size_t end) const {
        return String{str.substr(start, end - start)};
      }

      Vector<String> Split(char delim) const {
        Vector<String> result;
        
        int last_pos = 0;
        for(int i = 0; i < str.size(); i++) {
          if(str[i] == delim) {
            result.Add(str.substr(last_pos, i - last_pos));
            last_pos = i + 1;
          }
        }
        result.Add(str.substr(last_pos, str.size() - last_pos));
        
        return result;
      }

      Vector<char> Split() {
        std::vector<char> result(str.begin(), str.end());
        return Vector<char>{std::move(result)};
      }

      String Join(const Vector<String>& vec) const {
        std::string result;
        
        for(int i = 0; i < vec.Size(); i++) {
          result += vec[i].str;
          if(i != vec.Size() - 1) result += this->str;
        }

        return String{result};
      }

      std::size_t Length() const {
        return str.size();
      }

      friend std::ostream& operator<<(std::ostream& os, const String& str) {
        os << str.str;
        return os;
      }

    private:
      std::string str;
  };

  inline String operator""_hs(const char* str, std::size_t size) {
    return String{str};
  }


  template<typename T>
  std::ostream& operator<<(std::ostream& os, const Vector<T>& vec) {
    os << "[";
    for(int i = 0; i < vec.size(); i++) {
      os << vec[i];
      if(i != vec.size() - 1) os << ", ";
    }
    os << "]";
    return os;
  }
};



int main() {
  using namespace hyper::script;
  String str("Hello World  nem");
  auto result = str.Split(' ');
  std::cout << str[3] << std::endl;

  std::cout << " || "_hs.Join (
      "hey whats up with ya all?    "_hs
        .Trim()
        .Split(' ')
        .Map([](auto s) { return s.Length(); })
        .Filter([](auto s) { return s >= 3; })
        .Map([](std::size_t s) { return std::to_string(s); })
        .Map([](auto s) { return String(s); })
    ) 
    << std::endl;
  return 0;
}