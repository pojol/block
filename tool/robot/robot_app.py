import asyncio
from asyncio import windows_events

from gsf.tool.robot.connect_mgr import connect_mgr
from gsf.tool.robot.scene.login import LoginCase

if __name__ == '__main__':

    loop = windows_events.ProactorEventLoop()
    asyncio.set_event_loop(loop)

    connect_mgr.loop = loop

    for i in range(0,1000):
        connect_mgr.append_login(LoginCase("test" + str(i)), '127.0.0.1', 8888)
    connect_mgr.try_connect()

    try :
        loop.run_forever()
    finally:
        loop.close()