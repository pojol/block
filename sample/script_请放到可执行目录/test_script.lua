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
    package.path = table.concat(package_path, ';')

    require "event"
    require "event_list"

	cb_dispatch(eid.app_id, eid.get_module, "LogModule", function(args) 
		log_m_ = args:pop_uint32(0)
	end)

	cb_dispatch(eid.app_id, eid.get_module, "TimerModule", function(args)
		timer_m_ = args:pop_uint32(0)
	end)

	cb_dispatch(log_m_, eid.log.log_callback, 0, function(args) 
		log_f_ = args:pop_log_callback(0)

	end)	
end

module.init = function()
	print_info("test_script", "module.init")

	listen(module_id, eid.timer.timer_arrive, function(args, callback)
		print_info("test_script" , "time arrive!")
	end)

	cb_dispatch(timer_m_, eid.timer.delay_milliseconds, module_id, 1000, function(args) 
		print_info("test_script", "create time succ id = " .. args:pop_uint64(0))
	end)
end

module.execute = function()
end

module.shut = function()
end