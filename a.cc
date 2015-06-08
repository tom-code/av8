#include "include/v8.h"
#include "include/libplatform/libplatform.h"

#include <unistd.h>

using namespace v8;

v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
  FILE* file = fopen(name, "rb");
  if (file == NULL) return v8::Handle<v8::String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
    i += read;
  }
  fclose(file);
  v8::Handle<v8::String> result =
      v8::String::NewFromUtf8(isolate, chars, v8::String::kNormalString, size);
  delete[] chars;
  return result;
}

void myfunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value u(args[0]);
  printf("in myfunc %s\n", *u);
}

void params_cb(Local<String> property, const PropertyCallbackInfo<Value> &info)
{
  printf("params\n");
  info.GetReturnValue().Set(String::NewFromUtf8( info.GetIsolate(), "01234567"));
}

void f2();

void mfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
  v8::String::Utf8Value u(args[0]);
  Handle<External> dta = Handle<External>::Cast(args.Data());
  printf("mfo IN! %s %p\n", *u, dta->Value());

  Handle<External> dta2 = Handle<External>::Cast(args.This()->GetInternalField(0));
  printf("mfo IZ! %s %p\n", *u, dta2->Value());
  args.GetReturnValue().Set(String::NewFromUtf8( args.GetIsolate(), "mfo"));


//  f2();
}

struct Wr
{
  static Persistent<ObjectTemplate> request_template_;
  static Handle<ObjectTemplate> templ;
  static void GetPath(Local<String> name, const PropertyCallbackInfo<Value>& info)
  {
    printf("aaaaaaaaa IN!\n");
    info.GetReturnValue().Set(String::NewFromUtf8( info.GetIsolate(), "tadata"));
  }


  void init(Isolate *isolate) {

    Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
    //templ = ObjectTemplate::New(isolate);
    result->SetInternalFieldCount(1);
  

    result->SetAccessor( String::NewFromUtf8(isolate, "path", String::kInternalizedString), GetPath);
    Handle<External> dta = External::New(isolate, (void*)0x333);
    result->Set(v8::String::NewFromUtf8(isolate, "mfo"), v8::FunctionTemplate::New(isolate, mfo, dta));
//    result->SetNamedPropertyHandler(params_cb);

    request_template_.Reset(isolate, result);
    templ = Local<ObjectTemplate>::New(isolate, request_template_);

  }

  //void wrapper(Isolate *isolate) {
  Handle<Object> wrapper(Isolate *isolate) {
    EscapableHandleScope handle_scope(isolate);
  
  
    Local<Object> result = templ->NewInstance();
  
    Handle<External> request_ptr = External::New(isolate, (void*)0x444);
  
    result->SetInternalField(0, request_ptr);
  
    return handle_scope.Escape(result);
  }

};

Isolate* isolate;
v8::Handle<v8::ObjectTemplate> global;


void f2()
{
 //   Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

    Context::Scope context_scope(context);

    v8::Handle<v8::String> source = ReadFile(isolate, "a.js");
    Local<Script> script = Script::Compile(source);
    Local<Value> result = script->Run();

    Local<v8::Value> value = context->Global()->Get(v8::String::NewFromUtf8(isolate, "b"));
    Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);
printf("i1\n");
      result = func->Call(context->Global(), 0, NULL);;
      Local<v8::String> res = Local<v8::String>::Cast(result);
      printf("result=%s\n", *String::Utf8Value(res));
printf("i2\n");
}

Persistent<ObjectTemplate> Wr::request_template_;
Handle<ObjectTemplate> Wr::templ;
int main(int argc, char* argv[]) {
  // Initialize V8.
  V8::InitializeICU();
  Platform* platform = platform::CreateDefaultPlatform();
  V8::InitializePlatform(platform);
  V8::Initialize();
  V8::SetFlagsFromString("--expose-gc", 11);

  /*Isolate* */isolate = Isolate::New();
  {
    Isolate::Scope isolate_scope(isolate);

    HandleScope handle_scope(isolate);



    /*v8::Handle<v8::ObjectTemplate>*/ global = v8::ObjectTemplate::New(isolate);
    global->Set(v8::String::NewFromUtf8(isolate, "myfunc"), v8::FunctionTemplate::New(isolate, myfunc));
    v8::Handle<v8::Context> context = v8::Context::New(isolate, NULL, global);

    Context::Scope context_scope(context);

    v8::Handle<v8::String> source = ReadFile(isolate, "a.js");

    Local<Script> script = Script::Compile(source);

    Local<Value> result = script->Run();

    // Convert the result to an UTF8 string and print it.
    String::Utf8Value utf8(result);
    printf("%s\n", *utf8);


f2();
printf("---------\n");
    Wr wrp;
    wrp.init(isolate);
    while (true) {
    HandleScope handle_scope(isolate);
      usleep(1000000);
      //Handle<Object> request_obj = Wr::templ->NewInstance();
      Local<Object> request_obj = wrp.wrapper(isolate);

      Local<Value> argv[] = { request_obj };

      Local<v8::Value> value = context->Global()->Get(v8::String::NewFromUtf8(isolate, "a"));
      Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);

      result = func->Call(context->Global(), 1, argv);;
      Local<v8::String> res = Local<v8::String>::Cast(result);
      printf("result=%s\n", *String::Utf8Value(res));


      request_obj.Clear();
      //isolate->RequestGarbageCollectionForTesting(Isolate::kFullGarbageCollection);

      HeapStatistics hs;
      isolate->GetHeapStatistics(&hs);
      printf("used=%ld\n", hs.used_heap_size());
    }
  }
  
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  delete platform;
  return 0;
}

