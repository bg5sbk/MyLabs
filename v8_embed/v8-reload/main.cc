#include <v8.h>

using namespace v8;

const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

Handle<String> ReadFile(const char* name) {
  FILE* file = fopen(name, "rb");
  if (file == NULL) return Handle<String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = fread(&chars[i], 1, size - i, file);
    i += read;
  }
  fclose(file);
  Handle<String> result = String::New(chars, size);
  delete[] chars;
  return result;
}

Handle<Value> Print(const Arguments& args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    HandleScope handle_scope;
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    String::Utf8Value str(args[i]);
    const char* cstr = ToCString(str);
    printf("%s", cstr);
  }
  printf("\n");
  fflush(stdout);
  return Undefined();
}

Handle<Value> Include(const Arguments& args) {
  if (args.Length() == 1) {
    HandleScope handle_scope;
    String::Utf8Value str(args[0]);
    Handle<String> source = ReadFile(*str);
    Handle<Script> script = Script::Compile(source);
    return script->Run();
  }
  return Undefined();
}

void InitJS() {
  Handle<String> source = String::New("include('main.js')");
  Handle<Script> script = Script::Compile(source);
  script->Run();
}

int main(int argc, char* argv[]) {
  char c;

  HandleScope handle_scope;

  Handle<ObjectTemplate> global = ObjectTemplate::New();

  global->Set(String::New("print"), FunctionTemplate::New(Print));
  global->Set(String::New("include"), FunctionTemplate::New(Include));

  Persistent<Context> context = Context::New(NULL, global);
  
  Context::Scope context_scope(context);

  InitJS();

  printf("> ");

  while ((c = getchar()) != 'q') {
    if (c == '\r' || c == '\n')
      continue;

    if (c == 'r')
      InitJS();

    Handle<String> mod_name = String::New("mod_player");

    Handle<Value> mod_value = context->Global()->Get(mod_name);

    Handle<Object> mod_object = Handle<Object>::Cast(mod_value);

    Handle<Value> fun_name = String::New("login");

    Handle<Value> fun_value = mod_object->Get(fun_name);

    Handle<Function> fun_object = Handle<Function>::Cast(fun_value);

    if (fun_object->IsFunction()) {
      fun_object->Call(context->Global(), 0, NULL);
    }
    else {
      printf("function not found\n");
    }

    printf("> ");
  }
  
  context.Dispose();

  return 0;
}
