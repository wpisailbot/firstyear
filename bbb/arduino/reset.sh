#!/bin/bash
GPIO=112
GPIO_DIR=/sys/class/gpio
GPIO_SUB_DIR=${GPIO_DIR}/gpio${GPIO}
GPIO_VAL=${GPIO_SUB_DIR}/value
echo ${GPIO} > ${GPIO_DIR}/export
sleep 0.5
echo out > ${GPIO_SUB_DIR}/direction
echo 0 > ${GPIO_VAL}
sleep 1
echo 1 > ${GPIO_VAL}
