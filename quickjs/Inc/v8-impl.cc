#include "v8.h"
#include<cstring>

namespace v8 {
namespace platform {

std::unique_ptr<v8::Platform> NewDefaultPlatform() {
    return std::unique_ptr<v8::Platform>{};
}

}  // namespace platform
}  // namespace v8

namespace v8 {
MaybeLocal<String> String::NewFromUtf8(
    Isolate* isolate, const char* data,
    NewStringType type, int length) {
    auto str = new String();
    //printf("NewFromUtf8:%p\n", str);
    str->u_.str_.data_ = const_cast<char*>(data);
    str->u_.str_.len_ = length > 0 ? length : strlen(data);
    str->jsValue_ = false;
    return Local<String>(str);
}

//！！如果一个Local<String>用到这个接口了，就不能再传入JS
MaybeLocal<Script> Script::Compile(
    Local<Context> context, Local<String> source,
    ScriptOrigin* origin) {
    Script* script = new Script();
    script->source_ = source;
    if (origin) {
        script->resource_name_ = MaybeLocal<String>(Local<String>::Cast(origin->resource_name_));
    }
    return MaybeLocal<Script>(Local<Script>(script));
}

MaybeLocal<Value> Script::Run(Local<Context> context) {
    auto isolate = context->GetIsolate();
    Isolate::Scope isolateScope(isolate);
    HandleScope handleScope(isolate);
    Context::Scope contextScope(context);

    String::Utf8Value source(isolate, source_);
    auto ret = JS_Eval(context->context_, *source, source.length(), resource_name_.IsEmpty() ? "eval" : *String::Utf8Value(isolate, resource_name_.ToLocalChecked()), JS_EVAL_FLAG_STRICT);

    if (JS_IsException(ret)) {
        return MaybeLocal<Value>();
    } else {
        Value* val = new Value();
        val->u_.value_ = ret;
        return MaybeLocal<Value>(Local<Value>(val));
    }
}

Script::~Script() {
    //JS_FreeValue(context_->context_, source_->value_);
    //if (!resource_name_.IsEmpty()) {
    //    JS_FreeValue(context_->context_, resource_name_.ToLocalChecked()->value_);
    //}
}

Local<External> External::New(Isolate* isolate, void* value) {
    External* external = new External();
    external->u_.value_ = JS_MKPTR(JS_TAG_EXTERNAL, value);
    return Local<External>(external);
}

External* External::Cast(class Value* obj) {
    return static_cast<External*>(obj);
}

void* External::Value() const {
    return JS_VALUE_GET_PTR(u_.value_);
}

}  // namespace v8
