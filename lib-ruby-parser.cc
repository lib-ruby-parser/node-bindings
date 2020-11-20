#include <node.h>

namespace demo
{
    using v8::Exception;
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::MaybeLocal;
    using v8::Object;
    using v8::String;
    using v8::Value;

    void parse(const FunctionCallbackInfo<Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        if (!args[0]->IsString())
        {
            Local<String> err = String::NewFromUtf8(
                                    isolate,
                                    "argument must be a string",
                                    v8::NewStringType::kNormal)
                                    .ToLocalChecked();
            isolate->ThrowException(Exception::TypeError(err));
        }
        MaybeLocal<String> source = args[0]->ToString(isolate->GetCurrentContext());
        args.GetReturnValue().Set(source.ToLocalChecked());
    }

    void Initialize(Local<Object> exports)
    {
        NODE_SET_METHOD(exports, "parse", parse);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

} // namespace demo
