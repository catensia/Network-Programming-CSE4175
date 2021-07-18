#include <bits/stdc++.h>
using namespace std;
int dataword_size;
int codeword;
//g++ -Wall -o crc_decoder_20161634 crc_decoder_20161634.cpp
// ./crc_decoder_20161634 codedstream.rx datastream.rx result.txt 1101 4

bool has_error(string remainder){
    for(int i=0;i<(int)remainder.length();i++){
        if(remainder[i]=='1') return true;
    }
    return false;
}

unsigned char binary_to_byte(string s) {
    reverse(s.begin(), s.end());
    unsigned char ret = 0;
    for (int i = 0, b=1; i < (int)s.length(); i++, b*=2) {
        if (s[i] == '1') {
            ret += b;
        }
    }
    return ret;
}

string int_to_binary(int n) {
    if (n > (1 << 8) - 1) {
        cout << "overflow" << endl;
        return "";
    }

    string res = "";
    while (n > 0) {
        res += n % 2 + '0';
        n /= 2;
    }
    reverse(res.begin(), res.end());
    int len=8-res.length();
    for (int i = 0; i < len; i++) {
        res.insert(0, "0");
    }

    return res;
}

string bitwiseXOR(string a, string b){
    //length should equal if not, add padding to the right
    while(a.length()>b.length()) b+='0';
    string ret="";
    for(int i=0;i<(int)a.length();i++){
        int left=a[i]-'0';
        int right=b[i]-'0';

        ret+=(left^right)+'0';
    }

    return ret;

}
string zero_string(int length){
    string ret="";
    for(int i=0;i<length;i++) ret+='0';

    return ret;
}

string get_rem(string generator, string dataword) {
    //function gets remainder dataword%generator
    //ex: dataword: 1001 generator: 1011
    int dlen = dataword.length();
    int glen = generator.length();
    //dataword is now 1001000
    string upper = dataword;
    string lower;
    for (int i = 0; i < dlen-glen+1; i++) {
        int quo = (generator[0] - '0') & (upper[0] - '0');

        if (quo == 1) lower = generator;
        else lower = zero_string(1);
        
        upper = bitwiseXOR(upper, lower);

        upper.erase(0,1);
    }
    return upper;
}


int main(int argc, char* argv[]) {


    if(argc!=6){
        cout << "usage: ./crc_encoder input_file output_file generator dataword_size" << endl;
        exit(1);
    }


    ifstream input_file(argv[1], ios::binary);
    ofstream output_file(argv[2], ios::binary);
    ofstream result_file(argv[3], ios::binary);


    if(!input_file){
        cout << "input file open error." << endl;
        exit(1);
    }

    if(!output_file){
        cout << "output file open error." << endl;
        exit(1);
    }

    if(!result_file){
        cout << "result file open error." << endl;
        exit(1);
    }

    if(strcmp(argv[5], "4") && strcmp(argv[5],"8")){
        cout << "dataword size must be 4 or 8." << endl;
        exit(1);
    }
    dataword_size=argv[5][0]-'0';
    string generator(argv[4]);

    codeword=2*dataword_size-1;

    int codeword_num=0;
    int error_num=0;
    unsigned char byte;
    string revert="";
    int glen=generator.length();
    int dlen=dataword_size;
    int blocklen=glen+dlen-1;

    //get padding
    input_file.read((char*)&byte, sizeof(unsigned char));

    unsigned char output=0;
    int padding = (int)byte;
    int n=0;
    string buffer="";
    while(!input_file.eof()){
        unsigned char byte;
        string curbyte;
        string curcode="";

        
        input_file.read((char*)&byte, sizeof(unsigned char));

        
        if(input_file.fail()){
            break;
        }

        curbyte=int_to_binary((int)byte);

        if(n==0)
            for(int i=padding;i<8;i++) buffer+=curbyte[i];

        else
            for(int i=0;i<8;i++) buffer+=curbyte[i];
        

        if((int)buffer.length()==blocklen*2){
            //blocklen = glen + dlen - 1;
            string temp;
            string check;
            for(int j=0;j<2;j++){
                for(int i=0;i<blocklen;i++) curcode+=buffer[i];
                temp=curcode;
                temp.erase(dlen, glen-1);
                revert+=temp;
                buffer.erase(0,blocklen);
                check=get_rem(generator, curcode);
                if(has_error(check)) error_num++;

                if(dlen==8) codeword_num+=1;
                else codeword_num+=1;

                if((dlen==8 || (dlen==4 && codeword_num%2==0)) && n!=0){
                    output=binary_to_byte(revert);
                    output_file.write((char*)&output, sizeof(output));
                    output=0;
                    revert="";
                }
                curcode="";
            }
        }
        else if((int)buffer.length()>=blocklen){
            for(int i=0;i<blocklen;i++) curcode+=buffer[i];
            unsigned char output;
            string temp=curcode;
            temp.erase(dlen, glen-1);
            revert+=temp;
            buffer.erase(0,blocklen);
            string check=get_rem(generator, curcode);
            if(has_error(check)) error_num++;

            if(dlen==8) codeword_num+=1;
            else codeword_num+=1;

            if((dlen==8 || (dlen==4 && codeword_num%2==0)) && n!=0){
                output=binary_to_byte(revert);
                output_file.write((char*)&output, sizeof(output));
                output=0;
                revert="";
            }
        }

        //printf("errors until now:%d\n", error_num);
        //printf("codewords until now :%d\n\n", codeword_num);
        n++;
        
    }
    result_file << codeword_num << " " << error_num;

}