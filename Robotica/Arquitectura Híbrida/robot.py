import sys

import cv2
import numpy as np
from controller import Robot

from cascada import water

MATRIX_SIZE = 40

INFRARED_THRESHOLD = 160

FORWARD = -1

UP = 0
LEFT = 1
DOWN = 2
RIGHT = 3

SPEED = 5


DIRECTION_TO_TURN = {
            (DOWN, (0, 1)): LEFT,
            (DOWN, (0, -1)): RIGHT,
            (DOWN, (-1, 0)): LEFT,
            (DOWN, (1, 0)): FORWARD,
            (UP, (0, 1)): RIGHT,
            (UP, (0, -1)): LEFT,
            (UP, (-1, 0)): FORWARD,
            (UP, (1, 0)): LEFT,
            (LEFT, (0, 1)): LEFT,
            (LEFT, (0, -1)): FORWARD,
            (LEFT, (-1, 0)): RIGHT,
            (LEFT, (1, 0)): LEFT,
            (RIGHT, (0, 1)): FORWARD,
            (RIGHT, (0, -1)): LEFT,
            (RIGHT, (-1, 0)): LEFT,
            (RIGHT, (1, 0)): RIGHT,
        }

ORIENTATION_TO_MOVEMENT = {
            DOWN: (1, 0),
            UP: (-1, 0),
            LEFT: (0, -1),
            RIGHT: (0, 1)
        }

DIRECTION_TO_AXIS = {UP    : ((-1, 0), (0 , -1), (0, 1), (1, 0)),
                    RIGHT : ((0, 1), (-1 , 0), (1, 0), (0, -1)),
                    DOWN  : ((1, 0), (0 , 1), (0, -1), (-1, 0)),
                    LEFT  :  ((0, -1), (1 , 0), (-1, 0), (0, 1))}

class Map:
    def __init__(self):
        self.matrix = np.zeros((MATRIX_SIZE, MATRIX_SIZE), dtype=np.int8)
    
    def update_wall(self, position):
        x, y = position
        self.matrix[x, y] = 1
    
    def update_home(self, position):
        x, y = position
        self.matrix[x, y] = 2
    
    def print_map(self):
        for row in self.matrix:
            print(' '.join(str(cell) for cell in row))
    
    def save_map(self, filename):
        np.savetxt(filename, self.matrix, fmt='%d')

    def reshape(self, home_position) -> None:
        index_i_min = -1
        index_i_max = -1
        index_j_min = MATRIX_SIZE
        index_j_max = -1
        for (i, j), content in np.ndenumerate(self.matrix):
            if content == 1:
                if index_i_min == -1:
                    index_i_min = i

                if index_i_max < i:
                    index_i_max = i

                if index_j_min > j:
                    index_j_min = j

                if index_j_max < j:
                    index_j_max = j
        self.matrix = self.matrix[index_i_min:index_i_max + 1, index_j_min:index_j_max + 1]

        index_i = home_position[0] - index_i_min
        index_j = home_position[1] - index_j_min

        return index_i, index_j
        


class LogicRobot:
    def __init__(self) -> None:
        self.robot = Robot()
        self.map = Map()
        self.starting_position = [int((MATRIX_SIZE/2)) - 1, int((MATRIX_SIZE/2)) - 1]
        self.map.update_home(self.starting_position)
        self.robot_position = self.starting_position.copy()
        self.object_position = [-1, -1]
        self.initial_orientation = DOWN
        self.orientation = DOWN
        self.is_positioned = False
        self.time_step = int(self.robot.getBasicTimeStep())
        self.exit = False

        self.camera = self.robot.getDevice("camera")
        self.camera.enable(self.time_step)

        self.left_wheel = self.robot.getDevice("left wheel motor")
        self.right_wheel = self.robot.getDevice("right wheel motor")

        self.left_encoder = self.robot.getDevice("left wheel sensor")
        self.right_encoder = self.robot.getDevice("right wheel sensor")
        self.left_encoder.enable(self.time_step)
        self.right_encoder.enable(self.time_step)

        self.front_infrared = self.robot.getDevice("front infrared sensor")
        self.rear_infrared = self.robot.getDevice("rear infrared sensor")
        self.left_infrared = self.robot.getDevice("left infrared sensor")
        self.right_infrared = self.robot.getDevice("right infrared sensor")
        self.rear_infrared.enable(self.time_step)
        self.right_infrared.enable(self.time_step)
        self.left_infrared.enable(self.time_step)
        self.front_infrared.enable(self.time_step)

        self.left_wheel.setPosition(0)
        self.right_wheel.setPosition(0)
        self.left_wheel.setVelocity(SPEED)
        self.right_wheel.setVelocity(SPEED)

        self.offset = 250/21
        self.delta = 4.05
        self.stage = 0
        
        self.return_path = []

        self.init_stage()

    def init_stage(self) -> None:
        self.turning = True
        self.turn = FORWARD

        self.left_target_position = 0
        self.right_target_position = 0

    def next_stage(self) -> None:
        self.stage += 1
        self.init_stage()

    def detect_object(self) -> bool:
        image = self.camera.getImage()
        image = np.frombuffer(image, np.uint8).reshape((self.camera.getHeight(), self.camera.getWidth(), 4))
        image = image[55:372, 195:560]
        image = cv2.cvtColor(image, cv2.COLOR_BGRA2BGR)
        hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        lower_yellow = np.array([20, 100, 100])
        upper_yellow = np.array([30, 255, 255])
        mask = cv2.inRange(hsv_image, lower_yellow, upper_yellow)
        yellow_count = np.sum(mask == 255)

        return yellow_count > 20000

        
    def update_map(self) -> None:
        row, column = self.robot_position
        sensor_values = [self.front_infrared_value, self.left_infrared_value, self.right_infrared_value, self.rear_infrared_value]
        axis = DIRECTION_TO_AXIS[self.orientation]
        
        for (i, j), value in zip(axis, sensor_values):
            if value > INFRARED_THRESHOLD:
                self.map.update_wall((row + i, column + j))

        if self.detect_object():

            if self.orientation == UP:
                self.map.update_wall((row - 1, column - 1))
            elif self.orientation == DOWN:
                self.map.update_wall((row + 1, column + 1))
            elif self.orientation == RIGHT:
                self.map.update_wall((row - 1, column + 1))
            elif self.orientation == LEFT:
                self.map.update_wall((row + 1, column - 1))

            self.turn = RIGHT



    

    def determine_turn_direction(self) -> int:
        turn_direction = LEFT
        front_obstacle_detected = self.front_infrared_value > INFRARED_THRESHOLD
        left_obstacle_detected = self.left_infrared_value > INFRARED_THRESHOLD

        if self.stage == 0:
            if left_obstacle_detected:
                if not self.is_positioned:
                    self.is_positioned = True
                    self.initial_orientation = self.orientation
                turn_direction = RIGHT if front_obstacle_detected else FORWARD
            else:
                if not self.is_positioned:
                    turn_direction = LEFT
                else:
                    if self.turn == FORWARD:
                        turn_direction = LEFT
                    else:
                        turn_direction = RIGHT if front_obstacle_detected else FORWARD
        else:
            if left_obstacle_detected:
                turn_direction = RIGHT if front_obstacle_detected else FORWARD
            else:
                if self.turn == FORWARD:
                    turn_direction = LEFT
                else:
                    turn_direction = RIGHT if front_obstacle_detected else FORWARD

        return turn_direction

    def print_position(self):
        print(self.robot_position)

    def next_position(self) -> int:
        movement = (self.return_path[0][0] - self.robot_position[0], self.return_path[0][1] - self.robot_position[1])
        turn = DIRECTION_TO_TURN.get((self.orientation, movement), FORWARD)
        if turn == FORWARD:
            self.return_path.pop(0)
        return turn

    
    def reshape(self) -> None:
        i, j = self.map.reshape(home_position=self.starting_position)
        self.robot_position = [i, j]
        self.starting_position = self.robot_position.copy()

    def update_position_object(self) -> None:
        movement = ORIENTATION_TO_MOVEMENT[self.orientation]
        self.object_position = [self.robot_position[0] + movement[0], self.robot_position[1] + movement[1]]

    def update_position_robot(self) -> None:
        movement = ORIENTATION_TO_MOVEMENT[self.orientation]
        self.robot_position[0] += movement[0]
        self.robot_position[1] += movement[1]

    def pathfinding(self) -> None:
        start = (self.robot_position[0], self.robot_position[1])
        end = (self.starting_position[0], self.starting_position[1])
        
        copy = self.map.matrix.copy()

        self.return_path = water(copy, start, end)
        self.return_path.pop(0)
        print(f"Path to home: {self.return_path}")

    def get_positions(self) -> None:
        self.left_position = self.left_encoder.getValue()
        self.right_position = self.right_encoder.getValue()

    def get_infrared(self) -> None:
        self.front_infrared_value = self.front_infrared.getValue()
        self.rear_infrared_value = self.rear_infrared.getValue()
        self.left_infrared_value = self.left_infrared.getValue()
        self.right_infrared_value = self.right_infrared.getValue()

    def get_sensors(self) -> None:
        self.get_positions()
        self.get_infrared()

    def get_stage(self):
        return self.stage

    def robot_working(self) -> bool:
        if self.exit: return False
        return self.robot.step(self.time_step) != -1 
      

    def mapping(self) -> None:
        self.get_sensors()

        if self.turning is not True:
            if self.turn == FORWARD:
                self.turning = True
                self.left_target_position = self.left_position + self.offset
                self.right_target_position = self.right_position + self.offset
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
                self.update_position_robot()
            elif self.turn == LEFT:
                self.turning = True
                self.orientation = (self.orientation + 1) % 4
                self.left_target_position = self.left_position - self.delta
                self.right_target_position = self.right_position + self.delta
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
            elif self.turn == RIGHT:
                self.turning = True
                self.orientation = (self.orientation - 1) % 4
                self.left_target_position = self.left_position + self.delta
                self.right_target_position = self.right_position - self.delta
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
        else:
            if self.turn == FORWARD:
                if self.left_position >= self.left_target_position or self.right_position >= self.right_target_position:
                    self.print_position()
                    if self.robot_position == self.starting_position and self.left_position != 0 and self.initial_orientation == self.orientation:
                        self.reshape()
                        self.next_stage()
                    else:
                        self.turn = self.determine_turn_direction()
                        self.update_map()
                        self.turning = False
            elif self.turn == LEFT:
                if self.left_position <= self.left_target_position + 0.001 or self.right_position + 0.001 >= self.right_target_position:
                    self.turn = self.determine_turn_direction()
                    self.turning = False
                    if self.detect_object():
                        self.turn = RIGHT
            elif self.turn == RIGHT:
                if self.left_position + 0.001 >= self.left_target_position or self.right_position <= self.right_target_position + 0.001:
                    self.turn = self.determine_turn_direction()
                    self.turning = False
                    if self.detect_object():
                        self.turn = RIGHT
    

    def patrolling(self) -> None:
        self.get_sensors()
        if self.turning is not True:
            if self.turn == FORWARD:
                self.turning = True
                self.left_target_position = self.left_position + self.offset
                self.right_target_position = self.right_position + self.offset
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
                self.update_position_robot()
            elif self.turn == LEFT:
                self.turning = True
                self.orientation = (self.orientation + 1) % 4
                self.left_target_position = self.left_position - self.delta
                self.right_target_position = self.right_position + self.delta
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
            elif self.turn == RIGHT:
                self.turning = True
                self.orientation = (self.orientation - 1) % 4
                self.left_target_position = self.left_position + self.delta
                self.right_target_position = self.right_position - self.delta
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
        else:
            if self.turn == FORWARD:
                if self.left_position >= self.left_target_position or self.right_position >= self.right_target_position:
                    self.print_position()
                    self.turn = self.determine_turn_direction()
                    if self.detect_object():
                        self.update_position_object()
                        self.pathfinding()
                        self.next_stage()
                    else:
                        self.turning = False
            elif self.turn == LEFT:
                if self.left_position <= self.left_target_position + 0.001 or self.right_position + 0.001 >= self.right_target_position:
                    self.turn = self.determine_turn_direction()
                    self.turning = False
                    if self.detect_object():
                        self.update_position_object()
                        self.pathfinding()
                        self.next_stage()
            elif self.turn == RIGHT:
                if self.left_position + 0.001 >= self.left_target_position or self.right_position <= self.right_target_position + 0.001:
                    self.turn = self.determine_turn_direction()
                    self.turning = False
                    if self.detect_object():
                        self.update_position_object()
                        self.pathfinding()
                        self.next_stage()

    def returning_home(self) -> None:
        self.get_sensors()
        if self.turning is not True:
            if self.turn == FORWARD:
                self.turning = True
                self.left_target_position = self.left_position + self.offset
                self.right_target_position = self.right_position + self.offset
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
                self.update_position_robot()
            elif self.turn == LEFT:
                self.turning = True
                self.orientation = (self.orientation + 1) % 4
                self.left_target_position = self.left_position - self.delta
                self.right_target_position = self.right_position + self.delta
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
            elif self.turn == RIGHT:
                self.turning = True
                self.orientation = (self.orientation - 1) % 4
                self.left_target_position = self.left_position + self.delta
                self.right_target_position = self.right_position - self.delta
                self.left_wheel.setPosition(self.left_target_position)
                self.right_wheel.setPosition(self.right_target_position)
        else:
            if self.turn == FORWARD:
                if self.left_position >= self.left_target_position or self.right_position >= self.right_target_position:
                    self.print_position()
                    if self.robot_position == self.starting_position:
                        self.next_stage()
                        self.exit = True
                        print("Finished!")
                    else:
                        self.turn = self.next_position()
                        self.turning = False
            elif self.turn == LEFT:
                if self.left_position <= self.left_target_position + 0.001 or self.right_position + 0.001 >= self.right_target_position:
                    self.turn = self.next_position()
                    self.turning = False
            elif self.turn == RIGHT:
                if self.left_position + 0.001 >= self.left_target_position or self.right_position <= self.right_target_position + 0.001:
                    self.turn = self.next_position()
                    self.turning = False
