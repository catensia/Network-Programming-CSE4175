#include <bits/stdc++.h>
using namespace std;
int dataword_size;
//>> ./crc_encoder_20161634 datastream.tx codedstream.tx 1101 4
//g++ -Wall -o crc_encoder_20161634 crc_encoder_20161634.cpp


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
string zero_string(int length){
    string ret="";
    for(int i=0;i<length;i++) ret+='0';

    return ret;
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

string get_rem(string generator, string dataword) {
    //function gets remainder dataword%generator
    //ex: dataword: 1001 generator: 1011
    int dlen = dataword.length();
    int glen = generator.length();

    for (int i = 0; i < glen - 1; i++) {
        dataword += '0';
    }
    //dataword is now 1001000
    string upper = dataword;
    string lower;
    for (int i = 0; i < dlen; i++) {
        int quo = (generator[0] - '0') & (upper[0] - '0');

        if (quo == 1) lower = generator;
        else lower = zero_string(1);
        
        upper = bitwiseXOR(upper, lower);

        upper.erase(0,1);
    }


    return upper;
}




int main(int argc, char* argv[]) {

    ifstream input_file(argv[1], ios::binary);
    ofstream output_file(argv[2], ios::binary);

    if(argc!=5){
        cout << "usage: ./crc_encoder input_file output_file generator dataword_size" << endl;
        exit(1);
    }

    if(!input_file){
        cout << "input file open error." << endl;
        exit(1);
    }

    if(!output_file){
        cout << "output file open error." << endl;
        exit(1);
    }

    if(strcmp(argv[4], "4") && strcmp(argv[4],"8")){
        cout << "dataword size must be 4 or 8." << endl;
        exit(1);
    }
    dataword_size=argv[4][0]-'0';

    string generator(argv[3]);
    string result="";

    
    int total=0;

    while(!input_file.eof()){
        unsigned char byte;
        input_file.read((char*)&byte, sizeof(unsigned char));

        if(input_file.fail()){
            break;
        } 

        if(dataword_size==4){
            //byte = a|b (seperated by 4 bytes)
            unsigned char a, b;
            string part_a="", part_b="";
            a= (byte & 0b11110000) >> 4;
            b= (byte & 0b00001111);

            for(int i=8, offset=3;i>0;i/=2, offset--){
                part_a+=((a&i)>>offset)+'0';
                part_b+=((b&i)>>offset)+'0';
            }
            //cout << part_a << " | " << part_b << endl;
            
            part_a+=get_rem(generator, part_a);
            part_b+=get_rem(generator, part_b);

            
            result+=part_a;
            result+=part_b;
        }
        else{
            
            string part_a="";
            for(int i=128, offset=7;i>0;i/=2, offset--){
                part_a+=((byte&i)>>offset)+'0';
            }
            part_a+=get_rem(generator, part_a);
            result+=part_a;
        }
        total++;
    }


    int cur_len=result.length();
    int paddings=8-cur_len%8;
    for(int i=0;i<paddings;i++){
        result.insert(0, "0");
    }
    result.insert(0,int_to_binary(paddings));

    int ttotal=0;
    for(int i=0;i<(int)result.length();i+=8){
        string sep="";
        unsigned char output_byte;
        for(int j=0;j<8;j++){ 
            sep+=result[i+j];
        }
        output_byte=binary_to_byte(sep);
        output_file.write((char*)&output_byte, sizeof(output_byte));
        ttotal++;
    }   

    input_file.close();
    output_file.close();
    
    return 0;
}