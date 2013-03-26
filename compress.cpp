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

// A node Tree to build to huffman code
class Node {
  public:
    // frequency of char or sum of two chars frequency if InnerNode
     int freq;
    Node(int freq){
      this->freq = freq;
    }
    virtual void print() {};
    // You can't compare pointers so this struct is put in place to overide the compare operator.
    // http://stackoverflow.com/questions/1541560/stl-priority-queue-on-custom-class
    struct CompareNode : public binary_function<Node*, Node*, bool>{
      bool operator()(const Node* l, const Node* r) const {
        return l->freq > r->freq;
      }
    };
};
class InnerNode : public Node{
  public:
    Node* left;
    Node* right;
    InnerNode(Node* l, Node* r) : Node(r->freq + l->freq){
      this->left = l;
      this->right = r;
    }
};
class LeafNode : public Node{
  public:
    char c;
    LeafNode(int freq, char c) : Node(freq){
      this->c = c;
    }
};

// Compresses a string using the Huffman Algorithm and saves it to a zip301 file
class Huffman{
  private:
     // frequency table for each character and it's frequency. I found
     // that using simple arrays was much faster than unordered hash
    unsigned long char_freq[256] = {0};
    //Huffman key
    string huff_key[256];
    Node* huffman_tree = NULL;
    //a reference to the string representation of the data to be compressed
    int binary_string_length;
    string binary_string;
    vector<unsigned char> bytes;
  public:
    Huffman() {}
    // build a tree we can traverse through in order to create our
    // huffman codes
    void build_huffman_tree(){
      //Using Priority Queue so we don't need to sort
      priority_queue<Node*, vector<Node*>, Node::CompareNode> nodes;
      // for each char insert a new leaf node using the global char_freq array
      for (int i = 0; i < 256; ++i) {
        if(char_freq[i]) nodes.push(new LeafNode(char_freq[i], (char)i));
      }
      //combine the two smallest leafNodes into an InnerNode until there is only one node remaining
      while (nodes.size()>1) {
        Node *n1 = nodes.top();
        nodes.pop();
        Node *n2 = nodes.top();
        nodes.pop();
        nodes.push(new InnerNode(n1,n2));
      }
      //return the remaining inner node to be the parent node
      huffman_tree =  nodes.top();
    }
    //Recursive algorithm that builds the huff_key array
    void generate_key(Node* root, string bs){
       // check if child class type is either leaf or inner node.
       // http://stackoverflow.com/questions/4589226/type-checking-in-c
      if(LeafNode* ln = dynamic_cast<LeafNode*>(root)){
        huff_key[ln->c] = bs;
      }else if(InnerNode* in = dynamic_cast<InnerNode*>(root)){
        string bsl = bs+"0";
        generate_key(in->left, bsl);
        string bsr = bs + "1";
        generate_key(in->right, bsr);
      }
    }
     // returns a string representation of the Huffman Key
     // for the key for the output zip301 file
    string print_huff_key(){
      string hk;
      for (int i = 0; i < 256; ++i) {
        if (huff_key[i].length()) {
          hk += huff_key[i] + " ";
          if (i == 10) hk += "newline";
          else if (i == 13) hk += "return";
          else if (i == 32) hk += "space";
          else if (i == 9) hk += "tab";
          else hk += i;
          hk += "\n";
        }
      }
      hk += "*****\n" + this->binary_string_length + '\n';
      return hk;
    }
     // build and array of character Frequencies char_freq
    void build_frequency_table(string const& content){
      for(char c: content){
        char_freq[(int)c]++;
      }
    }
     // Encodes input string as binary string of 0s and 1s
    string to_binary_string(string const& content){
        string bs;//binary string
        for(char c: content){
            bs += huff_key[c];
        }
        binary_string_length = bs.length();
        return bs;
    }
     // Takes binary string and converts it into unsigned char vector of bytes
     void generate_bytes(string& binary_string){
        if (int buffer = binary_string.length() % 8) {
            binary_string.append(buffer,'0');
        }
        for (int i = 0; i < binary_string.size(); i+=8) {
          bytes.push_back((char) stol(binary_string.substr(i,8), nullptr,2));
        }
    }
    // save compressed file in zip301 format with print_huff_key followed
    // by the compressed bytes
    void save_zip301(vector<unsigned char> const& bytes, string const& out_name){
        ofstream os;
        os.open("test.zip301", ios::out|ios::binary);
        string key = print_huff_key();
        os << key;
        os.write((const char*)&bytes[0], bytes.size());
        os.close();
    }
    //compress the input file into the zip301 format
    void compress_zip301(string const& to_encode, string out_filename){
      build_frequency_table(to_encode);
      build_huffman_tree();
      generate_key(huffman_tree, "");
      string encoded_binary_string = to_binary_string(to_encode);
      generate_bytes(encoded_binary_string);
      save_zip301(bytes, "test.zip301");
    }
};

int main(int argc, char *argv[]){
  //start a clock for poor mans profiling
  clock_t start = clock();
  //print out an error if there no file is provided
  if (argc <= 1) {
    cout << "Usage: " << argv[0] << " <Filename>" << endl;
    exit(1);
  }
  //Open file
  char* in_filename = argv[1];
  ifstream ifs (in_filename);
  //print error if file is not found
  if(!ifs.good()){
    cout<< "File " << argv[1] << " not found" << endl;
    exit(1);
  }
  //if the file is good load the whole thing into a string called content
  string file_content( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
  ifs.close();
  Huffman* h = new Huffman();
  //determine our output file name
  string temp = in_filename;
  int pos = temp.find('a');
  string out_filename =  temp.substr(0, pos) + ".zip301";
  //compress and save out file
  h->compress_zip301(file_content, out_filename);
  clock_t end = clock();
  cout << "Time Elapsed:" << (double)(end-start)/CLOCKS_PER_SEC<< "s" << endl;
  return 0;
}
