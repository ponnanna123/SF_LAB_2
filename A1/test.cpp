#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Utility {
public:
    static string register_encode(const string& reg) {
        if (reg[0] == 'x') return bitset<5>(stoi(reg.substr(1))).to_string();
        else if (reg[0] == 'a') {
            int id = stoi(reg.substr(1));
            if (id >= 0 && id <= 7) return bitset<5>(id + 10).to_string();
        } else if (reg[0] == 's') {
            int id = stoi(reg.substr(1));
            if (id == 0 || id == 1) return bitset<5>(id + 8).to_string();
            else if (id >= 2 && id <= 11) return bitset<5>(id + 16).to_string();
        } else if (reg[0] == 't') {
            int id = stoi(reg.substr(1));
            if (id >= 0 && id <= 2) return bitset<5>(id + 5).to_string();
            else if (id >= 3 && id <= 6) return bitset<5>(id + 25).to_string();
        }
        return "00000";
    }

    static string trim(const string& str) {
        int start = str.find_first_not_of(" \t");
        int end = str.find_last_not_of(" \t");
        return (start == string::npos) ? "" : str.substr(start, end - start + 1);
    }

    static vector<string> tokenize(const string& line) {
        vector<string> tokens;
        string token;
        stringstream ss(line);
        
        if (ss >> token) tokens.push_back(Utility::trim(token));

        while (getline(ss, token, ',')) {
            tokens.push_back(Utility::trim(token));
        }

        return tokens;
    }
};

class AssemblyInstruction {
public:
    string abbr, rd, rs1, rs2;
    string imm = "";

    static void parse_line(const string& line) {
        vector<string> tokens = Utility::tokenize(line);
        for(auto& token : tokens) {
            cout << token << endl;
        }
        cout << endl;

        string r_abbr[] = {"add", "sub", "xor", "or", "and", "sll", "srl", "sra", "slt", "sltu"};
        string i_abbr[] = {"addi", "xori", "ori", "andi", "slli", "srli", "srai", "slti", "sltiu", "lb", "lh", "lw", "lbu", "lhu", "jalr"};
        string b_abbr[] = {"beq", "bne", "blt", "bge", "bltu", "bgeu"};
        string s_abbr[] = {"sb", "sh", "sw"};
        string j_abbr[] = {"jal"};

        // if (find(begin(r_abbr), end(r_abbr), inst.abbr) != end(r_abbr)) {
        //     iss >> inst.rd >> inst.rs1 >> inst.rs2;
        // } else if (find(begin(i_abbr), end(i_abbr), inst.abbr) != end(i_abbr)) {
        //     iss >> inst.rd >> inst.rs1 >> inst.imm;
        // } else if (find(begin(b_abbr), end(b_abbr), inst.abbr) != end(b_abbr)) {
        //     iss >> inst.rs1 >> inst.rs2 >> inst.imm;
        // } else if (find(begin(s_abbr), end(s_abbr), inst.abbr) != end(s_abbr)) {
        //     iss >> inst.rs1 >> inst.rs2 >> inst.imm;
        // } else if (find(begin(j_abbr), end(j_abbr), inst.abbr) != end(j_abbr)) {
        //     iss >> inst.rd >> inst.imm;
        // }

        // return inst;
    }
};

class Assembler {
private:
    unordered_map<string, int> label_address;

public:
    void parse_label(const string& input_file) {
        ifstream in_file(input_file);
        if (!in_file) {
            cerr << "Error: Could not open assembly file.\n";
            return;
        }

        string line;
        int mem_address = 1000;

        while (getline(in_file, line)) {
            int start_index = line.size() - 2;
            if (line.empty() || line[0] == '#') continue;
            if (line[start_index]== ':') {
                label_address[line.substr(0, start_index)] = mem_address;
            } else {
                mem_address += 4;
            }
        }
    }

    void parse_inst(const string& input_file, const string& output_file) {
        ifstream in_file(input_file);
        if (!in_file) {
            cerr << "Error: Could not open assembly file.\n";
            return;
        }

        string line;
        int mem_address = 1000;

        while (getline(in_file, line)) {
            if (line.empty() || line[0] == '#' || line[line.size() - 2] == ':') continue;
            try {
                AssemblyInstruction::parse_line(line);
            } catch (const exception& e) {
                cerr << "Error: Failed to process line: " << line << "\n";
            }
            
        }
    }
};

int main() {
    Assembler assembler;
    assembler.parse_label("asm.s");
    assembler.parse_inst("asm.s","etrfdg");
    return 0;
}