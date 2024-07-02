-- Cone is the Lua standard library of Pine engine
-- It consists of:

--     Cone -> language utils like enum
--     Cone.Math    -> math utils
--     Cone.Event   -> interfaces for using core events
Cone = {}
Cone.Math = require("core.Cone.math")
Cone.Event = require("core.Cone.event")

--[[    enum

MyEnum = Cone.enum { "Key1", "Key2", ... }
some_var = MyEnum.Key1
]]
Cone.enum = function(keys)
    local Enum = {}
    for _, value in ipairs(keys) do
        Enum[value] = {}
    end
    return Enum
end

--[[    switch

Cone.switch(name)
.case("Kate", function() print("This player's name rhymes with Fate")end)
.case("Tod", function() print("This player's name rhymes with Cod") end)
.default(function() print("This player's name is not Kate or Tod") end)
.process()
]]
Cone.switch = function(element)
    local Table = {
        ["Value"] = element,
        ["DefaultFunction"] = nil,
        ["Functions"] = {}
    }
    
    Table.case = function(testElement, callback)
        Table.Functions[testElement] = callback
        return Table
    end
    
    Table.default = function(callback)
        Table.DefaultFunction = callback
        return Table
    end
    
    Table.process = function()
        local Case = Table.Functions[Table.Value]
        if Case then
            Case()
        elseif Table.DefaultFunction then
            Table.DefaultFunction()
        end
    end
    
    return Table
end

-- print any lua table
Cone.print_table = function(node)
    local cache, stack, output = {},{},{}
    local depth = 1
    local output_str = "{\n"

    while true do
        local size = 0
        for k,v in pairs(node) do
            size = size + 1
        end

        local cur_index = 1
        for k,v in pairs(node) do
            if (cache[node] == nil) or (cur_index >= cache[node]) then

                if (string.find(output_str,"}",output_str:len())) then
                    output_str = output_str .. ",\n"
                elseif not (string.find(output_str,"\n",output_str:len())) then
                    output_str = output_str .. "\n"
                end

                -- This is necessary for working with HUGE tables otherwise we run out of memory using concat on huge strings
                table.insert(output,output_str)
                output_str = ""

                local key
                if (type(k) == "number" or type(k) == "boolean") then
                    key = "["..tostring(k).."]"
                else
                    key = "['"..tostring(k).."']"
                end

                if (type(v) == "number" or type(v) == "boolean") then
                    output_str = output_str .. string.rep('  ',depth) .. key .. " = "..tostring(v)
                elseif (type(v) == "table") then
                    output_str = output_str .. string.rep('  ',depth) .. key .. " = {\n"
                    table.insert(stack,node)
                    table.insert(stack,v)
                    cache[node] = cur_index+1
                    break
                else
                    output_str = output_str .. string.rep('  ',depth) .. key .. " = '"..tostring(v).."'"
                end

                if (cur_index == size) then
                    output_str = output_str .. "\n" .. string.rep('  ',depth-1) .. "}"
                else
                    output_str = output_str .. ","
                end
            else
                -- close the table
                if (cur_index == size) then
                    output_str = output_str .. "\n" .. string.rep('  ',depth-1) .. "}"
                end
            end

            cur_index = cur_index + 1
        end

        if (size == 0) then
            output_str = output_str .. "\n" .. string.rep('  ',depth-1) .. "}"
        end

        if (#stack > 0) then
            node = stack[#stack]
            stack[#stack] = nil
            depth = cache[node] == nil and depth + 1 or depth - 1
        else
            break
        end
    end

    -- This is necessary for working with HUGE tables otherwise we run out of memory using concat on huge strings
    table.insert(output,output_str)
    output_str = table.concat(output)

    print(output_str)
end

return Cone