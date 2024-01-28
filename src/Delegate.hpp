#pragma once

template<typename Signature>
class DelegateBindingBase;

template<typename Ret, typename... Args>
class DelegateBindingBase<Ret(Args...)>
{
  public:
    virtual ~DelegateBindingBase() = default;

    virtual DelegateBindingBase<Ret (Args...)>* Copy() const = 0;
    virtual Ret Call(Args... args) const = 0;
};

template<typename Signature>
class RegularFuncBinding;

template<typename Ret, typename... Args>
class RegularFuncBinding<Ret(Args...)>
  : public DelegateBindingBase<Ret(Args...)>
{
  public:
    typedef Ret (*FuncPtr)(Args...);

    RegularFuncBinding(FuncPtr function) 
      : m_Function(function) {}

    DelegateBindingBase<Ret(Args...)>* Copy() const final
    {
      return new RegularFuncBinding<Ret(Args...)>(m_Function);
    }

    Ret Call(Args... args) const final
    {
      return (*m_Function)(args...);
    }

  private:
    FuncPtr     m_Function;
};

template<bool IsConst, typename Object, typename Signature>
class MemberFuncPtr;

template<typename Object, typename Ret, typename... Args>
class MemberFuncPtr<false, Object, Ret(Args...)>
{
  public:
    typedef Ret (Object::*Type)(Args...);
};

template<typename Object, typename Ret, typename... Args>
class MemberFuncPtr<true, Object, Ret(Args...)>
{
  public:
    typedef Ret (Object::*Type)(Args...) const;
};

template<bool IsConst, typename Object, typename Signature>
class MemberFuncBinding;

template<bool IsConst, typename Object, typename Ret, typename... Args>
class MemberFuncBinding <IsConst, Object, Ret(Args...)>
  : public DelegateBindingBase <Ret(Args...)>
{
  public:
    typedef typename MemberFuncPtr<IsConst, Object, Ret(Args...)>::Type FuncPtr;

    MemberFuncBinding(Object* object, FuncPtr function) 
      : m_Object(object), m_Function(function) {}

    DelegateBindingBase<Ret(Args...)>* Copy() const final
    {
      return new MemberFuncBinding<IsConst, Object, Ret(Args...)>(m_Object, m_Function);
    }

    Ret Call(Args... args) const final
    {
      return (m_Object->*m_Function)(args...);
    }

  private:
    Object* m_Object;
    FuncPtr m_Function;
};

template<typename LambdaFuncPtr, typename Signature>
class LambdaFuncBinding;

template<typename LambdaFuncPtr, typename Ret, typename... Args>
class LambdaFuncBinding<LambdaFuncPtr, Ret(Args...)>
  : public DelegateBindingBase<Ret(Args...)>
{
  public:
    LambdaFuncBinding(LambdaFuncPtr function) 
      : m_Function(function) {}

    DelegateBindingBase<Ret(Args...)>* Copy() const final
    {
      return new LambdaFuncBinding<LambdaFuncPtr, Ret(Args...)>(m_Function);
    }

    Ret Call(Args... args) const final
    {
      return Ret((m_Function)(args...));
    }  

  private:
    LambdaFuncPtr     m_Function;
};

template<typename Signature>
class Delegate;

template<typename Ret, typename... Args>
class Delegate<Ret(Args...)>
{
  public:
    Delegate() = default;

    Delegate(Ret (*function)(Args...)) 
      : m_Binding(new RegularFuncBinding<Ret(Args...)>(function)) {}

    template<typename Object>
    Delegate(Object* object, Ret (Object::*function)(Args...))
      : m_Binding(new MemberFuncBinding<false, Object, Ret (Args...)>(object, function)) {}

    template<typename Object>
    Delegate(Object* object, Ret (Object::*function)(Args...) const)
      : m_Binding(new MemberFuncBinding<true, Object, Ret (Args...)>(object, function)) {}

    template<typename LambdaFuncPtr>
    Delegate(LambdaFuncPtr function) 
      : m_Binding(new LambdaFuncBinding<LambdaFuncPtr, Ret(Args...)>(function)) {}

    Delegate(const Delegate& other) 
    {
      m_Binding = other.m_Binding->Copy();
    }

    Delegate(Delegate&& other)
    {
      m_Binding = other.m_Binding;
      other.m_Binding = nullptr;
    }

    ~Delegate() 
    {
      if (m_Binding != nullptr)
      {
        delete m_Binding;
      }
    }

    Delegate& operator=(const Delegate& other)
    {
      m_Binding = other.m_Binding->Copy();
      return *this;
    }

    Delegate& operator=(Delegate&& other)
    {
      m_Binding = other.m_Binding;
      other.m_Binding = nullptr;
      return *this;
    }

    Ret operator()(Args... args) const 
    { 
      return m_Binding->Call(args...); 
    }

    Ret Call(Args... args) const 
    { 
      return m_Binding->Call(args...); 
    }

  private:
    DelegateBindingBase<Ret(Args...)>* m_Binding = nullptr;
};