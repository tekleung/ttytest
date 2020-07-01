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

#define FALSE 	0
#define TRUE 	1

int speed_arr[] = {B38400, B19200, B9600, B4800, B2400, B1200, B600, B230400, B115200, B57600, B1800, };
int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 600, 230400, 115200, 57600, 1800, };

//char buf[1024] = "aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff1111111111222222222233333333334444444444"
//				"==========0000000000PPPP";
char buf[6] = "##a$$";

//********设置波特率********//
int Set_Speed(int fd, int speed)
{
	int i, status;
	struct termios Opt;
	
	tcgetattr(fd, &Opt);
	for(i= 0; i < (int)(sizeof(speed_arr) / sizeof(int)); i++)
	{
		if(speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if(status != 0)
			{
				perror("tcsetattr fd1");
		 		return -1;
		 	}
		 	return 0;
		}
		tcflush(fd, TCIOFLUSH);
	}
	return -1;
}

//********设置奇偶检验********//
int Set_Parity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;
	
	if(tcgetattr(fd, &options) != 0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}
	options.c_oflag &= ~OPOST;
	options.c_cflag &= ~CSIZE;
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	/**设置数据位数**/
	switch(databits)
	{
	  	case 7:
	  		options.c_cflag |= CS7;
	  		break;
	  	case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n");
			return (FALSE);
	}
	/**设置奇偶校验**/
	switch(parity)
  	{
	  	case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;				/* Clear parity enable */
		//	options.c_iflag &= ~INPCK;				/* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);	/* 设置为奇效验*/
			options.c_iflag |= INPCK;				/* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;				/* Enable parity */
			options.c_cflag &= ~PARODD;				/* 转换为偶效验*/
			options.c_iflag |= INPCK;				/* Disnable parity checking */
			break;
		case 'S':
		case 's':									/*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported parity\n");
			return (FALSE);
	}
	/**设置停止位**/
	switch(stopbits)
  	{
	  	case 1:
	  		options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return (FALSE);
	}
	/**使能硬件流控**/
	options.c_cflag &= ~CRTSCTS;
	/**设置read函数**/
    options.c_cc[VTIME] = 10;	//设置等待超时时间　1s
    options.c_cc[VMIN] = 1;		//最低字节读数据

    options.c_iflag &= ~(ICRNL | IXON);
	options.c_cflag |= CLOCAL|CREAD;

	tcflush(fd, TCIFLUSH);		/**Update the options and do it NOW**/
	if(tcsetattr(fd, TCSANOW,&options) != 0)
  	{
  		perror("SetupSerial 3");
		return (FALSE);
	}
	return (TRUE);
}

//********打开设备********//
int Open_Dev(char *Dev)
{
 	int fd;
 	fd = open(Dev, O_RDWR | O_NOCTTY | O_NDELAY);
 	if(fd == -1)
	{
		return -1;
	}

    if(fcntl(fd, F_SETFL, 0) < 0)
		printf("fail\n");
    else
		printf("success\n");

 	return fd;
}

//********客户端函数********//
int Client(int fd, int length)
{
	int ret, sum, times, success=0, fail=0;
	//struct termios opts;
	unsigned char wbuf[length];
	unsigned char rbuf[length];
	unsigned char destbuf[length];

	memcpy(wbuf, buf, length);
	while(1)
	{     
	  	//usleep(500000);
	    sum = 0;
	    times = 0;
		memset(rbuf, 0, length);
		memset(destbuf, 0, length);
		ret = write(fd, wbuf, length); //写1k的数据
		
		while(1)//循环分包接收1k的数据
		{
			memset(rbuf, 0, length);
			ret = read(fd, rbuf, length-sum);
            //usleep(1000);
	  		if((sum>=length) || (ret<=0) || (times>=100))
				break;
			if(ret > 0){
				memcpy(destbuf+sum, rbuf, ret);
				sum = sum + ret;
				printf("rec>0: ret1=%d	rbuf=%s\n", ret, rbuf);
			}else
			{
			    printf("Error: Read the equipment is empty or the return value is less than 0!!\n");
			}
			times++;
			//ret=0;
		}
			
		//ret = strcmp(wbuf, destbuf);//对比发送和接收的数据是否一致
        ret = memcmp(wbuf, destbuf, length);
		if(ret == 0){
			success ++;
		}else{
			fail ++;
		}
		printf("Result: success=%d, fail=%d, sum:%d, times:%d\n", success, fail, sum, times);
		//usleep(1000);
    }

	return 0;
}

//********服务器函数********//
int Server(int fd, int length)
{
	int ret, success=0, fail=0, sum, times;
	//struct termios opts;
	unsigned char wbuf[length];
	unsigned char rbuf[length];
	unsigned char destbuf[length];

	memcpy(wbuf, buf, length);
	while(1)
	{     
	  	//usleep(500000);
	    sum=0;
	    times=0;
		memset(rbuf, 0, length);
		memset(destbuf, 0, length);
	
		while(1)//循环分包接收1k的数据
		{
			memset(rbuf, 0, length);
			ret = read(fd, rbuf, length-sum);
            //usleep(1000);
	  		if((sum>=length) || (ret<=0) || (times>=100))
				break;
			if(ret>0){
				memcpy(destbuf+sum, rbuf, ret);
				sum = sum + ret;
				printf("rec>0: ret1=%d	rbuf=%s\n", ret, rbuf);
			}else
			{
			    printf("Error: Read the equipment is empty or the return value is less than 0!!\n");
			}
			times++;
		}
		
		//ret = strcmp(wbuf, destbuf);//对比发送和接收的数据是否一致
        ret=memcmp(wbuf, destbuf, length);
		if(ret == 0){
			success ++;
		}else{
			fail ++;
		}
		printf("Result: success=%d, fail=%d, sum:%d, times:%d\n", success, fail, sum, times);
		
		//usleep(1000);
		ret=write(fd, wbuf, length); //写1k的数据
    }
    
	return 0;
}

int main(int argc,char **argv)
{
	int fd, speed, length;
	char *file_name = NULL;
	
	if(argc <= 4)
	{
		printf("Error: The parameteres need to be 4, but now is %d\n", argc-1);
		printf("Usage: ./tty_test [c/s Type] [Address] [Baud rate] [Data size]. Ex.: ./tty_test c /dev/ttysWK1 115200 1024 !\n");
		return -1;
	}
	
	if(atoi(argv[4])>1024 || atoi(argv[4])<=0)
	{
		printf("Error: The fourth parameter need to be >0 and <=1024, but now is %s\n", argv[4]);
		printf("Usage: ./tty_test [c/s Type] [Address] [Baud rate] [Data size]. Ex.: ./tty_test c /dev/ttysWK1 115200 1024 !\n");
		return -1;	
	}
	length = atoi(argv[4]);
	
	file_name = argv[2];
	fd = Open_Dev(file_name);
	if(fd < 0)
	{
		printf("Error: Can't Open Serial Port! Please enter the correct file name.\n");
		printf("Usage: ./tty_test [c/s Type] [Address] [Baud rate] [Data size]. Ex.: ./tty_test c /dev/ttysWK1 115200 1024 !\n");
		return -1;
 	}

    speed = atoi(argv[3]);
    if(Set_Speed(fd, speed))
    {
    	printf("Error: Can't set Serial Baud rate! Please enter the correct baud rate or modify the code to add.\n");
		printf("Usage: ./tty_test [c/s Type] [Address] [Baud rate] [Data size]. Ex.: ./tty_test c /dev/ttysWK1 115200 1024 !\n");
		return -1;
    }
    
    if(Set_Parity(fd,8,1,'s') == FALSE)
    {
        printf("Error: Set Parity Error\n");
        return -1;
    }
    
    if(!strcmp(argv[1], "c"))
	{
		Client(fd, length);
	}else if(!strcmp(argv[1], "s"))
	{
		Server(fd, length);
	}else
	{
		printf("Error: The first parameter need to be fill in the 'c' or 's', but now is %s\n", argv[1]);
		printf("Usage: ./tty_test [c/s Type] [Address] [Baud rate] [Data size]. Ex.: ./tty_test c /dev/ttysWK1 115200 1024 !\n");
		return -1;
	}
	
    close(fd);
	return 0;
}

