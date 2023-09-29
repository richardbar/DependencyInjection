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
#include <map>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <utility>

namespace DependencyInjection
{
    typedef std::function<std::shared_ptr<void>(ServiceProvider&)> ServiceFactory;

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
            const DependencyInjection::ServiceFactory _factory;
            const DependencyInjection::ServiceLifetime _lifetime;
        public:
            ServiceDescriptor() = delete;
            ServiceDescriptor(const std::type_info& serviceType,
                ServiceFactory factory,
                const ServiceLifetime lifetime) :
                    _serviceType{ serviceType },
                    _factory{ std::move(factory) },
                    _lifetime{ lifetime } { }

            [[nodiscard]] auto& GetTypeInfo() const;
            [[nodiscard]] auto GetFactory() const;
            [[nodiscard]] auto GetLifetime() const;
    };

    class IServiceProvider
    {
        private:

        public:
            virtual std::shared_ptr<void> GetService(const std::type_info& type) = 0;
    };

    class ServiceProvider : IServiceProvider
    {
        private:

        public:
            [[nodiscard]] std::shared_ptr<void> GetService(const std::type_info& type);
    }
}

auto& DependencyInjection::ServiceDescriptor::GetTypeInfo() const
{
    return this->_typeInfo;
}

auto DependencyInjection::ServiceDescriptor::GetFactory() const
{
    return this->_factory;
}

auto DependencyInjection::ServiceDescriptor::GetLifetime() const
{
    return this->_lifetime;
}

std::shared_ptr<void> DependencyInjection::ServiceProvider::GetService(const std::type_info& type)
{

}

#endif
