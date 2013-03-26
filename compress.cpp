/*****************************************
 * TODO: rename variables to more logical names
 * TODO: Make Huffman class and move all functions
 *       into it
 * TODO: for speed optimization
 * TODO: Insert profiling out where specifed by teacher
 * TODO: make output file dynamic
 * *****************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <queue>
#include <functional>
#include <bitset>
#include <time.h>
#include <cstdio>

using namespace std;
/**************************************
 * frequency table for each character
 * and for our key
 * I found that using simple arrays was
 * much faster than unordered hash.....
 * Need to test tree traversal
 * ************************************/
unsigned long char_freq[256] = {0};
//Huffman key
string huff_key[256];
class Huffman{

};
class Node {
  public:
    int freq;
    Node(int freq){
      this->freq = freq;
    }
    virtual void print(){
    }
};
class InnerNode : public Node{
  public:
    Node* left;
    Node* right;
    InnerNode(Node* l, Node* r) : Node(r->freq + l->freq){
      this->left = l;
      this->right = r;
    }
    void print(){
      cout << "Frequency: " << freq << " lFreq: " << left->freq << " rFreq: " << right->freq << endl;
    }
};
class LeafNode : public Node{
  public:
    char c;
    LeafNode(int freq, char c) : Node(freq){
      this->c = c;
    }
    void print(){
    if(c == 10) cout << "newline";
    else if(c == 13) cout << "return";
    else if(c == 32) cout << "space";
    else if(c == 9) cout << "tab";
    else cout << c;
    cout << " : " << freq << endl;
    }
};
/********************************************************************
 * Apparently you can't compare pointers so this struct is put in place
 * to overide the compare operators. Solution found on stackoverflow
 * http://stackoverflow.com/questions/1541560/stl-priority-queue-on-custom-class
 * *******************************************************************/
struct CompareNode : public binary_function<Node*, Node*, bool>{
  bool operator()(const Node* l, const Node* r) const {
    return l->freq > r->freq;
  }
};
/************************************************************
 * build a tree we can travers through in order to create our
 * huffman codes
 * ***********************************************************/
Node *build_tree(){
  /***********************************************************
   * Using a Priority_Queue to hold the nodes so I don't
   * have to sort them
   * *******************************************************/
  //a priority tree to hold out nodes in
  priority_queue<Node*, vector<Node*>, CompareNode> nodes;
  // for each char insert a new leaf node using the global char_freq array
  for(int i = 0; i < 256; ++i){
    if(char_freq[i]) nodes.push(new LeafNode(char_freq[i], (char)i));
  }
  //combine the two smallest leafNodes into an InnerNode until there is only one node remaining
  while(nodes.size()>1){
    Node *n1 = nodes.top();
    nodes.pop();
    Node *n2 = nodes.top();
    nodes.pop();
    nodes.push(new InnerNode(n1,n2));
  }
  //return the remaining inner node to be the parent node
  return nodes.top();
}
void generate_key(Node* root, string bs){
  /******************************************************************
   * check if child class type is eith
   * leaf or inner node. code from stackoverflow
   * http://stackoverflow.com/questions/4589226/type-checking-in-c
   * ****************************************************************/
  if(LeafNode* ln = dynamic_cast<LeafNode*>(root)){
    huff_key[ln->c] = bs;
  }else if(InnerNode* in = dynamic_cast<InnerNode*>(root)){
    string bsl = bs+"0";
    generate_key(in->left, bsl);

    string bsr = bs+"1";
    generate_key(in->right, bsr);
  }
}
/****************************************
 * TODO: rename this function
 * generates the key for the output
 * zip301 file
 * *************************************/
string print_huff_key(){
  string hk;
  for(int i = 0; i < 256; ++i){
    if(huff_key[i].length()){
      hk += huff_key[i] + " ";
      if(i == 10) hk += "newline";
      else if(i == 13) hk += "return";
      else if(i == 32) hk += "space";
      else if(i == 9) hk += "tab";
      else hk += i;
      hk += "\n";
    }
  }
  hk += "*****\n";
  return hk;
}
/********************************
 * build and array of character
 * Frequencies
 * *******************************/
void build_frequency_table(string &content){
  for(char c: content){
    char_freq[(int)c]++;
  }
}
/**********************************
 * take string and encode it as a
 * binary string of 0s and 1s
 * ********************************/
string encode_string(string &content){
    string bs;//binary string
    for(char c: content){
        bs += huff_key[c];
    }
    return bs;
}
/*************************************
 * Write key and encoded data to file
 * **********************************/
void huffman_binary(string &bs){
    ofstream os;
    os.open("test.zip301", ios::out|ios::binary);
    string key = print_huff_key() + to_string(bs.length()) + "\n";
    os << key;
    if(int buffer = bs.length()%8){
        bs.append(buffer,'0');
    }
    vector<unsigned char> lll;
    for(int i = 0; i < bs.size(); i+=8){
      lll.push_back((char) stol(bs.substr(i,8), nullptr,2));

    }
    os.write((const char*)&lll[0], lll.size());
    os.close();
}

int main(int argc, char *argv[]){
  //start a clock for poor mans profiling
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
  string content( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
  ifs.close();//close our file

  build_frequency_table(content);
  Node* root = build_tree();
  generate_key(root, "");
  string bs = encode_string(content);
  huffman_binary(bs);
  clock_t end = clock();
  cout << "Time Elapsed:" << (double)(end-start)/CLOCKS_PER_SEC<< "s" << endl;
  return 0;
}
