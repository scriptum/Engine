scrupp.init('Test', 800, 600, 32, false, true, true)
--~ scrupp.setDelta(0)
--~ scrupp.scale(1,1)
require 'lib'
--~ require 'data/font'

require 'lib/console'

--~ bg = E:new(screen):image('data/background.dds'):draggable():size(800, 600)
--~ bg.tw = 512
--~ bg.th = 512
--img = G.newImage('krig_Aqua_button.dds')
--for i = 1, 10 do E:new(screen):image(img):draggable():animate({x = math.random(0, 800), y = math.random(0, 600)}, 1) end

im = S.newImage('data/menu-button.png', true)
E:new(screen):image(im, true):animate({qx=256},{loop=true}):delay({loop = true}):animate({qx=0},{loop=true}):animate({w=800, h=600}, {queue = "grow", speed = 5})
--~ E:new(screen):draw(function()
--~ for i = 1, 50 do

--~ end
--~ scrupp.print("FPS: " .. scrupp.fps(),0,0)
--~ for i = 1, 50 do
--~ scrupp.print("Привет, как дела? Привет, как дела? Привет, как дела? \nПривет, как дела? Привет, как дела? Привет, как дела? Привет, как дела? Привет, как дела? Привет, как дела? Привет, как дела? Привет, как дела?", 0,i * scrupp.getStringHeight()*2)
--~ end
--~ end):color(255,255,255,255)

