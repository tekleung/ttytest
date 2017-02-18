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

#include <sys/time.h>
//#define UART_SPEED B9600
#define UART_SPEED B115200
//#define UART_SPEED B921600
#define DEV	"/dev/ttyUSB0"
//#define DEV	"/dev/ttyS1"
#define MAX_SIZE 1024

//server PC
int main(void)
{
        int fd, ret, err, success=0, fail=0;
        struct termios opts;
	char buf[MAX_SIZE]="aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444==========0000000000PPPP";
//	char buf[1024];
	char rbuf[MAX_SIZE];
	char destbuf[MAX_SIZE];
	struct timeval start, end;

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
//	printf("Ready for Client Data to Read : \n");
	gettimeofday(&start, NULL);
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
			printf("2.err:%d: rbuf=%s\n",err, rbuf);
			ret += err;
//		sleep(1);
		}
#endif
        gettimeofday(&end, NULL );
        int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
        printf("time: %d us\n", timeuse);
		
	buf[MAX_SIZE]='\0';
	destbuf[MAX_SIZE]='\0';
	err = ret;
		ret = strcmp(buf, destbuf);
//	printf("3.ret:%d: destbuf=%s\n",ret, destbuf);
//	printf("-->%s\n", buf);
//	printf("++>%s\n", destbuf);
		if (ret == 0){
			success ++;
		}else{
			fail ++;
			//printf("tty result: success=%d, fail=%d\n", success, fail);
		}
			printf("tty result: success=%d, fail=%d\n", success, fail);
                ret=write(fd, destbuf, err); //server write
	//printf("Write back Data to Client : \n");
        }


        close(fd);
        return 0;
}
