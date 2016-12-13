import asyncio
import struct

from gsf.tool.robot.connect_mgr import connect_mgr

class LoginCase(asyncio.Protocol):

    account_name = ''
    is_connect = False

    def __init__(self, account_name):
        self.account_name = account_name

    def connection_made(self, transport):
        self.transport = transport
        self.is_connect = True
        print ("connection made")

        connect_mgr.try_connect()

    def data_received(self, data):
        fmt = '<III'

        size, mid, dat = struct.unpack(fmt, data)
        print("recv :",size,mid,dat)

        fmt = '<III'
        self.transport.write(struct.pack(fmt, 12, 100, dat+1))


    def eof_received(self):
        self.is_connect = True
        pass

    def connection_lost(self, exc):
        self.is_connect = False
        print ("connection lost:", exc)