class MyClass {
public:
    static int ZERO;
public:
    int myFunction(int a, int b);
};
int MyClass::ZERO=0.001;
int MyClass::myFunction(int a, int b) {
  float fff = a + b / 10  + MyClass::ZERO ;
  return a + b;
}