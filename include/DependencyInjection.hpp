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

#include <any>
#include <cstdint>
#include <functional>
#include <map>
#include <ranges>
#include <typeinfo>
#include <typeindex>
#include <utility>
#include <vector>

namespace DependencyInjection
{
    enum class ServiceLifetime : std::uint8_t;
    class IServiceProvider;
    class ServiceCollection;

    typedef std::function<std::any(IServiceProvider&)> ServiceFactory;

    enum class ServiceLifetime : std::uint8_t
    {
        Singleton = 0, /*!< The service is created once and reused. */
        // Scoped = 1, /*!< The service is created once per scope. */
        Transient = 2, /*!< The service is created each time it is requested. */
    };

    class ServiceDescriptor
    {
        private:
            const std::type_info& _typeInfo;
            const ServiceFactory _factory;
            const ServiceLifetime _lifetime;

        public:
            ServiceDescriptor() = delete;
            ServiceDescriptor(const std::type_info& typeInfo, ServiceFactory factory, const ServiceLifetime lifetime) :
                    _typeInfo{ typeInfo },
                    _factory{ std::move(factory) },
                    _lifetime{ lifetime } { }
            ServiceDescriptor(const ServiceDescriptor& obj) = default;
            ServiceDescriptor(ServiceDescriptor&& deadObj) noexcept = default;

            [[nodiscard]] auto& GetTypeInfo() const
            {
                return _typeInfo;
            }
            [[nodiscard]] auto GetFactory() const
            {
                return _factory;
            }
            [[nodiscard]] auto GetLifetime() const
            {
                return _lifetime;
            }

            ServiceFactory& operator=(const ServiceFactory&) = delete;
            ServiceFactory& operator=(ServiceFactory&&) noexcept = delete;
    };

    class IServiceProvider
    {
        public:
            virtual std::any GetService(const std::type_info& type) = 0;
    };

    class ServiceProvider : public IServiceProvider
    {
        private:
            std::map<std::type_index, std::vector<ServiceDescriptor>> _services;

            explicit ServiceProvider(const std::vector<ServiceDescriptor>& descriptors)
            {
                using std::ranges::reverse_view;

                for (const auto& descriptor : reverse_view(descriptors))
                {
                    const auto& typeInfo = descriptor.GetTypeInfo();
                    bool serviceExists = _services.contains(typeInfo);
                    if (serviceExists)
                    {
                        _services[typeInfo].push_back(descriptor);
                    }
                    else
                    {
                        _services.insert({ typeInfo, { descriptor }});
                    }
                }
            }

        public:
            ServiceProvider() = delete;

            [[nodiscard]] std::any GetService(const std::type_info& type) final
            {
                const auto position = _services.find(type);
                const auto notFound = (position == _services.end());
                if (notFound)
                {
                    return {};
                }

                const auto descriptorList = position->second;
                const auto& descriptor = descriptorList[0];
                const auto factory = descriptor.GetFactory();
                auto service = factory(*this);

                return service;
            }


            template<class T>
            auto GetService()
            {
                auto service = GetService(typeid(T));
                return std::any_cast<T>(service);
            }

            friend class ServiceCollection;
    };

    class IServiceCollection
    {
        public:
            virtual IServiceCollection& Add(const ServiceDescriptor& descriptor) = 0;
    };

    class ServiceCollection : public IServiceCollection
    {
        private:
            std::vector<ServiceDescriptor> _descriptors;

        public:
            ServiceCollection& Add(const ServiceDescriptor& descriptor) final
            {
                _descriptors.push_back(descriptor);
                return *this;
            }

            template<class TService, class TImplementation = TService>
            ServiceCollection& AddSingleton()
            {
                const auto factory = [] (IServiceProvider&)
                {
                    static const auto& service = TImplementation();
                    return service;
                };
                return AddSingleton<TService, TImplementation>(factory);
            }

            template<class TService, class TImplementation = TService>
            ServiceCollection& AddSingleton(const ServiceFactory& factory)
            {
                const ServiceDescriptor descriptor {
                    typeid(TService),
                    factory,
                    ServiceLifetime::Singleton
                };
                return Add(descriptor);
            }

            template<class TService, class TImplementation = TService>
            ServiceCollection& AddTransient()
            {
                const auto factory = [] (IServiceProvider&)
                {
                    TImplementation service;
                    return service;
                };
                return AddTransient<TService, TImplementation>(factory);
            }

            template<class TService, class TImplementation = TService>
            ServiceCollection& AddTransient(const ServiceFactory& factory)
            {
                const ServiceDescriptor descriptor {
                    typeid(TService),
                    factory,
                    ServiceLifetime::Transient
                };
                return Add(descriptor);
            }

            [[nodiscard]] ServiceProvider BuildServiceProvider() const
            {
                return ServiceProvider(_descriptors);
            }
    };
}

#endif
