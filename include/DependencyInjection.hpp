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
#include <ranges>
#include <typeinfo>
#include <typeindex>
#include <utility>
#include <vector>

namespace DependencyInjection
{
    class IServiceProvider;

    typedef std::function<std::shared_ptr<void>(IServiceProvider&)> ServiceFactory;

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
            ServiceDescriptor(const std::type_info& typeInfo,
                    ServiceFactory factory,
                    const ServiceLifetime lifetime) :
                    _typeInfo{ typeInfo },
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

    class ServiceProvider : public IServiceProvider
    {
        private:
            std::map<std::type_index, std::vector<ServiceDescriptor>> _services;

            explicit ServiceProvider(const std::vector<ServiceDescriptor>& serviceDescriptors);

        public:
            ServiceProvider() = delete;

            [[nodiscard]] std::shared_ptr<void> GetService(const std::type_info& type);

            template<class T>
            auto GetService();
    };

    class IServiceCollection
    {
        private:

        public:
            virtual IServiceCollection& Add(const ServiceDescriptor& serviceDescriptor) = 0;
    };

    class ServiceCollection : public IServiceCollection
    {
        private:
            std::vector<ServiceDescriptor> _serviceDescriptors;

        public:
            ServiceCollection& Add(const ServiceDescriptor& serviceDescriptor) final;

            template<class TService, class TImplementation = TService>
            ServiceCollection& AddSingleton();

            template<class TService, class TImplementation = TService>
            ServiceCollection& AddSingleton(const DependencyInjection::ServiceFactory& factory);
    };
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

DependencyInjection::ServiceProvider::ServiceProvider(const std::vector<ServiceDescriptor>& serviceDescriptors)
{
    for (const auto& serviceDescriptor : std::ranges::reverse_view(serviceDescriptors))
    {
        bool serviceDoesntExists = (!_services.contains(serviceDescriptor.GetTypeInfo()));
        if (serviceDoesntExists)
        {
            _services.insert({ serviceDescriptor.GetTypeInfo(), { serviceDescriptor } });
        }
        else
        {
            _services[serviceDescriptor.GetTypeInfo()].push_back(serviceDescriptor);
        }
    }
}

std::shared_ptr<void> DependencyInjection::ServiceProvider::GetService(const std::type_info& type)
{
    auto positionInMap = this->_services.find(type);

    auto notFound = (positionInMap == this->_services.end());
    if (notFound)
    {
        return nullptr;
    }

    auto serviceDescriptorList = positionInMap->second;
    auto firstServiceDescriptor = serviceDescriptorList[0];
    auto serviceFactory = firstServiceDescriptor.GetFactory();
    auto service = serviceFactory(*this);

    return service;
}

template<class T>
auto DependencyInjection::ServiceProvider::GetService()
{
    auto service = GetService(typeid(T));
    return std::static_pointer_cast<T>(service);
}

DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::Add(const ServiceDescriptor& serviceDescriptor)
{
    this->_serviceDescriptors.push_back(serviceDescriptor);

    return *this;
}

template<class TService, class TImplementation>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton()
{
    return this->AddSingleton<TService, TImplementation>([] (IServiceProvider&) {
        static auto value = std::make_shared<TImplementation>();

        return value;
    });
}

template<class TService, class TImplementation>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton(const DependencyInjection::ServiceFactory& factory)
{
    auto serviceDescriptor = DependencyInjection::ServiceDescriptor(typeid(TService), factory, DependencyInjection::ServiceLifetime::Singleton);

    return this->Add(serviceDescriptor);
}

#endif
