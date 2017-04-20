event = nil

function dispatch(...)
	arg = { ... }
	
	local _len = #arg
	local _args = Args.new()
	
    for k,v in pairs(arg) do
		
		if k > 2 and k < _len then
			if type(v) == "number" then
				_args:push_uint32(v)
			end
			if type(v) == "string" then
				_args:push_string(v)
			end
			if type(v) == "function" then
				_args:push_remote_callback(v)
			end
		end

	end

	event:ldispatch(arg[1], arg[2], _args, arg[_len])
end


function listen(module_id, event_id, func)
	event:llisten(module_id, event_id, func)
end

function lprint( ... )
    arg = { ... }

    local _len = #arg
	local _args = Args.new()

    for k, v in pairs(arg) do
        if k > 1 then
            if type(v) == "number" then
                _args:push_uint32(v)
            end
            if type(v) == "string" then
			    _args:push_string(v)
		    end
        end
    end

    event:ldispatch(arg[1], eid.log.info, _args)
end

module = {
    before_init = function() end,
	init = function(module_id) end,
	execute = function() end,
	shut = function() end,
}

local connect_id_ = 0
local log_id_ = 0
local module_id_ = 0
local session_id_ = 0

function msg_function(fd, msg_id, block)
    lprint(log_id_, "msg_function " .. tostring(msg_id) .. " " .. block)

    event:ldispatch_remote(connect_id_, session_id_, 1001, "hello")
end

function connect(ip, port)

    local function _new_connect(args, callback)
        session_id_ = args:pop_uint32(0)
        lprint(log_id_, "new connect fd : ", session_id_)

        event:ldispatch_remote(connect_id_, session_id_, 1001, "hello")
    end

    local function _dis_connect(args, callback)
        lprint(log_id_, "dis connect fd : ", args:pop_uint32(0)) 
    end

    listen(module_id_, eid.network.new_connect, _new_connect)
    listen(module_id_, eid.network.dis_connect, _dis_connect)

    local function __cb(args)
    end

    dispatch(connect_id_, eid.network.make_connector, module_id_, ip, port, __cb)

end

module.before_init = function()
    print("before_init")

    local package_path = {}
    table.insert(package_path, "F:/github/gsf/common/?.lua")
    package.path = table.concat(package_path, ';')

    require("event_list")
    
    local function _get_log(args)
        log_id_ = args:pop_uint32(0)
        lprint(log_id_, "log id : " .. log_id_)
    end

    dispatch(eid.app_id, eid.get_module, "LogModule", _get_log)
end

module.init = function(module_id)
    module_id_ = module_id
	print("init case_login module : " .. module_id)

    local function _create_connector(args)
        connect_id_ = args:pop_uint32(0)
        lprint(log_id_, "connect id : " .. connect_id_)
        connect("127.0.0.1", 8001)
    end

    dispatch(eid.app_id, eid.create_dynamic_module, "ConnectorModule", _create_connector)
    
    local function __cb(args) end

    dispatch(eid.app_id
        , eid.network.bind_remote_callback
        , module_id_
        , 1002
        , msg_function
        , __cb)

end


module.execute = function()
	
end


module.shut = function()
	print("shut")
end
