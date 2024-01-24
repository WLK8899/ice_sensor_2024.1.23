#include <Arduino.h>

#define FILTER_N 100
const int A_force_pin = 14;
const int B_force_pin = 12;
const int C_force_pin = 11;
const int D_force_pin = 10;

int A_val_arr_0[FILTER_N];
int A_force_val;

int B_val_arr_0[FILTER_N];
int B_force_val;

int C_val_arr_0[FILTER_N];
int C_force_val;

int D_val_arr_0[FILTER_N];
int D_force_val;

int all_aver_value=0;
// 中位值平均滤波法（又称防脉冲干扰平均滤波法）（算法1）
int FilterMid(int *arr)
{
  int i, j;
  int filter_temp, filter_sum = 0;
  int filter_buf[FILTER_N];
  for (i = 0; i < FILTER_N; i++)
  {
    filter_buf[i] = arr[i];
    delay(1);
  }
  // 采样值从小到大排列（冒泡法）
  for (j = 0; j < FILTER_N - 1; j++)
  {
    for (i = 0; i < FILTER_N - 1 - j; i++)
    {
      if (filter_buf[i] > filter_buf[i + 1])
      {
        filter_temp = filter_buf[i];
        filter_buf[i] = filter_buf[i + 1];
        filter_buf[i + 1] = filter_temp;
      }
    }
  }
  // 去除最大最小极值后求平均
  for (i = 1; i < FILTER_N - 1; i++)
    filter_sum += filter_buf[i];
  return filter_sum / (FILTER_N - 2);
}

// 递推平均滤波法（又称滑动平均滤波法）

int filter_buf[FILTER_N + 1] = {0};
int FilterAver(int value)
{
  int i;
  int filter_sum = 0;
  filter_buf[FILTER_N] = value;
  for (i = 0; i < FILTER_N; i++)
  {
    filter_buf[i] = filter_buf[i + 1]; // 所有数据左移，低位仍掉
    filter_sum += filter_buf[i];
  }
  return (int)(filter_sum / FILTER_N + 2);
}

// 抖动滤波  预测值，实际值，最大差值
int32_t FilterJitter(int32_t x, int32_t oldy, int32_t threshold)
{
  int32_t result;

  result = x - oldy;
  if (result >= threshold)
  {
    return x - threshold;
  }
  else if (result <= -threshold)
  {
    return x + threshold;
  }

  return oldy;
}

// // 用于随机产生一个300左右的当前值
// int Get_AD() {
//   return random(295, 305);
// }
//  int Filter_Value;
// int Value;
// // 限幅消抖滤波法
// #define FILTER_A 1
// #define FILTER_N 5
// int i = 0;
// int Filter() {
//   int NewValue;
//   int new_value;
//   NewValue = Get_AD();
//   if(((NewValue - Value) > FILTER_A) || ((Value - NewValue) > FILTER_A))
//     new_value = Value;
//   else
//     new_value = NewValue;
//   if(Value != new_value) {
//     i++;
//     if(i > FILTER_N) {
//       i = 0;
//       Value = new_value;
//     }
//   }
//   else
//     i = 0;
//   return Value;
// }
int val(const int pin, int *arr)
{
  for (int i = 0; i < FILTER_N; i++)
  {
    arr[i] = analogRead(pin);
  }
  // 中位值平均滤波法
  return FilterMid(arr);
}
void setup()
{
  // Serial Port begin
  Serial.begin(115200);
  // Define inputs and outputs
  pinMode(A_force_pin, INPUT);
  pinMode(B_force_pin, INPUT);
  pinMode(C_force_pin, INPUT);
  pinMode(D_force_pin, INPUT);
}

void loop()
{
  A_force_val = val(A_force_pin, A_val_arr_0);
  B_force_val = val(B_force_pin, B_val_arr_0);
  C_force_val = val(C_force_pin, C_val_arr_0);
  D_force_val = val(D_force_pin, D_val_arr_0);

  all_aver_value=(A_force_val +B_force_val+ C_force_val+ D_force_val)/4;
  Serial.print("all_aver_value: ");
  Serial.println(all_aver_value);

  Serial.print("A_force_value: ");
  Serial.println(A_force_val);

  Serial.print("B_force_value: ");
  Serial.println(B_force_val);

  Serial.print("C_force_value: ");
  Serial.println(C_force_val);

  Serial.print("D_force_value: ");
  Serial.println(D_force_val);



  delay(100);
}