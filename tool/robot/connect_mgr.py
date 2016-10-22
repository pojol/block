import asyncio

from  scene.login import LoginCase

class ConnectActor:
    def __init__(self, case, task):
        self.case = case
        self.task = task

class ConnectMgr:
    actors = {}     #connect actors

    def open_connect(self, ip, port, case):
        task = asyncio.Task(self.loop.create_connection(lambda : case, ip, port))
        if case.account_name in self.actors:
            self.actors[case.account_name].case = case
            self.actors[case.account_name].task = task
        else:
            self.actors[case.account_name] = ConnectActor(case, task)

    def close_connect(self, actor_name):
        if actor_name in self.actors:
            self.actors[actor_name].case.transport.close()
            self.actors[actor_name].task.cancel()
        else:
            print('close connect warning!', actor_name)

    def append_login(self, account_name, address, port):

        self.open_connect(address, port, LoginCase(account_name))

        pass


connect_mgr = ConnectMgr()