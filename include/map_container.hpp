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

#ifndef _MAPCONTAINER_H_
#define _MAPCONTAINER_H_

#include <array>
#include <algorithm>



namespace utils 
{
    template<typename T>
    class CMapContainerBase
    {
        public:
            virtual ~CMapContainerBase() = default;
            virtual size_t size() const noexcept = 0;
            virtual unsigned int count() const noexcept = 0;
            virtual T* data() const noexcept = 0;
    };

    template<typename T,int N>
    class CMapContainer final : public CMapContainerBase<T>
    {
        public:
            constexpr CMapContainer() = default;

            template<typename ...Args>
            constexpr CMapContainer(Args&&... args) noexcept : 
                m_map({std::forward<Args>(args)...})
            { };

            constexpr size_t size() const noexcept override
            { 
                return (sizeof(T) * m_map.size());
            }
            
            constexpr unsigned int count() const noexcept override
            {
                return m_map.size();
            }    

            constexpr T* data() const noexcept override
            {
                return const_cast<T*>(m_map.data());
            }

            //
            template<typename Func>
            void foreach(Func f)
            {
                std::for_each(m_map.begin(), m_map.end(), f);
            }        
        private:
            std::array<T, N> m_map;
    };
}

#endif /*_MAPCONTAINER_H_*/