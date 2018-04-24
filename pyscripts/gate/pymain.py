import sys
from os.path import abspath, join, dirname
sys.path.insert(0, join(abspath(dirname(__file__)), '../../pycommon'))
sys.path.insert(0, join(abspath(dirname(__file__)), '../../pycommon/library'))
sys.path.insert(0, join(abspath(dirname(__file__)), '../../pycommon/proto_python'))

from defines import *

from server.gate import GateServerRepo,GateServer,GateService
from rpc.TcpConnection import TcpConnection
from rpc.RpcChannel import RpcChannel

conn_maps = {}

def init():
	'''初始化python server'''
	print("py init")
	GateServerRepo.gate_server = GateServer.GateServer()


def OnServer(sockfd, type, data):
	print("OnServer:%d,%d,%s"%(sockfd,type,data))

	if type == FD_TYPE_CONN:
		conn = TcpConnection(sockfd)
		service_factory = GateService.GateService()
		rpc_channel = RpcChannel(service_factory, conn)  # TODO, 多个connection的RpcChannel保存
		conn.attach_rpc_channel(rpc_channel)
		conn_maps[sockfd] = conn
	elif type == FD_TYPE_READ:
		conn = conn_maps[sockfd]
		conn.recv_data(data)
	

	return 1,"success"
