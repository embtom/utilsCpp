#ifndef MAKEUNORDERDMAP_H
#define MAKEUNORDERDMAP_H

#include <unordered_map>
#include <type_traits>

namespace utils
{

template <class T>
using remove_reference_t = typename std::remove_reference<T>::type;

template <typename A, typename B, typename... Args>
std::unordered_map<remove_reference_t<A>,remove_reference_t<B>> make_unordered_map(Args&&... args)
{
   std::unordered_map<remove_reference_t<A>,remove_reference_t<B>> map;
   map.reserve(sizeof...(Args));
   using arr_t = int[];
   (void) arr_t {0,(map.emplace(std::forward<Args>(args)),0)...};
   return map;
}

} // utils

#endif // MAKEMAP_H
