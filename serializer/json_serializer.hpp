#include <fstream>
//TODO(Kristofy): should find a way to remove the dependancy on string stream 
#include <sstream>
#include "reflect.hpp"
#include "serialize.hpp"

#include <iostream>


namespace hyper::serialize::json {

namespace delimiters {
  using ArrayStart      = std::integral_constant<int, 0>;
  using ArrayDelimiter  = std::integral_constant<int, 1>;
  using ArrayEnd        = std::integral_constant<int, 2>;
  using ObjectStart     = std::integral_constant<int, 3>;
  using ObjectDelimiter = std::integral_constant<int, 4>;
  using ObjectEnd       = std::integral_constant<int, 5>;
}


class JsonSerializerStream : public SerializerStreamInterface<JsonSerializerStream> {
  public:
  JsonSerializerStream(std::ostream& out) : out(out) {
    this->out << "{\n";
    indent++;
  }

  ~JsonSerializerStream() {
    out << '\n' << std::string(--indent, '\t') << "}";
  }

  JsonSerializerStream& operator<<(SerializeDelimiter<delimiters::ArrayDelimiter>) {
    out << ",\n";
    return *this;
  }

  JsonSerializerStream& operator<<(SerializeDelimiter<delimiters::ArrayEnd>) {
    out << '\n' << std::string(--indent, '\t') << "]\n" << std::string(--indent, '\t') << "}";
    nth.pop_back();
    return *this;
  }

  JsonSerializerStream& operator<<(SerializeDelimiter<delimiters::ObjectDelimiter>) {
    out << ",\n";
    return *this;
  }

  JsonSerializerStream& operator<<(SerializeDelimiter<delimiters::ObjectEnd>) {
    out << '\n' << std::string(--indent, '\t') << '}';
    nth.pop_back();
    return *this;
  }

  JsonSerializerStream& operator<<(const SerializeDelimiter<std::tuple<delimiters::ArrayStart, std::size_t, std::string_view>>& delimiter) {
    std::string type = std::string{'"'} + std::string{std::get<2>(delimiter.Get())} + '_' + std::to_string(nth.back()) + "\": ";
    
    out
      << std::string(indent, '\t')
      << (nth.back() == -1 ? type : "")         
      << "{\n" 
      << std::string(++indent, '\t') << "\"size\": " << std::get<1>(delimiter.Get()) << ",\n" 
      << std::string(indent++, '\t') << "\"data\": [\n";

    nth.push_back(-1);
    
    return *this;
  }

  JsonSerializerStream& operator<<(const SerializeDelimiter<std::tuple<delimiters::ObjectStart, std::string_view>>& delimiter) {
    std::string type = std::string{'"'} + std::string{std::get<1>(delimiter.Get())} + '_' + std::to_string(nth.back()) + "\": ";
    
    out 
      << std::string(indent++, '\t')
      << (nth.back() != -1 ? type : "")
      << "{\n";
    
    nth.push_back(0);
  
    return *this;
  }

  template <NonSerializeDelimiter T>
  JsonSerializerStream& operator<<(T& val) {
    std::string type = std::string{'"'} + std::string{reflection::GetTypeName<T>()} + '_' + std::to_string(nth.back()) + "\": ";

    out
      << std::string(indent, '\t')
      << (nth.back() != -1 ? type : "")
      << (std::is_arithmetic_v<T> ? "" : "\"")
      << val
      << (std::is_arithmetic_v<T> ? "" : "\"");

    if (nth.back() >= 0) { nth.back()++; }

    return *this;
  }

  template <typename T = void>
  static auto ArrayStart(std::size_t size) -> SerializeDelimiter<std::tuple<delimiters::ArrayStart, std::size_t, std::string_view>> {
    return SerializeDelimiter { std::make_tuple<delimiters::ArrayStart, std::size_t, std::string_view>({}, std::move(size), reflection::GetTypeName<T>()) };
  }
  
  template <typename T = void>
  static const auto ObjectStart()   {
    return SerializeDelimiter {std::make_tuple<delimiters::ObjectStart, std::string_view>({}, reflection::GetTypeName<T>()) };
  }

  static auto ArrayDelimiter() -> SerializeDelimiter<delimiters::ArrayDelimiter> { return {}; }
  
  static auto ArrayEnd() -> SerializeDelimiter<delimiters::ArrayEnd> { return {}; }

  static auto ObjectDelimiter() -> SerializeDelimiter<delimiters::ObjectDelimiter> { return {}; }

  static auto ObjectEnd() -> SerializeDelimiter<delimiters::ObjectEnd> { return {}; }
  
  private:
  int indent = 0;
  std::ostream& out;
  std::vector<int> nth = { 0 };
};


class JsonDeserializerStream : public DeserializerStreamInterface<JsonDeserializerStream> {
  public:
  JsonDeserializerStream(std::istream& in) : in(in) {
    DeleteNextLines(1);
    // read the in buffer into the line 
    indent++;
  }

  ~JsonDeserializerStream() {
    DeleteNextLines(2);
    indent--;
    is_array.pop_back();
  }

  JsonDeserializerStream& operator>>(SerializeDelimiter<delimiters::ArrayDelimiter>) {
    DeleteNextLines(1);
    return *this;
  }

  JsonDeserializerStream& operator>>(SerializeDelimiter<delimiters::ArrayEnd>) {
    DeleteNextLines(2);
    DeleteNextChars(indent - 1);
    indent -= 2;
    is_array.pop_back();
    return *this;
  }

  JsonDeserializerStream& operator>>(SerializeDelimiter<delimiters::ObjectDelimiter>) {
    DeleteNextLines(1);
    return *this;
  }

  JsonDeserializerStream& operator>>(SerializeDelimiter<delimiters::ObjectEnd>) {
    DeleteNextLines(1);
    DeleteNextChars(indent);
    indent--;
    is_array.pop_back();
    return *this;
  }

  JsonDeserializerStream& operator>>(const SerializeDelimiter<std::tuple<delimiters::ArrayStart, size_t*>>& delimiter) {
    DeleteNextLines(1);
    DeleteNextChars(indent + 1 + sizeof("\"size\": ") - 1);
    in >> *std::get<1>(delimiter.Get());
    DeleteNextLines(2);
    indent += 2;
    is_array.push_back(1);
    return *this;
  }

  JsonDeserializerStream& operator>>(SerializeDelimiter<delimiters::ObjectStart>) {
    DeleteNextLines(1);
    is_array.push_back(0);
    indent++;
    return *this;
  }

  template <NonSerializeDelimiter T>
  JsonDeserializerStream& operator>>(T& val) {
    DeleteNextChars(indent);
    if(!is_array.back()) {
      DeleteNextChars(1);
      std::string trash;
      std::getline(in, trash, '"');
      DeleteNextChars(2);
    }

    if constexpr (not std::is_arithmetic_v<T>) {
      DeleteNextChars(1);
      std::string tmp;
      std::getline(in, tmp, '"');
      if constexpr (std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, char*> || std::is_same_v<std::decay_t<T>, char[]>) {
        val = tmp;
      } else {
        std::istringstream(tmp) >> val;
      }
    } else {
      in >> val;
    }

    return *this;
  }

  static auto ArrayStart(size_t *size) -> SerializeDelimiter<std::tuple<delimiters::ArrayStart, size_t*>> { return SerializeDelimiter<std::tuple<delimiters::ArrayStart, size_t*>>{{delimiters::ArrayStart{}, size}}; }
  static auto ObjectStart()     -> SerializeDelimiter<delimiters::ObjectStart>     { return {}; }
  static auto ArrayDelimiter()  -> SerializeDelimiter<delimiters::ArrayDelimiter>  { return {}; }
  static auto ArrayEnd()        -> SerializeDelimiter<delimiters::ArrayEnd>        { return {}; }
  static auto ObjectDelimiter() -> SerializeDelimiter<delimiters::ObjectDelimiter> { return {}; }
  static auto ObjectEnd()       -> SerializeDelimiter<delimiters::ObjectEnd>       { return {}; }
  
  private:
  int indent = 0;
  std::istream& in;
  std::vector<bool> is_array = { false };


  void DeleteNextLines(int n) {
    static std::string trash;
    while(n--) { getline(in, trash); }
  }

  void DeleteNextChars(int n) {
    in.seekg(n, std::ios_base::cur);
  }

};

} // namespace hyper::serialize::json