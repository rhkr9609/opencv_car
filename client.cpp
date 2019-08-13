// #include <ros/ros.h>
//#include <cv_bridge/cv_bridge.h>
//#include <cv.hpp>
//#include "std_msgs/Int16.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
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
#define PORT 9999

using namespace std;
using namespace cv;
int main(void)
{
    int sock;
    struct sockaddr_in addr;
    char buffer[1024];
    int recv_len;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "socket error" << endl;
        return 1;
    }

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cout << "connect error" << endl;
        return 1;
    }

    VideoCapture cap("/home/cseecar/ser_cli/client/track1.mp4");
    Mat frame;
    int frame_size;
    while (1)
    {
        cap >> frame;
        resize(frame, frame, Size(320, 240));
        Rect rect_roi(0, 120, 320, 120);
        frame = frame(rect_roi);
        frame_size = frame.total() * frame.elemSize();

        send(sock, frame.data, frame_size, 0);

        if ((recv_len = recv(sock, buffer, 1024, 0)) < 0)
        {
            cout << "recv error" << endl;
            return 1;
        }
        buffer[recv_len] = '\0';
        cout << "recevied data : " << buffer << endl;
    }
    close(sock);

    return 0;
}
