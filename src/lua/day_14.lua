-- util

local function file_exists(file)
    local f = io.open(file, "rb")
    if f then f:close() end
    return f ~= nil
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

local function split(inputstr, sep, exclude)
    if sep == nil then
        sep = "%s"
    end

    local t = {}
    local i = 0
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
        if exclude == nil or str ~= exclude then
            table.insert(t, str)
        end
    end

    return t
end

local function sign(x)
    return x>0 and 1 or x<0 and -1 or 0
end

-- Space class

Space = { x_0 = 0, y_0 = 0 }

function Space:new (x_0, y_0, width, height)
    local grid = {}
    setmetatable(grid, self)
    self.__index = self

    self.width  = width
    self.height = height
    self.x_0    = x_0
    self.y_0    = y_0
    self.data   = {}
    self.sand   = nil

    for i = 1, height do
        self.data[i] = {}
        for j = 1, width do
            self.data[i][j] = '.'
        end
    end

    return grid
end

function Space:contains(pos)
    return (#self.data + self.y_0) > pos[2] and pos[2] >= self.y_0 and
        (#self.data[1] + self.x_0) > pos[1] and pos[1] >= self.x_0
end

function Space:set (pos, value)
    if not self:contains(pos) then
        return false
    end

    self.data[pos[2] - self.y_0 + 1][pos[1] - self.x_0 + 1] = value
    return true
end

function Space:get(pos)
    if not self:contains(pos) then
        return nil
    end

    return self.data[pos[2] - self.y_0 + 1][pos[1] - self.x_0 + 1]
end

function Space:add_rock(rock)
    for i = 1, #rock - 1 do
        local from = rock[i]
        local to   = rock[i + 1]

        local dx = sign(to[1] - from[1])
        local dy = sign(to[2] - from[2])

        self:set(from, '#')

        while from[1] ~= to[1] or from[2] ~= to[2] do
            from[1] = from[1] + dx
            from[2] = from[2] + dy
            self:set(from, '#')
        end
    end
end

function Space:print()
    local str = ""
    for y = 1, #self.data do
        for x = 1, #self.data[1] do
            str = str .. self.data[y][x]
        end
        str = str .. "\n"
    end
    print(str)
end

function Space:add_sand(pos)
    if self.sand ~= nil then
        print("adding sand to already sanded space")
        return false
    end

    if self:get(pos) == 'o' then
        return false
    end

    self.sand = pos
    self:set(pos, 'o')
    return true
end

function Space:tick()
    if self.sand == nil then
        return
    end

    local success = true
    local deltas  = {{0, 1}, {-1, 1}, {1, 1}}
    for i = 1, #deltas do
        local new_pos = {self.sand[1] + deltas[i][1], self.sand[2] + deltas[i][2]}
        local char = self:get(new_pos)
        if char == nil then
            success = false
        elseif char == '.' then
            self:set(self.sand, '.')
            self.sand = new_pos
            return self:set(self.sand, 'o')
        end
    end
    -- no moves, so has rested
    self.sand = nil
    return success
end


local function make_rocks(lines)
    local min = {math.maxinteger, math.maxinteger}
    local max = {math.mininteger, math.mininteger}

    local rocks = {}

    for line_index = 1, #lines do
        local line = lines[line_index]
        local tokens = split(line, " ", "->")

        for i = 1, #tokens do
            tokens[i] = split(tokens[i], ",")
            for j = 1, #tokens[i] do
                tokens[i][j] = tonumber(tokens[i][j])

                if tokens[i][j] < min[j] then
                    min[j] = tokens[i][j]
                end
                if tokens[i][j] > max[j] then
                    max[j] = tokens[i][j]
                end
            end
        end
        rocks[#rocks + 1] = tokens
    end

    return rocks, min, max
end

local function part_1(lines)
    local rocks, min, max = make_rocks(lines)

    local space = Space:new(min[1] - 3, 0, max[1] - min[1] + 6, max[2] + 1)

    for i = 1, #rocks do
        space:add_rock(rocks[i])
    end

    local no_more_space = false
    local cnt = 0
    while true do
        space:add_sand({500, 0})
        while space.sand ~= nil do
            if not space:tick() then
                print("falling forever", cnt)
                no_more_space = true
            end
        end

        if no_more_space then
            break
        end
        cnt = cnt + 1
        space:print()
    end
end

local function part_2(lines)
    local rocks, min, max = make_rocks(lines)

    max[2] = max[2] + 2
    min[1] = min[1] - 200
    max[1] = max[1] + 200
    local floor = {{min[1], max[2]}, {max[1], max[2]}}
    rocks[#rocks + 1] = floor

    local space = Space:new(min[1] - 3, 0, max[1] - min[1] + 6, max[2] + 1)

    for i = 1, #rocks do
        space:add_rock(rocks[i])
    end

    space:print()

    local cnt = 0
    while true do
        if not space:add_sand({500, 0}) then
            break
        end
        while space.sand ~= nil do
            space:tick()
        end

        cnt = cnt + 1
    end

    space:print()
    print("full after", cnt, "grains")

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
