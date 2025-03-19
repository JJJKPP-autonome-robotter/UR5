#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <cstdio> // For std::remove and std::rename

// Define a struct to represent a single data entry
struct Data {
    int b;
    int c;
    int d;
};

class DataSaver {
private:
    std::string filename;
    std::vector<Data> data; // Store all data in memory

public:
    // Constructor to initialize the file name
    DataSaver(const std::string& file);

    // Function to read all data from the file into memory
    void readData();

    // Function to add a new data set
    void addData(const Data& newData);

    // Function to modify data programmatically
    void modifyData(const std::function<void(std::vector<Data>&)>& modifier);

    // Function to write all data back to the file
    void writeData();

    // Getter for the data (optional, for debugging or external use)
    const std::vector<Data>& getData() const;
};