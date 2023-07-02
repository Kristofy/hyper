#include <type_traits>
#include <tuple>
#include <span>

#include "mpl.hpp"

namespace hyper::serialize {

template <typename DataType, typename PointerType>
class CArrayType {
public:
  CArrayType(DataType& data, PointerType* array, std::size_t size) : data(data), array(array), size(size) {}

  template <std::size_t Size>
  explicit CArrayType(PointerType (&array)[Size]) : data(array), array(static_cast<PointerType*>(array)), size(Size) {}

  PointerType* GetBegin() const { return array; }
  PointerType* GetEnd() const { return array + size; }
  std::size_t  GetSize() const { return size; }
  DataType& GetData() { return data; }

private:
  DataType& data;
  PointerType* array;
  std::size_t size;
};

// deduction guide from c style array to CArrayType
template <typename T, std::size_t Size>
CArrayType(T (&)[Size]) -> CArrayType<T[Size], T>;


namespace detail {

template <typename T>
struct is_span : std::false_type {};

template <typename ...Ts>
struct is_span<std::span<Ts...>> : std::true_type {};

template <typename T>
inline constexpr auto is_span_v = is_span<T>::value;


template<typename T>
struct is_carray_type : std::false_type {};

template<typename T, typename U>
struct is_carray_type<CArrayType<T, U>> : std::true_type {};

} // namespace detail


namespace inserter {

template <typename Base>
class InserterInterface {
public:
  template <typename ContainerType>
  void ResizeIfNeeded(ContainerType& container, std::size_t size) {
    if constexpr ( requires { static_cast<Base*>(this)->ResizeIfNeeded(container, size); }) {
      static_cast<Base*>(this)->ResizeIfNeeded(container, size);
    } else {
      // do nothing
    }
  }

  template <typename ContainerType ,typename ValueType>
  void Insert(ContainerType& container, ValueType&& value) {
    static_cast<Base*>(this)->Insert(container, std::forward<ValueType>(value));
  }

};


template <typename T>
concept FixedSizedContainer = std::is_array_v<T> || mpl::is_span_v<T> || mpl::is_specialization_of_v<std::vector, T>;
class FixedSizedContainerInserter : public InserterInterface<FixedSizedContainerInserter> {
public:
  template <typename ContainerType>
  void ResizeIfNeeded(ContainerType& container, std::size_t size) {
    if constexpr (mpl::is_specialization_of_v<std::vector, ContainerType>) {
      container.resize(size);
    }
  }

  template <typename ContainerType, typename ValueType>
  void Insert(ContainerType& container, ValueType&& value) {
    if constexpr (mpl::is_span_v<ContainerType>) {
      container.data()[index] = std::forward<ValueType>(value);
    } else {
      container[index] = std::forward<ValueType>(value);
    }

    ++index;
  }

private:
  std::size_t index = 0;
};

template<typename T>
concept AssociativeContainer = requires(T a, typename T::value_type value) {
    { a.insert(value) } -> std::same_as<std::pair<typename T::iterator, bool>>;
};
class AssociativeContainerInserter : public InserterInterface<AssociativeContainerInserter> {
public:
  template <typename ContainerType>
  void ResizeIfNeeded(ContainerType& container, std::size_t size) {
    // do nothing
  }

  template <typename ContainerType, typename ValueType>
  void Insert(ContainerType& container, ValueType&& value) {
    container.insert(std::forward<ValueType>(value));
  }
};

template <typename ContainerType>
auto Create(ContainerType& container) {
  if constexpr (FixedSizedContainer<ContainerType>) {
    return FixedSizedContainerInserter();
  } else if constexpr (AssociativeContainer<ContainerType>) {
    return AssociativeContainerInserter();
  } else {
    using namespace std::literals;
    []<bool flag = std::is_void_v<ContainerType>>() {
      static_assert(flag, "The container is not supported");
    }();
  }
}

}; // namespace inserter

template <typename Base>
class Serializable;

template <typename T>
class is_serializable;

template <typename T>
concept ExplicitlySerializable = std::is_fundamental_v<T>
    || std::is_enum_v<T>
    || std::is_same_v<std::string, std::remove_cvref_t<T>>;

template <typename T>
concept RecursivelyMemberSerializable = std::is_base_of_v<Serializable<T>, T>;

template <typename T>
concept RecursivelyContainerSerializable = requires(T& container) {
  { std::begin(container) } -> std::same_as<typename T::iterator>;
  { std::end(container) } -> std::same_as<typename T::iterator>;
  requires is_serializable<typename T::value_type>::value;
  requires !std::is_same_v<std::string, T>;
};

template <typename T>
concept RecursivelyAggregateSerializable = !ExplicitlySerializable<T> 
  && !RecursivelyContainerSerializable<T> 
  && !RecursivelyMemberSerializable<T>
  && requires(T container) {
  requires reflection::StructEach(container, [](auto& ...members) {
    return ((is_serializable<decltype(members)>::value) && ...);
  });
};

template <typename T>
concept RecursivelySerializable = !mpl::is_specialization_of_v<CArrayType, T> && (RecursivelyMemberSerializable<T> || RecursivelyContainerSerializable<T> || RecursivelyAggregateSerializable<T>);

template<typename T>
struct is_serializable {
  private:
  using Type = std::remove_cvref_t<T>;
  public:
  static constexpr bool value = mpl::is_specialization_of_v<CArrayType, Type> || ExplicitlySerializable<Type> || RecursivelySerializable<Type> ;
};

template <typename Base>
class Serializable {
  public:

  decltype(auto) GetMembers() {
    static_assert(requires (Base& base) {
      base.GetMembers();
    }, "The Derived class did not implement the interface");

    static_assert(requires (Base& base){
      requires mpl::is_tuple<decltype(base.GetMembers())>::value;
    }, "The GetMembers method is not returning a make_tuple");

    auto result = static_cast<Base*>(this)->GetMembers();
    
    using Tuple = decltype(result);
    static_assert(mpl::tuple_of_references<Tuple>::value);
    static_assert(mpl::tuple_of_non_const<Tuple>::value);
    static_assert(mpl::all_tuple<Tuple>([](auto type) {
      using Type = std::remove_cvref<typename decltype(type)::type>::type;
      return is_serializable<Type>::value;
    }));

    return result;
  }

};

template <typename T = std::nullptr_t>
class SerializeDelimiter {
  public: 
    SerializeDelimiter() = default;
    SerializeDelimiter(const T& data) : data(data) {}

    T Get() const {
      return data;
    }

    using value_type = T;

  private:
  T data;
};

template <typename T>
struct is_serialize_delimiter : std::false_type {};
template <typename U>
struct is_serialize_delimiter<SerializeDelimiter<U>> : std::true_type {};

template <typename T>
concept NonSerializeDelimiter = !is_serialize_delimiter<std::remove_cvref_t<T>>::value;

template <typename Base>
class SerializerStreamInterface {
  public:
  SerializerStreamInterface() {
    static_assert( requires (Base base) {
      { base << Base::template ArrayStart<void>(0) } -> std::same_as<Base&>;
      { base << Base::ArrayDelimiter() } -> std::same_as<Base&>;
      { base << Base::ArrayEnd() } -> std::same_as<Base&>;
      { base << Base::template ObjectStart<void>() } -> std::same_as<Base&>;
      { base << Base::ObjectDelimiter() } -> std::same_as<Base&>;
      { base << Base::ObjectEnd() } -> std::same_as<Base&>;
    }, "The derived class must implement the above mentioned static methods");
  }

  decltype(auto) operator<<(auto&& arg) {
    return (static_cast<Base&>(*this) << arg);
  } 

};

template <typename Base>
class DeserializerStreamInterface {
  public:
  DeserializerStreamInterface() {
    static_assert( requires (Base& base, size_t size) {
      { base >> Base::ArrayStart(&size) } -> std::same_as<Base&>;
      { base >> Base::ArrayDelimiter() } -> std::same_as<Base&>;
      { base >> Base::ArrayEnd() } -> std::same_as<Base&>;
      { base >> Base::ObjectStart() } -> std::same_as<Base&>;
      { base >> Base::ObjectDelimiter() } -> std::same_as<Base&>;
      { base >> Base::ObjectEnd() } -> std::same_as<Base&>;
    }, "The derived class must implement the above mentioned static methods");
  }

  decltype(auto) operator>>(auto&& arg) {
    return (static_cast<Base&>(*this) >> arg);
  } 

};

template<ExplicitlySerializable T, typename Stream>
void Serialize(T& val, SerializerStreamInterface<Stream>& out) {
  if constexpr (std::is_fundamental_v<T>) {
    out << val;
  } else if constexpr (std::is_enum_v<T>) {
    out << static_cast<std::underlying_type_t<T>>(val);
  } else if constexpr (std::is_same_v<char*, std::decay_t<T>>) {
    out << val;
  } else if constexpr (std::is_same_v<std::string, std::remove_cvref_t<T>>) {
    out << val;
  }else {
    []<bool flag = std::is_void_v<T>>(){
      static_assert(flag, "The type is not serializable");
    }();
  }
}

template<RecursivelyContainerSerializable T, typename Stream>
void Serialize(T& container, SerializerStreamInterface<Stream>& out) {
  
  std::size_t size = std::size(container);
  out << Stream::template ArrayStart<T>(size);

  for(auto it = std::begin(container); it != std::end(container); ++it) {
    if(it != std::begin(container)) {
      out << Stream::ArrayDelimiter();
    }
    Serialize((*it), out);
  }

  out << Stream::ArrayEnd();
}

template<typename T, std::size_t N, typename Stream>
void Serialize(T (&array)[N], SerializerStreamInterface<Stream>& out) {
  auto carray = CArrayType(array);
  Serialize(carray, out);
}

template<typename T, typename U, typename Stream>
void Serialize(CArrayType<T, U>& array, SerializerStreamInterface<Stream>& out) {
  if constexpr (requires{ out.WriteBuffer(array); }) {
    out.WriteBuffer(array);
  } else {
    auto span = std::span<std::remove_pointer_t<decltype(array.GetBegin())> >(array.GetBegin(), array.GetSize());
    Serialize(span, out);
  }
}

template<RecursivelyMemberSerializable T, typename Stream>
void Serialize(Serializable<T>& object, SerializerStreamInterface<Stream>& out) {
  auto&& members = object.GetMembers();
  out << Stream::template ObjectStart<T>();

  std::apply([&](auto& ...member){
    int cnt = 0;
    ((cnt++ && (out << Stream::ObjectDelimiter(), true), Serialize(member, out)),...);
  }, members);

  out << Stream::ObjectEnd();
}

template<RecursivelyAggregateSerializable T, typename Stream>
void Serialize(T& object, SerializerStreamInterface<Stream>& out) {
  out << Stream::template ObjectStart<T>();

  reflection::StructEach(object, [&](auto& ...members) {
    int cnt = 0;
    ((cnt++ && (out << Stream::ObjectDelimiter(), true), Serialize(members, out)), ...);
  });

  out << Stream::ObjectEnd();
}



template<ExplicitlySerializable T, typename Stream>
void Deserialize(T& val, DeserializerStreamInterface<Stream>& out) {
  // log
  std::remove_cvref_t<T>& val_ref = const_cast<std::remove_cvref_t<T>&>(val);
  if constexpr (std::is_fundamental_v<T>) {
    out >> val_ref;
   } else if constexpr (std::is_enum_v<T>) {
    out >> val_ref;
    //TODO(kristofy): move the two string types below to array or string out specialization
  } else if constexpr (std::is_same_v<char*, std::decay_t<T>>) {
    out >> val_ref;
  } else if constexpr (std::is_same_v<std::string, std::remove_cvref_t<T>>) {
    out >> val_ref;
  } else {
    []<bool flag = std::is_void_v<T>>(){
      static_assert(flag, "The type is not deserializable");
    }();
  }
  std::cerr << "The current type is : " << reflection::detail::GetTypeNameCV<decltype(val_ref)>() << ", read in: " << val_ref << std::endl;
}

template<RecursivelyContainerSerializable T, typename Stream>
void Deserialize(T& container, DeserializerStreamInterface<Stream>& in) {
  
  std::size_t size = 0;

  in >> Stream::ArrayStart(&size);

  auto ins = inserter::Create(container);
  ins.ResizeIfNeeded(container, size);

  for(int i = 0; i < size; i++) {
    if(i != 0) {
      in >> Stream::ArrayDelimiter();
    }
    // FIXME(Kristofy): This requires T::value_type to be default constructable
    // maybe only reserve the size on the stack if available
    typename T::value_type element;
    Deserialize(element, in);

    // FIXME(Kristofy): Unecessary copy, but std::move may not be applicable for all containers
    // container.insert(element);
    ins.Insert(container, element);
    // Could insert a dummy memory into the container and than get a pointer inside the container for that elmenet, this could avoid the copys and the but not the defult construction
  }

  in >> Stream::ArrayEnd();
}

template <typename T, std::size_t N, typename Stream>
void Deserialize(T (&array)[N], DeserializerStreamInterface<Stream>& in) {
  auto carray = CArrayType(array);
  Deserialize(carray, in);
}

template<typename T, typename U, typename Stream>
void Deserialize(CArrayType<T, U>& array, DeserializerStreamInterface<Stream>& in) {
  if constexpr (requires{ in.ReadBuffer(array); }) {
    in.ReadBuffer(array);
  } else {
    auto span = std::span<std::remove_pointer_t<decltype(array.GetBegin())>>(array.GetBegin(), array.GetSize());
    Deserialize(span, in);
  } 
}


template<RecursivelyMemberSerializable T, typename Stream>
void Deserialize(Serializable<T>& object, DeserializerStreamInterface<Stream>& in) {
  auto&& members = object.GetMembers();

  in >> Stream::ObjectStart();

  std::apply([&](auto& ...member){
    int cnt = 0;
    ((cnt++ && (in >> Stream::ObjectDelimiter(), true), Deserialize(member, in)),...);
  }, members);

  in >> Stream::ObjectEnd();
}


template<RecursivelyAggregateSerializable T, typename Stream>
void Deserialize(T& object, DeserializerStreamInterface<Stream>& in) {
  in >> Stream::ObjectStart();

  reflection::StructEach(object, [&](auto& ...member){
    // log
    ((std::cerr << "The current member type is : " << reflection::detail::GetTypeNameCV<decltype(member)>() << std::endl), ...);
    int cnt = 0;
    ((cnt++ && (in >> Stream::ObjectDelimiter(), true), Deserialize(member, in)),...);
  });

  in >> Stream::ObjectEnd();
}

} // namespace serializer