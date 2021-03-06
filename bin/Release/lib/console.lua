local small = Fonts["Arial"][9]

table.findindex = function(arr, needle)
  local i = 1, m
  local l = needle:len()
  if type(arr) ~= "table" then return nil, nil end
  for m = 1, 3 do
    for k, v in pairs(arr) do
      if string.sub(k, 1, l) == needle then
        if i == Console.tabindex then
          Console.tabindex = Console.tabindex + 1
          return k, v
        else
          i = i + 1
        end
      end
    end
    if Console.tabindex == 1 then
      break
    else
      Console.tabindex = 1
      i = 1
    end
  end
  return nil, nil
end

debug_keypressed = function(s, key, unicode)
  if key == "`" then
    if s.disabled == true then
      s:stop():animate({y = 400})
    else
      s:stop():animate({y = 600 + 31 / screen_scale})
    end
    s.disabled = not s.disabled
    return
  end
  if s.disabled == false then
    if key =="return" then
      table.insert(s.lines, '> ' .. s.input)
      if s.history_cursor == #s.history and s.input:len() > 0 then table.insert(s.history, "") end
      s.history_cursor = #s.history
      xpcall(loadstring("print("..s.input..")"), function()xpcall(loadstring(s.input), print)end)
      s.input = ""
      s.cursor = 0
      s.tabupdate(s)
      s.scroll = 0
    elseif key == "left" then
      s.cursor = s.cursor - 1
      if s.cursor < 0 then s.cursor = 0 end
      s.tabupdate(s)
    elseif key == "up" then
      s.history_cursor = s.history_cursor - 1
      if s.history_cursor < 1 then s.history_cursor = 1 end
      s.input = s.history[s.history_cursor]
      s.cursor = s.input:len()
      s.tabupdate(s)
    elseif key == "down" then
      s.history_cursor = s.history_cursor + 1
      if s.history_cursor > #s.history then s.history_cursor = #s.history end
      s.input = s.history[s.history_cursor]
      s.cursor = s.input:len()
      s.tabupdate(s)
    elseif key == "home" then
      s.cursor = 0
      s.tabupdate(s)
    elseif key == "end" then
      s.cursor = s.input:len()
      s.tabupdate(s)
    elseif key == "tab" then
      local arr = _G
      local needle = s.tabstr
      local pos = s.tabstr:find("[.:][^.:]*$") -- last . or :
      if pos then
        local str = "return " .. s.tabstr:sub(1, pos - 1)
        local stat, res = pcall(loadstring(str))
        if stat == true and res then
          arr = res
          needle = s.tabstr:sub(pos+1, s.tabstr:len())
        end
      end

      local k, v = table.findindex(arr, needle)
      if k then
        local buf = 0
        local buf2 = 0
        if type(v) == 'function' then 
          k = k .. '()' buf = 1 
        elseif type(k) == 'number' then 
          k = '[' .. k .. ']'
          buf2 = 1
        elseif type(k) == 'string' then 
          if not k:find("^[a-zA-Z_][a-zA-Z_0-9]*$") then
            k = '["' .. k .. '"]'
            buf2 = 1
          end
        end
        local buf2 = s.tabstr:sub(1, s.tabstr:len() - needle:len() - buf2) .. k
        s.input = buf2 .. s.tabstr2
        s.history[#s.history] = s.input
        s.cursor = buf2:len() - buf
      end
    elseif key == "right" then
      s.cursor = s.cursor + 1
      if s.cursor > s.input:len() then
        s.cursor = s.input:len()
      end
      s.tabupdate(s)
    elseif key == "backspace" then
      if s.tabindex ~= 1 then
        s.input = s.tabstr .. s.tabstr2
        s.cursor = s.tabstr:len()
        s.tabupdate(s)
      else
        if s.cursor > 0 then
          s.input = s.input:sub(1, s.cursor - 1) .. s.input:sub(s.cursor + 1, s.input:len())
          s.cursor = s.cursor - 1
          s.tabupdate(s)
        end
      end
      s.history_cursor = #s.history
      s.history[#s.history] = s.input
    elseif key == "delete" then
      if s.cursor < string.len(s.input) then
        s.input = s.input:sub(1, s.cursor) .. s.input:sub(s.cursor + 2, s.input:len())
      end
    else
      if unicode ~= nil and unicode ~= 0 then
        local char = string.char(unicode%255)
        if s.cursor == s.input:len() then
          s.input = s.input .. char
        else
          s.input = s.input:sub(1, s.cursor) .. char .. s.input:sub(s.cursor + 1, s.input:len())
        end
        s.cursor = s.cursor + 1
      end
      s.history_cursor = #s.history
      s.history[#s.history] = s.input
      s.tabupdate(s)
    end
  end
end
screen_scale = 1
--if not gameoptions.console_history then gameoptions.console_history = {""} end
local _old_print = print
--Override print
_G["print"] = function(...)
	_old_print(...)
	local str = ""
	local args = {...}
	for i,v in pairs(args) do
		str = str .. tostring(v)
		if i < #args then
			str = str .. "       "
		end
	end
	table.insert(Console.lines, str)
end
Console = E:new()
:size(800, 200)
:move(0, 600 + 31 / screen_scale)
:set({
  lines = {}, 
  input = "", 
  cursor = 0, 
  disabled = true, 
  history = {""}--[[gameoptions.console_history]], 
  history_cursor = 1--[[#gameoptions.console_history]], 
  tabindex = 1, 
  tabstr = "", 
  tabstr2 = "",
  scroll = 0,
  tabupdate = function(s) 
    s.tabstr = s.input:sub(1, s.cursor) 
    s.tabstr2 = s.input:sub(s.cursor + 1, s.input:len()) 
    s.tabindex = 1 
  end
})
:wheel(function(s, x, y, b)
  local lines = math.ceil(180 * screen_scale / small:height())
  if b == "u" then
    s.scroll = s.scroll + 1
  else
    s.scroll = s.scroll - 1
  end
  if s.scroll < 0 then s.scroll = 0 end
  if s.scroll > #s.lines - lines - 4 then s.scroll = #s.lines - lines - 4 end
end)
:keyrepeat(debug_keypressed)
:draw(function(s)
  if(s.y >= 600) then return end
  scrupp.setLineWidth(1/screen_scale)
  scrupp.setSmooth(false)
  S.setColor(20,20,20,190)
  local x, y, w, h = math.floor(s.x), math.floor(s.y), math.floor(s.w), math.floor(s.w)
  local off1 = (w - 700) / screen_scale
  local sh = small:height()
  local off2 = (sh*2 + 2) / screen_scale
  local off3 = (sh + 1) / screen_scale
  sh = sh / screen_scale
  S.rectangle(x, y, w, h, true)
  S.rectangle(w - off1 - 2, y - off2, off1, off2, true)
  S.setColor(128,128,128,255)
  S.rectangle(x, y, w, h)
  S.rectangle(w - off1 - 2, y - off2, off1 + 2, off2)
  small:select()
  S.setColor(255,255,255,255)
  S.line(x, y + 200 - off3, w, y + 200 - off3)
  Gprint('fps: '..scrupp.fps() .. '\nMemory: ' .. gcinfo(), w - off1, y - off2)
  Gprint('> ' .. s.input,x,y + 200 - sh - 1)
  local lines = math.ceil(180 * screen_scale / sh)
  local lx = (small:width('> ') + small:width(string.sub(s.input, 0, s.cursor))) / screen_scale
  local ly = y + 200
  if math.sin(time*6) > 0 then
    S.line(lx, ly - sh, lx, ly)
  end
  local c, i = 0, 0
  for i = math.max(#s.lines - lines + 1 - s.scroll, 1), #s.lines - s.scroll do
    Gprint(s.lines[i], x, y + c * sh / screen_scale)
    c = c + 1
  end
end)