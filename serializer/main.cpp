#include <map>
#include <iostream>
#include <type_traits>
#include <fstream>
#include <tuple>

#include "json_serializer.hpp"
#include "reflect.hpp"
#include "mpl.hpp"


class Vec3 : public hyper::serialize::Serializable<Vec3> {
public:
  Vec3(int x = 0, int y = 0, int z = 0) :x(x), y(y), z(z) {}

  auto GetMembers() {
    return mpl::tie(x, y, z, hyper::serialize::CArrayType(asd));
  }

private:
  int x;
  int y;
  int z;

  std::map<std::string, int> mp {
    {"Hello", 1},
    {"World", 2},
    {"This", 3},
    {"Is", 4},
    {"A", 5},
    {"Test", 6}
  };

  int asd[10] = {1, 2, 3, 4, 5, 6, 7, 8};

};

class Circle : public hyper::serialize::Serializable<Circle> {
public:

  Circle() = default;

  Circle(const Vec3& pos, const float radius, std::vector<std::string> vals, std::vector<Vec3> sasd)
    :pos(pos), radius(radius), vals(vals), sasd(sasd) {};

  auto GetMembers() {
    return mpl::tie(pos, radius, vals, sasd);
  }


public:

  Vec3 pos;
  float radius;
  std::vector<std::string> vals;
  std::vector<Vec3> sasd;

};

bool test(auto any) {

  auto fout = std::ofstream("hey.txt");
  hyper::serialize::json::JsonSerializerStream out(fout);
  hyper::serialize::Serialize(any, out);
  fout.close();
  // std::cerr << "type of any: " << reflection::detail::GetTypeNameCV<decltype(any)>() << '\n';
  // std::remove_reference_t<decltype(any)> other;

  // auto fin = std::ifstream("hey.txt");
  // hyper::serialize::json::JsonDeserializerStream in(fin);

  // hyper::serialize::Deserialize(other, in);


  // hyper::serialize::json::JsonSerializerStream terminal_out(std::cout);
  // hyper::serialize::Serialize(other, terminal_out);

  return true;
}


template <typename T>
concept TestConcept = requires(T t) {
  requires std::is_same_v<T, int[10]>;
};

template <typename T>
void t(T asd) {
  std::cout << reflection::detail::GetTypeNameCV<decltype(asd)>() << '\n';
}

int main() {
  Vec3 v3 {1, 2, 3};

  Circle c {
    v3,
    4,
    {"Hello", "World", "this is", "a test"},
    {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}
  };

  // hyper::serialize::json::JsonSerializerStream out(std::cout);
  // hyper::serialize::Serialize(v3, out);



  // if (hyper::serialize::RecursivelyMemberSerializable<decltype(v3)>) {
  //   std::cout << "Vec3 is recursively serializable!\n";
  // } else {
  //   std::cout << "Vec3 is not recursively serializable!\n";   
  // }

  // if (hyper::serialize::RecursivelyAggregateSerializable<decltype(c)>) {
  //   std::cout << "Circle is recursively serializable!\n";
  // } else {
  //   std::cout << "Circle is not recursively serializable!\n";   
  // }

  // using T = std::tuple<int&, int&, int&, hyper::serialize::CArrayType<int [10], int>&>;
  // constexpr bool val = mpl::all_tuple<T>([](auto type) {
  //   using Type = std::remove_cvref<typename decltype(type)::type>::type;
  //   return hyper::serialize::is_serializable<Type>::value;
  // });
  // constexpr bool val = hyper::serialize::is_serializable<hyper::serialize::CArrayType<int [10], int>&>::value;
  // constexpr bool val0 = mpl::is_specialization_of_v<hyper::serialize::CArrayType, std::remove_cvref_t<hyper::serialize::CArrayType<int [10], int>&>>;
  // constexpr bool val1 = hyper::serialize::ExplicitlySerializable<std::remove_cvref_t<hyper::serialize::CArrayType<int [10], int>&>>;
  // constexpr bool val2 = hyper::serialize::RecursivelySerializable<std::remove_cvref_t<hyper::serialize::CArrayType<int [10], int>&>>;
  // std::cout << val0 << ' ' << val1 << ' ' << val2 << '\n';
  // // std::cout << std::size(mp) << '\n';
  if(test(v3)) {
    std::cout << "Test passed!\n";
  } else {
    std::cout << "Test failed!\n";
  }


  return 0;
}