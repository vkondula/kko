#include "bwted.h"

#define MTF_SIZE '\x40'

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


string rle_encode(string * decoded) {
    string encoded;
    unsigned char counter = 0;
    for (unsigned long i = 0 ; i < decoded->size(); i++){
        if ((*decoded)[i] == 0 && counter != 255) {
            counter++;
        } else {
            if (counter != 0){
                encoded += (char)'\0';
                encoded += counter;
                counter = 0;
            }
            if ((*decoded)[i] == 0){
                counter ++;
            } else {
                encoded += (*decoded)[i];
            }
        }
    }
    if (counter != 0){
        encoded += (char)'\0';
        encoded += counter;
    }
    return encoded;
}

string rle_decode(string * encoded) {
    string decoded;
    for (unsigned long i = 0 ; i < encoded->size(); i++){
        if ((*encoded)[i] != '\0'){
            decoded += (*encoded)[i];
        }
        else {
            string s((unsigned char)(*encoded)[++i], '\0');
            decoded += s;
        }
    }
    return decoded;
}


string mtf_encode(string * decoded) {
    vector<char> table;
    vector<char>::iterator it;
    string encoded_str;
    for (unsigned long i = 0 ; i < decoded->size(); i++){
        it = find(table.begin(), table.end(), (*decoded)[i]);
        if (it == table.end()){ //not found
            encoded_str += char(table.size());
        } else {
            encoded_str += char(table.end() - it - 1);
            table.erase(it);
        }
        table.push_back((*decoded)[i]);
    }
    string table_str(table.begin(), table.end());
    return char(table.size()) + table_str + encoded_str;
}

string mtf_decode(string * encoded) {
    //recreate table
    string decoded;
    vector<char> table(encoded->begin() + 1, encoded->begin() + 1 + (*encoded)[0]);
    unsigned long table_size = table.size();
    for (unsigned long i = encoded->size() - 1 ; i > table_size; i--){
        if ((unsigned char)(*encoded)[i] < table_size) {
            char c = table.at(table.size() - 1);
            decoded += c;
            if ((*encoded)[i] != 0){
                table.pop_back();
                table.insert(table.end() - (*encoded)[i], c);
            }
        } else {
            cerr << "invalid encoding!!!\n";
        }
    }
    reverse(decoded.begin(), decoded.end());
    return decoded;
}

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
    string mtf_encoded = mtf_encode(&btw_encoded);
    string rle_encoded = rle_encode(&mtf_encoded);
    outputFile << rle_encoded;
    cout << rle_encoded;
    return 0;
}



int BWTDecoding(tBWTED *ahed, istream& inputFile, ostream& outputFile) {
    string encoded((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    // 1st step bwt
    string rle_decoded = rle_decode(&encoded);
    string mtf_decoded = mtf_decode(&rle_decoded);
    string btw_decoded = bwt_decode(&mtf_decoded);
    outputFile << btw_decoded;
    cout << btw_decoded;
    return 0;
}
