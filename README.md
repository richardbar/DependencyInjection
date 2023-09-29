# DependencyInjection
## Overview
DependencyInjection is a library that mimics the functionality of the nuget package "Microsoft.Extensions.DependencyInjection."

## Examples
```cpp
auto serviceCollection = DependencyInjection::ServiceCollection();
serviceCollection.AddSingleton<IConfiguration, Configuration>();
serviceCollection.AddTransient<IDbConnection, DbConnection>();

auto serviceProvider = serviceCollection.BuildServiceProvider();

std::shared_ptr<IDbConnection> dbConnection = serviceProvider.GetServicer<IDbConnection>();
```

## Constructor with arguments
When a class has a constructor with arguments, then a custom service factory is needed. There are two ways to create custom service factories, an inline and a template function.
```cpp
serviceCollection.AddSingleton<IConfiguration, Configuration>([] (DependencyInjection::IServiceProvider&)
{
    static auto value = std::make_shared<Configuration>("file.json", ...);
});
```

```cpp
template<>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton<IConfiguration, Configuration>()
{
    return this->AddSingleton<IConfiguration, Configuration>([] (IServiceProvider&) {
        static auto value = std::make_shared<Configuration>("file.json", ...);

        return value;
    });
}
```
The template function will allow for components to be reused if needed.
To improve compile time speed a template function can be externed to a source file.
```cpp
// header.hpp
extern template
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton<IConfiguration, Configuration>();

// source.cpp
template<>
DependencyInjection::ServiceCollection& DependencyInjection::ServiceCollection::AddSingleton<IConfiguration, Configuration>()
{
    return this->AddSingleton<IConfiguration, Configuration>([] (IServiceProvider&) {
        static auto value = std::make_shared<Configuration>("file.json", ...);

        return value;
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
        auto configuration = serviceProvider.Get(typeid(IConfiguration));

        auto value = std::make_shared<DbConnection>(configuration);

        return value;
    });
}
```