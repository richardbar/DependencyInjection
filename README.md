# DependencyInjection
## Overview
DependencyInjection is a library that mimics the functionality of the nuget package "Microsoft.Extensions.DependencyInjection."

## Examples
```cpp
auto serviceCollection = DependencyInjection::ServiceCollection();
serviceCollection.AddSingleton<Configuration>();
serviceCollection.AddTransient<DbConnection>();

auto serviceProvider = serviceCollection.BuildServiceProvider();

auto dbConnection = serviceProvider.GetService<DbConnection>();
```

## Constructor with arguments
When a class has a constructor with arguments, then a custom service factory is needed. There are two ways to create custom service factories, an inline and a template function.
```cpp
serviceCollection.AddSingleton<Configuration>([] (DependencyInjection::IServiceProvider&)
{
    static Configuration service { "file.json", ...};

    return service;
});
```

```cpp
template<>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton<Configuration>()
{
    return this->AddSingleton<Configuration>([] (DependencyInjection::IServiceProvider&) {
        static Configuration service { "file.json", ...};

        return service;
    });
}
```
The template function will allow for components to be reused if needed.
To improve compile time speed a template function can be externed to a source file.
```cpp
// header.hpp
extern template
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton<Configuration>();

// source.cpp
template<>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton<Configuration>()
{
    return this->AddSingleton<Configuration>([] (DependencyInjection::IServiceProvider&) {
        static Configuration service { "file.json", ...};

        return service;
    });
}
```

## Constructor with service
Many times the construction of a class depends on other services. To achieve a similar results the service provider argument can be used to get the remaining services.

```cpp
template<>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddTransient<IDbConnection, DbConnection>()
{
    return this->AddTransient<IDbConnection, DbConnection>([] (IServiceProvider& serviceProvider) {
        auto configuration = std::any_cast<Configuration>(serviceProvider.Get(typeid(Configuration)));

        DbConnection service { configuration };

        return service;
    });
}
```