//
// Created by Vaclav Kondula on 01/05/2018.
//

#ifndef KKO_BWTED_H
#define KKO_BWTED_H

#include <sys/types.h>
#include <iostream>
#include <fstream>

using namespace std;

typedef struct{
    int64_t uncodedSize;
    int64_t codedSize;
} tBWTED;


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


#endif //KKO_BWTED_H
