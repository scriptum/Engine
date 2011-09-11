Font = {}
Fonts = {}
Font.new = function(file)
	local img = nil
	local texture, textureId, textureW, textureH
	local c,x,y,cw,ch,cx,cy,w,h
	local fontname, fontsize, fontsizetype
	local fontdata
	for line in io.lines(file) do
		if not img then 
			img = line:match("textures:(.+)")
			texture = G.newImage(img:trim())
			textureW, textureH = texture:getSize()
			textureId = texture:getTextureId()
			print(textureId)
		end
		c,x,y,cw,ch,cx,cy,w,h = line:match("([%d]+)%s+([%d]+)%s+([%d]+)%s+([%d]+)%s+([%d]+)%s+([%d]+)%s+([%d]+)%s+([%d]+)%s+([%d]+)")
		fontname, fontsize, fontsizetype = line:match("(.+)%s([%d]+)(p[tx])")
		if fontsizetype then
			if fontsizetype == "px" then fontsize = math.floor(fontsize * 72 / 96) end
			if not Fonts[fontname] then Fonts[fontname] = {} end
			fontdata = scrupp.addFont(textureId, textureW, textureH)
			table.insert(Fonts[fontname], {size = fontsize, data = fontdata, img = texture})
		elseif w and h then
			fontdata:setGlyph(c,x,y,cw,ch,cx,cy,w,h)
		end
	end
end

Font.get = function(name, size)
	if not Fonts[name] then return nil end
	local delta, mindelta, mindeltafont, maxsizefont
	mindelta = 99999
	local maxsize = 0
	for k, v in ipairs(Fonts[name]) do
		if math.abs(v.size - size) < mindelta then 
			mindelta = math.abs(v.size - size) 
			mindeltafont = v
		end
		if v.size + 0 > maxsize then
			maxsize = v.size + 0
			maxsizefont = v
		end
	end
	if mindelta <= 1 or mindeltafont.size <= 9 then return mindeltafont.data end
	return maxsizefont.data
end
