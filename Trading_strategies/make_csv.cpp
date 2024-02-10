#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// Function to write data to a CSV file
void writeCSV(const string& filename, const vector<vector<string>>& data) {
    ofstream file(filename);

    // Check if the file is opened successfully
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }

    // Iterate over each row of data
    for (const auto& row : data) {
        // Iterate over each cell in the row
        for (auto iter = row.begin(); iter != row.end(); ++iter) {
            file << *iter; // Write the cell value
            if (iter != row.end() - 1) {
                file << ","; // Add a comma if it's not the last cell in the row
            }
        }
        file << endl; // Move to the next line after writing each row
    }

    // Close the file
    file.close();

    cout << "CSV file '" << filename << "' created successfully." << endl;
}

int main() {
    // Example data to be written to the CSV file
    vector<vector<string>> data = {
        {"Name", "Age", "City"},
        {"John", "30", "New York"},
        {"Alice", "25", "Los Angeles"},
        {"Bob", "35", "Chicago"}
    };

    // Name of the CSV file to be created
    string filename = "example.csv";

    // Write the data to the CSV file
    writeCSV(filename, data);

    return 0;
}
