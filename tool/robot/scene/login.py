import asyncio

class LoginCase(asyncio.Protocol):

    account_name = ''
    is_connect = False

    def __init__(self, account_name):
        self.account_name = account_name

    def connection_made(self, transport):
        self.is_connect = True
        print ("connection made")

    def data_received(self, data):
        print ("data received: ", data.decode())

    def eof_received(self):
        self.is_connect = True
        pass

    def connection_lost(self, exc):
        self.is_connect = False
        print ("connection lost:", exc)