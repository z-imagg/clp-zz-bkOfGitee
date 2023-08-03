class MyClass {
public:
    static int ZERO;
public:
    int myFunction(int a, int b);
};
int MyClass::ZERO=0.001;
int MyClass::myFunction(int a, int b) {
  return a + b;
}