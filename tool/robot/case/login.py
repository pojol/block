import struct
import asyncio


class LoginCase(asyncio.Protocol):

    def __init__(self, account_name):
        self.account_name = account_name
        self.connected = False

        # bind

    def data_received(self, data):
        if self.connected == False:
            print("data received warning!", self.account_name)
            return

        # 是否要拆包取决于服务器的发包策略

    def connection_made(self, transport):
        self.connected = True
        self.transport = transport

    def connection_lost(self, exc):
        self.connected = False

    def send(self, dat):
        if self.connected:
            self.transport.write(dat)