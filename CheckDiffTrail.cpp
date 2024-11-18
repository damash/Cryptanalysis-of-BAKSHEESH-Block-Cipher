#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;
#define uint32 unsigned int

vector<vector<int>> ReadData(string filename, int base) {
    //  Can read integers with sign with various length, base can take 10 or 16
    ifstream in(filename);
    if (!in) cout << "No such file!" << endl;
    string s;
    vector<vector<string>> res_str;
    int i, j;
    while (getline(in, s)) {
        string t;
        //  Filter data to store integers (include hexadecimal code), space and x (for 16)
        for (i = 0; i < s.size(); i++) {
            if ((s[i] >= 48 && s[i] <= 57) || s[i] == 32 || s[i] == 120 || s[i] >= 97 && s[i] <= 102 || s[i] == 45) t += s[i];
        }

        j = 0;
        vector<string> tmp;
        for (i = 0; i < t.size(); i++) {
            if (t[i] == 32) {   //  deal with space
                string tmp2 = t.substr(j, i - j + 1);
                tmp.push_back(tmp2);
                j = i + 1;
            }
        }

        if (t[t.size() - 1] != 32) {
            string tmp2 = t.substr(j);
            tmp.push_back(tmp2);
        }
        res_str.push_back(tmp);
    }

    vector<vector<int>> result;
    for (i = 0; i < res_str.size(); i++) {
        vector<int> tmp;
        for (j = 0; j < res_str[i].size(); j++) {
            const char* nptr = res_str[i][j].c_str();
            char* endptr = NULL;
            errno = 0;
            int val = strtol(nptr, &endptr, base);
            tmp.push_back(val);
        }
        result.push_back(tmp);
        
    }

    return result;
}

vector<vector<int>> DDT = { {16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
{0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 4, 0, },
{0, 0, 0, 4, 0, 4, 0, 0, 0, 0, 0, 4, 0, 4, 0, 0, },
{0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 0, },
{0, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 4, 0, },
{0, 0, 0, 0, 0, 4, 4, 0, 4, 0, 0, 4, 0, 0, 0, 0, },
{0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 4, 4, 0, },
{0, 0, 0, 4, 0, 0, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, },
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, },
{0, 4, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, },
{0, 0, 4, 0, 4, 0, 0, 0, 0, 0, 4, 0, 4, 0, 0, 0, },
{0, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, },
{0, 4, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 4, 0, 0, 0, },
{0, 0, 0, 0, 4, 0, 0, 4, 0, 4, 4, 0, 0, 0, 0, 0, },
{0, 4, 4, 0, 4, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, },
{0, 0, 4, 0, 0, 0, 0, 4, 0, 4, 0, 0, 4, 0, 0, 0, }, };


vector<int> P = { 0, 33, 66, 99, 96, 1, 34, 67, 64, 97, 2, 35, 32, 65, 98, 3, 4, 37, 70, 103, 100, 5, 38, 71, 68, 101, 6, 39, 36, 69, 102, 7, 8, 41, 74, 107, 104, 9, 42, 75, 72, 105, 10, 43, 40, 73, 106, 11, 12, 45, 78, 111, 108, 13, 46, 79, 76, 109, 14, 47, 44, 77, 110, 15, 16, 49, 82, 115, 112, 17, 50, 83, 80, 113, 18, 51, 48, 81, 114, 19, 20, 53, 86, 119, 116, 21, 54, 87, 84, 117, 22, 55, 52, 85, 118, 23, 24, 57, 90, 123, 120, 25, 58, 91, 88, 121, 26, 59, 56, 89, 122, 27, 28, 61, 94, 127, 124, 29, 62, 95, 92, 125, 30, 63, 60, 93, 126, 31 };
vector<int> Pinv = {0, 5, 10, 15, 16, 21, 26, 31, 32, 37, 42, 47, 48, 53, 58, 63, 64, 69, 74, 79, 80, 85, 90, 95, 96, 101, 106, 111, 112, 117, 122, 127, 12, 1, 6, 11, 28, 17, 22, 27, 44, 33, 38, 43, 60, 49, 54, 59, 76, 65, 70, 75, 92, 81, 86, 91, 108, 97, 102, 107, 124, 113, 118, 123, 8, 13, 2, 7, 24, 29, 18, 23, 40, 45, 34, 39, 56, 61, 50, 55, 72, 77, 66, 71, 88, 93, 82, 87, 104, 109, 98, 103, 120, 125, 114, 119, 4, 9, 14, 3, 20, 25, 30, 19, 36, 41, 46, 35, 52, 57, 62, 51, 68, 73, 78, 67, 84, 89, 94, 83, 100, 105, 110, 99, 116, 121, 126, 115,};



bool CheckTrail(vector<vector<int>> a, int round) {  

    int i, j, k, r, in, out, count = 0;

    for (r = 0; r < round; r++) {
        //  Sbox
        cout << "[Round " << r + 1 << "]" << endl;
        for (i = 0; i < 32; i++) {
            in = 0;
            out = 0;

            for (j = 0; j < 4; j++) {
                in ^= a[r][i * 4 + 3 - j];
                if (j < 3) in <<= 1;

                out ^= a[r + 1][P[i * 4 + 3 - j]];
                if (j < 3) out <<= 1;
            }

            if (in == 0 && out != 0) {
                cout << "error1" << endl;
            }
            else if (in != 0 && out == 0) {
                cout << "error2" << endl;
            }
            else if (in != 0 && DDT[in][out] == 0) {
                cout << "error3" << endl;
            }
            else if (in != 0 && DDT[in][out] != 0) {
                if (DDT[in][out] == 4)
                    count += 2;
                else if (DDT[in][out] == 16) count += 0;
            }
        }
        cout << ", prob " << count << endl;
    }

    cout << "total prob: " << count << endl;

    return 0;
}

void PrintTrail(vector<vector<int>> a) {
    int i, j;
    vector<vector<int>> b2(a.size());
    vector<vector<int>> b = a;

    for (i = 0; i < a.size(); i++) {
        for (j = 0; j < a[i].size(); j ++) {
            b[i][j] = a[i][a[i].size() - 1 - j];
        }
    }

    a = b;

    for (i = 0; i < a.size(); i++) {
        for (j = 0; j < a[i].size(); j += 4) {
            int tmp = 0;
            for (int k = 0; k <= 3; k++) {
                tmp ^= (a[i][j + k] << (3-k));
            }
            b2[i].push_back(tmp);
        }
    }

    for (i = 0; i < b2.size(); i++) {
        //cout << b[i].size() << " " << a[i].size() << endl;
        cout << dec << "round: " << i << " ";
        cout << "0x";
        for (j = 16; j < b2[i].size(); j++) {
            cout << hex << b2[i][j] << "";
        }

        cout << ", 0x";
        for (j = 0; j < 16; j++) {
            cout << hex << b2[i][j] << "";
        }

        cout << endl;
    }

}

int main() {
    int round = 18;
   	vector<vector<int>> a = ReadData("Round18_Sol60.txt", 10);

    vector<vector<int>> roundvar = ReadData("var18.txt", 10);

    vector<int> RoundVar = roundvar[0];

    vector<int> bini;
    int i, j;
    for (i = 0; i < a.size(); i++) {
        for (j = 0; j < a[i].size(); j++) {
            bini.push_back(a[i][j]);
        }
    }

    vector<int> b;
    for (i = 0; i < RoundVar.size(); i++) {
        b.push_back(bini[RoundVar[i]]);
    }

    vector<vector<int>> Sol(b.size()/128);

    for (i = 0; i < Sol.size(); i++) {
        for (j = 0; j < 128; j++) {
            if (b[i * 128 + j] > 0) Sol[i].push_back(1);
            else Sol[i].push_back(0);
        }
    }

    PrintTrail(Sol);

    cout << dec << endl;

    CheckTrail(Sol, Sol.size() - 1);

	return 0;
}
