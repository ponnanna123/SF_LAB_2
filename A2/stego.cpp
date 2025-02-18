#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

using namespace std;

struct BMPHeader {
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offsetData;
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    int32_t xPelsPerMeter;
    int32_t yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
};

struct Pixel {
    uint8_t b, g, r;
};

class Image {
public:
    int width, height;
    vector<vector<Pixel>> pixels;

    Image(int w, int h) : width(w), height(h), pixels(h, vector<Pixel>(w)) {}
};

Image readBMP(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        exit(1);
    }

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    Image img(header.width, header.height);

    file.seekg(header.offsetData, ios::beg);

    int padding = (4 - (img.width * sizeof(Pixel)) % 4) % 4;

    for (int y = img.height - 1; y >= 0; --y) {
        file.read(reinterpret_cast<char*>(img.pixels[y].data()), img.width * sizeof(Pixel));
        file.seekg(padding, ios::cur);
    }

    file.close();
    return img;
}

void writeBMP(const string& filename, const Image& img) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    BMPHeader header = {};
    header.fileType = 0x4D42;
    header.offsetData = sizeof(BMPHeader);
    header.size = sizeof(BMPHeader) - 14;
    header.width = img.width;
    header.height = img.height;
    header.planes = 1;
    header.bitCount = 24;
    
    int padding = (4 - (img.width * sizeof(Pixel)) % 4) % 4;
    header.sizeImage = (img.width * sizeof(Pixel) + padding) * img.height;
    header.fileSize = header.offsetData + header.sizeImage;

    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    for (int y = img.height - 1; y >= 0; --y) {
        file.write(reinterpret_cast<const char*>(img.pixels[y].data()), img.width * sizeof(Pixel));
        for (int p = 0; p < padding; ++p) {
            file.put(0);
        }
    }

    file.close();
}

void encode(Image& img, const string& sample_file) {
    ifstream file(sample_file, ios::binary);

    if (!file) {
        cerr << "Error: Cannot open file " << sample_file << endl;
        return;
    }

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    text += '\0';

    int textIndex = 0, bitIndex = 0;
    int h = img.height;
    int w = img.width;

    bool finished = false;

    for (int y = 0; y < h && !finished; ++y) {
        for (int x = 0; x < w && !finished; x += 1) {
            Pixel& px = img.pixels[y][x];
            for (int j = 0; j < 3; ++j) {
                if (finished) break;
                uint8_t& channel = (j == 0) ? px.b : (j == 1) ? px.g : px.r;
                channel = (channel & ~1) | ((text[textIndex] >> bitIndex) & 1);

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

void decode(const Image& img, const string& out_file) {
    ofstream file(out_file, ios::binary);

    if (!file) {
        cerr << "Error: Cannot open file " << out_file << endl;
        return;
    }

    string text;
    char currChar = 0;
    int bitIndex = 0;
    bool finished = false;
    int h = img.height;
    int w = img.width;

    for (int y = 0; y < h && !finished; ++y) {
        for (int x = 0; x < w && !finished; x += 1) {
            const Pixel& px = img.pixels[y][x];
            for (int j = 0; j < 3; ++j) {
                uint8_t channel = (j == 0) ? px.b : (j == 1) ? px.g : px.r;
                currChar |= (channel & 1) << bitIndex;

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
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_image.bmp> <input_text_file>" << endl;
        return 1;
    }

    Image img = readBMP(argv[1]);

    encode(img, argv[2]);
    writeBMP("encoded.bmp", img);

    Image encoded_img = readBMP("encoded.bmp");
    decode(encoded_img, "out_text.txt");

    return 0;
}