local function file_exists(file)
    local f = io.open(file, "rb")
    if f then f:close() end
    return f ~= nil
end

local function parse_packet(line, start)
    local packet = {}
    local num_str = ""
    local index = start
    while index <= #line do
        local c = line:sub(index, index)
        if c == "[" then
            local sub_packet, sub_i = parse_packet(line, index + 1)
            packet[#packet + 1] = sub_packet
            index = sub_i
        elseif c == "," or c == "]" then
            if num_str ~= "" then
                packet[#packet + 1] = tonumber(num_str)
                num_str = ""
            end
            if c == "]" then
                return packet, index
            end
        else
            num_str = num_str .. c
        end
        index = index + 1
    end
end

local function packet_to_string(packet)
    if type(packet) ~= "table" then
        return tostring(packet)
    end
    local str = "["
    for i = 1, #packet do
        local elem = packet[i]
        if type(elem) == "table" then
            str = str .. packet_to_string(elem)
        else
            str = str .. tostring(elem)
        end

        if i < #packet then
            str = str .. ","
        end
    end
    return str .. "]"
end

local function packet_compare(left, right)
    -- print("comparing ", packet_to_string(left), "and", packet_to_string(right))
    if left == nil and right ~= nil then
        return 1
    elseif left ~= nil and right == nil then
        return -1
    end

    local left_is_table  = (type(left) == "table")
    local right_is_table = (type(right) == "table")
    if left_is_table and (not right_is_table) then
        return packet_compare(left, {right})
    elseif (not left_is_table) and right_is_table then
        return packet_compare({left}, right)
    elseif (not left_is_table) and (not right_is_table) then
        if left < right then
            return 1
        elseif left > right then
            return -1
        else
            return 0
        end
    else
        for i = 1, math.max(#right, #left) do
            local l_elem = left[i]
            local r_elem = right[i]
            local child = packet_compare(l_elem, r_elem)
            if child ~= 0 then
                return child
            end
        end
        return 0
    end
end

local function lines_from(file)
    if not file_exists(file) then return {} end
    local lines = {}
    for line in io.lines(file) do
        if line ~= "" then
            lines[#lines + 1] = line
        end
    end
    return lines
end

local function part_1(lines)
    local packet_pairs = {}
    for i = 1, #lines, 2 do
        packet_pairs[#packet_pairs + 1] =
            {parse_packet(lines[i], 2), parse_packet(lines[i + 1], 2)}
    end

    local cnt = 0
    for k, v in pairs(packet_pairs) do
        local order = packet_compare(v[1], v[2])
        if order ~= -1 then
            cnt = cnt + k
        end
    end
    print("sum of in order pair indices:", cnt)
end

local function part_2(lines)
    local packets = {}

    local two = "[[2]]"
    local six = "[[6]]"
    lines[#lines + 1] = two
    lines[#lines + 1] = six

    for i = 1, #lines do
        packets[#packets + 1] = parse_packet(lines[i], 2)
    end

    -- sort packets
    for step = 1, (#packets - 1) do
        local curr = step
        for i = step, #packets do
            local cmp = packet_compare(packets[i], packets[curr])
            if cmp > 0 then
                curr = i
            end
        end

        local temp = packets[curr]
        packets[curr] = packets[step]
        packets[step] = temp
    end

    local decoder = 1
    for i = 1, #packets do
        local str = packet_to_string(packets[i])
        if str == two or str == six then
            decoder = decoder * i
        end
    end

    print("decoder key:", decoder)
end

function Main()
    if #arg < 2 then
        print("Usage " .. arg[0] .. " -- give me a file and a part " .. #arg)
        return
    end

    local file = arg[1]
    local part = tonumber(arg[2])
    local lines = lines_from(file)

    if part == 0 then
        part_1(lines)
    else
        part_2(lines)
    end
end

Main()
