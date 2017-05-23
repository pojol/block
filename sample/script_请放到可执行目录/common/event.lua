
function dispatch(...)
	arg = { ... }
	
	local _len = #arg
	local _args = Args.new()
	
    for k,v in pairs(arg) do
		
		if k > 2 then
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

	local function __cb() end
	
	event:ldispatch(module_id, arg[1], arg[2], _args, __cb)
end

-- 带callbakc的dispatch
function cb_dispatch(...)
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

	event:ldispatch(module_id, arg[1], arg[2], _args, arg[_len])
end 

function listen(self_id, event_id, func)
	event:llisten(module_id, self_id, event_id, func)
end

g_log_id_ = 0

function print_info( ... )
    arg = { ... }

    local _len = #arg
	local _args = Args.new()
	local _title = ""

    for k, v in pairs(arg) do
		if k == 1 then
			_title = v
		else 
			if type(v) == "number" then
				_args:push_uint32(v)
			end
			if type(v) == "string" then
				_args:push_string(v)
			end
		end
    end

    log_f_(eid.log.info, _title, _args)
end

function print_warning( ... )
    arg = { ... }

    local _len = #arg
	local _args = Args.new()

    for k, v in pairs(arg) do
        if type(v) == "number" then
            _args:push_uint32(v)
        end
        if type(v) == "string" then
		    _args:push_string(v)
	    end
    end

    event:ldispatch(module_id, g_log_id_, eid.log.warning, _args)
end

function print_error( ... )
    arg = { ... }

    local _len = #arg
	local _args = Args.new()

    for k, v in pairs(arg) do
        if type(v) == "number" then
            _args:push_uint32(v)
        end
        if type(v) == "string" then
		    _args:push_string(v)
	    end
    end

    event:ldispatch(module_id, g_log_id_, eid.log.error, _args)
end



