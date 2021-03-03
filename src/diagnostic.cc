#include "diagnostic.h"
#include "loc.h"
#include "message.h"
#include "convert/string.h"

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
                InstanceValue("rendered", env.Null(), (napi_property_attributes)(napi_writable | napi_enumerable | napi_configurable)),
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

        if (info.Length() == 4)
        {
            self.Set("level", info[0]);
            self.Set("message", info[1]);
            self.Set("rendered", info[2]);
            self.Set("loc", info[3]);
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
        auto rendered = diagnostic.render_message();

        return Diagnostic::ctor.New({
            level,
            convert(std::move(diagnostic.message), env),
            convert(rendered, env),
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
