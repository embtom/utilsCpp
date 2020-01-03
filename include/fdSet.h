/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/embtom)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//******************************************************************************
// Header

#include <vector> // std::vector
#include <functional> //std::function
#include <unique_opaque.h>

namespace utils
{

class CFdSetPrivate;

//*****************************************************************************
//! \brief CFdSet
//!
enum class CFdSetRetval
{
   OK,
   UNBLOCK
};


class CFdSet
{
public:
   using Callback = std::function<void(int fd)>;
   
   CFdSet(const CFdSet&)            = delete;
   CFdSet& operator=(const CFdSet&) = delete;
   CFdSet(CFdSet&&)                 = default;  
   CFdSet& operator=(CFdSet&&)      = default;

   explicit CFdSet();
   virtual ~CFdSet();
   
   void AddFd(int fd, Callback cb);
   void RemoveFd(int fd);
   CFdSetRetval Select();
   void UnBlock();
private:
   utils::unique_opaque<CFdSetPrivate>  m_pPrivate;
};

} //util