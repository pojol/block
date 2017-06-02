
module = {
    before_init = function(dir) end,
	init = function() end,
	execute = function() end,
	shut = function() end,
}

log_m_ = 0
log_f_ = nil

timer_m_ = 0

module.before_init = function(dir)
	local package_path = {}
    table.insert(package_path, dir .. "/common/?.lua")
	table.insert(package_path, dir .. "/protobuf/?.lua")
    package.path = table.concat(package_path, ';')

    require "event"
    require "event_list"
	
	cb_dispatch(eid.app_id, eid.get_module, "LogModule", function(args) 
		log_m_ = args:pop_int32(0)
	end)

	cb_dispatch(eid.app_id, eid.get_module, "TimerModule", function(args)
		timer_m_ = args:pop_int32(0)
	end)

	cb_dispatch(log_m_, eid.log.log_callback, 0, function(args) 
		log_f_ = args:pop_log_callback(0)
	end)

end

module.init = function()
	print_info("test_script", "module.init")

	
end

module.execute = function()
end

module.shut = function()
end