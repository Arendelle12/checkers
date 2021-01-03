# from multiprocessing import Process, Lock
from threading import Thread, Lock
import time
from random import random
from time import sleep


# def threaded_function(arg):
#     for i in range(arg):
#         print("running")
#         sleep(1)


# if __name__ == "__main__":
#     thread = Thread(target = threaded_function, args = (10, ))
#     thread.start()
#     thread.join()
#     print("thread finished...exiting")


class GameState:
    def __init__(self):
        self.lock = Lock()
        self.number = 0
        self.id = random()

    def increase(self):
        self.lock.acquire()
        self.number = self.number + 1
        # print(self.number)
        self.lock.release()

def f(gameState):
    print(gameState.id)
    for i in range(1000):
        gameState.increase()
        print(gameState.number)
    #l.acquire()
    # try:
    #     print('hello world', i)
    # finally:
     #   l.release()

if __name__ == '__main__':
    gameState = GameState()

    #lock = Lock()
    # procs = []
    threads = []

    for x in range(10):
        thread = Thread(target = f, args = (gameState, ))
        threads.append(thread)
        thread.start()
        # proc = Process(target=f, args=(gameState,))
        # procs.append(proc)
        # proc.start()

    for thread in threads:
        thread.join()

    # time.sleep(2)

    # for proc in procs:
    #     proc.join(timeout=0)
    #     if proc.is_alive():
    #         print("Job is not finished!")
    print(gameState.id)
    print(gameState.number)

    # for num in range(10):
    #     Process(target=f, args=(gameState).start())


