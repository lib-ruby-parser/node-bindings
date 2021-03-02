#include "loc.h"
#include "input.h"
#include "bytes.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference Loc::ctor;

    void Loc::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env, "Loc",
            {
                InstanceValue("begin", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("end", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),

                InstanceMethod("source", &Loc::Source, napi_default),
            });
        exports.Set("Loc", ctor);

        Loc::ctor = Napi::Persistent(ctor);
        Loc::ctor.SuppressDestruct();
    }

    Napi::Value Loc::Source(const Napi::CallbackInfo &info)
    {
        auto env = info.Env();

        if (!info[0].IsExternal())
        {
            Napi::TypeError::New(env, "argument must Input").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto ptr = info[0].As<Napi::External<Input>>().Data();
        auto input = lib_ruby_parser::Input(ptr);

        auto self = info.This().ToObject();

        auto bytes = input.range(
            self.Get("begin").ToNumber(),
            self.Get("end").ToNumber());

        input.ptr = nullptr;

        return Bytes(std::move(bytes)).ToV8(env);
    }

    Loc::Loc(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Loc>(info)
    {
    }

    Napi::Value convert(std::unique_ptr<lib_ruby_parser::Loc> loc, Napi::Env env)
    {
        if (!loc)
        {
            return env.Null();
        }
        auto result = Loc::ctor.New({});

        result.Set("begin", Napi::Value::From(env, loc->begin));
        result.Set("end", Napi::Value::From(env, loc->end));

        return result;
    }
}
