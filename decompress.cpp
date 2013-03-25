#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <map>
#include <algorithm>
#include <queue>
#include <functional>
#include <bitset>
#include <time.h>
#include <unordered_map>
#include <cstdio>
#include <stdexcept>

using namespace std;
class Tree{
  private:
    struct TreeNode{
        TreeNode* t[2] {NULL};
        char c;
        bool leaf = false;
    };
    TreeNode* root;
  public:
    Tree(){
      root = new TreeNode;
    }
    void insert(string bs, char c, TreeNode* r){
      TreeNode* n =new TreeNode;
      int lr = bs[0] == '0' ? 0 : 1;
      if(r->t[lr] == NULL) r->t[lr] = n;
      if(bs.length() == 1){
        r->t[lr]->c = c;
        r->t[lr]->leaf = true;
        return;
      }
      insert(bs.substr(1), c, r->t[lr]);
    }
    void insert(string bs,char c){
      insert(bs, c,  this->root);
    }
    string decode(string bs){
      string out;
      TreeNode* temp = root;
      for(int i = 0; i < bs.length(); ++i){
          int lr = bs[i] == '0' ? 0 : 1;
          if(temp->t[lr]->leaf){
            out += temp->t[lr]->c;
            temp = this->root;
          }else{
            temp = temp->t[lr];
          }
      }
      return out;
    }
    void print(TreeNode* n){
      cout << "Node: " << n->c << "; ";
      cout << " L: ";
      if(!n->t[0]) cout << "null; ";
      else cout << n->t[0]->c << "; ";
      cout << "R: ";
      if(!n->t[1]) cout << "null " << endl;
      else cout << n->t[1]->c << " " << endl;
      //recurse
      if(n->t[0]) print(n->t[0]);
      if(n->t[1]) print(n->t[1]);

     }

    void print(){
      if(!this->root){
        cout << "tree is empty can not print";
      }else{
        print(this->root);
      }
    }
};

//Huffman key
Tree huff_key;

int main(int argc, char *argv[]){
  clock_t start = clock();
  //print out an error if there no file is provided
  if (argc <= 1) {
    cout << "Usage: " << argv[0] << " <Filename>" << endl;
    exit(1);
  }
  //Open file
  char* file_name = argv[1];
  ifstream ifs (file_name);
  //print error if file is not found
  if(!ifs.good()){
    cout<< "File " << argv[1] << " not found" << endl;
    exit(1);
  }
  //if the file is good load the whole thing into a string called content
  string line;
  string code;
  int key;
  while(getline(ifs, line)){
      if(line[0] == '*') break;
      line.pop_back();
      unsigned pos = line.find(" ");
      code = line.substr(0,pos);
      line = line.substr(pos+1);
      if(line == "space") key = ' ';
      else if(line ==  "newline") key = 10;
      else if(line == "tab") key = 9;
      else if(line == "return") key =13;
      else key = line[0];
      huff_key.insert(code, key);
      //cout << code<< " " << line << endl;
  }
  getline(ifs, line);
  long binary_length = stol(line);
  //get the rest of the file
  string encoded_data( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
  ifs.close();//close our file
  //build a binary string
  string binary_string;
  for(int i = 0; i < encoded_data.length(); ++i){
      binary_string += bitset<8>(encoded_data[i]).to_string();
  }
  string decoded =  huff_key.decode(binary_string.substr(0, binary_length)) ;
  ofstream os;
  os.open("test.txt");
  os << decoded;
  os.close();
  clock_t end = clock();
  cout << "Time Elapsed:" << (double)(end-start)/CLOCKS_PER_SEC<< "s" << endl;
  return 0;
}
