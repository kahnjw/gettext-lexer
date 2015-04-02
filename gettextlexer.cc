#include <node.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include "lexer.h"

using namespace v8;

/* Type Definitions */
#define STRINGTYPE 0x03
#define COMMENTTYPE 0x01
#define KEYTYPE 0x02

/* State definitions */
#define NONESTATE 0x01
#define COMMENTSTATE 0x02
#define KEYSTATE 0x03
#define STRINGSTATE 0x04

#define VALLENGTH 100


char *v8StrToCharStar(v8::Local<v8::Value> value) {
    v8::String::Utf8Value string(value);
    char *str = (char *) malloc(string.length() + 1);
    strcpy(str, *string);
    return str;
}

void parse(const FunctionCallbackInfo<Value>& args) {
    bool escaped = false;
    char chr;
    char quote;
    int i;
    int ntokens;
    int state = NONESTATE;
    struct Token * tokens;
    char * po_string;
    char * buffer;

    buffer = NULL;
    tokens = NULL;
    ntokens = 0;
    quote = '\0';

    Isolate* isolate = Isolate::GetCurrent();
    Local<Object> catalog_obj;
    Local<String> catalog_str;

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
    catalog_str = args[0]->ToString();

    for (i = 0; i < catalog_str->Length(); i++) {
        chr = po_string[i];

        switch(state) {
            case NONESTATE:
                if (chr == '\'' || chr == '"') {
                    state = STRINGSTATE;
                    quote = chr;
                } else if (chr == '#') {
                    state = COMMENTSTATE;
                    tokens = append(tokens, ntokens, COMMENTTYPE, '\0', NULL);
                    ++ntokens;
                } else if (!isspace(chr)) {
                    state = KEYSTATE;
                    buffer = append_to_buffer(buffer, chr);
                } else if (isspace(chr) && buffer != NULL) {
                    tokens = append(tokens, ntokens, KEYTYPE, '\0', buffer);
                    free(buffer);
                    buffer = NULL;
                    ++ntokens;
                }

                break;

            case COMMENTSTATE:
                if (chr == '\n') {
                    tokens = append(tokens, ntokens, COMMENTTYPE, '\0', buffer);
                    state = NONESTATE;
                    free(buffer);
                    buffer = NULL;
                    ++ntokens;
                } else if (chr != '\r') {
                    buffer = append_to_buffer(buffer, chr);
                }

                break;

            case STRINGSTATE:
                if(escaped) {
                    switch(chr) {
                        case 't':
                            buffer = append_to_buffer(buffer, '\t');
                            break;

                        case 'n':
                            buffer = append_to_buffer(buffer, '\n');
                            break;

                        case 'r':
                            buffer = append_to_buffer(buffer, '\r');
                            break;
                    }

                    escaped = false;

                } else {
                    if(chr == quote) {
                        tokens = append(tokens, ntokens, STRINGTYPE, quote, buffer);
                        free(buffer);
                        buffer = NULL;
                        state = NONESTATE;
                        ++ntokens;
                    } else if (chr == '\\') {
                        escaped = true;
                        break;
                    } else {
                        buffer = append_to_buffer(buffer, chr);
                    }
                    escaped = false;
                }

                break;

            case KEYSTATE:
                if(isalpha(chr) || chr == '-' || chr == '[' || chr == ']') {
                    buffer = append_to_buffer(buffer, chr);
                } else {
                    state = NONESTATE;
                    --i;
                }

                break;
        }
    }

    args.GetReturnValue().Set(catalog_obj);
    print_token_array(tokens, ntokens);
    free_token_array(tokens, ntokens);
    free(po_string);
}

void Init(Handle<Object> exports) {
    NODE_SET_METHOD(exports, "parse", parse);
}

NODE_MODULE(gettextlexer, Init)
