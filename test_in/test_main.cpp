#include <iostream>
/**
 * 这是一个示例类
 */
class MyClass {
public:
    static int ZERO;
public:
    /**
     * 这是一个示例成员函数
     * @param a 参数a
     * @param b 参数b
     * @return 返回结果
     */
    int myFunction(int a, int b) {
      return a + b;
    }
};

// 这是一个示例全局函数
/**
 * @brief 这是一个示例全局函数
 * @param x 参数x
 * @param y 参数y
 * @return 返回结果
 */
int myGlobalFunction(int x, int y) {
  return x * y;
}

int main() {
  MyClass obj;
  int result = obj.myFunction(10, 20);
  int x,y,z;
  if(true)
  x = y + MyClass::ZERO   ;   

  int product = myGlobalFunction(5, 6);
  std::cout << "Product: " << product << std::endl;

  return 0;
}