#include "diagnostic.h"
#include "loc.h"

namespace lib_ruby_parser_node
{
    Napi::FunctionReference Diagnostic::ctor;

    void Diagnostic::Init(Napi::Env &env, Napi::Object &exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function ctor = DefineClass(
            env,
            "Diagnostic",
            {
                InstanceValue("name", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("message", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
                InstanceValue("loc", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
            });
        exports.Set("Diagnostic", ctor);

        Diagnostic::ctor = Napi::Persistent(ctor);
        Diagnostic::ctor.SuppressDestruct();
    }

    Diagnostic::Diagnostic(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Diagnostic>(info)
    {
        auto env = info.Env();
        auto self = info.This().As<Napi::Object>();

        if (info.Length() == 3)
        {
            self.Set("level", info[0]);
            self.Set("message", info[1]);
            self.Set("loc", info[2]);
        }
        else
        {
            Napi::TypeError::New(env, "new Diagnostic() takes 3 arguments").ThrowAsJavaScriptException();
        }
    }

    Napi::Value convert(lib_ruby_parser::Diagnostic diagnostic, Napi::Env env)
    {
        Napi::String level;
        switch (diagnostic.level)
        {
        case lib_ruby_parser::ErrorLevel::WARNING:
            level = Napi::String::New(env, "warning");
            break;
        case lib_ruby_parser::ErrorLevel::ERROR:
            level = Napi::String::New(env, "error");
            break;
        }
        return Diagnostic::ctor.New({
            level,
            Napi::String::New(env, diagnostic.render_message()),
            convert(std::move(diagnostic.loc), env),
        });
    }

    Napi::Value convert(std::vector<lib_ruby_parser::Diagnostic> diagnostics, Napi::Env env)
    {
        Napi::Array arr = Napi::Array::New(env, diagnostics.size());
        for (size_t i = 0; i < diagnostics.size(); i++)
        {
            arr.Set(i, convert(std::move(diagnostics[i]), env));
        }
        return arr;
    }
}
