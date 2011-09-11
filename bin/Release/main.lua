if scrupp then 
	scrupp.init('Test', 800, 600, 32, false, true, false)
	scrupp.setDelta(0)
	G=scrupp
end
require 'lib.lquery'
require 'lib.table'
if scrupp then 
	require 'data.liberation'
	lQuery.addhook(S.clear)
end
local loadShader = function(name)
	return S.newShader('data/shader/'..name..'/'..name..'.vert', 'data/shader/'..name..'/'..name..'.frag')
end
local shader = loadShader('julia')
--~ print(shader)
--~ E:new(screen):image('data/background.dds')
--~ local mask = S.newImage('data/menu-buttonmask.png')
local butt = S.newImage('data/background.dds')
E:new(screen):draw(function(s)
	S.useShader(shader)
	S.setUniformf('time', time)
	--~ S.setUniformi('tex0', 0)
	--~ S.rectangle(s.x,s.y,s.w,s.h)
	butt:draw(s.x,s.y,0,s.w,s.h)
	S.useShader(0)
end):size(800,600):draggable()
--~ ui = require 'lib.lqueryui'
--~ def = ui.loadStyle('default')
--~ oxy = ui.loadStyle('oxygen')
--~ style = oxy
--~ ui.setDefaultStyle('layout', style.layout)
--~ ui.setDefaultStyle('button', style.button)
--~ ui.setDefaultStyle('slider', style.slider)
--~ 
--~ 
txt = E:new(screen):draw(function(s)
if love then 
	G.print("FPS: " .. love.timer.getFPS().."\nMemory: " .. gcinfo(),s.x,0)
else
	Fonts["Liberation Sans bold"][9]:select()
	scrupp.print("FPS: " .. scrupp.fps().."\nMemory: " .. gcinfo(),s.x,0)
end
end):color(0,0,0,255)
--~ 
--~ layout = ui.newLayout(screen):draggable()
--~ 
--~ ui.newButton(layout, 'Fluid 1')
--~ ui.newButton(layout, 'Fluid 2')
--~ ui.newButton(layout, 'Fluid 3')
--~ layout1 = ui.newLayout(layout, 'horizontal')
--~ ui.newButton(layout1, 'Fluid 3')
--~ ui.newButton(layout1, 'Fluid 3')
--~ ui.newButton(layout1, 'Fluid 3')
--~ ui.newSlider(layout, 'Slider')
--~ layout:size(200,200):move(-200, 100):animate({x = 300})
