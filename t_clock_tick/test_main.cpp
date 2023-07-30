
void empty_func(int z, float y){

}
class Person{

};
class User{
public:
    operator Person() const;
};

User::operator Person() const {
  return Person();
}
