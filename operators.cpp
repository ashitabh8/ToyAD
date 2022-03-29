#include <functional>
#include <vector>
// #include "constant.h"
#include "fixed.hpp"
// #include "global.h"
#include "ios.hpp"
#include "math.hpp"
// #include "variable.h"
#include <typeinfo>
#include <ostream>
#include <stdlib.h>
#include <iostream>
#include <stack>
#include <set>
#include <map>
#include <queue>


template<typename T>
class Node{
private:

    inline static int idx;
    int node_idx;
public:
    bool isPlaceholder;
    // virtual void compute(){};
    virtual T getValue()  = 0;
    virtual T diff(int) =0;
    virtual void register_parent(Node<T> *) =0;
    virtual std::vector<Node<T> *> const getParents() =0;

    Node(){
      node_idx = idx;
      idx++;
      std::cout << "ID Allotted: " << node_idx << "\n";
    }

    int get_unq_node_idx() {
      return node_idx;
    }
};

// int Node::idx

class DummyNode{
  public:
    DummyNode() {}
};

// template<typename T>
// class Placeholder : public Node<T> {
  // template<typename T>
//
  // void * entry;
  // bool isConstant;
  // public:
    // Placeholder(Constant<T> &c){
      // this->isPlaceholder = true;
    // Constant<T> entry(c.getValue()) ;
    // entry = &c;
    // isConstant = true;
    // std::cout << "Placeholder recieved value: " << entry <<"\n";
    // }
//
    // Placeholder(Variable<T> &c){
      // this->isPlaceholder = true;
    // Variable<T> entry(c.getValue()) ;
    // entry = &c;
    // isConstant = false;
    // std::cout << "Placeholder recieved value: " << entry <<"\n";
    // }
//
    // T getValue()
    // {
      // if(isConstant)
      // {
        // return (static_cast<Constant<T> *>(entry))->getValue();
      // }
      // else{
        // return (static_cast<Variable<T> *>(entry))->getValue();
      // }
    // }
//
// };

template <typename T>
class Placeholder : public Node<T>,public DummyNode  {
  // private:
    // static int placeholder_index;
  // protected:
    // int placeholder_ID;
  public: 
    Placeholder() {
      // placeholder_ID = placeholder_index;
      // placeholder_index++;
      this->isPlaceholder = true;
    }
    // int get_ID(){
      // return placeholder_ID;
    // }
    // virtual int getPlaceholderType() =0;
};


template <typename TYPE>
class Constant: public Placeholder<TYPE>{
  private:
    TYPE value;
    std::string name;
    int grad =0;
    std::vector<Node<TYPE> *> parents;
  public:
    // Constant(TYPE val){
      // value = val;
    // }

    Constant(auto val){
      TYPE num{val};
      value = num;
      std::cout << "Constant Node allotted\n";
    }

    Constant(TYPE val, std::string n){
      value = val;
      name = n;
    }

    TYPE getValue()  {
      return value;
    }

    int getGrad(){
      return grad;
    }

    TYPE diff(int wrt_id){
      return 0;
    }


    void register_parent(Node<TYPE> * parent) {
      parents.push_back(parent);
    }

    std::vector<Node<TYPE> *> const getParents()  {
      return parents;
    }
    // int getPlaceholderType(){ // {0 -> constant, 1-> variable}
      // return 0;
    // }
};


template<typename T>
std::ostream& operator<<(std::ostream& os,  Constant<T> &obj) {
     os << obj.getValue();
     return os;
}

template <typename TYPE>
class Variable: public Placeholder<TYPE>{
  private:
    TYPE value;
    std::string name;
    TYPE gradient;
    std::vector<Node<TYPE> *> parents;
  public:
    inline static int count;
    Variable(auto val){
      TYPE num{val};
      value = num;
      name = "x_"+ std::to_string(count);
      count++;
    }

    std::string getName_str() const{
      return name;
    }

    TYPE getValue() {
      return value;
    }

    void setValue(TYPE input) {
      value = input;
    }

    TYPE diff(int wrt_id){
      if(wrt_id == this->get_unq_node_idx()){
        return 1;
      }
      else{
        return 0;
      }
    }
    void register_parent(Node<TYPE> * parent) {
      parents.push_back(parent);
    }

    std::vector<Node<TYPE> *> const getParents()  {
      return parents;
    }
    // int getPlaceholderType(){ // {0 -> constant, 1-> variable}
      // return 0;
    // }
};

// template<typename T>
// int Variable<T>::count;

template<typename T>
std::ostream& operator<<(std::ostream& os,  Variable<T> &obj) {
     os << obj.getName_str() << ": " << obj.getValue();
     return os;
}

template<typename T>
class Operator : public Node<T>,public DummyNode {
  private:
    std::vector<Node<T> *> inputs; //it's node * because you can have input as either operator or placeholder
    std::vector<Node<T> *> parents;
    // Placeholder<T> * output;
    T output;
    T diff;
    // static int operator_index; // Unique identifier of Operator instances, there to make future passes easier.
  protected:
    int OP_ID;
    // int unq_op_id;
  public:
    bool isComputed=false;
    bool isDiffComputed = false;

    Operator()
    {
      std::cout <<"Default C\n";
      this->isPlaceholder = false;

      // operator_index++;
    }

    Operator(Node<T> &a, Node<T> &b){
      // this->isPlaceholder = false;
      std::cout << "New: Node operator\n";
      inputs.push_back(&a);
      inputs.push_back(&b);
      // setOP_ID();
      this->isPlaceholder = false;
      a.register_parent(this);
      b.register_parent(this);
      // unq_op_id =  operator_index;
      // operator_index++;
    }

    std::vector<Node<T> *> &getInputs(){
      return inputs;
    }

    // void setOpID(int id) {op_id = id;}
    T getOutput() {return output;}
    T getDiff() {return diff;}

    void setOutput(T out){output = out;}
    void setDiff(T d){diff = d;}
    virtual void setOP_ID() = 0;

    int getOP_ID(){
      return OP_ID;
    }
    void register_parent(Node<T> * parent) {
      parents.push_back(parent);
    }

    std::vector<Node<T> *> const getParents()  {
      return parents;
    }

    // int get_unq_id(){
      // return unq_op_id;
    // }
};


template <typename T>
class add: public Operator<T> {
  int CHILD_ID = 1;
  public:
    add(Node<T> &a, Node<T> &b) : Operator<T>(a, b){setOP_ID();std::cout<<"OP:ADD\n";}

    T getValue() {
      if(this->isComputed){
        return this->getOutput(); 
      }
      T temp =0;
      for(auto curr_input: this->getInputs()) // could be optimized further - the casts would not be needed from the second iteration of the graph
      {
        temp+= curr_input->getValue();
      }
      this->isComputed = true;
      this->setOutput(temp);
      return temp;
    }


    T diff(int wrt_v){
      getValue();
      if(this->isDiffComputed) { 
        return this->getDiff();
      }
      else{
      T temp =0;
      for(auto curr_input: this->getInputs()) // could be optimized further - the casts would not be needed from the second iteration of the graph
      {
        temp+= curr_input->diff(wrt_v);
      }
      this->isDiffComputed = true;
      this->setDiff(temp);
      return temp;
      }
    }

    void setOP_ID(){
      this->OP_ID = CHILD_ID;
    }
};


template <typename T>
class sub: public Operator<T> {
  int CHILD_ID = 2;
  public:
    sub(Node<T> &a, Node<T> &b) : Operator<T>(a, b){setOP_ID();std::cout<<"OP:SUB\n";}

    T getValue() {
      if(this->isComputed){
        return this->getOutput(); 
      }
      // T temp =this->getInputs()->at(0)->getValue() - this->getInputs()->at(1)->getValue();
      T temp =(this->getInputs()).at(0)->getValue() - (this->getInputs()).at(1)->getValue();
      this->isComputed = true;
      this->setOutput(temp);
      return temp;
    }

    void setOP_ID(){
      this->OP_ID = CHILD_ID;
    }
    T diff(int wrt_v){
      getValue();
      if(this->isDiffComputed) { 
        return this->getDiff();
      }
      else{
      T temp =(this->getInputs()).at(0)->diff(wrt_v) - (this->getInputs()).at(1)->diff(wrt_v);
      this->isDiffComputed = true;
      this->setDiff(temp);
      return temp;
      }
    }
};

template <typename T>
class mul: public Operator<T> {
  int CHILD_ID = 3;
  public:
    mul(Node<T> &a, Node<T> &b) : Operator<T>(a, b){setOP_ID();std::cout<<"OP:MUL\n";}

    T getValue() {
      if(this->isComputed){
        return this->getOutput(); 
      }
      T temp =(this->getInputs()).at(0)->getValue() * (this->getInputs()).at(1)->getValue();
      this->isComputed = true;
      this->setOutput(temp);
      return temp;
    }
    void setOP_ID(){
      this->OP_ID = CHILD_ID;
    }

    T diff(int wrt_v) { 
      T curr_val = getValue();
      if(this->isDiffComputed) { 
        return this->getDiff();
      }
      else{
        T temp =(this->getInputs()).at(0)->getValue() * (this->getInputs()).at(1)->diff(wrt_v) + (this->getInputs()).at(1)->getValue() * (this->getInputs()).at(0)->diff(wrt_v) ;
        this->isDiffComputed = true;
        this->setDiff(temp);
        return temp;
      }
    }
    

};

template <typename T>
class division: public Operator<T> {
  int CHILD_ID = 4;
  public:
    division(Node<T> &a, Node<T> &b) : Operator<T>(a, b){setOP_ID();std::cout<<"OP:DIV\n";}
    T getValue() {
      if(this->isComputed){
        return this->getOutput(); 
      }
      // T temp =this->getInputs()->at(0)->getValue() / this->getInputs()->at(1)->getValue();
      T temp =(this->getInputs()).at(0)->getValue() / (this->getInputs()).at(1)->getValue();
      this->isComputed = true;
      this->setOutput(temp);
      return temp;
    }

    void setOP_ID(){
      this->OP_ID = CHILD_ID;
    }

    T diff(int wrt_v) { 
      T curr_val = getValue();
      if(this->isDiffComputed) { 
        return this->getDiff();
      }
      else{
        T temp =(this->getInputs()).at(1)->getValue() * (this->getInputs()).at(0)->diff(wrt_v) - (this->getInputs()).at(0)->getValue() * (this->getInputs()).at(1)->diff(wrt_v) ;
        temp = temp/((this->getInputs()).at(1)->getValue() * (this->getInputs()).at(1)->getValue());
        this->isDiffComputed = true;
        this->setDiff(temp);
        return temp;
      }
    }
};

//Operator overloading
// Change this -> this should be lazy evaluation and not eager evaluation

// template <typename T>
// Constant<T> operator-(const Placeholder<T> &a, const Placeholder<T> &b){
  // return Constant<T>(a.getValue() - b.getValue());
// }

// template <typename T>
// Constant<T> operator*(const Placeholder<T> &a, const Placeholder<T> &b){
  // return Constant<T>(a.getValue() * b.getValue());
//static

// template <typename T>
// Constant<T> operator/(const Placeholder<T> &a, const Placeholder<T> &b){
  // return Constant<T>(a.getValue() / b.getValue());
// }

template<typename T>
T DiffEngine(std::vector<Node<T> *>&order, Variable<T> &wrt_v){
  std::map<int, T> NodeIDtoDiff;
  int idx = wrt_v.get_unq_node_idx();
  for(auto curr_node: order) {
   curr_node->diff(idx);
  }
  return (order.back())->diff(idx);
}

template< typename T>
class Expression{
  //TODO: The expression class should number the nodes not the node class. This will create confusion between multiple expressions running, or dangling Placeholders.

  std::vector<Node<T> *> ordering; // ordering is a topological sorting of the nodes.
  std::set<int> visited_nodes;
  std::map<int, T> NodeIDtoDiff;
  public:
      Node<T> *output_node;
      Expression () {};
      Expression<T> * operator<<=( Node<T> &rhs_node){
        std::cout<< " Assignment operator called\n";
        output_node = &rhs_node;
        topological_sort(*output_node);
        return this;
      }

      void topological_sort(Node<T> &curr_node){
        // assert(curr_node.isPlaceholder == false); // Currently output node needs to be an operator -> z = c or z = v do not work. Will handle this seperately.
        // std::stack<Node<T> *> op_stack;

        if(visited_nodes.find(curr_node.get_unq_node_idx()) == visited_nodes.end()) // If curr_node is not visited
        {
          visited_nodes.insert(curr_node.get_unq_node_idx());

          if(curr_node.isPlaceholder == false){

            Operator<T> * curr_op_parent =static_cast<Operator<T> *>(&curr_node);

            for(Node<T> *curr_child: curr_op_parent->getInputs())
            {
              topological_sort(*curr_child);
            }
          }

          ordering.push_back(&curr_node);
        }
      }

      T getValue(){
        output_node->getValue();
      }

      Node<T> &getOutput()
      {
        return output_node;
      }

      std::vector<Node<T> *> get_Nodes_Topologically()
      {
        return ordering;
      }

      // T diff(Variable<T> &wrt_v) {
        // DiffEngine(ordering,wrt_v);
      // }

      T diff(Variable<T> &wrt_v, std::map<Variable<T> *, T> &new_values) {
        //Update the values of Variables.

        // struct MapWrapper {
          // typedef std::map<Variable<T> &, T>::iterator it;
        // }
        std::queue<Node<T> *> q;
        typename std::template map<Variable<T> *, T>::iterator it;
        for( it = new_values.begin() ; it != new_values.end(); it++) {
          it->first->setValue(it->second);
          q.push(it->first);
        }
        bool isChanged = true;
        while(!q.empty() || isChanged)
        {
          Node<T> *curr_node = q.front();
          q.pop();
          for(Node<T> * parent: curr_node->getParents()) {
            Operator<T> * temp = static_cast<Operator<T> *>(parent);
            bool old_status = temp->isComputed;
            if(old_status == true) {
              isChanged = true;
              temp->isComputed = false;
              q.push(parent);
            }
            else{
              isChanged = false;
            }
          } 

          if(q.empty() && curr_node->getParents().size() == 0) {
            break;
          }
        } 
          // }
        return DiffEngine(ordering,wrt_v);
        std::cout << " Value of wrt_v: " << wrt_v.getValue() << std::endl;
      }

      void print_top_ordered_dbg() {
        std::cout << " Topological ordering of CG nodes\n";
        for(auto elem: ordering){
          std::cout << elem->get_unq_node_idx() << "\t";
        }
        std::cout << "\n";
      }

};


template <typename T>
Node<T> &operator+( Node<T> &a,  Node<T> &b){
  add<T> *temp = new add<T>(a,b);
  return *temp;
  // return Constant<T>(a.getValue() + b.getValue());
}

template <typename T>
Node<T> &operator*( Node<T> &a,  Node<T> &b){
  mul<T> *temp = new mul<T>(a,b);
  return *temp;
  // return Constant<T>(a.getValue() + b.getValue());
}

template <typename T>
Node<T> &operator-( Node<T> &a,  Node<T> &b){
  sub<T> *temp = new sub<T>(a,b);
  return *temp;
  // return Constant<T>(a.getValue() + b.getValue());
}

template <typename T>
Node<T> &operator/( Node<T> &a,  Node<T> &b){
  division<T> *temp = new division<T>(a,b);
  return *temp;
  // return Constant<T>(a.getValue() + b.getValue());
}
// PASSES
//
//

class AD_PASS {
  public:
    AD_PASS(){}
    virtual void runPass() =0;
};

class CSE_PASS: public AD_PASS{
  public: 
    template<typename T>
    CSE_PASS(Expression<T> &E){}

};


class FORWARD_PASS: public AD_PASS{
  public: 
    template<typename T>
    FORWARD_PASS(Expression<T> &E){}

};


int main(){
  Constant<float> c(1);
  // Constant<float> d(8);
  Variable<float> v(9);
  std::cout << "Constant: " << c <<"\n";
  std::cout << "Variable: " << v << "\n";
  // std::cout << "Addition C+V: " << c+v << "\n";

  Expression<float> z;
  z <<= c/v;
  std::cout << "Expression evaluation: "<< z.getValue() << "\n";
  z.print_top_ordered_dbg();
  std::map<Variable<float> *, float>  m1= {{&v,5}};
  std::cout << "Diff testing: " << z.diff(v,m1) << std::endl;
  // Placeholder<float> p(c);
  // Placeholder<float> x(d);
  // std::cout << x.getValue() ;
  // sub<float> op_1(p,x);
//
  // Operator<float> o_1(x,p);
  // std::cout << "Answer of add: " << add<float>(p,x).getValue();
  // std::cout << "Subtract of add: " << sub<float>(p,x).getValue();
  // std::cout << "Multiplication of add: " << mul<float>(p,x).getValue();
  // std::cout << "Division of add: " << division<float>(p,x).getValue();
  // std::cout << "Recursive combination: " << add<float>(p,op_1).getValue();
}
