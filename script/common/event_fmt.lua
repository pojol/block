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

    coordinat_select = function(self, module_name, module_fature)
        print(module_name, module_fature)
        local pack = Args.new()
        pack:push_string(module_name)
        pack:push_i32(module_fature)
        return pack:pop_block(0, pack:get_size())
    end,

    mysql_query = function(self, module_id, sql)
        local pack = Args.new()
        pack:push_i32(module_id)
        pack:push_string(sql)
        return pack:pop_block(0, pack:get_size())
    end,

    entity_update = function(self, table, entity_id, arr)
        
        local pack = Args.new()
        pack:push_string(table)
        pack:push_i32(entity_id)

        for i = 1, #arr, 2 do

            pack:push_string(arr[i])

            if arr[i] == "id" then
                pack:push_i32(arr[i + 1])
            elseif arr[i] == "name" then
                pack:push_string(arr[i + 1])
            elseif arr[i] == "hp" then
                pack:push_i32(arr[i + 1])
            elseif arr[i] == "lv" then
                pack:push_i32(arr[i + 1])
            elseif arr[i] == "loginTime" then
                pack:push_i32(arr[i + 1])
            end
        end

        return pack:pop_block(0, pack:get_size())

    end,

    delay_milliseconds = function(self, module_id, milliseconds)
        local pack = Args.new()
        pack:push_i32(module_id)
        pack:push_i32(milliseconds)
        return pack:pop_block(0, pack:get_size())
    end,

    node_regist = function(self, module_id, reg_event, ip, port)
        local pack = Args.new()
        pack:push_i32(module_id)
        pack:push_i32(reg_event)
        pack:push_string(ip)
        pack:push_i32(port)
        return pack:pop_block(0, pack:get_size())
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

        return pack:pop_block(0, pack:get_size())

    end,

    res_nodinfo = function(self, ip, port, node_id)
        local pack = Args.new()
        pack:push_string(ip)
        pack:push_i32(port)
        pack:push_i32(node_id)
        return pack:pop_block(0, pack:get_size())
    end,

    make_acceptor = function(self, module_id, ip, port)
        local pack = Args.new()
        pack:push_i32(module_id)
        pack:push_string(ip)
        pack:push_i32(port)
        return pack:pop_block(0, pack:get_size())
    end,

    make_connector = function(self, module_id, ip, port)
        local pack = Args.new()
        pack:push_i32(module_id)
        pack:push_string(ip)
        pack:push_i32(port)
        return pack:pop_block(0, pack:get_size())
    end,

    send = function(self, fd, msg_id, buf)
        local pack = Args.new()
        pack:push_ui16(fd)
        pack:push_i32(msg_id)
        pack:push_string(buf)
        return pack:pop_block(0, pack:get_size())
    end,

    send2 = function(self, msg_id, buf)
        local pack = Args.new()
        pack:push_i32(msg_id)
        pack:push_string(buf)
        return pack:pop_block(0, pack:get_size())
    end,

    kick_connect = function(self, fd)
        local pack = Args.new()
        pack:push_ui16(fd)
        return pack:pop_block(0, pack:get_size())
    end
}


function evunpack(buf, len)
    unpack = Args.new(buf, len)

    _args = {}
    _idx = 1
    _tag = unpack:get_tag()
    while(_tag ~= 0) do

        if _tag == at_uint16 then
            table.insert(_args, _idx, unpack:pop_ui16())
        elseif _tag == at_int32 then
            table.insert(_args, _idx, unpack:pop_i32())
        elseif _tag == at_string then
            table.insert(_args, _idx, unpack:pop_string())
        elseif _tag == at_uint32 then
            table.insert(_args, _idx, unpack:pop_ui32())
        elseif _tag == at_int64 then
            table.insert(_args, _idx, unpack:pop_i64())
        elseif _tag == at_uint64 then
            table.insert(_args, _idx, unpack:pop_ui64())
        elseif _tag == at_bool then
            table.insert(_args, _idx, unpack:pop_bool())
        end
        
        _idx = _idx + 1
        _tag = unpack:get_tag()
    end

    return _args
end