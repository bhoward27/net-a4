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
using std::pair;
using std::min_element;

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

    uint32_t operator&(const IPAddress& right) const {
        return bin_addr & right.bin_addr;
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

int best_match(const vector<Row>& routing_table, const vector<int>& matches) {
    /*
        Rules:
            - Favour matches with longer genmasks
            - If genmasks are equal length, choose row with cheapest cost/metric
            - If genmasks and costs are equal, choose row of smaller index.

        Rules have to work if more than two matches are at play. Should work for any number.
    */

    vector<pair<int, int>> equal_genmasks;
    IPAddress prev_match_mask("");
    bool prev_pair_was_equal = false;
    for (int i = 0; i < matches.size(); i++) {
        int match = matches[i];
        IPAddress match_mask = routing_table[match].genmask;
        if (i && prev_match_mask == match_mask) {
            if (!prev_pair_was_equal && i > 1) {
                break;
            }
            if (i == 1) {
                equal_genmasks.push_back({matches[0], routing_table[matches[0]].metric});
            }
            equal_genmasks.push_back({match, routing_table[match].metric});
            prev_pair_was_equal = true;
        } else if (prev_pair_was_equal) {
            break;
        }
        prev_match_mask = match_mask;
    }

    // TODO: Add this code back in. It currently causes a seg fault on dereferencing.
    // Find the min cost from equal_genmasks.
    // auto min_elem = *min_element(equal_genmasks.begin(), equal_genmasks.end(), [](const auto& lhs, const auto& rhs) {
    //     return lhs.second < rhs.second;    
    // });
    // int min_cost = min_elem.second;
    int min_cost = 0;

    // Then choose the first index that has the min cost.
    int best_match;
    for (auto match : equal_genmasks) {
        if (min_cost == match.second) {
            best_match = match.first;
        }
    }

    return best_match;
}

// Returns the index of the row of the routing table that the packet should be sent to.
int forward(IPAddress packet_addr, const vector<Row>& routing_table) {
    vector<int> matches;
    int len = routing_table.size();
    for (int i = 0; i < len; i++) {
        Row row = routing_table[i];
        if ((row.destination & row.genmask) == (packet_addr & row.genmask)) {
            matches.push_back(i);
        }
    }
    return best_match(routing_table, matches);
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
        Row best_row = routing_table[forward(addr, routing_table)];
        cout << "The destination IP address is " << best_row.destination.addr << endl;
        // TODO: print next hop address
        cout << "The interface the packet will leave through is " << best_row.interface << endl;

        // Ask whether to continue.
        cout << "Would you like to forward another packet? (Y/n): ";
        cin >> cont;
    } while (cont == 'Y');

    return 0;
}
