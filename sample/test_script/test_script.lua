package.path = "E:/github/gsf/sample/test_script/?.lua"

local _utils = require "utils"

module = {}

function module:init()
	print("init")
end

function module:execute()
	
end

function module:shut()
	print("shut")
end

