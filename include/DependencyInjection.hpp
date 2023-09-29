/**
 * MIT License
 * 
 * Copyright (c) 2023 Rikarnto Bariampa
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#ifdef __cplusplus

#include <cstdint>
#include <functional>
#include <typeinfo>
#include <typeindex>

namespace DependencyInjection
{
    enum class ServiceLifetime : uint8_t
    {
        Singleton = 0,
        // Scoped = 1,
        Transient = 2
    };

    class ServiceDescriptor
    {
        private:
            const std::type_info& _typeInfo;
            const DependencyInjection::ServiceLifetime _lifetime;
        public:
            [[nodiscard]] auto& GetTypeInfo() const;
            [[nodiscard]] auto GetLifetime() const;
    };
}

auto& DependencyInjection::ServiceDescriptor::GetTypeInfo() const
{
    return this->_typeInfo;
}

auto DependencyInjection::ServiceDescriptor::GetLifetime() const
{
    return this->_lifetime;
}

#endif
