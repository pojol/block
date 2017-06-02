module = {
    before_init = function(dir) end,
	init = function() end,
	execute = function() end,
	shut = function() end,
}

local connect_id_ = 0
local session_id_ = 0

log_m_ = 0
log_f_ = nil


module.before_init = function(dir)

    local package_path = {}
    table.insert(package_path, dir .. "/common/?.lua")
	table.insert(package_path, dir .. "/protobuf/?.lua")
    package.path = table.concat(package_path, ';')

	protobuf = require "protobuf"
	addr = io.open(dir .. "/protobuf/addressbook.pb","rb")
	buffer = addr:read "*a"
	addr:close()

	protobuf.register(buffer)
	
    require "event"
    require "event_list"

	cb_dispatch(eid.app_id, eid.get_module, "LogModule", function(args) 
		log_m_ = args:pop_int32(0)
	end)
	
	cb_dispatch(log_m_, eid.log.log_callback, 0, function(args) 
		log_f_ = args:pop_log_callback(0)
	end)

    cb_dispatch(eid.app_id, eid.get_module, "ConnectorModule", function(args) 
		connect_id_ = args:pop_int32(0)
	end)
end

module.init = function()


	cb_dispatch(connect_id_, eid.network.send_remote_callback, 1, function(args)
		module.send = args:pop_remote_callback(0)
		
		print_info("client", "get send callback!")
	end)  

	listen(module_id, eid.network.new_connect, function(args) 
		print_info("client", "connect!")
		session_id_ = args:pop_int32(0)

		local addressbook = {
			name = "lucy",
			id = 2000,
			email = "127.0.0.1",
		}
		
		block = protobuf.encode("tutorial.Person", addressbook)
		module.send(session_id_, 1001, block)
	end)

	listen(module_id, eid.network.dis_connect, function(args)
	end)

	dispatch(connect_id_, eid.network.make_connector, module_id, "127.0.0.1", 8001)

end


module.execute = function()
	
end


module.shut = function()
end