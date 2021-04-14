
/* Example application of Columbo Simple Serial Library
 * Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cssl.h"
#define FRAMELENGTH 200 //定义数据帧长度 字节
// using namespace std;
/* if it is time to finish */
static int finished = 0;
static uint8_t pos = 0;
uint8_t data[FRAMELENGTH]; //
static int data_already = 0;
int head_rmc = 0, head_ddmmyy = 0, head_lf = 0;
/* example callback, it gets its id, buffer, and buffer length */
static void callback(int id, uint8_t *buf, int length) {
  int i;
  // printf("length:%d\n",length);
  printf("length:%d\n", length);
  //如果数据帧不是一次接收到的
  //则进行下面的处理，数据包的长度为32
  for (i = 0; i < length; i++) {
    data[i] = buf[i];
    // data[i+pos] = buf[i];
    // printf("%c",data[i]);
  }
  for (int j = 0; j < length; ++j) {
    if (data[j] == 0x56) {        // check for GPRMC location status
      printf("invalid data!!\n"); // A(0x41) for valid , V(0x56) for
                                  // invalid
    }
    if (data[j] == 0x24 && data[j + 1] == 0x47 && data[j + 2] == 0x50 &&
        data[j + 3] == 0x52 && data[j + 4] == 0x4d && data[j + 5] == 0x43) {
      head_rmc = j;
      printf("GPRMC data find!!\n");
      //$GPRMC finded
    }
    uint8_t temp = 0;
    for (int k = head_rmc; k < (head_rmc + 100); ++k) {
      if (data[k] == 0x2c) {
        temp++;
      }
      if (temp == 12) {
        head_ddmmyy = k + 1;
        for (int l = head_ddmmyy; l < (head_ddmmyy + 50); l++) {
          if (data[l] == 0x0a) {
            head_lf = l;
            break;
          }
        }
        break;
      }
    }
  }
  data_already = 1;

  //记录位置
  // pos += length;
  //收到既定的一帧
  // if(pos>=FRAMELENGTH){
  //	pos = 0;
  //	data_already = 1;
  //}
  //进行处理
  //   if (data_already == 0) { //

  //     for (i = 0; i < length; i++)
  //       printf("%c", data[i]);
  //     memset(data, 0, FRAMELENGTH);
  //     // data_already = 0 ;//数据包准备好信号置零
  //     printf("\n");
  //   }

  // printf("\n");
  // fflush(stdout);
}

int main(int argc, char **argv) {
  cssl_t *serial;
  my_cssl myserial;
  memset(data, 0, sizeof(data));
  myserial.cssl_start();
  // test
  //	printf("%d\n",1234^5678);
  serial = myserial.cssl_open("/dev/ttyTHS1", callback, 0, 9600, 8, 0, 1);

  if (!serial) {
    printf("%s\n", myserial.cssl_geterrormsg());

    return -1;
  }

  //   myserial.cssl_putstring(serial,"Type some data, ^D finishes.");
  myserial.cssl_putstring(serial, "Sir this is a serial testing programer.");

  while (true) {
    if (data_already == 1) {
      myserial.cssl_putdata(serial, &data[head_rmc], (head_lf - head_rmc));
      printf("%d\n", head_rmc);
      printf("%d\n", head_lf);
      //   for (int i = head_rmc; i < (head_lf - head_rmc + 1); i++) {
      //     printf("%02x\n", data[i]);
      //   }

      printf("data send success!!\n");
      memset(data, 0, FRAMELENGTH);
      data_already = 0;
      head_rmc = 0;
      head_ddmmyy = 0;
      head_lf = 0;
    }
  }
  // pause();

  printf("\n^D - we exit\n");

  myserial.cssl_close(serial);
  myserial.cssl_stop();

  return 0;
}
