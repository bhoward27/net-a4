#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using std::cin;
using std::cout;
using std::string;
using std::ifstream;
using std::endl;
using std::vector;
using std::stringstream;
using std::stoi;

struct IPAddress {
    string addr;
    int num1, num2, num3, num4;

    IPAddress(string s) {
        addr = s;

        // process to num 1, num2, etc..
    }
};

struct Row {
    IPAddress destination;
    IPAddress gateway;
    int net_id_len;
    int metric;
    string interface;
};

int count_consecutive_ones(IPAddress mask) {
    return 1;
}

void print_table(const vector<Row>& table) {
    for (auto row : table) {
        cout << row.net_id_len << " " << row.interface << endl;
    }
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
    while (getline(file, line)) {
        // Tokenize line with tabs \t.
        // Write each component into memory as needed.
        stringstream ss(line);
        string elem;
        // Initialize row with nulls.
        IPAddress null_addr("");
        Row row = { null_addr, null_addr, 0, 0, "" };

        // Extract the first element.
        getline(ss, elem, '\t');
        row.destination = IPAddress(elem);
        
        // Get second element.
        getline(ss, elem, '\t');
        row.gateway = IPAddress(elem);

        // Third.
        getline(ss, elem, '\t');
        IPAddress genmask(elem);
        row.net_id_len = count_consecutive_ones(genmask);

        // Fourth.
        getline(ss, elem, '\t');
        // row.metric = stoi(elem); // This fails.
        
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
