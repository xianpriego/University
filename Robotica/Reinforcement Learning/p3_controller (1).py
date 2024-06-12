'''
PRÁCTICA 3: Q-LEARNING

XIAN PRIEGO MARTÍN, xian.priego@udc.es
SAMUEL OTERO AGRASO, s.agraso@udc.es

'''


from controller import Robot, DistanceSensor, Motor, LED
import random
import numpy as np


#################################### INICIALIZACIÓN DE CONSTANTES, ESTRUCTURAS DE DATOS, SENSORES Y ACTUADORES ###################################

# Definición de constantes
NUM_STATES = 3
NUM_ACTIONS = 3
CRUISE_SPEED_AVOID = 25
CRUISE_SPEED = 5
UMBRAL = 800
ITERATIONS = 1000
RADIO_RUEDA = 21

# Inicialización de matrices Q y VISITAS para el aprendizaje por refuerzo
Q = np.zeros((NUM_STATES, NUM_ACTIONS))
VISITAS = np.zeros((NUM_STATES, NUM_ACTIONS))

# Inicialización del robot y sus componentes
robot = Robot()
paso_tiempo = int(robot.getBasicTimeStep())

# Configuración de los sensores infrarrojos
sensores_infrarrojos = [robot.getDevice(name) for name in [
    'rear left infrared sensor', 'left infrared sensor',
    'front left infrared sensor', 'front infrared sensor',
    'front right infrared sensor', 'right infrared sensor',
    'rear right infrared sensor', 'rear infrared sensor',
    'ground left infrared sensor', 'ground front left infrared sensor',
    'ground front right infrared sensor', 'ground right infrared sensor']]

for sensor in sensores_infrarrojos:
    sensor.enable(paso_tiempo)

# Configuración de los motores y encoders del robot
motor_izquierdo = robot.getDevice('left wheel motor')
motor_derecho = robot.getDevice('right wheel motor')
motor_izquierdo.setPosition(float('inf'))
motor_derecho.setPosition(float('inf'))
motor_izquierdo.setVelocity(0.0)
motor_derecho.setVelocity(0.0)
encoderL = robot.getDevice("left wheel sensor")
encoderR = robot.getDevice("right wheel sensor")
encoderL.enable(paso_tiempo)
encoderR.enable(paso_tiempo)


########################################################################################################
################################### FUNCIONES AUXILIARES ###############################################

# Función para detectar si hay una pared
def there_is_wall() -> bool:
    return (sensores_infrarrojos[4].getValue() > UMBRAL or 
            sensores_infrarrojos[5].getValue() > UMBRAL or 
            sensores_infrarrojos[3].getValue() > UMBRAL or 
            sensores_infrarrojos[6].getValue() > UMBRAL or 
            sensores_infrarrojos[2].getValue() > UMBRAL)

# Función para evitar paredes ajustando las velocidades de los motores
def avoid_walls() -> None:
    speed_offset = 0.2 * (CRUISE_SPEED_AVOID - 0.03 * sensores_infrarrojos[3].getValue())
    speed_delta = 0.03 * sensores_infrarrojos[2].getValue() - 0.03 * sensores_infrarrojos[4].getValue()
    motor_izquierdo.setVelocity(speed_offset + speed_delta)
    motor_derecho.setVelocity(speed_offset - speed_delta)

# Función para obtener los valores actuales de los sensores infrarrojos del suelo
def get_infrared_values() -> tuple:
    return (sensores_infrarrojos[8].getValue(), 
            sensores_infrarrojos[9].getValue(), 
            sensores_infrarrojos[10].getValue(), 
            sensores_infrarrojos[11].getValue())

# Función para calcular la probabilidad de elegir una acción aleatoria basada en la iteración actual
def probability_function(i: int) -> float:
    if i <= 0:
        return 1.0
    elif i >= ITERATIONS:
        return 0.0
    else:
        return 1 - (i / ITERATIONS)
    
# Función para elegir una acción basada en la probabilidad calculada
def choose_action(i: int, state: int) -> int:
    threshold = probability_function(i)
    if random.random() < threshold:
        action = random.randint(0, NUM_ACTIONS-1)
    else:
        action = np.argmax(Q[state])
    return action

##############################################################################
###################### DEFINICIÓN DE ESTADOS #################################

# Función para obtener el estado actual del robot basado en los sensores infrarrojos del suelo
def get_state() -> int:
    if (sensores_infrarrojos[9].getValue() > 750 and sensores_infrarrojos[11].getValue() < 500):
        state = 0
    elif(sensores_infrarrojos[8].getValue() < 500 and sensores_infrarrojos[10].getValue() > 750):
        state = 1
    else:
        state = 2
    return state

#################################################################################
####################### DEFINICIÓN DE ACCIONES ##################################

# Función para que el robot gire a la derecha durante un tiempo
def a1() -> int:
    motor_izquierdo.setVelocity(0)
    motor_derecho.setVelocity(CRUISE_SPEED)
    duracion_giro = 0.1
    tiempo_inicial = robot.getTime()
    while robot.getTime() - tiempo_inicial < duracion_giro:
        if there_is_wall():
            return 0
        else:
            robot.step(paso_tiempo)
    motor_izquierdo.setVelocity(0)
    motor_derecho.setVelocity(0)
    return 1

# Función para que el robot gire a la izquierda durante un tiempo
def a2() -> int:
    motor_izquierdo.setVelocity(CRUISE_SPEED)
    motor_derecho.setVelocity(0)
    duracion_giro = 0.1
    tiempo_inicial = robot.getTime()
    while robot.getTime() - tiempo_inicial < duracion_giro:
        if there_is_wall():
            return 0
        else:
            robot.step(paso_tiempo)
    motor_izquierdo.setVelocity(0)
    motor_derecho.setVelocity(0)
    return 1

# Función para que el robot avance en línea recta durante 3 segundos
def a3() -> int:
    velocidad_izquierda = CRUISE_SPEED 
    velocidad_derecha = CRUISE_SPEED  
    motor_izquierdo.setVelocity(velocidad_izquierda)
    motor_derecho.setVelocity(velocidad_derecha)
    
    duracion_rotacion = 0.1
    tiempo_inicial = robot.getTime()
    while robot.getTime() - tiempo_inicial < duracion_rotacion:
        if there_is_wall():
            return 0
        else:
            robot.step(paso_tiempo)
    return 1

###############################################################################
####################### ALGORITMO Q-LEARNING ##################################

# Diccionario para mapear las acciones a sus respectivas funciones
DO_ACTION = {0 : a1, 1: a2, 2: a3}

def reinforcement(current_values: tuple) -> int:
    """
    Calcula la recompensa basada en los valores actuales de los sensores infrarrojos.
    
    Parámetros:
    current_values (tuple): Los valores actuales de los sensores infrarrojos del suelo.
    
    Salida:
    int: La recompensa calculada.
    
    Objetivo:
    La estrategia de refuerzo cuenta cuántos sensores están detectando negro tras ejecutar una acción determinada.
    """
    r = 0
    for value in current_values:
        if value <= 500:  # Un valor menor o igual a 500 indica que el sensor está detectando negro
            r += 1
    return r

def update_Q(s: int, s_prima: int, a: int, r: int, alpha: float, gamma: float = 0.5) -> None:
    """
    Actualiza el valor Q para un par (estado, acción) utilizando la fórmula del Q-learning.
    
    Parámetros:
    s (int): El estado actual del robot.
    s_prima (int): El nuevo estado del robot después de realizar la acción.
    a (int): La acción tomada en el estado s.
    r (int): La recompensa recibida después de realizar la acción a.
    alpha (float): La tasa de aprendizaje, que determina cuánto influye la nueva información en la actualización.
    gamma (float, opcional): El factor de descuento, que determina la importancia de las recompensas futuras. Por defecto es 0.5.
    
    Objetivo:
    Actualizar el valor Q en la matriz Q para el par (estado, acción) (s, a) usando la fórmula de Q-learning:
    Q(s, a) = (1 - alpha) * Q(s, a) + alpha * (r + gamma * max_a' Q(s', a'))
    """
    
    # Valor Q actual para el par (estado, acción)
    previous_q = Q[s][a]
    
    # Mejor valor Q futuro posible en el nuevo estado s'
    future_q = max(Q[s_prima])
    
    # Actualización del valor Q usando la fórmula de Q-learning
    Q[s][a] = (1 - alpha) * previous_q + alpha * (r + gamma * future_q)

def q_learning_algorithm(i: int) -> int:
    """
    Ejecuta un ciclo del algoritmo de Q-learning, eligiendo una acción, actualizando el estado del robot y la matriz Q.
    
    Parámetros:
    i (int): El índice de la iteración actual.
    
    Salida:
    int: El índice de la próxima iteración (i + 1) si la acción fue completada exitosamente, de lo contrario retorna i.
    
    Objetivo:
    Realizar un ciclo de aprendizaje Q-learning, que incluye:
    1. Obtener el estado actual del robot.
    2. Elegir una acción.
    3. Ejecutar la acción y observar el nuevo estado y la recompensa.
    4. Actualizar la matriz Q con la nueva información.
    5. Incrementar el contador de visitas para el par (estado, acción).
    """
    
    # Obtener el estado actual del robot
    s = get_state()
    
    # Elegir una acción basada en la política ε-greedy
    a = choose_action(i, s)
    
    # Ejecutar la acción seleccionada
    flag = DO_ACTION[a]()
    
    # Si la acción fue interrumpida por una pared, retornar el índice actual
    if flag == 0:
        return i #Esto se hace para que en cuanto entre en juego avoid_walls() no se tenga en cuenta para el proceso de aprendizaje.
    
    # Obtener el nuevo estado del robot después de la acción
    s_prima = get_state()
    
    # Obtener los nuevos valores de los sensores infrarrojos
    current_values = get_infrared_values()
    
    # Calcular la recompensa basada en los valores actuales de los sensores
    r = reinforcement(current_values=current_values)
    
    # Calcular la tasa de aprendizaje (alpha) basada en el número de visitas al par (estado, acción)
    alpha = 1 / (1 + VISITAS[s][a])
    
    # Actualizar la matriz Q con la nueva información
    update_Q(s, s_prima, a, r, alpha)
    
    # Incrementar el contador de visitas para el par (estado, acción)
    VISITAS[s][a] += 1
    
    # Imprimir la matriz Q para monitoreo
    print(Q)
    
    # Retornar el índice de la próxima iteración
    return i + 1


##########################################################################
####################### BUCLE PRINCIPAL ##################################

i = 0
while robot.step(paso_tiempo) != -1:
    print(i)
    if there_is_wall():
        avoid_walls()
        print("Avoiding walls...")
    else:
        i = q_learning_algorithm(i)
