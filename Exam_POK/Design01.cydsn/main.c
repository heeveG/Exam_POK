#include "project.h"
#include "cons.h"
#include "stdio.h"

VOLTAGE_ENUM curr_vol[4];

// Array for input in UART
char output[300];

// Main array of combinations
int a = 0, b = 0, c = 0, d = 0;
int* arr[4] = {&a, &b, &c, &d};

// To test hold
int a_hold = 0, b_hold = 0, c_hold = 0, d_hold = 0;
int *arr_hold[] = {&a_hold, &b_hold, &c_hold, &d_hold};

// For left right shifts
int l = 0, r = 0;
int* arr_lr[2] = {&l, &r};


void to_binary(int num, int* arr[], int n){
    
    
    for (int i = n - 1; i >= 0; i--){
        *arr[i] = num%2;
        num = num / 2;
    }
        
}


void set_S_pins(VOLTAGE_ENUM s0, VOLTAGE_ENUM s1){

    Pin_S0_Write(s0);
    Pin_S1_Write(s1);
    CyDelay(10);
    
}

void load(VOLTAGE_ENUM a, VOLTAGE_ENUM b, VOLTAGE_ENUM c, VOLTAGE_ENUM d){
    
    Pin_CL_Write(LOW);
    
    
    Pin_A_OUT_Write(a);
    Pin_B_OUT_Write(b);
    Pin_C_OUT_Write(c);
    Pin_D_OUT_Write(d);
    
    Pin_CL_Write(HIGH);
    
    curr_vol[0] = a;
    curr_vol[1] = b;
    curr_vol[2] = c;
    curr_vol[3] = d;
}


void shift_left(VOLTAGE_ENUM dsl){

    Pin_CL_Write(LOW);
    set_S_pins(LOW, HIGH);
    Pin_SHIFT_L_Write(dsl);
    CyDelay(1);
    Pin_CL_Write(HIGH);
    
    curr_vol[0] = curr_vol[1];
    curr_vol[1] = curr_vol[2];
    curr_vol[2] = curr_vol[3];
    curr_vol[3] = dsl;
    
}

void shift_right(VOLTAGE_ENUM dsr){

    Pin_CL_Write(LOW);
    set_S_pins(HIGH, LOW);
    Pin_SHIFT_R_Write(dsr);
    CyDelay(1);
    Pin_CL_Write(HIGH);
    
    curr_vol[3] = curr_vol[2];
    curr_vol[2] = curr_vol[1];
    curr_vol[1] = curr_vol[0];
    curr_vol[0] = dsr;
    
}


int test_load(){
    // Test Load
    for (int i = 0; i < 16; i++){
        to_binary(i, arr,4);
        for (int j = 0; j < 4; j++){
            to_binary(j, arr_lr, 2);
            Pin_SHIFT_L_Write(l);
            Pin_SHIFT_R_Write(r);
            set_S_pins(HIGH, HIGH);
            load(a, b, c, d);
            if (!((Pin_INPUT_A_Read() == curr_vol[0]) & (Pin_INPUT_B_Read() == curr_vol[1]) & (Pin_INPUT_C_Read() == curr_vol[2]) & (Pin_INPUT_D_Read() == curr_vol[3]))){         
                return 0;
            }   
        }
    }
    return 1;
}

int test_shift_left(){
    // Test shift left
    for (int i = 0; i < 16; i++){
        to_binary(i, arr, 4);
        set_S_pins(HIGH, HIGH);
        load(a, b, c, d);
        for (int j = 0; j < 2; j++){
            Pin_SHIFT_L_Write(j);
            shift_left(LOW);
            if (!((Pin_INPUT_A_Read() == curr_vol[0]) & (Pin_INPUT_B_Read() == curr_vol[1]) & (Pin_INPUT_C_Read() == curr_vol[2]) & (Pin_INPUT_D_Read() == curr_vol[3]))){         
                return 0;
            }
            
            shift_left(HIGH);
            if (!((Pin_INPUT_A_Read() == curr_vol[0]) & (Pin_INPUT_B_Read() == curr_vol[1]) & (Pin_INPUT_C_Read() == curr_vol[2]) & (Pin_INPUT_D_Read() == curr_vol[3]))){
                return 0;
            }
        }
    }
    return 1;
}

int test_shift_right(){

    // Test shift right
    for (int i = 0; i < 16; i++){
        to_binary(i, arr, 4);
        set_S_pins(HIGH, HIGH);
        load(a, b, c, d);
        
        for (int j = 0; j < 2; j++){
            Pin_SHIFT_R_Write(j);
            shift_right(HIGH);
            if (!((Pin_INPUT_A_Read() == curr_vol[0]) & (Pin_INPUT_B_Read() == curr_vol[1]) & (Pin_INPUT_C_Read() == curr_vol[2]) & (Pin_INPUT_D_Read() == curr_vol[3]))){
                return 0;
            }
            
            shift_right(LOW);
            if (!((Pin_INPUT_A_Read() == curr_vol[0]) & (Pin_INPUT_B_Read() == curr_vol[1]) & (Pin_INPUT_C_Read() == curr_vol[2]) & (Pin_INPUT_D_Read() == curr_vol[3]))){
                return 0;
            }
        }
    }
    return 1;
}

int test_hold(){
    // Test Hold 
    for (int i = 0; i < 16; i++){
        to_binary(i, arr, 4);
        set_S_pins(HIGH, HIGH);
        load(a, b, c, d);
        
        
        for (int j = 0; j < 16; j++){
            to_binary(j, arr_hold, 4);
            for (int j = 0; j < 4; j++){
                to_binary(j, arr_lr, 2);
                Pin_SHIFT_L_Write(l);
                Pin_SHIFT_R_Write(r);
                set_S_pins(LOW, LOW);
                load(a_hold, b_hold, c_hold, d_hold);
                if (!((Pin_INPUT_A_Read() == a) & (Pin_INPUT_B_Read() == b) & (Pin_INPUT_C_Read() == c) & (Pin_INPUT_D_Read() == d))){
                    return 0;
                    
                }
            }
        }
    }
    return 1;
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    UART_1_Start(); 

    if (!(test_load() & test_shift_left() & test_shift_right() & test_hold())){
        sprintf(output,  "Error\n");
        UART_1_UartPutString(output);
        while(1);
    }
    else{
        sprintf(output,  "Register works correctly\n");
        UART_1_UartPutString(output);
        Pin_TRUE_Write(HIGH);
    }
}







     






