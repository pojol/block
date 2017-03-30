package.path = "E:/github/gsf/sample/test_script/?.lua"

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

module.init = function(module_id)
	--local a = A.new()
	
	--print(tt)
	--a:pt(tostring(module_id))
	
	--[[
	local _args = Args.new()
	_args.push_uint32(1)
	_args.push_string("hello")
	event:ldispatch(_args)
	]]--
	
	event:ldispatch(1, "hello")
	
	--test(1, "hello")
end

module.execute = function()
	
end

module.shut = function()
	print("shut")
end
