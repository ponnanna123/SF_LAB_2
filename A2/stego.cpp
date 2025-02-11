#include <iostream>
#include <fstream>
#include <opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void encode(Mat& img, const string& sample_file) {
    ifstream file(sample_file, ios::binary);

    if (!file) {
        cerr << "Error: Cannot open file " << sample_file << endl;
        return;
    }

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    text += '\0';

    int textIndex = 0, bitIndex = 0;
    int h = img.rows;
    int w = img.cols;

    bool finished = false;

    for (int y = 0; y < h && !finished; ++y) {
        for (int x = 0; x < w && !finished; x += 3) {
            Vec3b& px = img.at<Vec3b>(y, x);
            for (int j = 0; j < 3; ++j) { // Process R, G, B channels
                if (finished) break;
                px[j] = (px[j] & ~1) | ((text[textIndex] >> bitIndex) & 1);

                ++bitIndex;
                if (bitIndex == 8) {
                    bitIndex = 0;
                    ++textIndex;
                    if (textIndex >= text.size()) {
                        finished = true;
                    }
                }
            }
        }
    }

    cout << "Encoding complete." << endl;
}

void decode(Mat& img, const string& out_file) {
    ofstream file(out_file, ios::binary);

    if (!file) {
        cerr << "Error: Cannot open file " << out_file << endl;
        return;
    }

    string text;
    char currChar = 0;
    int bitIndex = 0;
    bool finished = false;
    int h = img.rows;
    int w = img.cols;

    for (int y = 0; y < h && !finished; ++y) {
        for (int x = 0; x < w && !finished; x += 3) {
            Vec3b px = img.at<Vec3b>(y, x);
            for (int j = 0; j < 3; ++j) {
                currChar |= (px[j] & 1) << bitIndex;

                ++bitIndex;
                if (bitIndex == 8) {
                    if (currChar == '\0') {
                        finished = true;
                        break;
                    }
                    file.put(currChar);
                    text += currChar;
                    currChar = 0;
                    bitIndex = 0;
                }
            }
        }
    }

    file.close();
    cout << "Decoding complete. Output saved to " << out_file << endl;
}

int main(int argc, char *argv[]) {
    Mat img = imread(argv[1], IMREAD_COLOR);

    if (img.empty()) {
        cout << "Error loading image!" << endl;
        return -1;
    }

    encode(img, argv[2]);
    imwrite("encoded.png", img);

    Mat encoded_img = imread("encoded.png");
    decode(encoded_img, "out_text.txt");
}