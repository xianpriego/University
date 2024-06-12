from robot import LogicRobot

if __name__ == "__main__":

    robot = LogicRobot()

    while robot.robot_working():
        stage =  robot.get_stage()
        if stage == 0:
            robot.mapping()
        elif stage == 1:
            robot.patrolling()
        elif stage == 2:
            robot.returning_home()

    robot.map.print_map()
    robot.map.save_map('map.txt')

