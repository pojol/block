import asyncio
import sys

try:
    import signal
except ImportError:
    signal = None

if sys.platform == 'win32':
    from asyncio import windows_events

from gsf.tool.robot.connect_mgr import connect_mgr
from gsf.tool.robot.scene.login import LoginCase

if __name__ == '__main__':

    if sys.platform == 'win32':
        loop = windows_events.ProactorEventLoop()
    else :
        loop = asyncio.get_event_loop()
        if signal is not None:
            loop.add_signal_handler(signal.SIGINT, loop.stop)

    asyncio.set_event_loop(loop)

    connect_mgr.loop = loop

    for i in range(0, 1):
        connect_mgr.append_login(LoginCase("test" + str(i)), '127.0.0.1', 8888)
    connect_mgr.try_connect()

    try :
        loop.run_forever()
    finally:
        loop.close()
