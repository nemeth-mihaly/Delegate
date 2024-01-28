# Delegate #

## Usage ##

```
static void RegularFunc(int argc, char** argv) {}

auto delegate = Delegate<void(int, char**)>(&RegularFunc);
delegate(argc, argv); // or delegate.Call(...);

// -------------------------

class Object
{
  public:
    void MemberFunc(int argc, char** argv) const {}
};

Object object;

auto delegate = Delegate<void(int, char**)>(&object, &Object::MemberFunc);
delegate(argc, argv); // or delegate.Call(...);

// -------------------------

auto delegate = Delegate<void(int, char**)>([](int argc, char** argv) {});

delegate(argc, argv); // or delegate.Call(...);

```