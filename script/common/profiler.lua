local profiler = {
    start_time_ = 0,
    end_time_ = 0,

    report_by_title = {},     -- 
    reports = {},
}

function profiler:start()

    self.start_time_ = os.clock()

    self.reports = {}
    self.report_by_title = {}

    debug.sethook(profiler._profiling_handler, 'cr', 0)

end

function profiler:stop()
    print("profiler:end")

    self.end_time_ = os.clock()
    debug.sethook()

    local _total_time = self.end_time_ - self.start_time_

    table.sort(self.reports, function(a, b)
        return a.total_time > b.total_time
    end)

    str = "prof : \n"

    for _, report in ipairs(self.reports) do
        print(report.total_time)
    
        local _percent = (report.total_time / _total_time) * 100
        --if _percent < 1 then
        --    break
        --end

        str = str .. string.format("%6.3f, %6.2f%%, %7d, %s", report.total_time, _percent, report.call_count, report.title) .. "\n"
        --str = str .. (tostring(report.total_time) .. " " .. tostring(percent) .. " " .. tostring(report.call_count) .. " " .. report.title .. "\n")
        
    end

    print(str)
end

function profiler:_func_title(function_info)

    local _name = function_info.name or 'anonymous'

    local _line = string.format("%d", function_info.linedefined or 0)

    local _source = function_info.short_src or 'C_FUNC'
    --if os.isfile(_source) then
    --    _source = path.relative(source, "")
    --end

    return string.format("%-30s: %s: %s", _name, _source, _line)
end

function profiler:_func_report(function_info)
    
    local _title = self:_func_title(function_info)

    local _report = self.report_by_title[_title]
    if not _report then
        _report = {
            title = _title,
            call_count = 0,
            total_time = 0,
        }

        self.report_by_title[_title] = _report
        table.insert(self.reports, _report)
    end

    return _report

end

function profiler:_profiling_call(function_info)

    local _function_report = self:_func_report(function_info)
    assert(_function_report)

    _function_report.call_time = os.clock()
    _function_report.call_count = _function_report.call_count + 1
    
end

function profiler:_profiling_return(function_info)

    local _stop_time = os.clock()

    local _report = self:_func_report(function_info)
    assert(_report)

    if _report.call_time and _report.call_count > 0 then
        _report.total_time = _report.total_time + (_stop_time - _report.call_time)
    end

end

function profiler._profiling_handler(hooktype)

    local _function_info = debug.getinfo(2, 'nS')
    assert(_function_info)

    if hooktype == "call" then
        profiler:_profiling_call(_function_info)
    elseif hooktype == "return" then
        profiler:_profiling_return(_function_info)
    end

end


return profiler