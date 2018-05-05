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


string huf_encode(string * decoded) {
    string encoded;
    unsigned long chars[256] = {0};
    for (unsigned long long i = 0 ; i < decoded->size(); i++){
        chars[(unsigned char) (*decoded)[i]]++;
    }
    vector <node> build;
    vector <node> final;
    int counter = 0;
    // initialize 2 vectors - final tree + build tree
    for (unsigned long i = 0 ; i < 256; i++){
        if (!chars[i]) continue; //ignore chars without occurrence
        node record = {(unsigned char)i, counter++, chars[i], -1, -1};
        build.push_back(record);
        final.push_back(record);
    }
    int size = counter;
    // end of huffman encoding sequence
    node record = {0, counter++, 0, -1, -1};
    build.push_back(record);
    final.push_back(record);

    // create the huffman tree
    while(build.size() > 1){
        int min_0_ptr = -1;
        int min_1_ptr = -1;
        unsigned long long min_0_value = ULONG_MAX;
        unsigned long long min_1_value = ULONG_MAX;
        for (unsigned int i = 0 ; i < build.size(); i++){
            if (build[i].sum < min_0_value){
                min_1_value = min_0_value;
                min_1_ptr = min_0_ptr;
                min_0_value = build[i].sum;
                min_0_ptr = i;
            } else if (build[i].sum < min_1_value){
                min_1_value = build[i].sum;
                min_1_ptr = i;
            }
        }
        node combination = {0, counter++, min_0_value + min_1_value, build[min_0_ptr].ptr, build[min_1_ptr].ptr};
        build.erase(build.begin() + max(min_0_ptr, min_1_ptr));
        build.erase(build.begin() + min(min_0_ptr, min_1_ptr));
        build.push_back(combination);
        final.push_back(combination);
    }
    //create the encoding table
    map<char, string> lookup;
    for (long i = final.size() - 1 ; i >= 0 ; i--){
        if (final[i].ptr_l >= 0){
            final[final[i].ptr_l].encoded += (final[i].encoded + '0');
        }
        if (final[i].ptr_r >= 0){
            final[final[i].ptr_r].encoded += (final[i].encoded + '1');
        }
        if (final[i].ptr < size){
            lookup[final[i].key] = final[i].encoded;
        }
    }
    //store encoding table
    HuffmanEncoder encoder;
    for (unsigned char i = 0 ; i <= size; i++){
        encoded += (unsigned char)(final[i].encoded.size());
        encoded += final[i].key;
        encoder.add(&(final[i].encoded));
        encoded += encoder.flush();
    }
    encoded += (char)'\0';
    //encode input
    for (unsigned long i = 0; i < decoded->size(); i ++){
        encoder.add(&(lookup[(*decoded)[i]]));
    }
    encoder.add(&(final[size].encoded));
    return (encoded + encoder.flush());
}


string huf_decode(string * encoded, char * error) {
    string decoded;
    unsigned int ptr = 0;
    //recreate huffman tree
    vector<char> keys;
    vector<unsigned long> values;
    while ((*encoded)[ptr]!= 0){
        unsigned char size = (unsigned char)((*encoded)[ptr++]);
        keys.push_back((*encoded)[ptr++]);
        HuffmanDecoder decoder(encoded, ptr);
        unsigned long value = 1;
        for (unsigned long i = 0; i < size ; i++){
            char retval = decoder.get_bit();
            if (retval == 1){
                value += (value*2) + 1;
            } else if (retval == 0) {
                value += value*2;
            } else { // read out of range
                (*error) = 1;
                return "";
            }
        }
        values.push_back(value);
        ptr += (size % 8) ? (size / 8) + 1 : (size / 8);
    }
    ptr++;
    //decoding text
    HuffmanDecoder decoder(encoded, ptr);
    unsigned long value = 1;
    while(true){
        char retval = decoder.get_bit();
        if (retval == 1){
            value += (value * 2) + 1;
        } else if (retval == 0) {
            value += value * 2;
        } else { // read out of range
            (*error) = 1;
            return "";
        }
        vector<unsigned long>::iterator it = find(values.begin(), values.end(), value);
        if (it != values.end()) {
            long index = distance(values.begin(), it);
            if (index == (long)(values.size() - 1)) { //end of sequence code
                break;
            }
            decoded += keys[index];
            value = 1;
        }
    }
    return decoded;
}


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

string rle_decode(string * encoded, char * error) {
    string decoded;
    for (unsigned long i = 0 ; i < encoded->size(); i++){
        if ((*encoded)[i] != '\0'){
            decoded += (*encoded)[i];
        } else {
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

string mtf_decode(string * encoded, char * error) {
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
            (*error) = 1;
            return "";
        }
    }
    reverse(decoded.begin(), decoded.end());
    return decoded;
}

string bwt_encode(string * decoded){
    (*decoded) += '\x03';
    vector<record_index> table;
    string encoded;
    // build the table, with pointers to decoded string to reduce size
    for (unsigned long i = 0 ; i < decoded->size(); i++){
        record_index tmp = {(char)(*decoded)[i != 0 ? i - 1 : decoded->size() - 1], i};
        table.push_back(tmp);
    }
    comparator1.encoded = decoded;
    // sort by pointer
    stable_sort(table.begin(), table.end(), comparator1);
    for (unsigned long i = 0 ; i < decoded->size(); i++){
        encoded += table.at(i).key;
    }
    return encoded;
}

string bwt_decode(string * encoded, char * error){
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
    return decoded.substr(0, decoded.size() - 1);
}


int BWTEncoding(tBWTED *bwted, istream& inputFile, ostream& outputFile) {
    string decoded((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    // 1st step bwt
    string btw_encoded = bwt_encode(&decoded);
    string mtf_encoded = mtf_encode(&btw_encoded);
    string rle_encoded = rle_encode(&mtf_encoded);
    string huf_encoded = huf_encode(&rle_encoded);
    bwted->uncodedSize = (int64_t) decoded.size();
    bwted->codedSize = (int64_t) huf_encoded.size();
    outputFile << huf_encoded;
    return 0;
}


int BWTDecoding(tBWTED *ahed, istream& inputFile, ostream& outputFile) {
    char retval = 0;
    string encoded((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    string huf_decoded = huf_decode(&encoded, &retval);
    if (retval) return -1;
    string rle_decoded = rle_decode(&huf_decoded, &retval);
    if (retval) return -1;
    string mtf_decoded = mtf_decode(&rle_decoded, &retval);
    if (retval) return -1;
    string btw_decoded = bwt_decode(&mtf_decoded, &retval);
    if (retval) return -1;
    ahed->codedSize = (int64_t) encoded.size();
    ahed->uncodedSize = (int64_t) btw_decoded.size();
    outputFile << btw_decoded;
    return 0;
}
