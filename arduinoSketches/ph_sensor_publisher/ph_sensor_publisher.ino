#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>

#include <std_msgs/msg/float32.h>

#define SensorPin 25 //Analog pin for PH sensor
#define Offset 0.00 // for calibration
#define samplingInterval 20 //sampling interval in ms
#define ArrayLength 40 //length of sample for averaging
#define LED_PIN 13 // error indicator led

int pHArray[ArrayLength];
int pHArrayIndex=0;


rcl_publisher_t publisher;
std_msgs__msg__Float32 msg;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}


double avergearray(int* arr, int number){
  int i,max,min;
  double avg;
  long amount=0;

  if(number<=0){
    Serial.println("Error: Invalid array size for averaging!!/n");
    error_loop();
    return 0;
  }

  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}



void setup() {
  
  Serial.begin(115200);
  delay(500);


  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);

  Serial.println("Initializing pH Sensor...");


  set_microros_transports();

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "ph_sensor_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "ph_value"));

  msg.data = 0;
 
 }


void loop() {

  delay(100);
  

  static unsigned long samplingTime = millis();
  static float pHValue,voltage;

  if(millis()-samplingTime > samplingInterval)
  {
    pHArray[pHArrayIndex++]=analogRead(SensorPin);

    if(pHArrayIndex==ArrayLength)pHArrayIndex=0;

    voltage = avergearray(pHArray, ArrayLength)*3.3/4095;
    pHValue = 3.5*voltage+Offset;

    // // Debug output
    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("pH Value: ");
    Serial.println(pHValue);

    msg.data= pHValue;
    
    RCSOFTCHECK(rcl_publish(&publisher,&msg, NULL));


    samplingTime=millis();
 }
}



