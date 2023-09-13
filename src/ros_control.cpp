//#include "ros_control.h"

//#define RCCHECK1(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop1();}}
/*
void error_loop1(){
  while(1){
    digitalWrite(13, !digitalRead(13));
    delay(100);
  }
}

RosControl::RosControl(        
    rcl_service_t srv,
    rclc_executor_t exc,
    rclc_support_t sup,
    rcl_allocator_t allc,
    rcl_node_t node,
    example_interfaces__srv__AddTwoInts_Request req,
    example_interfaces__srv__AddTwoInts_Response res,
    void (*callback)(const void *, void *)
)
{
    srv = srv;
    exc = exc;
    _callback = callback;
    set_microros_serial_transports(Serial);
  //set_microros_serial_transports(Serial);
    
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);  

    delay(2000);

    allc = rcl_get_default_allocator();
    rclc_support_init(&sup, 0, NULL, &allc);
    rclc_node_init_default(&node, "micro_ros_teensy_hexapod", "", &sup);

    rclc_service_init_default(
    &srv,
    &node,
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts),
    "teensy_hexapod_service");

    rclc_executor_init(&exc, &sup.context, 1, &allc);
    rclc_executor_add_service(&exc, &srv, &req, &res, _callback);
}

// void RosControl::service_callback(const void *req, void *res){
//     for(int i = 0; i < 20; i++){
//       digitalWrite(13, !digitalRead(13));
//       delay(100);
//   }
//   example_interfaces__srv__AddTwoInts_Request * req_in = (example_interfaces__srv__AddTwoInts_Request *)req;
//   example_interfaces__srv__AddTwoInts_Response * res_in = (example_interfaces__srv__AddTwoInts_Response *)res;
//     // 计算sum
//   printf("Client requested sum of %d and %d.\n", (int)req_in->a, (int)req_in->b);
//   res_in->sum = req_in->a + req_in->b;  //当进入回调函数时会出现
//   //  srv_req = (example_interfaces__srv__AddTwoInts_Request *)req;
//   //  srv_res = (example_interfaces__srv__AddTwoInts_Response *)res;
//   // // 计算sum
//   //  srv_res->sum = srv_req->a + srv_req->b;
//   //  return;
// }

void RosControl::process(){
    RCCHECK1(rclc_executor_spin_some(&exc, RCL_MS_TO_NS(100)));
    // RCCHECK(rclc_executor_spin(&exc));
    return;
}

int RosControl::getInfo1(){
  return int(srv_req->a);
}

int RosControl::getInfo2(){
  return int(srv_req->b); 
}

*/