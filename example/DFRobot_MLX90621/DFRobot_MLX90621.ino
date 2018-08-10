#include "DFRobot_MLX90621.h"

DFRobot_MLX90621  mlx90621;

void setup(){ 
    Serial.begin(115200);
    while(!Serial);
    Serial.println("Initialize sensor...");
    mlx90621.initMLX90621();                                 //Initialize MLX90621
}

void loop(){
    float Ta, Tmin, Tmax;
    mlx90621.measureTemperature();                           //Measure temperature
    Serial.println("The temperature matrix :");
    for(int y=1;y<5;y++){
        Serial.print("[");
        for(int x=1;x<17;x++){
            double tempAtXY= mlx90621.getTemperature(y,x);   //Get the temperature in matrix
            Serial.print(tempAtXY);
            if (x<16){
                Serial.print(", ");
            }
        }
        Serial.println("]");
    }
    Ta   = mlx90621.getAmbientTemperature();                 //Get the ambient temperature
    Tmin = mlx90621.getMinTemperature();                     //Get the Minimum temperature in matrix
    Tmax = mlx90621.getMaxTemperature();                     //Get the Maximum temperature in matrix
    Serial.print("Ambient temperature = ");
    Serial.println(Ta);
    Serial.print("Minimum temperature = ");
    Serial.println(Tmin);
    Serial.print("Maximum temperature = ");
    Serial.println(Tmax);
    Serial.println();
    delay(500);
}