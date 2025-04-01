#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define MULTICAST_GROUP "239.255.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

using namespace std;

void receiver(const string &username) {
    int sockfd;
    struct sockaddr_in addr{};
    struct ip_mreq mreq{};
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("\U0001F6AB Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("\U0001F6AB setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("\U0001F6AB Bind failed");
        exit(EXIT_FAILURE);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("\U0001F6AB Multicast group join failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        socklen_t addrlen = sizeof(addr);
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addrlen);
        if (len < 0) {
            perror("\U0001F6AB Receive failed");
            continue;
        }
        buffer[len] = '\0';

        string msg(buffer);
        size_t sep = msg.find(':');

        if (sep != string::npos) {
            string recipient = msg.substr(0, sep);
            string message = msg.substr(sep + 1);

            if (recipient == username) {
                cout << "\n\U0001F4E9 Message received: " << message << "\n";
                cout << "\U0001F4AC Enter recipient: ";
                cout.flush();
            }
        }
    }
    close(sockfd);
}

void sender(const string &username) {
    int sockfd;
    struct sockaddr_in addr{};
    string recipient, message;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("\U0001F6AB Socket creation failed");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    addr.sin_port = htons(PORT);

    while (true) {
        cout << "\U0001F4AC Enter recipient: ";
        getline(cin, recipient);
        
        if (recipient == username) {
            cout << "\U0001F6AB You cannot send a message to yourself!\n";
            continue;
        }

        cout << "\U0001F4AC Enter message: ";
        getline(cin, message);
        
        string fullMessage = recipient + ":" + username + " says: " + message;
        if (sendto(sockfd, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("\U0001F6AB Send failed");
        } else {
            cout << "\U0001F680 Message sent successfully!\n";
        }
    }
    close(sockfd);
}

int main() {
    string username;
    cout << "\U0001F464 Enter your username: ";
    getline(cin, username);

    thread recvThread(receiver, username);
    thread sendThread(sender, username);

    recvThread.join();
    sendThread.join();

    return 0;
}
