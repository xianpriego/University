#pragma config(Sensor, S1,     touchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S2,     gyroSensor,     sensorEV3_Gyro, modeEV3Gyro_RateAndAngle)
#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S4,     sonarSensor,    sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          armMotor,      tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define ANGULO_GIRO 90
#define DISTANCIA_DETECCION_PARED 20
#define VELOCIDAD 100
#define MAX_TAREAS 10

TSemaphore semaphore;
bool inhibido[MAX_TAREAS];
int n_tareas;


//------------------------------------FUNCIONES DE SENSORIZACI�N------------------------------------------//

bool paredCerca() {
    int distancia = SensorValue(sonarSensor);
    if (distancia < DISTANCIA_DETECCION_PARED)
        return true;
    else
        return false;
}

bool atascado() {
    bool contactoPresionado = (SensorValue(touchSensor) != 0); // Convertir el valor a bool
    return contactoPresionado;
}

bool detectaLuz(int umbral) {
    int valorSensorLuz = SensorValue(colorSensor);
    if (valorSensorLuz > umbral)
    	return true;
   	else
   		return false;
}


//------------------------------------FUNCIONES DE CONTROL DE TAREAS(INHIBIR/DESINHIBIR)------------------------------------------//


void inicializarInhibido() {
    for (int i = 0; i < MAX_TAREAS; i++) {
        inhibido[i] = false;
    }
}

bool estoyInhibido(int nivel) {
    bool valorInhibido;

    semaphoreLock(semaphore);

    if (bDoesTaskOwnSemaphore(semaphore)){
	    valorInhibido = inhibido[nivel];
	    semaphoreUnlock(semaphore);
  	}

    return valorInhibido;
}

void inhibir(int nivel) {

    semaphoreLock(semaphore);

    if (bDoesTaskOwnSemaphore(semaphore)) {
    		if (nivel > 0)
       		inhibido[nivel - 1] = true;
        semaphoreUnlock(semaphore);
    }
}

void desinhibir(int nivel) {

    semaphoreLock(semaphore);

    if (bDoesTaskOwnSemaphore(semaphore)) {
				if (nivel > 0)
					inhibido[nivel - 1] = false;
        semaphoreUnlock(semaphore);
    }
}


//------------------------------------M�DULOS DE COMPORTAMIENTO------------------------------------------//

//------------------ESCAPAR------------------//

void modulo_escape() {
    int velocidad_retroceso = -50;

    motor[leftMotor] = velocidad_retroceso;
    motor[rightMotor] = velocidad_retroceso;

    wait1Msec(2000);

    motor[leftMotor] = 0;
    motor[rightMotor] = 0;
}


//-----------------------LUZ-------------------//
void orientarseALuz(){
    // Variable para almacenar el par con el mayor valor de colorAmbiente
  	resetGyro(gyroSensor);
    float maxColorAmbiente = 0;
    long gradoGyroscopio = 0;
    long valor_inicial_gyro = getGyroDegrees(gyroSensor);
    // Giro de 360 grados
    while(abs(getGyroDegrees(gyroSensor) - valor_inicial_gyro) < 360){
        // Leer el valor de iluminaci�n ambiente
        int iluminacion = getColorAmbient(S3);
        // Almacenar el par (iluminacion, gradoGyroscopio) si es el m�ximo hasta ahora
        if (iluminacion > maxColorAmbiente){
            maxColorAmbiente = iluminacion;
            gradoGyroscopio = getGyroDegrees(gyroSensor);
            setLEDColor(ledRedPulse);
        }
        // Girar el robot un grado
        setMotorSpeed(leftMotor, 10);
        setMotorSpeed(rightMotor, -10);
        // Esperar un breve per�odo de tiempo para evitar consumir demasiados recursos del procesador
        setLEDColor(ledGreen);
        wait1Msec(2000 / 360);
    }

    // Detener el robot
    setMotorSpeed(leftMotor, 0);
    setMotorSpeed(rightMotor, 0);

    // Resetear el giroscopio

    while(getGyroDegrees(gyroSensor)%360 != gradoGyroscopio%360){
    		setMotorSpeed(leftMotor, 10);
        setMotorSpeed(rightMotor, -10);
        //wait1Msec(100);
    }
    //parar robot
    setMotorSpeed(leftMotor, 0);
    setMotorSpeed(rightMotor, 0);
}


void modulo_seguir_luz(bool *control){
	bool flag = false;
	int reset = 0;
	float luzActual = 0;
	float luzMaxima = 0;

	orientarseALuz();
	wait(1);
	luzMaxima = getColorAmbient(S3);
	while (luzActual < 40){
		luzActual = getColorAmbient(S3);
		if(luzActual > luzMaxima)
			luzMaxima = luzActual;
		setMotorSpeed(leftMotor, 35);
   	setMotorSpeed(rightMotor, 35);
   	wait1Msec(500);
   	luzActual = getColorAmbient(S3);
   	if (luzActual+10 < luzMaxima){
   		if (flag){
   			setMotorSpeed(leftMotor, -35);
   			setMotorSpeed(rightMotor, 35);
   			wait1Msec(500);
   			flag = false;
   			reset++;
   		}else{
   			setMotorSpeed(leftMotor, 35);
   			setMotorSpeed(rightMotor, -35);
   			wait1Msec(500);
   			flag = true;
   			reset++;
   		}
   		if (reset > 3){
   			orientarseALuz();
   			reset = 0;
   			wait1Msec(500);
   	  }
		}
	}
	//Actualizamos control a false para que una vez se encuentra una luz ya no se realice est� tarea m�s
	*control = false;
}



//---------SEGUIR PARED-----------

void modulo_seguir_pared(bool izq, int *control){

	int distancia = getUSDistance(sonarSensor);
	if (distancia < DISTANCIA_DETECCION_PARED) {
		*control = getTimer(T1, seconds);
  	float tiempo_inicial = getTimer(T1, seconds);
  	resetGyro(gyroSensor);
  	while (abs(getGyroDegrees(gyroSensor)) < ANGULO_GIRO){
      // Girar hacia el lado determinado
          if(getTimer(T1,seconds) - tiempo_inicial < 3){
          if (izq) {
              setMotorSpeed(leftMotor, -40);
            	setMotorSpeed(rightMotor, 40);
          } else {
              setMotorSpeed(leftMotor, 40);
            	setMotorSpeed(rightMotor, -40);
          }
          }else{
              setMotorSpeed(leftMotor, -40);
            	setMotorSpeed(rightMotor, -40);
              break;
          }

      // Esperar un breve per�odo de tiempo para evitar consumir demasiados recursos del procesador
      wait1Msec(1);
    }
  }else{
  	if(izq){
      // Cambiar de direcci�n girando hacia la derecha
     setMotorSpeed(leftMotor, 55);
     setMotorSpeed(rightMotor, 35);
   	}else{
  		setMotorSpeed(leftMotor, 35);
     	setMotorSpeed(rightMotor, 55);
   	}
   	sleep(150); // Gira durante 0.1 segundos (ajusta el tiempo seg�n sea necesario)
  }
}

//---------BUSCAR PARED----------

void parar(){
		//Parar el robot
    setMotorSpeed(leftMotor, 0);
    setMotorSpeed(rightMotor, 0);
}

void avanzar(int velocidad){
	 // Avanzar a una velocidad constante
   setMotorSpeed(leftMotor, velocidad);
   setMotorSpeed(rightMotor, velocidad);

}

int calcularVelocidad(int distancia)
{
      // Calcular la velocidad de forma lineal entre distancia m�nima y m�xima
      return 15 + (distancia - (DISTANCIA_DETECCION_PARED - 5)) * VELOCIDAD / (2*(DISTANCIA_DETECCION_PARED - 5) - (DISTANCIA_DETECCION_PARED - 5));

}


void modulo_buscar_pared(){
		int distancia = getUSDistance(sonarSensor);
		int velocidad;
		if (distancia > (DISTANCIA_DETECCION_PARED - 5)){
			velocidad = calcularVelocidad(distancia);
			avanzar(velocidad);
		}
		else{
			parar();
		}
}



//-------------------------------------TAREAS-----------------------------------------//



task escapar(){
	int nivel = 3;
	while (true) {

		if(!estoyInhibido(nivel)){

        if(atascado()){ //TIENE QUE ACTUAR
	        	//Debe inhibir a la tarea inferior
        		inhibir(nivel);

        		//INICIO ESCAPAR
        		modulo_escape();
        		//FIN ESCAPAR


        }
        else{ //SI NO DEBE ACTUAR
        		//Debe desinhibir a la tarea inferior
        		desinhibir(nivel);
        }
    }
    else //Si est� inhibido debe inhibir a la tarea inferior
    	inhibir(nivel);

    abortTimeslice();
	}
}






task dirigirseALuz(){
	int nivel = 2;
	bool control = true;
	while (true) {

		if(!estoyInhibido(nivel)){

        if(detectaLuz(5) && control){ //TIENE QUE ACTUAR
	        	//Debe inhibir a la tarea inferior
        		inhibir(nivel);

        		//INICIO DIRIGIRSE A LUZ
        		modulo_seguir_luz(&control);
        		//FIN DIRIGIRSE A LUZ


        }
        else{ //SI NO DEBE ACTUAR
        		//Debe desinhibir a la tarea inferior
        		desinhibir(nivel);
        }
    }
    else{ //Si est� inhibido debe inhibir a la tarea inferior
    	inhibir(nivel);
		}

    abortTimeslice();
	}
}





task seguirPared(){
	clearTimer(T1); //YA QUE UTILIZA EL TIMER T1
	int nivel = 1;
	int control = -1000; //Lo inicializamos con un valor negativo para que en el inicio de la ejecuci�n no se cumpla indeseadamente la segunda condici�n.
	while (true) {

		if(!estoyInhibido(nivel)){ //Si no est� inhibido

        if(paredCerca() || getTimer(T1, seconds) - control < 8){ //SI TIENE UNA PARED CERCA O SI LLEVA MENOS DE 8 SEGUNDOS SIN EVITAR UNA PARED
	        	//Debe inhibir a la tarea inferior
        		inhibir(nivel);

        		//INICIO SEGUIR PARED
        		modulo_seguir_pared(false, &control);
        		//FIN SEGUIR PARED
        }
        else{
        		//Debe desinhibir a la tarea inferior
        		desinhibir(nivel);
        }
    }
    else{ //Si est� inihibida debe inhibir a la tarea inferior
    	inhibir(nivel);
    }

    abortTimeslice();
	}

}



task buscarPared(){
	int nivel = 0;
	while (true) {

		if(!estoyInhibido(nivel)){ //Si no est� inhibido

        if(!paredCerca()){ //SI NO TIENE UNA PARED CERCA DEBE BUSCARLA
	        	//Debe inhibir a la tarea inferior
        		inhibir(nivel);

        		//INICIO BUSCAR PARED
        		modulo_buscar_pared();
        		//FIN BUSCAR PARED

        }
        else{
        		//Debe desinhibir a la tarea inferior
        		desinhibir(nivel);

        }
    }else{
    	inhibir(nivel);

    }

    abortTimeslice();
	}
}



//-----------------------MAIN----------------------//


task main()
{
	n_tareas= 4;
	semaphoreInitialize(semaphore);
	inicializarInhibido();
	startTask(buscarPared);
	startTask(seguirPared);
	startTask(dirigirseALuz);
	startTask(escapar);

	while (true) {
 		abortTimeslice();
 	}

}