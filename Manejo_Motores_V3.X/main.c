#include <18F4550.h>
#use delay(clock=20000000) //Frecuencia del cristal
#fuses hs, noprotect, nolvp, nowdt 
#use standard_io(B) 

#define HORARIO 1
#define ANTIHORARIO 0

#define MOTOR1 1
#define MOTOR2 2
#define MOTOR3 3
#define MOTOR4 4
#define MOTOR5 5
#define MOTOR6 6

/*--------------- ASIGNACION DE PINES -------------------*/

int MOTORES_ENABLE_PIN = PIN_A0;

int MOTOR1_STEP_PIN = PIN_A1;
int MOTOR1_DIR_PIN = PIN_A2;

int MOTOR2_STEP_PIN = PIN_A3;
int MOTOR2_DIR_PIN = PIN_A4;
//
//int MOTOR3_STEP_PIN = PIN_B0;
//int MOTOR3_DIR_PIN = PIN_B1;
//
//int MOTOR4_STEP_PIN = PIN_B0;
//int MOTOR4_DIR_PIN = PIN_B1;
//
//int MOTOR5_STEP_PIN = PIN_B0;
//int MOTOR5_DIR_PIN = PIN_B1;
//
//int MOTOR6_STEP_PIN = PIN_B0;
//int MOTOR6_DIR_PIN = PIN_B1;

/*------------------- VARIABLES -----------------------*/

int flag_inicio = 0;
int32 cont_pasos = 0;
int16 cont_vel = 0;

//Funcion de mapeado:

int16 map(int16 valor, int16 fS, int16 fE, int16 tS, int16 tE) {
    return ((long)(valor - fS) * (long)(tE - tS)) / ((fE - fS) + tS);
}

//Funcion para el control de los motores.
// * numero_motor: motor que se desea mover. Puede ser un valor del 1 al 6.
// * sent: sentido de giro del motor. 0 -> antihorario; 1 -> horario.
// * pos_ang: posicion en angulo del motor.
// * vel: velocidad del giro del motor en porcentaje. Los valores puden ser del 0% al 100%
// * micro_step: micro pasos seteados para el motor (1, 2, 4, 8, 16, 32)

void controlMotor(int numero_motor, int sent, int16 pos_ang, int16 vel, int micro_step) {
    
    int16 vel_delay;
    int16 angulo_x_paso;
    int32 cant_pasos;
    int motor;
    
    switch(numero_motor){
    
        case MOTOR1:
            if(!sent) output_low(MOTOR1_DIR_PIN); 
            else output_high(MOTOR1_DIR_PIN);
            motor = MOTOR1_STEP_PIN;
            break;
            
        case MOTOR2:
            if(!sent) output_low(MOTOR2_DIR_PIN);
            else output_high(MOTOR2_DIR_PIN);
            motor = MOTOR2_STEP_PIN;
            break;
            
        case MOTOR3:
//            if(!sent) output_low(MOTOR3_DIR_PIN);
//            else output_high(MOTOR3_DIR_PIN);
            break;
            
        case MOTOR4:
//            if(!sent) output_low(MOTOR4_DIR_PIN);
//            else output_high(MOTOR4_DIR_PIN);
            break;
            
        case MOTOR5:
//            if(!sent) output_low(MOTOR5_DIR_PIN);
//            else output_high(MOTOR5_DIR_PIN);
            break;
            
        case MOTOR6:
//            if(!sent) output_low(MOTOR6_DIR_PIN);
//            else output_high(MOTOR6_DIR_PIN);
            break;
        
    }
    
    angulo_x_paso = 1800 / micro_step;
    cant_pasos = ((pos_ang * 100) / angulo_x_paso) * 20;
    vel_delay = map(vel, 0, 100, 600, 0); //660 max
    
    if(cont_vel >= vel_delay){
        output_toggle(motor);
        cont_vel = 0;
        cont_pasos++;
    }
    
    if (cont_pasos >= cant_pasos) {
        flag_inicio = 0;
        cont_pasos = 0;
    }
}

#int_TIMER0

void TIMER0_isr(void) {
    
    if (flag_inicio == 1) cont_vel++; //Variable para temporizar
    else cont_vel = 0;
    
    set_timer0(217); //Se recarga el timer0. Desborda cada 1ms
    
    //set_timer0(208) y RTCC_DIV_4 para 200 us
    
} 

void main() {
    
    int motor_dir = ANTIHORARIO;
    int16 motor_ang = 5;
    int16 motor_vel = 90;
    int ustep = 8;
    
    int flag_boton1 = 0;
    int flag_boton2 = 0;
    int flag_boton3 = 0;
    int flag_boton4 = 0;
    
    setup_timer_0(RTCC_8_BIT | RTCC_DIV_32); //Configuración timer0
    set_timer0(217); //Carga del timer0. Desborda cada 1ms
    enable_interrupts(INT_TIMER0); //Habilita interrupción timer0
    enable_interrupts(GLOBAL); //Habilita interrupción general
    
    while(TRUE) {
        
        if(!input(PIN_B0)){
            if(!flag_boton1){
                flag_inicio = 1;
                flag_boton1 = 1;
            }
        }
        else flag_boton1 = 0;
        
        if(!input(PIN_B1)){
            if(!flag_boton2){
                motor_ang++;
                flag_boton2 = 1;
            }
        }
        else flag_boton2 = 0;
        
        if(!input(PIN_B2)){
            if(!flag_boton3){
                motor_vel = motor_vel - 5;
                flag_boton3 = 1;
            }
        }
        else flag_boton3 = 0;
        
        if(!input(PIN_B3)){
            if(!flag_boton4){
                motor_dir = ~motor_dir;
                flag_boton4 = 1;
            }
        }
        else flag_boton4 = 0;
        
        if(flag_inicio == 1){ 
            output_high(MOTORES_ENABLE_PIN);  //Habilita el movimiento de los motores
            controlMotor(MOTOR1, motor_dir, motor_ang, motor_vel, ustep); //Controla el motor
        }
        else{
            cont_vel = 0;
            cont_pasos = 0;
            flag_inicio = 0;
            output_low(MOTORES_ENABLE_PIN); //Deshabilita los motores
        }
        
    } 
    
}