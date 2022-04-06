#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <array>

using std::cin;
using std::cout;
using std::string;
using std::ifstream;
using std::endl;
using std::vector;
using std::stringstream;
using std::stoi;
using std::all_of;
using std::array;

struct IPAddress {
    string addr;
    array<unsigned int, 4> addr_quarters;

    IPAddress(string s) {
        addr = s;

        // Tokenize based on '.'
        stringstream ss(s);
        string quarter_addr = "";
        // cout << "Extracting IP address..." << endl;
        for (int i = 0; i < 4 && getline(ss, quarter_addr, '.'); i++) {
            if (quarter_addr == "*") {
                addr_quarters[i] = -1; // TODO: Careful! This is unsigned!
            }
            else {
                addr_quarters[i] = stoi(quarter_addr);
                // cout << quarter_addr << ".";
            }
        }
        // cout << endl;
    }
};

struct Row {
    IPAddress destination;
    IPAddress gateway;
    unsigned int genmask;
    int metric;
    string interface;
};

void print_table(const vector<Row>& table) {
    for (auto row : table) {
        cout << row.destination.addr << " "
            << row.gateway.addr << " " 
            << row.genmask << " " 
            << row.metric << " "
            << row.interface << endl;
    }
}

unsigned int make_mask(IPAddress mask_addr) {
    unsigned int mask = 0;
    for (int i = 0; i < 4; i++) {
        unsigned int quart = mask_addr.addr_quarters[3 - i];
        mask += (quart << 8*i);
    }
    return mask;
}

int main() {
    // Ask for the filename
    cout << "Please enter the file name for the routing table (note that the file must exist in this project's directory): ";
    string file_name = "";
    getline(cin, file_name);

    // Open the file.
    ifstream file;
    file.open(file_name);
    if (!file.is_open()) {
        cout << "Error: File does not exist." << endl;
        return 1;
    }

    // Read the file.
    string line = "";
    vector<Row> routing_table;
    while (getline(file, line) && !all_of(line.begin(), line.end(), isspace)) {
        // Tokenize line with tabs \t.
        // Write each component into memory as needed.
        stringstream ss(line);
        string elem;
        // Initialize row with nulls.
        IPAddress null_addr("");
        Row row = {null_addr, null_addr, 0, 0, ""};

        // Extract the first element.
        getline(ss, elem, '\t');
        row.destination = IPAddress(elem);
        
        // Get second element.
        getline(ss, elem, '\t');
        row.gateway = IPAddress(elem);

        // Third.
        getline(ss, elem, '\t');
        IPAddress genmask(elem);
        row.genmask = make_mask(genmask);

        // Fourth.
        getline(ss, elem, '\t');
        row.metric = stoi(elem); // This fails.
        
        // Fifth.
        getline(ss, elem, '\t');
        row.interface = elem;

        routing_table.push_back(row);
    }

    // Close file.
    file.close();

    print_table(routing_table);

    return 0;
}
