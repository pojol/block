module = {
    before_init = function(dir) end,
	init = function() end,
	execute = function() end,
	shut = function() end,
}

local connect_id_ = 0
local session_id_ = 0

function msg_function(args)
    fd = args:pop_uint32(0)
    msg_id = args:pop_uint32(1)
    block = args:pop_string(2)
    print_info("msg_function " .. tostring(msg_id) .. " " .. block)

    module.send(session_id_, 1001, "hello")
    --event:ldispatch_remote(connect_id_, session_id_, 1001, "hello")
end

function connect(ip, port)

    local function _send_callback(args)
        module.send = args:pop_remote_callback(0)
    end
    cb_dispatch(connect_id_, eid.network.send_remote_callback, 1, _send_callback)  

    local function _new_connect(args, callback)
        session_id_ = args:pop_uint32(0)
        print_info("new connect fd : ", session_id_)

        module.send(session_id_, 1001, "hello")
        --event:ldispatch_remote(connect_id_, session_id_, 1001, "hello")
    end

    local function _dis_connect(args, callback)
        print_info("dis connect fd : ", args:pop_uint32(0)) 
    end

    listen(module_id, eid.network.new_connect, _new_connect)
    listen(module_id, eid.network.dis_connect, _dis_connect)

    dispatch(connect_id_, eid.network.make_connector, module_id, ip, port)

end

module.before_init = function(dir)
    print("before_init")

    local package_path = {}
    table.insert(package_path, dir .. "/common/?.lua")
    package.path = table.concat(package_path, ';')

    require "event"
    require "event_list"

    local function _get_log(args)
        g_log_id_ = args:pop_uint32(0)
        print_info("log id : " .. g_log_id_)
    end

    cb_dispatch(eid.app_id, eid.get_module, "LogModule", _get_log)

    local function _get_connector(args)
        connect_id_ = args:pop_uint32(0)
        print("connector id : " .. connect_id_)
    end

    cb_dispatch(eid.app_id, eid.get_module, "ConnectorModule", _get_connector)
end

module.init = function()
	print_info("init case_login module : " .. module_id)

    connect("127.0.0.1", 8001)

    cb_dispatch(connect_id_
        , eid.network.recv_remote_callback
        , module_id
        , 1002
        , msg_function)

end


module.execute = function()
	
end


module.shut = function()
	print("shut")
end


--[[
    local protobuf = require "protobuf"
	addr = io.open(dir .. "/protobuf/addressbook.pb","rb")
	print(addr)
	buffer = addr:read "*a"
	addr:close()

	protobuf.register(buffer)

	t = protobuf.decode("google.protobuf.FileDescriptorSet", buffer)

	proto = t.file[1]

	print(proto.name)
	print(proto.package)

	message = proto.message_type

	for _,v in ipairs(message) do
		print(v.name)
		for _,v in ipairs(v.field) do
			print("\t".. v.name .. " ["..v.number.."] " .. v.label)
		end
	end

	addressbook = {
		name = "Alice",
		id = 12345,
		phone = {
			{ number = "1301234567" },
			{ number = "87654321", type = "WORK" },
		}
	}
	code = protobuf.encode("tutorial.Person", addressbook)
	decode = protobuf.decode("tutorial.Person" , code)

	print(decode.name)
	print(decode.id)
	for _,v in ipairs(decode.phone) do
		print("\t"..v.number, v.type)
	end
]]--