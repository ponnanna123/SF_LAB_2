#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

void execute_cmd(const string& command) {
    int result = system(command.c_str());
    if (result != 0) {
        cerr << "Error executing command: " << command << endl;
        exit(1);
    }
}

vector<string> iter_dir(const string& directory, const string& extension) {
    vector<string> files;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == extension) {
            files.push_back(entry.path().string());
        }
    }
    sort(files.begin(), files.end());
    return files;
}

vector<int> hist_bmp(const string& filename) {
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

vector<int> hist_text(const string& filename) {
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

double hist_diff(const vector<int>& hist1, const vector<int>& hist2) {
    double difference = 0.0;
    for (size_t i = 0; i < hist1.size(); ++i) {
        difference += abs(hist1[i] - hist2[i]);
    }
    return difference;
}

int main(int argc, char *argv[]) {
    string img_dir = "images/";
    string txt_dir = "messages/";
    string out_dir = "output/";
    
    fs::create_directory(out_dir);

    vector<string> img_files = iter_dir(img_dir, ".png");
    vector<string> txt_files = iter_dir(txt_dir, ".txt");

    for (size_t i = 0; i < txt_files.size(); ++i) {
        string textFilename = txt_files[i];
        string outputFilename = out_dir + "output_" + fs::path(textFilename).stem().string() + ".txt";
        ofstream outputFile(outputFilename);

        if (!outputFile) {
            cerr << "Error creating output file: " << outputFilename << endl;
            continue;
        }

        vector<int> textHistogram = hist_text(textFilename);
        
        double minDifference = numeric_limits<double>::max();
        string bestImage;

        outputFile << "Histogram differences for " << textFilename << ":\n\n";

        for (const auto& imageFile : img_files) {
            string bmpFilename = out_dir + fs::path(imageFile).stem().string() + ".bmp";
            string convertCommand = "convert \"" + imageFile + "\" \"" + bmpFilename + "\"";
            execute_cmd(convertCommand);

            vector<int> bmpHistogram = hist_bmp(bmpFilename);
            double difference = hist_diff(bmpHistogram, textHistogram);

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
