import asyncio
from asyncio import windows_events

from connect_mgr import connect_mgr



if __name__ == '__main__':
    loop = windows_events.ProactorEventLoop()
    asyncio.set_event_loop(loop)

    connect_mgr.loop = loop

    connect_mgr.append_login("test001" , '127.0.0.1', 9870)

    try :
        loop.run_forever()
    finally:
        loop.close()