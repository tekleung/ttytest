#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//#define UART_SPEED B9600
//#define UART_SPEED B115200
#define UART_SPEED B921600
#define DEV	"/dev/ttyS1"
#define MAX_SIZE 1024

//client
int main(void)
{
        int fd, ret, err, success=0, fail=0;
        struct termios opts;
	char buf[MAX_SIZE]="aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444==========0000000000PPPP";
//	char buf[1024];
	char rbuf[MAX_SIZE];
	char destbuf[MAX_SIZE];

        fd = open(DEV, O_RDWR|O_NOCTTY|O_NONBLOCK);
        if (fd < 0)
        {
                perror("open tty");
                return 1;
        }

        fcntl(fd, F_SETFL, 0); //恢复blocking 模式

        tcgetattr(fd, &opts); //获取串口设备的属性

        cfsetispeed(&opts, UART_SPEED); //设置输入的波特率
        cfsetospeed(&opts, UART_SPEED); //设置输出的波特率

        opts.c_cflag |= CLOCAL|CREAD; //忽略猫的控制线, 启动接收        

        opts.c_cflag &= ~(CSIZE); //清除数据位的值
        opts.c_cflag |= CS8;      //设8位数据位

        opts.c_cflag &= ~PARENB; //清除校验检查
        opts.c_cflag &= ~CSTOPB; //一位停止位
        opts.c_cflag &= ~CRTSCTS; //清除硬件流控

        // line options
        opts.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
        opts.c_oflag     &= ~OPOST; //raw output
        opts.c_cc[VMIN]  = 1; //设最少收多少个字符才返回
        opts.c_cc[VTIME] = 10; //设等待的上限时间

        tcsetattr(fd, TCSANOW, &opts); //更改串口的属性

        while (1)
        {
		memset(rbuf, 0, MAX_SIZE);
		memset(destbuf, 0, MAX_SIZE);
                ret=write(fd, buf, MAX_SIZE); //client write
	//printf("Ready for Server data to Read: \n");
//	printf("0.ret:%d, %s\n",ret, buf);
#if 1
		ret = read(fd, rbuf, sizeof(rbuf));
//	printf("1.ret:%d: rbuf=%s\n",ret, rbuf);
		memcpy(destbuf, rbuf, ret);
		while(1){
			if (ret >= MAX_SIZE || ret ==0)
				break;
			err = read(fd, rbuf, MAX_SIZE-ret);
			rbuf[err]='\0';
			strcat(destbuf+ret, rbuf);
//			printf("2.err:%d: rbuf=%s\n",err, rbuf);
			ret += err;
//		sleep(1);
		}
//#endif
		
	buf[MAX_SIZE]='\0';
	destbuf[MAX_SIZE]='\0';
		ret = strcmp(buf, destbuf);
//	printf("3.ret:%d: destbuf=%s\n",ret, destbuf);
//	printf("-->%s\n", buf);
//	printf("++>%s\n", destbuf);
		if (ret == 0){
			success ++;
		}else{
			fail ++;
			printf("tty result: success=%d, fail=%d\n", success, fail);
		}
			printf("tty result: success=%d, fail=%d\n", success, fail);
#endif
//	sleep(1);
        }


        close(fd);
        return 0;
}
