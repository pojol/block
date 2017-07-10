

case_delaymillisecond = {
	create_timer_ = function(millisecond, cb) end,
}

function case_delaymillisecond:test()

	local function _on_timer()
		print_info("case_delaymillisecond", "arrive ! timer id = " .. self.timer_id_)
	end

	self.timer_f_(2000, function(args) 
		local _timer_id = args:pop_uint64(0)

		print_info("test_script", "regist success! timer id = " .. tostring(_timer_id))

		self.timer_id_ = _timer_id
		self.timer_cb_ = _on_timer
	end)
end


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

	local function _delay_milliseconds(delay, cb)
		cb_dispatch(timer_m_, eid.timer.delay_milliseconds, module_id, delay, cb)
	end

	case_delaymillisecond.timer_f_ = _delay_milliseconds

	listen(module_id, eid.timer.timer_arrive, function (args) 
		local _timer_id = args:pop_uint64(0)

		if _timer_id == case_delaymillisecond.timer_id_ then
			case_delaymillisecond:timer_cb_()
		end
	end)
	

	case_delaymillisecond:test()
end

module.execute = function()
end

module.shut = function()
end