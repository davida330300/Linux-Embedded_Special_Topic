#include "mbed.h"
#include "ADXL345.h"
#include "L3G4200D.h"

I2C i2c(D14,D15);  // SDA, SCL
ADXL345 acc(i2c);
L3G4200D gyro(D14,D15);
Timer t;
Serial pc(USBTX, USBRX); //connection pc
InterruptIn button1(USER_BUTTON);
volatile bool button1_pressed = false; // Used in the main loop
volatile bool button1_enabled = true; // Used for debouncing
Timeout button1_timeout; // Used for debouncing

// Enables button when bouncing is over
void button1_enabled_cb(void)
{
    button1_enabled = true;
}

// ISR handling button pressed event
void button1_onpressed_cb(void)
{
    if (button1_enabled) { // Disabled while the button is bouncing
        button1_enabled = false;
        button1_pressed = true; // To be read by the main loop
        button1_timeout.attach(callback(button1_enabled_cb), 0.3); // Debounce time 300 ms
    }
}
 
int main() 
{
    pc.baud(115200); //set baud rate for USB serial to 115200bps
    float f[3]={0},temp=0;
    //float temp_f[3]={0},temp=0;
    int count=0;
    float count_time=0,time=0;
    //int g[3]={0},temp_g[3]={0};
    //printf("\r\n");
    //button1.mode(PullUp); // Activate pull-up
    button1.fall(callback(button1_onpressed_cb)); // Attach ISR to handle button press event
    while(1)
    {
      if(button1_pressed) { // Set when button is pressed
        button1_pressed = false;
        t.reset();
        t.stop();
        printf("start\n");
        while(button1_pressed==false){
            acc.read_g_data(f); 
            time=t.read_ms();
            //gyro.read(g);
            if((f[2]>=(+0.5))&&(time==0)){
                t.start();
                //time=t.read_ms();
                //printf("\r\n..................begin................\r\n");
            }
            if(time>0){
                count_time++;
            }
            if((f[2]<-0.75)&&(time>=10)){
                t.stop();
                count_time=t.read_ms();
                if((60000/(count_time+300))*4<=220){
                    temp=(60000/(count_time+300))*4;
                    //printf("\r\n\r\nacc: %f %f %f\r\n\r\n",f[0],f[1],f[2]);
                    //printf("\r\n\r\ncount_time = %f\r\n\r\n",count_time);
                    //printf("\r\n\r\ntime = %f\r\n\r\n",count_time);
                    printf("%f\n",temp);
                    t.reset();
                    t.stop();
                    count_time=0;
                    //wait(3);
                }
                else{
                    //printf("\r\n\r\ntime = %f\r\n\r\n",count_time);
                    printf("%f\n",temp);
                    t.reset();
                    t.stop();
                    count_time=0;
                }
            }
            /*printf("acc: %f %f %f\t\t",f[0],f[1],f[2]);
            printf("vector_acc: %f %f %f\t\t",f[0]-temp_f[0],f[1]-temp_f[1],f[2]-temp_f[2]);
            temp=(f[0]-temp_f[0])*(f[0]-temp_f[0])+(f[1]-temp_f[1])*(f[1]-temp_f[1])+(f[2]-temp_f[2])*(f[2]-temp_f[2]);
            printf("magnitude: %f\t",sqrt(temp));
            printf("\r\n");
            temp_f[0]=f[0];temp_f[1]=f[1];temp_f[2]=f[2];*/
            count++;
            //printf("g: %d %d %d\t\t",g[0],g[1],g[2]);
            //printf("time = %d\r\n",t.read_us());
            //printf("vector_g: %d %d %d\t\t",g[0]-temp_g[0],g[1]-temp_g[1],g[2]-temp_g[2]);
            /*temp=(g[0]-temp_g[0])*(g[0]-temp_g[0])+(g[1]-temp_g[1])*(g[1]-temp_g[1])+(g[2]-temp_g[2])*(g[2]-temp_g[2]);
            printf("%f\t",sqrt(temp));*/
            //temp_g[0]=g[0];temp_g[1]=g[1];temp_g[2]=g[2];
            //wait(0.05);
        }
        button1_pressed=false;
        printf("end\n");
        count=0;
        count_time=0;
        time=0;
      }
    }
}