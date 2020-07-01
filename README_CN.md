# tty测试
用于测试串口：回环测试、客户端与服务器之间的收发数据。

### 编译
将`tty_test/`文件夹放置SDK的`external/`目录下，执行：

	mmm external/tty_test
    
生成PC的可执行文件，在`external/tty_test`执行：

	gcc tty_test.c -o tty_test

### 执行
生成的可执行文件需要输入三个参数执行
 
 	./tty_test 参数一　参数二　参数三	参数四
     例：./tty_test s /dev/ttyUSB1 115200　256
**参数一：类型**
 
 	c:客户端	s:服务器
**参数二：设备**

	/dev/ttysWK0	/dev/ttysWK1	/dev/ttyUSB0	/dev/ttyUSB1	...	
**参数三：波特率**

	4800	9600	115200	230400	...
**参数四：数据大小**

	256	512	...	1024		
如果是两台机器之间进行收发数据，请先在作为服务器的机器中运行服务器程序，再在另一台机器执行客户端程序。

### 回环测试
进行回环测试需链接tx与rx，形成自发自收。并执行客户端的可执行程序。例：

	./tty_test c /dev/ttysWK1 115200　1024
