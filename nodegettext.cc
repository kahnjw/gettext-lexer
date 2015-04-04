#include <stdlib.h>
#include <errno.h>
#include <libintl.h>
#include <node.h>
#include <stdio.h>
#include <string.h>

using namespace v8;


char * null_helper(char * str)
{
    if(!strcmp(str, "null")) {
        return NULL;
    }

    return str;
}

char * v8StrToCharStar(v8::Local<v8::Value> value)
{
    v8::String::Utf8Value string(value);
    char *str = (char *) malloc(string.length() + 1);
    strcpy(str, *string);

    return str;
}

int v8StrToInt(v8::Local<v8::Value> value)
{
    int integer;
    char * str;

    str = v8StrToCharStar(value);
    integer = atoi(str);

    free(str);

    return integer;
}

void _bindtextdomain(const FunctionCallbackInfo<Value>& args)
{
    char * domain_name;
    char * dir_name;
    char * returned_dir_name;

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 2) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                "bindtextdomain takes domain_name and dir_name")));
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be a string")));
        return;
    }

    if (!args[1]->IsString()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Second argument must be a string")));
        return;
    }

    domain_name = v8StrToCharStar(args[0]);
    dir_name = v8StrToCharStar(args[1]);

    if((returned_dir_name = bindtextdomain(domain_name, dir_name)) == NULL) {
        printf("Ran out of memory during call to bindtextdomain()\n");
        printf("ERROR: %s\n", strerror(errno));
        exit(1);
    }

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, returned_dir_name));

    free(domain_name);
    free(dir_name);

    return;
}

void _gettext(const FunctionCallbackInfo<Value>& args)
{
    char * original_text;
    char * translated_text;

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "gettext takes argument text")));
        return;
    }

    if (!args[0]->IsString()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be a string")));
        return;
    }

    original_text = v8StrToCharStar(args[0]);

    translated_text = gettext(original_text);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, translated_text));

    free(original_text);

    return;
}

void _setlocale(const FunctionCallbackInfo<Value>& args)
{
    int lc_type;
    char * lc_code;
    char * returned_lc_code;

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 2) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                "setlocale takes LC_TYPE and LC_CODE")));
        return;
    }

    if (!args[0]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be a string")));
        return;
    }

    if (!(args[1]->IsString() || args[1]->IsNull())) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(
                isolate, "Second argument must be a string or null")));
        return;
    }

    lc_type = v8StrToInt(args[0]);
    lc_code = v8StrToCharStar(args[1]);

    lc_code = null_helper(lc_code);

    if((returned_lc_code = setlocale(lc_type, lc_code)) == NULL) {
        printf("Invalid locale code given: %s\n", lc_code);
        exit(1);
    }

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, returned_lc_code));

    free(lc_code);

    return;
}

void _textdomain(const FunctionCallbackInfo<Value>& args)
{
    char * domain_name;
    char * returned_domain;

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    if (args.Length() < 1) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "textdomain takes domain_name")));
        return;
    }

    if (!(args[0]->IsString() || args[0]->IsNull())) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "First argument must be a string")));
        return;
    }

    domain_name = v8StrToCharStar(args[0]);

    domain_name = null_helper(domain_name);

    if((returned_domain = textdomain(domain_name)) == NULL) {
        printf("Ran out of memory during call to textdomain()\n");
        printf("ERROR: %s\n", strerror(errno));
        exit(1);
    }

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, returned_domain));

    free(domain_name);

    return;
}

void Init(Handle<Object> exports) {
    NODE_SET_METHOD(exports, "bindtextdomain", _bindtextdomain);
    NODE_SET_METHOD(exports, "gettext", _gettext);
    NODE_SET_METHOD(exports, "setlocale", _setlocale);
    NODE_SET_METHOD(exports, "textdomain", _textdomain);

    NODE_DEFINE_CONSTANT(exports, LC_ALL);
    NODE_DEFINE_CONSTANT(exports, LC_CTYPE);
    NODE_DEFINE_CONSTANT(exports, LC_COLLATE);
    NODE_DEFINE_CONSTANT(exports, LC_MESSAGES);
    NODE_DEFINE_CONSTANT(exports, LC_MONETARY);
    NODE_DEFINE_CONSTANT(exports, LC_NUMERIC);
    NODE_DEFINE_CONSTANT(exports, LC_TIME);
}

NODE_MODULE(gettextlexer, Init)
