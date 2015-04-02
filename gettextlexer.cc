#include "lexer.h"
#include <node.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>

using namespace v8;


char *v8StrToCharStar(v8::Local<v8::Value> value) {
    v8::String::Utf8Value string(value);
    char *str = (char *) malloc(string.length() + 1);
    strcpy(str, *string);
    return str;
}

void binder(const FunctionCallbackInfo<Value>& args)
{
    int ntokens;
    int * ntokens_ptr;
    char * po_string;
    struct Token * tokens;

    tokens = NULL;
    ntokens_ptr = &ntokens;

    Isolate* isolate = Isolate::GetCurrent();
    Local<Object> catalog_obj;

    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Takes two arguments")));
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be a string")));
        return;
    }

    catalog_obj = Object::New(isolate);
    po_string = v8StrToCharStar(args[0]);
    tokens = parse(tokens, po_string, ntokens_ptr);

    args.GetReturnValue().Set(catalog_obj);

    print_token_array(tokens, ntokens);
    free_token_array(tokens, ntokens);

    free(po_string);
}

void Init(Handle<Object> exports) {
    NODE_SET_METHOD(exports, "parse", binder);
}

NODE_MODULE(gettextlexer, Init)
