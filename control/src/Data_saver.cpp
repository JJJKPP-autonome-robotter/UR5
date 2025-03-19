#include "Data_saver.hpp"

DataSaver::DataSaver(const std::string& file) : filename(file) {}

void DataSaver::readData() {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    data.clear();
    Data temp;
    while (fin >> temp.b >> temp.c >> temp.d) {
        data.push_back(temp);
    }
    fin.close();
}

void DataSaver::addData(const Data& newData) {
    data.push_back(newData);
}

void DataSaver::modifyData(const std::function<void(std::vector<Data>&)>& modifier) {
    modifier(data); // Apply the modification logic
}

void DataSaver::writeData() {
    std::ofstream fout("temp.txt");
    if (!fout.is_open()) {
        throw std::runtime_error("Error: Could not open temporary file for writing");
    }

    for (const auto& entry : data) {
        fout << entry.b << ' ' << entry.c << ' ' << entry.d << '\n';
    }
    fout.close();

    // Replace the original file with the temporary file
    if (std::remove(filename.c_str()) != 0) {
        throw std::runtime_error("Error: Could not delete the original file");
    }
    if (std::rename("temp.txt", filename.c_str()) != 0) {
        throw std::runtime_error("Error: Could not rename the temporary file");
    }
}

const std::vector<Data>& DataSaver::getData() const {
    return data;
}