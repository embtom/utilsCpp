#ifndef UTIL_SPAN_H
#define UTIL_SPAN_H

namespace utils
{

struct SSpan
{
   SSpan() = delete;
   
   template<typename T>
   SSpan(T&& object) = delete;
   
   SSpan(void* buf, unsigned bufLen) noexcept : 
      buf(buf), bufLen(bufLen) 
   { }
   
   template<typename T>
   SSpan(T& object) noexcept : 
      buf(reinterpret_cast<void*>(&object)), bufLen(sizeof(T)) 
   { }
   
   template<typename T>
   SSpan(const T& object) noexcept : 
      buf(reinterpret_cast<void*>(const_cast<T*>(&object))), bufLen(sizeof(T)) 
   { }
   
   template<typename T>
   const T& as() const noexcept 
   { 
      return *reinterpret_cast<const T*>(buf); 
   }
   
   template<typename T>
   T& as() noexcept 
   { 
      return *reinterpret_cast<T*>(buf); 
   }
   
   void*    buf;
   unsigned bufLen;
};

} // utils

#endif
