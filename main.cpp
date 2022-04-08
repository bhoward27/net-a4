#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <array>
#include <cstdint>
#include <bits/stdc++.h>

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
using std::sort;
using std::greater;

struct IPAddress {
    string addr;
    uint32_t bin_addr;

    IPAddress(string s) {
        addr = s;

        // Tokenize based on '.'
        array<uint32_t, 4> addr_quarters;
        stringstream ss(s);
        string quarter_addr = "";
        for (int i = 0; i < 4 && getline(ss, quarter_addr, '.'); i++) {
            if (quarter_addr == "*") {
            }
            else {
                addr_quarters[i] = stoi(quarter_addr);
            }
        }
        uint32_t bin_addr = 0;
        for (int i = 0; i < 4; i++) {
            uint32_t quart = addr_quarters[3 - i];
            bin_addr += (quart << 8*i);
        }
        this->bin_addr = bin_addr;
    }

    bool operator==(const IPAddress& right) const {
        return (bin_addr == right.bin_addr);
    }
};

struct Row {
    IPAddress destination;
    IPAddress gateway;
    IPAddress genmask;
    int metric;
    string interface;

    bool operator<(const Row& right) const {
        return (genmask.bin_addr < right.genmask.bin_addr);
    }

    bool operator>(const Row& right) const {
        return (genmask.bin_addr > right.genmask.bin_addr);
    }
};

void print_table(const vector<Row>& table) {
    const char sep = '\t';
    for (auto row : table) {
        cout << row.destination.addr << sep;
        if (row.gateway.addr == "*") cout << "*" << sep;
        else cout << row.gateway.addr << sep;
        cout << row.genmask.addr << sep 
            << row.metric << sep
            << row.interface << endl;
    }
}

// Returns the index of the row of the routing table that the packet should be sent to.
int forward(IPAddress packet_addr, const vector<Row>& routing_table) {
    int len = routing_table.size();
    int num_matches = 0;
    for (int i = 0; i < len; i++) {
        Row row = routing_table[i];

    }

    return len - 1;
}

int main() {
    // Ask for the filename.
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

        stringstream ss(line);
        string elem;

        // Initialize row with nulls.
        IPAddress null_addr("");
        Row row = {null_addr, null_addr, null_addr, 0, ""};

        // Extract the first element.
        getline(ss, elem, '\t');
        row.destination = IPAddress(elem);
        
        // Get second element.
        getline(ss, elem, '\t');
        row.gateway = IPAddress(elem);

        // Third.
        getline(ss, elem, '\t');
        row.genmask = IPAddress(elem);

        // Fourth.
        getline(ss, elem, '\t');
        row.metric = stoi(elem);
        
        // Fifth.
        getline(ss, elem, '\t');
        row.interface = elem;

        routing_table.push_back(row);
    }
    file.close();

    cout << "The table in its original order:\n";
    print_table(routing_table);
    cout << endl;
    
    sort(routing_table.begin(), routing_table.end(), greater<Row>());
    cout << "The table in sorted order:\n";
    print_table(routing_table);

    // Packet forwarding routine.
    string input_addr = "";
    char cont;
    do {
        cout << endl;
        cout << "Enter the IP address of the packet to be forwarded: ";
        cin.ignore();
        getline(cin, input_addr);
        IPAddress addr(input_addr);

        // Forward the packet.


        // Ask whether to continue.
        cout << "Would you like to forward another packet? (Y/n): ";
        cin >> cont;
    } while (cont == 'Y');

    return 0;
}
