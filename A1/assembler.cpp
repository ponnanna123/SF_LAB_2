#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

string reg_to_bin(const string& reg) {
    if (reg[0] == 'x') return bitset<5>(stoi(reg.substr(1))).to_string();
    else if (reg[0] == 'a') {
        int index = stoi(reg.substr(1));
        if (index >= 0 && index <= 7) return bitset<5>(index + 10).to_string();
    } else if (reg[0] == 's') {
        int index = stoi(reg.substr(1));
        if (index == 0 || index == 1) return bitset<5>(index + 8).to_string();
        else if (index >= 2 && index <= 11) return bitset<5>(index + 16).to_string();
    } else if (reg[0] == 't') {
        int index = stoi(reg.substr(1));
        if (index >= 0 && index <= 2) return bitset<5>(index + 5).to_string();
        else if (index >= 3 && index <= 6) return bitset<5>(index + 25).to_string();
    }
    return "00000";
}

class Instruction {
public:
    string abbr, rd, rs1, rs2;
    string imv = ""; // Updated to handle labels as strings

    static Instruction parse_line(const string& line) {
        istringstream iss(line);
        Instruction inst;
        iss >> inst.abbr;

        if (inst.abbr == "mv") {
            iss >> inst.rd >> inst.rs1;
        } else if (inst.abbr == "addi" || inst.abbr == "li") {
            iss >> inst.rd >> inst.rs1 >> inst.imv;
        } else if (inst.abbr == "mul") {
            iss >> inst.rd >> inst.rs1 >> inst.rs2;
        } else if (inst.abbr == "ble") {
            iss >> inst.rs1 >> inst.rs2 >> inst.imv; // imv could be a label
        }

        return inst;
    }
};

class Assembler {
private:
    unordered_map<string, tuple<string, string, string>> instructions;
    unordered_map<string, int> label_map; // Map of labels to memory addresses

    void write_to_file(const vector<pair<string, string>>& binary_inst, const string& file_name) {
        ofstream out_file(file_name);
        if (!out_file) {
            cerr << "Error: Could not open file for writing.\n";
            return;
        }
        for (const auto& [abbr, binary] : binary_inst) {
            out_file << abbr << ", " << binary << "\n";
            // out_file << binary << "\n";
        }
        out_file.close();
        cout << "Binary code saved to " << file_name << "\n";
    }

public:
    Assembler() {
        instructions = {
            // R-Type
            {"add", {"0110011", "000", "0000000"}}, {"sub", {"0110011", "000", "0100000"}},
            {"mul", {"0110011", "000", "0000001"}}, {"and", {"0110011", "111", "0000000"}},
            {"or", {"0110011", "110", "0000000"}}, {"xor", {"0110011", "100", "0000000"}},
            {"sll", {"0110011", "001", "0000000"}}, {"srl", {"0110011", "101", "0000000"}},
            {"sra", {"0110011", "101", "0100000"}},

            // I-Type
            {"addi", {"0010011", "000", ""}}, {"andi", {"0010011", "111", ""}},
            {"ori", {"0010011", "110", ""}}, {"xori", {"0010011", "100", ""}},
            {"slli", {"0010011", "001", "0000000"}}, {"srli", {"0010011", "101", "0000000"}},
            {"srai", {"0010011", "101", "0100000"}}, {"jalr", {"1100111", "000", ""}},
            {"lw", {"0000011", "010", ""}},

            // S-Type
            {"sw", {"0100011", "010", ""}},

            // B-Type
            {"beq", {"1100011", "000", ""}}, {"bne", {"1100011", "001", ""}}, {"ble", {"1100011", "110", ""}},

            // J-Type
            {"jal", {"1101111", "", ""}}, {"j", {"1101111", "", "00000"}}
        };
    }

    void parse_labels(const string& input_file) {
        ifstream in_file(input_file);
        if (!in_file) {
            cerr << "Error: Could not open input assembly file.\n";
            return;
        }

        string line;
        int address = 1000;
        while (getline(in_file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line.back() == ':') {
                string label = line.substr(0, line.size() - 1);
                label_map[label] = address;
            } else {
                address += 4;
            }
        }
    }

    void parse_asm(const string& input_file, const string& output_file) {
        ifstream in_file(input_file);
        if (!in_file) {
            cerr << "Error: Could not open input assembly file.\n";
            return;
        }

        vector<pair<string, string>> binary_inst;
        string line;
        int current_address = 1000;

        while (getline(in_file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line.back() == ':') cout << "Label => " << line << endl;

            try {
                auto inst = Instruction::parse_line(line);

                if (inst.abbr == "ble" || inst.abbr == "beq" || inst.abbr == "bne") {
                    int target_address = label_map[inst.imv];
                    inst.imv = to_string((target_address - current_address) / 4);
                } else if (inst.abbr == "jal" || inst.abbr == "j") {
                    inst.imv = to_string(label_map[inst.imv]);
                }

                auto binary = encode_inst(inst);

                if (binary.empty()) {
                    cerr << "Warning: Unable to encode instruction: " << line << "\n";
                    continue;
                }

                binary_inst.emplace_back(inst.abbr, binary);
                current_address += 4;
            } catch (const exception& e) {
                cerr << "Error: Failed to process line: " << line << "\n";
            }
        }

        write_to_file(binary_inst, output_file);
    }

    string encode_inst(const Instruction& inst) {
        string binary;
        string opcode, funct3, funct7, rd, rs1, rs2, imv;

        string actual_abbr = inst.abbr;
        if (inst.abbr == "mv") {
            actual_abbr = "addi";
            rs1 = reg_to_bin(inst.rs1);
            rd = reg_to_bin(inst.rd);
            imv = bitset<12>(0).to_string();
        } else if (inst.abbr == "li") {
            actual_abbr = "addi";
            rs1 = reg_to_bin("x0");
            rd = reg_to_bin(inst.rd);
            imv = bitset<12>(inst.imv).to_string();
        } else if (inst.abbr == "ret") {
            actual_abbr = "jalr";
            rs1 = reg_to_bin("x1");
            rd = reg_to_bin("x0");
            imv = bitset<12>(0).to_string();
        }

        if (instructions.count(actual_abbr)) {
            auto [op, f3, f7] = instructions[actual_abbr];
            opcode = op;
            funct3 = f3;
            funct7 = f7;

            if (opcode == "0110011") { // R-Type
                rd = reg_to_bin(inst.rd);
                rs1 = reg_to_bin(inst.rs1);
                rs2 = reg_to_bin(inst.rs2);
                binary = funct7 + rs2 + rs1 + funct3 + rd + opcode;
            } else if (opcode == "0010011" || opcode == "1100111" || opcode == "0000011") { // I-Type
                rd = reg_to_bin(inst.rd);
                rs1 = reg_to_bin(inst.rs1);
                imv = bitset<12>(inst.imv).to_string();
                binary = imv + rs1 + funct3 + rd + opcode;
            } else if (opcode == "0100011") { // S-Type
                rs1 = reg_to_bin(inst.rs1);
                rs2 = reg_to_bin(inst.rs2);
                imv = bitset<12>(inst.imv).to_string();
                binary = imv.substr(0, 7) + rs2 + rs1 + funct3 + imv.substr(7, 5) + opcode;
            } else if (opcode == "1100011") { // B-Type
                rs1 = reg_to_bin(inst.rs1);
                rs2 = reg_to_bin(inst.rs2);
                imv = bitset<12>(stoi(inst.imv)).to_string();
                binary = imv.substr(0, 1) + imv.substr(2, 6) + rs2 + rs1 + funct3 + imv.substr(8, 4) + imv.substr(1, 1) + opcode;
            } else if (opcode == "1101111") { // J-Type
                rd = reg_to_bin(inst.rd);
                imv = bitset<20>(stoi(inst.imv)).to_string();
                binary = imv.substr(0, 1) + imv.substr(10, 10) + imv.substr(9, 1) + imv.substr(1, 8) + rd + opcode;
            }
        }

        return binary;
    }
};

int main() {
    Assembler assembler;
    assembler.parse_labels("asm.s");
    assembler.parse_asm("asm.s", "binary.txt");
    return 0;
}