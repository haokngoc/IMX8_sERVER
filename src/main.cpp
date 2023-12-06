#include <iostream>
#include <thread>
#include "sockpp/tcp_acceptor.h"
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

using namespace std;



#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
    void   *start;
    size_t length;
};



enum {
	VVCSIOC_RESET = 0x100,
	VVCSIOC_POWERON,
	VVCSIOC_POWEROFF,
	VVCSIOC_STREAMON,
	VVCSIOC_STREAMOFF,
	VVCSIOC_S_FMT,
	VVCSIOC_S_HDR,
};


struct csi_sam_format {
	int64_t format;
	__u32 width;
	__u32 height;
};


// --------------------------------------------------------------------------
// The main thread runs the TCP port acceptor. Each time a connection is
// made a new thread is spawned to handle it leaving this main thread to
// immediately wait for the next incoming connection.

enum {
	DET_STATE_WORK = 11,
	DET_STATE_SLEEP,
	DET_STATE_CLOSE,
	DET_STATE_REVERSED,
};

#define MARKER_HEAD 0xAA
#define MARKER_TAIL 0x55


typedef struct _CMD_PACKET
{
	char data[5];
} CMD_PACKET;

//int parse_MGard300_CMD(sockpp::tcp_socket sock){
//	ssize_t n_read_bytes;
//	CMD_PACKET  cmd;
//	int ret = -1;
//	n_read_bytes = sock.read(cmd.data, sizeof(cmd.data));
//	if (n_read_bytes < 0 | n_read_bytes < 5)
//		return -2 ;
//	if (cmd.data[0] != MARKER_HEAD | cmd.data[4] != MARKER_TAIL )
//		return -3 ;
//	switch(cmd.data[1]){
//	case 0x1b:
//		ret = DET_STATE_SLEEP;
//		break;
//	case 0x58:
//		ret = DET_STATE_WORK;
////	case 0x07:
////		ret =
//	default:
//		ret = -1;
//		break;
//	}
//
//	return ret;
//}

int parse_Msg1_Client(const unsigned char* buf, ssize_t n) {
	int ret = -1;
	if(n<0 || n>5) {
		return -2;
	}
	if(buf[0] != MARKER_HEAD || buf[4] != MARKER_TAIL) {
		return -3;
	}
	switch (buf[1]) {
		case 0x1b:
			ret = 2;
			break;
		case 0x58:
			ret = 3;
			break;
		default:
			break;
	}
	return ret;
}
int parse_Msg2_Client(const unsigned char* buf, ssize_t n) {
	int ret = -1;
	if(n<0 || n>5) {
		return -2;
	}
	if(buf[0] != MARKER_HEAD || buf[4] != MARKER_TAIL) {
		return -3;
	}
	switch (buf[2]) {
		case 0x2b:
			ret = 2;
			break;
		case 0x56:
			ret = 3;
			break;
		default:
			break;
	}
	return ret;
}
int parse_Msg3_Client(const unsigned char* buf, ssize_t n) {
	int ret = -1;
	if(n<0 || n>5) {
		return -2;
	}
	if(buf[0] != MARKER_HEAD || buf[4] != MARKER_TAIL) {
		return -3;
	}
	switch (buf[3]) {
		case 0x3b:
			ret = 2;
			break;
		case 0x55:
			ret = 3;
			break;
		default:
			break;
	}
	return ret;
}

int main(int argc, char* argv[])
{
//    FILE                            *fout;
//    int state = DET_STATE_CLOSE;
//	unsigned char * frame_buf = new unsigned char[21980160];
//	memset(frame_buf, 0x00, 21980160);
//	for (int i = 0; i < 21980160; i++)
//	{
//		frame_buf[i] = i % 255;
//	}
	in_port_t port = 1024;
//	acquire_img(frame_buf);
	sockpp::initialize();
	sockpp::tcp_acceptor acc(port);
	if (!acc) {
		cerr << "Error creating the acceptor: " << acc.last_error_str() << endl;
		return 1;

	}
	cout << "Awaiting connections on port " << port << "..." << endl;



	// Accept a new client connection
	while (true) {
		sockpp::inet_address peer;
		sockpp::tcp_socket sock = acc.accept(&peer);
		if (!sock) {
			cerr << "Error accepting incoming connection: "
				<< acc.last_error_str() << endl;
			continue;
		}
		cout << "Received a connection request from " << peer << std::endl;
//		else {
//			ssize_t n;
//			unsigned char buf[5];
//			memset(buf, 0, 5);
//			while ((n = sock.read(buf, sizeof(buf))) > 0) {
//				//	std::string str(buf, n);
//				//	std::cout << "Server is getting:" << n << " bytes | " << "getting:" << std::setw(2) << str.c_str() << std::endl;
//				for (int i = 0; i < n; i++)
//				{
//					cout << "Packt."<< i <<":" << (int)buf[i]<< endl;
//				}
//				//if (buf[1] == 0x07 | buf[1] == 0x1B | buf[1] == 0x58  )
//					//break;
//			}
//		//	acquire_img(frame_buf);
////            fout = fopen("frame_buf_2.raw", "w");
////            fwrite(frame_buf, 21980160, 1, fout);
////            fclose(fout);
////			sock.write_n(frame_buf , 3456 * 3180* 2);
////			cout << "Going to sending frame buffer to Mgard300! " << "..." << endl;
//
//		//	sleep(1);
////			sleep(1);
////			cout << "Going to exit! " << "..." << endl;
//			//send out 1
//			//sock.write_n(buf, n);
//			//break;
//		//	std::cout << "Connection closed from " << sock.peer_address() << std::endl;
//		}
		// nhận dữ liệu từ client và hiển thị
		unsigned char buf[5];
		ssize_t n = sock.read(buf,size(buf));
		if(n>0) {
			for(int i=0; i<n; i++) {
				cout << "Packt." << i << ": " << static_cast<int>(buf[i]) << endl;
			}
		}
		// xử lý dữ liệu
		if(parse_Msg1_Client(buf,n) == 2) {
			cout << "Change working status A" << endl;
		}
		else if(parse_Msg1_Client(buf,n) == 3) {
			cout << "Change working status B" << endl;
		}
		else {
			cout << "Error" << endl;
		}
		if(parse_Msg2_Client(buf,n) == 2) {
			cout << "Change working status C" << endl;
		}
		else if(parse_Msg2_Client(buf,n) == 3) {
			cout << "Change working status D" << endl;
		}
		else {
			cout << "Error" << endl;
		}
		//đóng kết nối với client
		sock.close();
		cout << "Connection closed from " << peer << endl;

	}

//	delete[] frame_buf;

	return 0;
}


