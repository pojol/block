module = {
	before_init = function(dir) end,
	init = function() end,
	execute = function() end,
	shut = function() end,
	after_shut = function() end,
}

log_m_ = 0

module.before_init = function(dir)
	local package_path = {}
	table.insert(package_path, dir .. "/common/?.lua")
	package.path = table.concat(package_path, ';')

	require "utils"

	log_m_ = APP:getModule("LogModule")
	INFO_LOG("test", "log : " .. log_m_)
end

module.init = function()
	INFO_LOG("test", "hello, gsf!")
end

module.execute = function()
end

module.shut = function()
end

module.after_shut = function()
end