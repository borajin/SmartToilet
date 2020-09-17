#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include <wiringPi.h>

#include <pthread.h>	//-lpthread

#include <unistd.h>

#include <signal.h> 

#include <arpa/inet.h>

#include <sys/types.h>

#include <sys/socket.h>

 

#define MOTION 0	//��ü��������

#define COLLISION 3	//��������

#define WaterPump 21	//��������

#define FAN 22		//��

#define LEDBar 26	//LED

 

#define RGBLEDPOWER 24

#define RED 8

 

unsigned char humandetect = 0;	//��ü�����Ǹ� 0 �ƴϸ� 1

unsigned char sit = 0;		//���⿡ �ɾ����� 1 �ƴϸ� 0

 

int startSmartToliet(void);

int light(void);

int toilet(void);

 

//ctrl+c - end

void sig_handler(int signo) {

	printf("process stop\n");

	digitalWrite(COLLISION, 0);

	digitalWrite(FAN, 0);

	digitalWrite(LEDBar, 0);

	digitalWrite(RGBLEDPOWER , 0);

	digitalWrite(RED , 0);

	

	exit(0);

}

 

//��ü�������� ���ͷ�Ʈ �߻��� ��ü���� �÷��׸� 1�� ����

void myInterrupt(void) {

	humandetect =  1;

}

 

void clean_up(void *arg) {

	printf("thread end\n");

}

 

void error_handling(char *message) {

	fputs(message, stderr);

	fputc('\n', stderr);

	exit(1);

}

 

void timeOver() {

	pinMode(RGBLEDPOWER, OUTPUT);

	

	digitalWrite(RGBLEDPOWER, 1);

	digitalWrite(RED, 1);

	

	delay(1000);

	

	int sock;

	struct sockaddr_in serv_addr;

	int str_len;

	

	sock = socket(PF_INET, SOCK_STREAM, 0);

	if(sock == -1) error_handling("socket() error\n");

	

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	serv_addr.sin_addr.s_addr = inet_addr("192.168.51.91");

	serv_addr.sin_port = htons(atoi("8888"));

	

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)

		error_handling("connect() error");

	

	char message[1024] = "[Smart Toilet] The user's toilet usage time has exceeded 60 minutes.";

	write(sock, message, strlen(message));

	printf("send emergancy message\n");

	

	close(sock);

}

 

void *t_timerTread() {

	static int retval = 999;

	int timerLimit = 15;

	int timer;

	

	char *mydata;

	mydata = (char *)malloc(1000);

	

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	pthread_cleanup_push(clean_up, (void *)mydata);

	

	printf("timer start\n");

	while(1) {

		if(timer < timerLimit) {

			printf("count... %d\n", timer);

			timer++;

			sleep(1);

		} else {

			printf("time over");

			timeOver();

			pthread_exit((void*)&retval);

			break;

		}

	}

	pthread_cleanup_pop(0);

}

 

int main(void) {

	signal(SIGINT, (void *)sig_handler);

	

	int i = startSmartToliet();

 

	return i;

}

 

 

int startSmartToliet() {

	int plag;	//� ��Ȳ ������ ������

	int result;	//��Ȳ ���(������ 1��ȯ�ϵ��� �Ǿ�����)

 

	while(1) { 

		printf("1.light 2.toilet 0.end program\n");

		scanf("%d", &plag);

 

		switch(plag) {

			case 1:

				result = light();

				break;

			case 2:

				result = toilet();

				break;

			case 0:

				return 0;

				break;

			default :

				printf("wrong index");

				break;

		}

	}

}

 

//����� ������ �� ������ ������ ������ ���

int light() {	

	if(wiringPiSetup () == -1) return 1;

	if(wiringPiISR(MOTION, INT_EDGE_RISING, &myInterrupt) < 0) {

		printf("Unable to setup ISR\n");

		return 1;

	}

	pinMode(FAN, OUTPUT);

	pinMode(LEDBar, OUTPUT);

 

	while(1){

		printf("%d\n", humandetect);

		if(humandetect == 1) {

			//����� �ִٸ� �Ұ� �� on

			digitalWrite(LEDBar, 1);

			digitalWrite(FAN, 1);

 

			delay(3000);

		} 

		else if(humandetect == 0) {

			//����� ���ٰ� ǥ�õǸ� �ϴ� 2�� ��ٸ��� ��˻�.

			//�׷����� ���ٰ� ǥ�õǸ� led�� fan ����.

			delay(2000);

 

			digitalRead(MOTION);

			if(humandetect == 1) continue;	//��˻�� �ִٰ� ǥ�õǸ� �� ���� �Ѿ����

 

			digitalWrite(LEDBar, 0);

			digitalWrite(FAN, 0);

		}

 

		//����� ���ٰ� �����Ǹ� ��� ���ٰ� ǥ��...

		if(digitalRead(MOTION) == LOW) humandetect = 0;

		

		delay(1000);

	}

	return 0;

}

 

//���� �ڵ� ���������� ���

int toilet() {	

	pthread_t t_timer;

	void *tret = NULL;

	int timerOn = 0;

	

	if(wiringPiSetup () == -1) return 1;

	if(wiringPiISR(MOTION, INT_EDGE_RISING, &myInterrupt) < 0) {

		printf("Unable to setup ISR\n");

		return 1;

	}

	pinMode(COLLISION, INPUT);

 

	while(1) {

		if(digitalRead(COLLISION) == LOW)  {

			if(humandetect == 1) {

				sit = 1;

				

				if(timerOn == 0) {

					timerOn = 1;

					if(pthread_create(&t_timer, NULL, t_timerTread, NULL)< 0) {

						perror("thread create error: ");

						exit(0);

					}

				}

			}

		} else {

			if(sit == 1) {

				delay(1000); 

				digitalWrite(WaterPump, 1);

				delay(3000); 

 

				digitalWrite(WaterPump, 0); 

 

				pthread_cancel(t_timer);

				humandetect = 0;

				sit = 0;

				timerOn = 0;

			} else {

				humandetect = 0;

			}

		}

		

		if(humandetect == 0) {

			delay(2000);

			digitalRead(MOTION);

			if(humandetect == 1) {

 

				delay(1000); 

				continue;

			}

		delay(1000);

	}

	

}

return 0;

}