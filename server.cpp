#include <opencv2/opencv.hpp>
#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <termio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 5000

using namespace std;
using namespace cv;
int main(void)
{
    int sock, client_sock;
    struct sockaddr_in addr, client_addr;
    char buffer[1024];
    char buf_img[1024];
    int len, addr_len, recv_len;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "socket" << endl;
        return 1;
    }

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cout << "bind" << endl;
        return 1;
    }

    if (listen(sock, 5) < 0)
    {
        cout << "listen" << endl;
        return 1;
    }

    addr_len = sizeof(client_addr);
    cout << "waiting for clinet.." << endl;

    Mat frame = Mat::zeros(120,320,CV_8UC3);
    int frame_size = frame.total()*frame.elemSize();;
    uchar sockData[frame_size];
    int bytes = 0;
    
    while ((client_sock = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) > 0)
    {
        cout << "clinet ip : " << inet_ntoa(client_addr.sin_addr) << endl;
        while (1)
        {
            for (int i = 0; i < frame_size; i += bytes)
            {
                if ((bytes = recv(client_sock, sockData + i, frame_size - i, 0)) == -1)
                {
                    cout << "recv failed" << endl;
                }
            }
            int ptr = 0;
            for (int i = 0; i < frame.rows; i++)
            {
                for (int j = 0; j < frame.cols; j++)
                {
                    frame.at<cv::Vec3b>(i, j) = cv::Vec3b(sockData[ptr + 0], sockData[ptr + 1], sockData[ptr + 2]);
                    ptr = ptr + 3;
                }
            }
            if (frame.data)
            {
                imshow("frame", frame);
            }
            else
            {
                cout << "no frame" << endl;
            }
            if (waitKey(1) >= 0)
                break;
            //cout << "imshow" << endl;
            // if ((recv_len = recv(client_sock, buffer, 1024, 0)) < 0)
            // {
            //     cout << "recv" << endl;
            //     return 1;
            //     break;
            // }
            //buffer[recv_len] = '\0';
            // cout << "received data : " << buffer << endl;
            send(client_sock, "ad", 1024, 0);
        }
        close(client_sock);
    }
    close(sock);
    return 0;
}
