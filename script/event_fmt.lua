local at_uint8 = 1
local at_int8 = 2
local at_uint16 = 3
local at_int16 = 4
local at_uint32 = 5
local at_int32 = 6
local at_uint64 = 7
local at_int64 = 8
local at_bool = 9
local at_float = 10
local at_double = 11
local at_string = 12
local at_list = 13
local at_vec = 14
local at_map = 15
local at_eof = 20


-- auto generated
evpack = {

    pack = function(self, event, args)
        if event == eid.network.tcp_make_acceptor then
            local _buf = Args.new()
            _buf:push_string(args[1])
            _buf:push_i32(args[2])
            return _buf:exportBuf()
        end

        if event == eid.network.tcp_make_connector then
            local _buf = Args.new()
            pack:push_string(args[1])
            pack:push_i32(args[2])
            return pack:exportBuf()
        end

        if event == eid.network.kick_connect then
            local pack = Args.new()
            pack:push_ui16(args[1])
            return pack:exportBuf()
        end

        if event == eid.network.send then
            local pack = Args.new()
            pack:push_ui16(args[1])
            pack:push_i32(args[2])
            pack:push_string(args[3])
            return pack:exportBuf()
        end        

        if event == eid.dbProxy.connect then
            local pack = Args.new()
            pack:push_string(args[1])
            pack:push_string(args[2])
            pack:push_string(args[3])
            pack:push_string(args[4])
            pack:push_i32(args[5])
            pack:push_bool(args[6])
            return pack:exportBuf()
        end

        if event == eid.dbProxy.execSql then
            local pack = Args.new()
            pack:push_string(args[1])
            return pack:exportBuf()
        end

        if event == eid.dbProxy.insert then
            local pack = Args.new()
            pack:push_string(args[1])
            return pack:exportBuf()
        end

        if event == eid.dbProxy.load then
            local pack = Args.new()
            pack:push_i32(args[1])
            return pack:exportBuf()
        end

        if event == eid.dbProxy.update then
            local pack = Args.new()
            pack:push_i32(args[1])
            pack:push_string(args[2])
            return pack:exportBuf()
        end

        if event == eid.distributed.coordinat_select then 
            local pack = Args.new()
            pack:push_string(args[1])
            pack:push_i32(args[2])
            return pack:exportBuf()
        end

        return ""
    end,


--[[
 node_regist = function(self, module_id, reg_event, ip, port)
        local pack = Args.new()
        pack:push_i32(module_id)
        pack:push_i32(reg_event)
        pack:push_string(ip)
        pack:push_i32(port)
        return pack:exportBuf()
    end,

    node_create = function(self, node_id, node_type, module_id, ip, port, rootIp, rootPort, modules)
        
        local pack = Args.new()
        pack:push_i32(node_id)
        pack:push_i32(module_id)
        pack:push_string(node_type)

        pack:push_string(ip)
        pack:push_i32(port)

        pack:push_string(rootIp)
        pack:push_i32(rootPort)

        pack:push_i32(#modules)
        for i = 1, #modules do
            local mNode = modules[i]
            pack:push_string(mNode[1])
            pack:push_i32(mNode[2])
            pack:push_i32(mNode[3])
        end

        return pack:exportBuf()

    end,

    res_nodinfo = function(self, ip, port, node_id)
        local pack = Args.new()
        pack:push_string(ip)
        pack:push_i32(port)
        pack:push_i32(node_id)
        return pack:exportBuf()
    end,
]]

}

function evunpack(buf)
    unpack = Args.new()
    unpack:importBuf(buf)

    _args = {}
    _idx = 1
    _tag = unpack:get_tag()
    while(_tag ~= 0) do

        if _tag == at_uint16 then
            table.insert(_args, _idx, unpack:pop_ui16())
        elseif _tag == at_int16 then 
            table.insert(_args, _idx, unpack:pop_i16())
        elseif _tag == at_int32 then
            table.insert(_args, _idx, unpack:pop_i32())
        elseif _tag == at_uint32 then
            table.insert(_args, _idx, unpack:pop_ui32())
        elseif _tag == at_int64 then
            table.insert(_args, _idx, unpack:pop_i64())
        elseif _tag == at_uint64 then
            table.insert(_args, _idx, unpack:pop_ui64())
        elseif _tag == at_string then
            table.insert(_args, _idx, unpack:pop_string())
        elseif _tag == at_bool then
            table.insert(_args, _idx, unpack:pop_bool())
        end
        
        _idx = _idx + 1
        _tag = unpack:get_tag()
    end

    return _args
end