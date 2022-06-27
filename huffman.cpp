//#include <bits/stdc++.h>

//includes
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>
#include <map>
#include <set>
#include <algorithm>

//using namespace std;


//defines
#define ARRAY_LENGTH 256
#define MAX_TREE_HT 50

//settings
#define LOG 1
#define DO_ENCODE 1
#define DO_DECODE 1
#define IGNORE_LF_10 0 //Line Feed
#define IGNORE_CR_13 1 //Carriage Return //zalecane = 1

//--- FILES ---
//inputs
char* INPUT_FILE_NAME           = "huffmaninput.txt";
char* INPUT_MODEL_FILE_NAME     = "huffman.model";
char* INPUT_CODE_FILE_NAME     = "huffman.code";
char* INPUT_ENCODED_FILE_NAME   = "huffman.encoded";

//outputs
char* OUTPUT_MODEL_FILE_NAME    = "huffman.model";
char* OUTPUT_CODE_FILE_NAME    = "huffman.code";
char* OUTPUT_TREE_FILE_NAME    = "huffman.tree";
char* OUTPUT_ENCODED_FILE_NAME  = "huffman.encoded";
char* OUTPUT_DECODED_FILE_NAME  = "huffmanoutput.txt";

//other
char* LOG_FILE_NAME = "huffman_log.txt";

//fstreams
std::fstream MODEL_OUTPUT;
std::fstream TREE_OUTPUT;
std::fstream CODE_OUTPUT;
std::fstream INPUT_MODEL_FILE;
std::fstream INPUT_CODES_FILE;

//Log
std::ofstream out(LOG_FILE_NAME);   //przekierowanie strumienia
auto old_rdbuf = std::clog.rdbuf(); //Get rdbuf to reset value before exiting

//structs
typedef struct ModelNode     //struktura na symbole i ich czestosc
{
    int symbol;
    int frequency;
}Node;

struct MinHNode {
  unsigned freq;
  int item;
  struct MinHNode *left, *right;
};

struct MinH {
  unsigned size;
  unsigned capacity;
  struct MinHNode **arrayy;
};

//Arrays
Node NodeArray[ARRAY_LENGTH]; //tablica struktur Node

//maps
std::map<int,std::string> Symb_Code_Map;
std::map<std::string,int> Code_Symb_Map;

//sets
std::set<std::string> Codes_List_Set;

//ints
int DifferentChars = 0;
int TotalNumber = 0;
int licznikpieter;

//functions ENCODING
void Set_Node_Array() //ustawia kazda strukture na i/0
{
    for (int i = 0; i < 256; i++)
    {
        NodeArray[i].symbol = i;
        NodeArray[i].frequency = 0;
    }
}

void get_Codes_to_encode()
{
    INPUT_CODES_FILE.open(INPUT_CODE_FILE_NAME, std::ios::in);
    if(!INPUT_CODES_FILE.good())
        std::cerr<<"Blad w otwarciu pliku "<<INPUT_CODE_FILE_NAME<<std::endl;

    std::string tempLine;
    std::string SyCoStr[2];
    SyCoStr[0] = "";
    SyCoStr[1] = "";
    int j=0;
    int tempNum;
    if(LOG) std::clog<<"Wczytane kody: \n";
    while(getline(INPUT_CODES_FILE, tempLine))
    {
        if(LOG) std::clog<<tempLine<<std::endl;
        int tempLineLength = tempLine.length();
        for(int i=0;i<tempLineLength;i++)
        {
            if(tempLine[i] == 32)
            {
                j = 1;
                continue;
            }
            SyCoStr[j] = SyCoStr[j] + tempLine[i];
        }
        if(LOG) std::clog<<SyCoStr[0]<<" - "<<SyCoStr[1]<<std::endl;
        tempNum = atoi(SyCoStr[0].c_str());
        Symb_Code_Map.insert({tempNum, SyCoStr[1]});
        SyCoStr[0] = "";
        SyCoStr[1] = "";
        j = 0;
    }
    INPUT_CODES_FILE.close();
}

int CompareNodes(const void *item1, const void *item2) //funkcja porownujaca dla funkcji sortmodel
{
    Node *node1 = (Node*)item1; //pierwszy porownywany element
    Node *node2 = (Node*)item2; //drugi porownywany element
    int  compareResult = (node1->frequency - node2->frequency); //porownanie frequency
    if(compareResult==0) //jesli wystepuja tyle samo razy, porownujemy po symbolu
    {
        compareResult = (node1->symbol - node2->symbol);
        return -compareResult; //jak zwracamy normalnie- alfabetycznie (rosnaco)
    }
    return -compareResult;//zwracamy z minusem, aby najczestsze byly na poczatku
}

void SortModel()
{

    qsort(NodeArray, ARRAY_LENGTH, sizeof(Node),CompareNodes);//wywolanie funkcji qsort
}

void Generate_Model_From_file()
{
    if(LOG) std::clog<<"\nFunction: Generate_Model_From_File\n";
    if(LOG) std::clog<<"Pobrane bajty:\n";

    FILE *INPUT_FILE = fopen(INPUT_FILE_NAME, "rb");
        if(!INPUT_FILE)
            std::cerr<<"Blad w otwarciu pliku "<<INPUT_FILE_NAME<<std::endl;

    std::fstream OUTPUT_MODEL_FILE;
    OUTPUT_MODEL_FILE.open(OUTPUT_MODEL_FILE_NAME, std::ios::out);
        if(!OUTPUT_MODEL_FILE.good())
            std::cerr<<"Blad w otwarciu pliku "<<OUTPUT_MODEL_FILE_NAME<<std::endl;

    int byte = 0;
    while((byte = getc(INPUT_FILE)) != EOF)
    {
        if(LOG) std::clog<<byte<<" ";
        if(IGNORE_LF_10 && byte == 10) continue;
        if(IGNORE_CR_13 && byte == 13) continue;
        NodeArray[byte].frequency++;
        TotalNumber++;
    }
    if(LOG)
    {
        std::clog<<"\nTotalNumber: "<<TotalNumber;
        std::clog<<"\nModel: \n";
        for (int i = 0; i < 256; i++)
        {
            if(NodeArray[i].frequency>0)
            {
                std::clog<<NodeArray[i].symbol<<" "<<NodeArray[i].frequency<<std::endl;
            }
        }
    }
    if(LOG) std::clog<<"\n Sortowanie Modelu\n";
    SortModel();
    if(LOG) std::clog<<"\nModel Posortowany:\n";
    OUTPUT_MODEL_FILE<<TotalNumber<<std::endl;
    for (int i = 0; i < 256; i++)
    {
        if(NodeArray[i].frequency>0)
        {
            DifferentChars++;
            if(LOG)std::clog<<NodeArray[i].symbol<<" "<<NodeArray[i].frequency<<std::endl;
            OUTPUT_MODEL_FILE<<NodeArray[i].symbol<<" "<<NodeArray[i].frequency<<std::endl;
        }
    }
    fclose(INPUT_FILE);
    OUTPUT_MODEL_FILE.close();
}

void Encode()
{
    std::string codestring = "";
    std::string temp = "";
    std::string eight = "00000000";

    int codestringLength;
    int licznik = 0;

    char bitschar;

    FILE *INPUT_FILE = fopen(INPUT_FILE_NAME, "rb");
    if(!INPUT_FILE)
        std::cerr<<"Blad w otwarciu pliku "<<INPUT_FILE_NAME<<std::endl;

    std::fstream OUTPUT_ENCODED_FILE;
    OUTPUT_ENCODED_FILE.open(OUTPUT_ENCODED_FILE_NAME, std::ios::out);
    if(!OUTPUT_ENCODED_FILE.good())
        std::cerr<<"Blad w otwarciu pliku "<<OUTPUT_ENCODED_FILE_NAME<<std::endl;

    int byte = 0;
    while((byte = getc(INPUT_FILE)) != EOF)
    {
        auto itr = Symb_Code_Map.find(byte);
        temp = itr->second;
        codestring = codestring + temp;
    }

    codestringLength = codestring.length();

    if(LOG) std::clog<<"ZAKODOWANY CIAG: "<<codestring<<std::endl;
    if(LOG) std::clog<<"ZAPISANY CIAG: ";
    while(codestringLength > 0)
    {
        eight = "00000000";
        for(int i = 0; i < codestringLength && i < 8; i++)
        {
            eight[i] = codestring[i];
        }
        if(LOG) std::clog<<eight;
        int tempBtD = 1;
        int decVal = 0;
        for (int i = 7; i >= 0; i--)
        {
            if (eight[i] == '1')
            {
                decVal += tempBtD;
            }
            tempBtD = tempBtD * 2;
        }
        bitschar = decVal;
        OUTPUT_ENCODED_FILE<<bitschar;

        int actLen = codestring.length();
        if(actLen >= 8)
            codestring.erase(0,8);
        else
            codestring.erase(0,actLen);
        codestringLength = codestring.length();
    }
    fclose(INPUT_FILE);
    OUTPUT_ENCODED_FILE.close();
}



// ---- DRZEWO ----

// cale drzewko huffmana
struct MinHNode *newNode(int item, unsigned freq)
{
      struct MinHNode *temp = (struct MinHNode *)malloc(sizeof(struct MinHNode));

      temp->left = temp->right = NULL;
      temp->item = item;
      temp->freq = freq;

      return temp;
}

// dla pod drzewek
struct MinH *createMinH(unsigned capacity)
{
      struct MinH *minHeap = (struct MinH *)malloc(sizeof(struct MinH));
      minHeap->size = 0;
      minHeap->capacity = capacity;
      minHeap->arrayy = (struct MinHNode **)malloc(minHeap->capacity * sizeof(struct MinHNode *));

      return minHeap;
}

// do druku tabeli kodowej
void printarrayy(int arr[], int n)
{
      int i;
      for (i = 0; i < n; ++i)
        CODE_OUTPUT << arr[i];

      CODE_OUTPUT << "\n";
}

void swapMinHNode(struct MinHNode **a, struct MinHNode **b)
{
    struct MinHNode *t = *a;
    *a = *b;
    *b = t;
}

// do zmian  na stosie
void minHeapify(struct MinH *minHeap, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->arrayy[left]->freq < minHeap->arrayy[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->arrayy[right]->freq < minHeap->arrayy[smallest]->freq)
        smallest = right;

    if (smallest != idx)
    {
        swapMinHNode(&minHeap->arrayy[smallest], &minHeap->arrayy[idx]);
        minHeapify(minHeap, smallest);
    }
}

int checkSizeOne(struct MinH *minHeap)
{
    return (minHeap->size == 1);
}

// znajdz minimalne wartosci
struct MinHNode *extractMin(struct MinH *minHeap)
{
    struct MinHNode *temp = minHeap->arrayy[0];
    minHeap->arrayy[0] = minHeap->arrayy[minHeap->size - 1];

    --minHeap->size;
    minHeapify(minHeap, 0);

    return temp;
}

// wstawianie
void insertMinHeap(struct MinH *minHeap, struct MinHNode *minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->arrayy[(i - 1) / 2]->freq)
    {
        minHeap->arrayy[i] = minHeap->arrayy[(i - 1) / 2];
        i = (i - 1) / 2;
    }

  minHeap->arrayy[i] = minHeapNode;
}

void buildMinHeap(struct MinH *minHeap)
{
    int n = minHeap->size - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

int isLeaf(struct MinHNode *root)
{
    return !(root->left) && !(root->right);
}

struct MinH *createAndBuildMinHeap(int item[], int freq[], int size)
{
    struct MinH *minHeap = createMinH(size);

    for (int i = 0; i < size; ++i)
    minHeap->arrayy[i] = newNode(item[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

struct MinHNode *buildHfTree(int item[], int freq[], int size)
{
    struct MinHNode *left, *right, *top;
    struct MinH *minHeap = createAndBuildMinHeap(item, freq, size);

    while (!checkSizeOne(minHeap))
    {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('#', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}
void printHCodes(struct MinHNode *root, int arr[], int top)
{
    if (root->left)
    {
        arr[top] = 0;
        printHCodes(root->left, arr, top + 1);
    }

    if (root->right)
    {
        arr[top] = 1;
        printHCodes(root->right, arr, top + 1);
    }

    if (isLeaf(root))
    {
        int toPrint = root->item;
        CODE_OUTPUT << toPrint << " ";
        printarrayy(arr, top);
    }
}

void printHelper(struct MinHNode *root, std::string indent, bool last)
{

    if (root != NULL)
    {
        TREE_OUTPUT << indent;

        if (last)
        {
            TREE_OUTPUT << "R----";
            indent += "   ";
        }
        else
        {
            TREE_OUTPUT << "L----";
            indent += "|  ";
        }
        if(root->item == '#')
        {
            TREE_OUTPUT << licznikpieter << root->item << std::endl;
            licznikpieter--;
        }
        else
            TREE_OUTPUT << root->item << std::endl;

        printHelper(root->left, indent, false);
        printHelper(root->right, indent, true);
    }
}

  void printTree(struct MinHNode *root) {
    if (root) {
      printHelper(root, "", true);
    }
  }

// Wrapper function
void HuffmanCodes(int item[], int freq[], int size)
{
      struct MinHNode *root = buildHfTree(item, freq, size);

      int arr[MAX_TREE_HT], top = 0;

      CODE_OUTPUT.open(OUTPUT_CODE_FILE_NAME,std::ios::out);
      printHCodes(root, arr, top);
      CODE_OUTPUT.close();
      if(LOG)std::clog<<"\nZapisano do pliku "<<OUTPUT_CODE_FILE_NAME<<" tabele kodowa\n";
      TREE_OUTPUT.open(OUTPUT_TREE_FILE_NAME,std::ios::out);
      printTree(root);
      TREE_OUTPUT.close();
      if(LOG)std::clog<<"\nZapisano do pliku "<<OUTPUT_TREE_FILE_NAME<<" model drzewa\n";
}

//functions DECODING
void get_Total_Num()
{
    INPUT_MODEL_FILE.open(INPUT_MODEL_FILE_NAME, std::ios::in);
        if(!INPUT_MODEL_FILE.good())
            std::cerr<<"Blad w otwarciu pliku "<<INPUT_MODEL_FILE_NAME<<std::endl;

    std::string TempStr; //liczna totalNumber pobrana z pliku.model
    getline(INPUT_MODEL_FILE,TempStr);
    int tempNum = atoi(TempStr.c_str());
    TotalNumber = tempNum;
    if(LOG) std::clog<<"Wczytane z modeli TotalNumber: "<<TotalNumber<<"\n";
    INPUT_MODEL_FILE.close();
}

void get_Codes_to_decode()
{
    INPUT_CODES_FILE.open(INPUT_CODE_FILE_NAME, std::ios::in);
        if(!INPUT_CODES_FILE.good())
            std::cerr<<"Blad w otwarciu pliku "<<INPUT_CODE_FILE_NAME<<std::endl;

    std::string tempLine;
    std::string SyCoStr[2];
    SyCoStr[0] = "";
    SyCoStr[1] = "";
    int j=0;
    int tempNum;
    if(LOG) std::clog<<"Wczytane kody: \n";
    while(getline(INPUT_CODES_FILE, tempLine))
    {
        if(LOG) std::clog<<tempLine<<std::endl;
        int tempLineLength = tempLine.length();
        for(int i=0;i<tempLineLength;i++)
        {
            if(tempLine[i] == 32)
            {
                j = 1;
                continue;
            }
            SyCoStr[j] = SyCoStr[j] + tempLine[i];
        }
        if(LOG) std::clog<<SyCoStr[0]<<" - "<<SyCoStr[1]<<std::endl;
        tempNum = atoi(SyCoStr[0].c_str());
        Code_Symb_Map.insert({SyCoStr[1], tempNum});
        Codes_List_Set.insert(SyCoStr[1]);
        SyCoStr[0] = "";
        SyCoStr[1] = "";
        j = 0;
    }
    INPUT_CODES_FILE.close();
}

void Decode()
{
    FILE *INPUT_ENCODED_FILE = fopen(INPUT_ENCODED_FILE_NAME, "rb");
        if(!INPUT_ENCODED_FILE)
            std::cerr<<"Blad w otwarciu pliku "<<INPUT_ENCODED_FILE_NAME<<std::endl;

    std::fstream OUTPUT_DECODED_FILE;
    OUTPUT_DECODED_FILE.open(OUTPUT_DECODED_FILE_NAME, std::ios::out);
        if(!OUTPUT_DECODED_FILE.good())
            std::cerr<<"Blad w otwarciu pliku "<<OUTPUT_DECODED_FILE_NAME<<std::endl;

    int byte;
    int num;
    int TotalDecoded = 0;
    int input_encoded_string_length;

    std::string input_encoded_string = "";
    std::string output_decoded_string = "";
    std::string eight = "00000000";
    std::string tempString = "";

    char tempchar;

    while((byte = getc(INPUT_ENCODED_FILE)) != EOF)
    {
        num = byte; //198
        eight = "00000000";
        int r;
        int i = 0;
        while(num > 0)
        {
            r = num % 2;
            if(r == 1)
                eight[7-i] = '1';
            num = num / 2;
            i++;
        }
        input_encoded_string = input_encoded_string + eight;
    }
    if(LOG) std::clog<<"\nWczytany zakodowany ciag:\n"<<input_encoded_string<<std::endl;

    input_encoded_string_length = input_encoded_string.length();
    if(LOG) std::clog<<"ODKODOWANY CIAG: \n";
    for(int i = 0; i < input_encoded_string_length; i++)
    {
        tempString = tempString + input_encoded_string[i];
        if(Codes_List_Set.find(tempString)!= Codes_List_Set.end())
           {
                auto itr = Code_Symb_Map.find(tempString);
                tempchar = itr->second;
                if(LOG) std::clog<<tempchar;
                OUTPUT_DECODED_FILE<<tempchar;
                tempString="";
                TotalDecoded++;
           }
           if(TotalDecoded == TotalNumber)
           {
               break;
           }

    }
    fclose(INPUT_ENCODED_FILE);
    OUTPUT_DECODED_FILE.close();

}

int main()
{
    std::clog.rdbuf(out.rdbuf());       // Set the rdbuf of clog
    if(!LOG) std::clog<<"Logs Off";
    else
    {
        if(DO_ENCODE) std::clog<<"KODOWANIE WLACZONE\n";
        if(DO_DECODE) std::clog<<"DEKODOWANIE WLACZONE\n";

    }
    if(DO_ENCODE)
    {
        Set_Node_Array();
        Generate_Model_From_file();

        licznikpieter = DifferentChars-1;
        int arr[DifferentChars];
        int freq[DifferentChars];
        for(int i=0;i<DifferentChars;i++)
        {
            arr[i] = NodeArray[i].symbol;
            freq[i] = NodeArray[i].frequency;
        }
        int size = sizeof(arr) / sizeof(arr[0]);
        HuffmanCodes(arr, freq, size);
        get_Codes_to_encode();
        Encode();
    }
    if(DO_DECODE)
    {
        get_Total_Num();
        get_Codes_to_decode();
        Decode();
    }


    if(LOG) std::clog<<"\n\nkoniec\n";
    std::clog.rdbuf(old_rdbuf);     //Reset the rdbuf of clog.
    //std::cerr<<std::endl<<"KONIEC"<<std::endl;*
    return 0;
}
