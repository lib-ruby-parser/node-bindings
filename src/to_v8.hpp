#ifndef LIB_RUBY_PARSER_TO_V8_HPP
#define LIB_RUBY_PARSER_TO_V8_HPP

#include <napi.h>

void log_with_env(Napi::Value &value, Napi::Env &env);
#define log(value) log_with_env((value), env)

template <typename T>
Napi::Value ToV8(T value, Napi::Env &env, Napi::Object &objects);

#define decl_to_v8_for(T) \
    template <>           \
    Napi::Value ToV8<T>(T, Napi::Env &, Napi::Object &)

template <typename T>
T FromV8(Napi::Value value, Napi::Env &env, Napi::Object &objects);

#ifdef LIB_RUBY_PARSER_DEBUG
#define dbg(message) std::cerr << message << "\n"
#else
#define dbg(message)
#endif

#ifdef LIB_RUBY_PARSER_DEBUG
#define GetObject(name)                                        \
    if (!objects.Has(#name))                                   \
    {                                                          \
        std::cerr << "Unable to get object " << #name << "\n"; \
    }                                                          \
    Napi::Function name = objects.Get(#name).As<Napi::Function>();
#else
#define GetObject(name) \
    Napi::Function name = objects.Get(#name).As<Napi::Function>();
#endif

#endif // LIB_RUBY_PARSER_TO_V8_HPP
