#ifndef MAKEMAP_H
#define MAKEMAP_H

#include <map>
#include <type_traits>

namespace utils
{

template <class T>
using remove_reference_t = typename std::remove_reference<T>::type;

template <typename A, typename B, typename... Args>
std::map<std::remove_reference_t<A>,std::remove_reference_t<B>> make_map(Args&&... args)
{
   std::map<std::remove_reference_t<A>,std::remove_reference_t<B>> map;
   using arr_t = int[];
   (void) arr_t {0,(map.emplace(std::forward<Args>(args)),0)...};
   return map;
}

} // utils

#endif // MAKEMAP_H
