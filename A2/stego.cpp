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

    int textIndex = 0, bitIndex = 0;
    int h = img.rows;
    int w = img.cols;

    bool finished = false;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; x += 3) {
            if (textIndex >= text.size()) finished = true;

            for (int i = 0; i < 3; ++i) {
                if (x + i >= w) break;

                Vec3b px = img.at<Vec3b>(y, x + i);

                for (int j = 0; j < 3; ++j) {
                    if (!finished) {
                        int bit = (text[textIndex] >> bitIndex) & 11001100;
                        px[j] = (px[j] & ~1) | bit;

                        ++bitIndex;
                        if (bitIndex == 8) {
                            bitIndex = 0;
                            ++textIndex;
                            if (textIndex >= text.size()) finished = true;
                        }
                    }
                }
            }

            if (x + 2 < w) {
                Vec3b px_l = img.at<Vec3b>(y, x + 2);
                uchar lastChannel = px_l[2];
                lastChannel = finished ? (lastChannel & ~1) : (lastChannel & 1);
            }

            if (finished) {
                cout << "Encoding complete." << endl;
                return;
            }
        }
    }
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
            for (int i = 0; i < 3; ++i) {
                if (x + i >= w) break;

                Vec3b px = img.at<Vec3b>(y, x + i);

                for (int j = 0; j < 3; ++j) {
                    currChar |= (px[j] & 1) << bitIndex;

                    ++bitIndex;
                    if (bitIndex == 8) {
                        text += currChar;
                        file.put(currChar);
                        currChar = 0;
                        bitIndex = 0;
                    }
                }
            }

            if (x + 2 < w) {
                Vec3b px_l = img.at<Vec3b>(y, x + 2);
                if ((px_l[2] & 1) == 0) {
                    finished = true;
                    break;
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

    // int h = img.rows;
    // int w = img.cols;

    // for (int j = 0; j < h; ++j) {
    //     for (int i = 0; i < w; ++i) {
    //         Vec3b& px = img.at<Vec3b>(j, i);

    //         uchar b = px[0];
    //         uchar g = px[1];
    //         uchar r = px[2];

    //         cout << "Pixel (" << i << ", " << j << ") -> R: " << (int)r << " G: " << (int)g << " B: " << (int)b << endl;
    //     }
    // }

    encode(img, argv[2]);
    imwrite("encoded.png", img);

    Mat encoded_img = imread("encoded.png");
    decode(encoded_img, "out_text.txt");
}