//
// Created by Vaclav Kondula on 01/05/2018.
//

#ifndef KKO_BWTED_H
#define KKO_BWTED_H

#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <map>
#include <istream>
#include <ostream>
#include <climits>

using namespace std;

typedef struct{
    int64_t uncodedSize;
    int64_t codedSize;
} tBWTED;

typedef struct{
    char key;
    uint64_t index;
} record_index;

typedef struct{
    uint64_t key;
    uint64_t index;
} record_index_index;

typedef struct {
    unsigned char key;
    int ptr;
    unsigned long long sum;
    int ptr_l;
    int ptr_r;
    string encoded;
} node;

/* bwted – záznam o kódování
inputFile – vstupní soubor (nekódovaný)
outputFile – výstupní soubor (kódovaný)
návratová hodnota – 0 kódování proběhlo v pořádku,-1 při kódování nastala chyba */
int BWTEncoding(tBWTED *bwted, istream& inputFile, ostream& outputFile);


/* bwted – záznam o dekódování
inputFile – vstupní soubor (kódovaný)
outputFile – výstupní soubor (dekódovaný)
návratová hodnota – 0 dekódování proběhlo v pořádku,-1 při dekódování nastala chyba */
int BWTDecoding(tBWTED *ahed, istream& inputFile, ostream& outputFile);

/*1st step - b-w transformation, return string of same length in different order*/
string bwt_encode(string * decoded);
string bwt_decode(string * encoded, char * error);

/*2nd step - move to front alg, dynamically create table of all chars in string, encode chars as positions*/
string mtf_encode(string * decoded);
string mtf_decode(string * encoded, char * error);

/*3rd step - zero run length encoding , all sequences of zeros are replaced by 2 chars, ('\0', <number of zeros>)*/
string rle_encode(string * decoded);
string rle_decode(string * encoded, char * error);

/*4th step - huffman encoding, dynamic*/
string huf_encode(string * decoded);
string huf_decode(string * encoded, char * error);

class HuffmanDecoder {
    string * input;
    unsigned char bit_ptr = 7;
    unsigned long ptr = 0;
public:
    HuffmanDecoder(string * input, unsigned long ptr = 0){
        this->input = input;
        this->ptr = ptr;
    }
    char get_bit(){
        if (ptr >= input->size()) return -1;
        char c = (*input)[ptr];
        c >>= bit_ptr;
        c &= 1;
        if (bit_ptr != 0){
            bit_ptr--;
        } else {
            bit_ptr = (unsigned char) 7;
            ptr++;
        }
        return c;
    }
};


class HuffmanEncoder {
    string buffer;
    string output;
    void flush_buffer(){
        while(this->buffer.size() >= 8){
            char c = 0;
            for (unsigned int i = 0; i < 8 ; i ++){
                c <<= 1;
                if (this->buffer[i] == '1') c += 1;
            }
            output += c;
            this->buffer = this->buffer.substr(8, this->buffer.size() - 1);
        }
    }
public:
    void add(string * encoded){
        this->buffer += (*encoded);
        if (this->buffer.size() >= 8){
            this->flush_buffer();
        }
    }
    string flush(){
        unsigned long missing = 8 - this->buffer.size();
        if (missing != 8){
            string filler(missing, '0');
            this->buffer += filler;
            this->flush_buffer();
        }
        string retval = this->output;
        this->output = "";
        return retval;
    }
};


#endif //KKO_BWTED_H
