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

void* External::Value() const {
    return JS_VALUE_GET_PTR(u_.value_);
}

int Isolate::RegFunctionTemplate(Local<FunctionTemplate> data) {
    function_templates_.push_back(data);
    return function_templates_.size() - 1;
}

Local<FunctionTemplate>& Isolate::GetFunctionTemplate(int index) {
    return function_templates_[index];
}

Local<FunctionTemplate> FunctionTemplate::New(Isolate* isolate, FunctionCallback callback,
                                              Local<Value> data) {
    Local<FunctionTemplate> functionTemplate(new FunctionTemplate());
    functionTemplate->callback_ = callback;
    functionTemplate->data_ = data;
    functionTemplate->magic_ = isolate->RegFunctionTemplate(functionTemplate);
    return functionTemplate;
}

MaybeLocal<Function> FunctionTemplate::GetFunction(Local<Context> context) {
    JSValue func = JS_NewCFunctionMagic(context->context_, [](JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic) {
        JSValue ret = JS_UNDEFINED;
        Isolate* isolate = reinterpret_cast<Isolate*>(JS_GetContextOpaque(ctx));
        Local<FunctionTemplate> functionTemplate = isolate->GetFunctionTemplate(magic);
        FunctionCallbackInfo<Value> callbackInfo;
        callbackInfo.isolate_ = isolate;
        callbackInfo.argc_ = argc;
        callbackInfo.argv_ = argv;
        callbackInfo.context_ = ctx;
        callbackInfo.this_ = this_val;
        callbackInfo.magic_ = magic;
        callbackInfo.value_ = JS_UNDEFINED;
        
        functionTemplate->callback_(callbackInfo);
        
        return callbackInfo.value_;
    }, "native", 0, JS_CFUNC_generic_magic, magic_);
    
    Function* function = new Function();
    function->u_.value_ = func;
    
    return MaybeLocal<Function>(Local<Function>(function));
}

Maybe<bool> Object::Set(Local<Context> context,
                        Local<Value> key, Local<Value> value) {
    JSValue jsValue;
    if (value->jsValue_) {
        jsValue = value->u_.value_;
    } else {
        jsValue = JS_NewStringLen(context->context_, value->u_.str_.data_, value->u_.str_.len_);
    }
    if (!key->jsValue_) {
        JS_SetPropertyStr(context->context_, u_.value_, key->u_.str_.data_, jsValue);
    } else {
        if (key->IsNumber()) {
            JS_SetPropertyUint32(context->context_, u_.value_, key->Uint32Value(context).ToChecked(), jsValue);
        } else {
            JS_SetProperty(context->context_, u_.value_, JS_ValueToAtom(context->context_, key->u_.value_), jsValue);
        }
    }
    
    return Maybe<bool>(true);
}

}  // namespace v8
