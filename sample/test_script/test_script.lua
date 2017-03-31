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

function test_timer(des)
end

module.init = function(module_id)

	local _args = Args.new()
	_args:push_uint32(1)
	_args:push_string("hello")
	event:ldispatch(0, 101, _args)
	
	
end

module.execute = function()
	
end

module.shut = function()
	print("shut")
end
