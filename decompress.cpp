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
#include <cstdio>
#include <stdexcept>

using namespace std;
//Decompresses zip301 huffman files
class Zip301Decompressor{
  private:
    struct TreeNode{
      TreeNode* t[2];
      char c;
      bool leaf;
      TreeNode(): t{NULL}, leaf(false){}
    };
    TreeNode* root;
  public:
    Zip301Decompressor(){
      root = new TreeNode;
    }
    //add char into tree placed according to binary string
    void add_to_key(string bs, char c, TreeNode* r){
      TreeNode* n =new TreeNode;
      int lr = bs[0] == '0' ? 0 : 1;
      if(r->t[lr] == NULL) r->t[lr] = n;
      if(bs.length() == 1){
        r->t[lr]->c = c;
        r->t[lr]->leaf = true;
        return;
      }
      add_to_key(bs.substr(1), c, r->t[lr]);
    }
    void add_to_key(string bs, char c){
      add_to_key(bs, c, this->root);
    }

    //Decode binary_string to original
    string decode(string const& bs){
      string out;
      TreeNode* temp = root;
      for(char c: bs){
        int lr = c == '0' ? 0 : 1;
        if(temp->t[lr]->leaf){
          out += temp->t[lr]->c;
          temp = this->root;
        }else{
          temp = temp->t[lr];
        }
      }
      return out;
    }
};


int main(int argc, char *argv[]){
  clock_t start = clock();
  //print out an error if there no file is provided
  if (argc <= 1) {
    cout << "Usage: " << argv[0] << " <Filename>" << endl;
    exit(1);
  }
  //Open file
  char* file_name = argv[1];
  ifstream ifs (file_name, ios::in | ios::binary);
  //print error if file is not found
  if(!ifs.good()){
    cout<< "File " << argv[1] << " not found" << endl;
    exit(1);
  }
  //Huffman key
  Zip301Decompressor decompressor;
  //if the file is good load the whole thing into a string called content
  string line;
  string code;
  int key;
  // Read the zip 301 from file and add to Zip301Decompressor's key
  while(getline(ifs, line)) {
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
    decompressor.add_to_key(code, key);
    //cout << code<< " " << line << endl;
  }
  //Retrieve the bit length at the end of the zip301 key
  getline(ifs, line);
  long binary_length = stol(line);
  //get the rest of the file
  string encoded_data( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>() ) );
  ifs.close();
  //build a binary string
  string binary_string;
  for(unsigned char c: encoded_data){
    binary_string += bitset<8>(c).to_string();
  }
  //decode the binary_string
  string decoded =  decompressor.decode(binary_string.substr(0, binary_length)) ;
  //determine output filename
  string out_file_name = file_name;
  out_file_name = out_file_name.substr(0, out_file_name.find('.')) + "2.txt";
  //save to file
  ofstream os;
  os.open(out_file_name);
  os << decoded;
  os.close();
  clock_t end = clock();
  cout << "Time Elapsed:" << (double)(end-start)/CLOCKS_PER_SEC<< "s" << endl;
  return 0;
}
