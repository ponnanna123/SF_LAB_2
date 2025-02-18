#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <string>
#include <filesystem>
#include <algorithm>
#include <limits>

using namespace std;
namespace fs = std::filesystem;

void executeCommand(const string& command) {
    int result = system(command.c_str());
    if (result != 0) {
        cerr << "Error executing command: " << command << endl;
        exit(1);
    }
}

vector<string> getFilesInDirectory(const string& directory, const string& extension) {
    vector<string> files;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == extension) {
            files.push_back(entry.path().string());
        }
    }
    sort(files.begin(), files.end());
    return files;
}

vector<int> calculateBMPHistogram(const string& filename) {
    vector<int> histogram(256, 0);
    ifstream file(filename, ios::binary);
    
    if (!file) {
        cerr << "Error opening BMP file" << endl;
        exit(1);
    }
    
    file.seekg(54);
    
    unsigned char pixel;
    while (file.read(reinterpret_cast<char*>(&pixel), 1)) {
        histogram[pixel]++;
    }
    
    return histogram;
}

vector<int> calculateTextHistogram(const string& filename) {
    vector<int> histogram(256, 0);
    ifstream file(filename);
    
    if (!file) {
        cerr << "Error opening text file" << endl;
        exit(1);
    }
    
    char c;
    while (file.get(c)) {
        histogram[static_cast<unsigned char>(c)]++;
    }
    
    return histogram;
}

double calculateHistogramDifference(const vector<int>& hist1, const vector<int>& hist2) {
    double difference = 0.0;
    for (size_t i = 0; i < hist1.size(); ++i) {
        difference += abs(hist1[i] - hist2[i]);
    }
    return difference;
}

int main(int argc, char *argv[]) {
    string imageDirectory = "images/";
    string textDirectory = "messages/";
    string outputDirectory = "output/";
    
    fs::create_directory(outputDirectory);

    vector<string> imageFiles = getFilesInDirectory(imageDirectory, ".png");
    vector<string> textFiles = getFilesInDirectory(textDirectory, ".txt");

    for (size_t i = 0; i < textFiles.size(); ++i) {
        string textFilename = textFiles[i];
        string outputFilename = outputDirectory + "output_" + fs::path(textFilename).stem().string() + ".txt";
        ofstream outputFile(outputFilename);

        if (!outputFile) {
            cerr << "Error creating output file: " << outputFilename << endl;
            continue;
        }

        vector<int> textHistogram = calculateTextHistogram(textFilename);
        
        double minDifference = numeric_limits<double>::max();
        string bestImage;

        outputFile << "Histogram differences for " << textFilename << ":\n\n";

        for (const auto& imageFile : imageFiles) {
            string bmpFilename = outputDirectory + fs::path(imageFile).stem().string() + ".bmp";
            string convertCommand = "convert \"" + imageFile + "\" \"" + bmpFilename + "\"";
            executeCommand(convertCommand);

            vector<int> bmpHistogram = calculateBMPHistogram(bmpFilename);
            double difference = calculateHistogramDifference(bmpHistogram, textHistogram);

            outputFile << "Image: " << imageFile << ", Difference: " << difference << "\n";

            if (difference < minDifference) {
                minDifference = difference;
                bestImage = imageFile;
            }

            fs::remove(bmpFilename);
        }

        outputFile << "\nBest cover image: " << bestImage << " (Difference: " << minDifference << ")\n";
        outputFile.close();

        cout << "Processed " << textFilename << ", output written to " << outputFilename << endl;
    }

    return 0;
}
