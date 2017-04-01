package.path = "F:/github/gsf/sample/test_script/?.lua"

local _utils = require "utils"

-- 获取到其他module的id
-- 发送，侦听消息

module = {
	init = function(module_id) end,
	execute = function() end,
	shut = function() end,
}

-- 把对象传递进来
-- 把值传递进来

event = nil

local app_id = 1
local get_module = 101
local delay_milliseconds = 3001

local timer_module_id = nil

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
		end
		
	end

	event:ldispatch(arg[1], arg[2], _args, arg[_len])
end


module.init = function(module_id)

	local function delay_1000(args)
		print("timer!")
	end

	local function _callback(args)
		timer_module_id = args:pop_uint32(0)
		dispatch(timer_module_id, delay_milliseconds, module_id, 1000, delay_1000)
	end

	dispatch(app_id, get_module, "TimerModule", _callback)
end

module.execute = function()
	
end

module.shut = function()
	print("shut")
end
