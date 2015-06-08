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

void log(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value u(args[0]);
  printf("[LOG] %s\n", *u);
}

v8::Handle<v8::Context> context;
void register_event(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::String::Utf8Value u(args[0]);
  printf("[reg] %s\n", *u);
  Local<v8::Function> func = v8::Local<v8::Function>::Cast(args[1]);
  func->Call(context->Global(), 0, NULL);
}



Isolate* isolate;
v8::Handle<v8::ObjectTemplate> global;



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
    global->Set(v8::String::NewFromUtf8(isolate, "log"), v8::FunctionTemplate::New(isolate, log));
    global->Set(v8::String::NewFromUtf8(isolate, "register_event"), v8::FunctionTemplate::New(isolate, register_event));
    context = v8::Context::New(isolate, NULL, global);
    //v8::Handle<v8::Context> context = v8::Context::New(isolate, NULL, global);

    Context::Scope context_scope(context);

    v8::Handle<v8::String> source = ReadFile(isolate, "t.js");

    Local<Script> script = Script::Compile(source);

    Local<Value> result = script->Run();

    // Convert the result to an UTF8 string and print it.
    String::Utf8Value utf8(result);
    printf("%s\n", *utf8);


//f2();
printf("---------\n");
      Local<v8::Value> value = context->Global()->Get(v8::String::NewFromUtf8(isolate, "test"));
      Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);
    while (true) {
    HandleScope handle_scope(isolate);
      usleep(1000000);
      //Handle<Object> request_obj = Wr::templ->NewInstance();


      result = func->Call(context->Global(), 0, NULL);
      Local<v8::String> res = Local<v8::String>::Cast(result);
      printf("result=%s\n", *String::Utf8Value(res));


      isolate->RequestGarbageCollectionForTesting(Isolate::kFullGarbageCollection);

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

