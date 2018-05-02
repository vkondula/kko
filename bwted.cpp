#include "bwted.h"

using namespace std;

/*sort array<record_index> by first character*/
bool sort_record_index(record_index left, record_index right){
    return left.key < right.key;
}

bool sort_record_index_index(record_index_index left, record_index_index right){
    return left.key < right.key;
}

unsigned long increment (unsigned long value, string * encoded){
    unsigned long retval = value + 1;
    if (retval < encoded->size()) return retval;
    return 0;
}


/*compare rotations, return true if best should change*/
struct comparator {
    string * encoded;
    bool operator() (record_index left, record_index right) {
        unsigned long original = left.index;
        unsigned long iter_left = left.index;
        unsigned long iter_right = right.index;
        do {
            if ((*encoded)[iter_left] > (*encoded)[iter_right]) return false;
            if ((*encoded)[iter_left] < (*encoded)[iter_right]) return true;
            iter_right = increment(iter_right, encoded);
            iter_left = increment(iter_left, encoded);
        } while(original != iter_left);
        return false;
    }
} comparator1;

string bwt_encode(string * decoded){
    (*decoded) += '\x03';
    vector<record_index> table;
    string encoded;
    for (unsigned long i = 0 ; i < decoded->size(); i++){
        record_index tmp = {(char)(*decoded)[i != 0 ? i - 1 : decoded->size() - 1], i};
        table.push_back(tmp);
    }
    comparator1.encoded = decoded;
    stable_sort(table.begin(), table.end(), comparator1);
    for (unsigned long i = 0 ; i < decoded->size(); i++){
        encoded += table.at(i).key;
    }
    return encoded;
}

string bwt_decode(string * encoded){ //L
    std::vector<record_index> table;
    for (unsigned long i = 0; i < encoded->size(); i++){ //EOF INCLUDED
        record_index tmp = {(char)(*encoded)[i], i};
        table.push_back(tmp);
    }
    stable_sort(table.begin(), table.end(), sort_record_index);
    //
    std::vector<record_index_index> indexes;
    for (unsigned long i = 0; i < encoded->size(); i++){
        record_index_index tmp = {table.at(i).index, i};
        indexes.push_back(tmp);
    }
    stable_sort(indexes.begin(), indexes.end(), sort_record_index_index);
    //
    string decoded;
    uint64_t i = 0;
    do {
        decoded += table.at(i).key;
        i = indexes.at(i).index;
    } while (i);
    reverse(decoded.begin(), decoded.end());
    return decoded;
}


int BWTEncoding(tBWTED *bwted, istream& inputFile, ostream& outputFile) {
    string decoded((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    // 1st step bwt
    string btw_encoded = bwt_encode(&decoded);
    outputFile << btw_encoded;
    cout << btw_encoded;
    return 0;
}



int BWTDecoding(tBWTED *ahed, istream& inputFile, ostream& outputFile) {
    string encoded((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    // 1st step bwt
    string btw_decoded = bwt_decode(&encoded);
    outputFile << btw_decoded;
    cout << btw_decoded;
    return 0;
}
