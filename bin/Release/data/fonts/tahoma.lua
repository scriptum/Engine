if not Fonts then Fonts = {} end
if not FontTextures then FontTextures = {} end
local d
local t
t = S.newImage("data/fonts/tahoma.png")
table.insert(FontTextures, t)
if not Fonts["Tahoma"] then Fonts["Tahoma"] = {} end
d = scrupp.addFont(t)
Fonts["Tahoma"][8] = d
d:setGlyph(32,0,0,0,0,3,13,3,13)
d:setGlyph(97,123,37,5,6,0,5,6,13)
d:setGlyph(98,63,69,5,9,0,2,6,13)
d:setGlyph(99,120,98,4,6,0,5,5,13)
d:setGlyph(100,58,69,5,9,0,2,6,13)
d:setGlyph(101,74,122,5,6,0,5,6,13)
d:setGlyph(102,3,79,3,9,0,2,4,13)
d:setGlyph(103,70,94,5,8,0,5,6,13)
d:setGlyph(104,98,67,5,9,0,2,6,13)
d:setGlyph(105,118,91,1,8,0,3,2,13)
d:setGlyph(106,72,23,2,10,0,3,3,13)
d:setGlyph(107,78,69,5,9,0,2,5,13)
d:setGlyph(108,8,79,1,9,0,2,2,13)
d:setGlyph(109,88,116,7,6,0,5,8,13)
d:setGlyph(110,123,82,5,6,0,5,6,13)
d:setGlyph(111,118,113,5,6,0,5,6,13)
d:setGlyph(112,103,84,5,8,0,5,6,13)
d:setGlyph(113,80,94,5,8,0,5,6,13)
d:setGlyph(114,125,21,3,6,0,5,4,13)
d:setGlyph(115,124,116,4,6,0,5,5,13)
d:setGlyph(116,110,92,3,8,0,3,4,13)
d:setGlyph(117,114,120,5,6,0,5,6,13)
d:setGlyph(118,104,120,5,6,0,5,6,13)
d:setGlyph(119,67,116,7,6,0,5,8,13)
d:setGlyph(120,59,122,5,6,0,5,6,13)
d:setGlyph(121,35,95,5,8,0,5,6,13)
d:setGlyph(122,124,122,4,6,0,5,5,13)
d:setGlyph(65,113,75,6,8,0,3,7,13)
d:setGlyph(66,5,96,5,8,0,3,6,13)
d:setGlyph(67,12,87,6,8,0,3,7,13)
d:setGlyph(68,119,21,6,8,0,3,7,13)
d:setGlyph(69,83,86,5,8,0,3,6,13)
d:setGlyph(70,93,84,5,8,0,3,6,13)
d:setGlyph(71,6,88,6,8,0,3,7,13)
d:setGlyph(72,24,87,6,8,0,3,7,13)
d:setGlyph(73,113,91,3,8,0,3,4,13)
d:setGlyph(74,99,92,4,8,0,3,5,13)
d:setGlyph(75,118,83,5,8,0,3,6,13)
d:setGlyph(76,103,92,4,8,0,3,5,13)
d:setGlyph(77,79,78,7,8,0,3,8,13)
d:setGlyph(78,107,76,6,8,0,3,7,13)
d:setGlyph(79,93,76,7,8,0,3,8,13)
d:setGlyph(80,108,84,5,8,0,3,6,13)
d:setGlyph(81,120,11,7,10,0,3,8,13)
d:setGlyph(82,18,87,6,8,0,3,7,13)
d:setGlyph(83,88,86,5,8,0,3,6,13)
d:setGlyph(84,98,84,5,8,0,3,6,13)
d:setGlyph(85,0,88,6,8,0,3,7,13)
d:setGlyph(86,85,94,5,8,0,3,6,13)
d:setGlyph(87,31,79,9,8,0,3,10,13)
d:setGlyph(88,45,94,5,8,0,3,6,13)
d:setGlyph(89,60,94,5,8,0,3,6,13)
d:setGlyph(90,50,94,5,8,0,3,6,13)
d:setGlyph(48,55,94,5,8,0,3,6,13)
d:setGlyph(49,107,92,3,8,1,3,6,13)
d:setGlyph(50,40,95,5,8,0,3,6,13)
d:setGlyph(51,30,95,5,8,0,3,6,13)
d:setGlyph(52,0,96,5,8,0,3,6,13)
d:setGlyph(53,10,96,5,8,0,3,6,13)
d:setGlyph(54,25,95,5,8,0,3,6,13)
d:setGlyph(55,15,95,5,8,0,3,6,13)
d:setGlyph(56,20,95,5,8,0,3,6,13)
d:setGlyph(57,65,94,5,8,0,3,6,13)
d:setGlyph(46,19,125,1,2,1,9,4,13)
d:setGlyph(44,45,124,2,4,0,9,4,13)
d:setGlyph(33,119,75,1,8,1,3,4,13)
d:setGlyph(63,95,92,4,8,0,3,5,13)
d:setGlyph(45,32,51,3,1,0,7,4,13)
d:setGlyph(43,78,102,7,7,0,4,8,13)
d:setGlyph(92,60,12,3,11,0,2,4,13)
d:setGlyph(47,54,12,3,11,0,2,4,13)
d:setGlyph(40,57,12,3,11,0,2,4,13)
d:setGlyph(41,66,12,3,11,0,2,4,13)
d:setGlyph(58,126,0,1,6,1,5,4,13)
d:setGlyph(59,116,91,2,8,0,5,4,13)
d:setGlyph(37,12,79,10,8,0,3,11,13)
d:setGlyph(38,100,76,7,8,0,3,7,13)
d:setGlyph(96,17,125,2,2,1,2,6,13)
d:setGlyph(39,16,125,1,3,0,2,2,13)
d:setGlyph(42,123,43,5,5,0,2,6,13)
d:setGlyph(35,72,78,7,8,0,3,8,13)
d:setGlyph(36,41,12,5,11,0,2,6,13)
d:setGlyph(61,0,125,7,3,0,6,8,13)
d:setGlyph(91,63,12,3,11,0,2,4,13)
d:setGlyph(93,69,12,3,11,0,2,4,13)
d:setGlyph(64,79,31,9,9,0,3,10,13)
d:setGlyph(94,28,124,7,4,0,3,8,13)
d:setGlyph(123,46,12,4,11,0,2,5,13)
d:setGlyph(125,50,12,4,11,0,2,5,13)
d:setGlyph(95,24,33,6,1,0,12,6,13)
d:setGlyph(126,48,124,7,3,0,6,8,13)
d:setGlyph(34,13,125,3,3,0,2,4,13)
d:setGlyph(62,78,109,6,7,1,4,8,13)
d:setGlyph(60,84,109,6,7,1,4,8,13)
d:setGlyph(150,24,33,6,1,0,7,6,13)
d:setGlyph(151,104,127,10,1,0,7,10,13)
d:setGlyph(124,74,12,1,11,1,2,4,13)
d:setGlyph(224,123,37,5,6,0,5,6,13)
d:setGlyph(225,73,69,5,9,0,2,6,13)
d:setGlyph(226,64,122,5,6,0,5,6,13)
d:setGlyph(227,123,110,4,6,0,5,5,13)
d:setGlyph(228,90,109,6,7,0,5,7,13)
d:setGlyph(229,74,122,5,6,0,5,6,13)
d:setGlyph(184,90,94,5,8,0,3,6,13)
d:setGlyph(230,81,116,7,6,0,5,8,13)
d:setGlyph(231,124,98,4,6,0,5,5,13)
d:setGlyph(232,113,114,5,6,0,5,6,13)
d:setGlyph(233,93,67,5,9,0,2,6,13)
d:setGlyph(234,79,122,5,6,0,5,6,13)
d:setGlyph(235,84,122,5,6,0,5,6,13)
d:setGlyph(236,119,119,5,6,0,5,6,13)
d:setGlyph(237,69,122,5,6,0,5,6,13)
d:setGlyph(238,118,113,5,6,0,5,6,13)
d:setGlyph(239,108,114,5,6,0,5,6,13)
d:setGlyph(240,103,84,5,8,0,5,6,13)
d:setGlyph(241,120,98,4,6,0,5,5,13)
d:setGlyph(242,99,122,5,6,0,5,6,13)
d:setGlyph(243,35,95,5,8,0,5,6,13)
d:setGlyph(244,8,13,7,11,0,2,8,13)
d:setGlyph(245,59,122,5,6,0,5,6,13)
d:setGlyph(246,20,117,5,7,0,5,6,13)
d:setGlyph(247,89,122,5,6,0,5,6,13)
d:setGlyph(248,95,116,7,6,0,5,8,13)
d:setGlyph(249,62,102,8,7,0,5,9,13)
d:setGlyph(250,102,114,6,6,0,5,7,13)
d:setGlyph(251,74,116,7,6,0,5,8,13)
d:setGlyph(252,109,120,5,6,0,5,6,13)
d:setGlyph(253,121,104,4,6,0,5,5,13)
d:setGlyph(254,59,116,8,6,0,5,9,13)
d:setGlyph(255,94,122,5,6,0,5,6,13)
d:setGlyph(192,113,75,6,8,0,3,7,13)
d:setGlyph(193,123,29,5,8,0,3,6,13)
d:setGlyph(194,5,96,5,8,0,3,6,13)
d:setGlyph(195,113,83,5,8,0,3,6,13)
d:setGlyph(196,106,11,7,10,0,3,8,13)
d:setGlyph(197,83,86,5,8,0,3,6,13)
d:setGlyph(168,58,23,5,10,0,1,6,13)
d:setGlyph(198,40,78,9,8,0,3,10,13)
d:setGlyph(199,78,86,5,8,0,3,6,13)
d:setGlyph(200,42,86,6,8,0,3,7,13)
d:setGlyph(201,29,12,6,11,0,0,7,13)
d:setGlyph(202,54,86,6,8,0,3,7,13)
d:setGlyph(203,60,86,6,8,0,3,7,13)
d:setGlyph(204,79,78,7,8,0,3,8,13)
d:setGlyph(205,24,87,6,8,0,3,7,13)
d:setGlyph(206,93,76,7,8,0,3,8,13)
d:setGlyph(207,72,86,6,8,0,3,7,13)
d:setGlyph(208,108,84,5,8,0,3,6,13)
d:setGlyph(209,12,87,6,8,0,3,7,13)
d:setGlyph(210,98,84,5,8,0,3,6,13)
d:setGlyph(211,30,87,6,8,0,3,7,13)
d:setGlyph(212,86,78,7,8,0,3,8,13)
d:setGlyph(213,45,94,5,8,0,3,6,13)
d:setGlyph(215,66,86,6,8,0,3,7,13)
d:setGlyph(214,113,11,7,10,0,3,8,13)
d:setGlyph(216,49,78,9,8,0,3,10,13)
d:setGlyph(217,96,11,10,10,0,3,11,13)
d:setGlyph(218,65,78,7,8,0,3,8,13)
d:setGlyph(219,58,78,7,8,0,3,8,13)
d:setGlyph(220,75,94,5,8,0,3,6,13)
d:setGlyph(221,36,87,6,8,0,3,7,13)
d:setGlyph(222,22,79,9,8,0,3,10,13)
d:setGlyph(223,48,86,6,8,0,3,7,13)
if not Fonts["Tahoma"] then Fonts["Tahoma"] = {} end
d = scrupp.addFont(t)
Fonts["Tahoma"][9] = d
d:setGlyph(32,0,0,0,0,4,14,4,14)
d:setGlyph(97,0,118,5,7,0,5,6,14)
d:setGlyph(98,6,24,6,10,0,2,7,14)
d:setGlyph(99,106,107,5,7,0,5,6,14)
d:setGlyph(100,24,23,6,10,0,2,7,14)
d:setGlyph(101,12,111,6,7,0,5,7,14)
d:setGlyph(102,68,23,4,10,0,2,4,14)
d:setGlyph(103,30,70,6,9,0,5,7,14)
d:setGlyph(104,30,23,6,10,0,2,7,14)
d:setGlyph(105,9,79,1,9,0,3,2,14)
d:setGlyph(106,72,12,2,11,0,3,3,14)
d:setGlyph(107,53,23,5,10,0,2,6,14)
d:setGlyph(108,74,23,1,10,0,2,2,14)
d:setGlyph(109,27,103,9,7,0,5,10,14)
d:setGlyph(110,120,75,6,7,0,5,7,14)
d:setGlyph(111,48,109,6,7,0,5,7,14)
d:setGlyph(112,84,58,6,9,0,5,7,14)
d:setGlyph(113,36,69,6,9,0,5,7,14)
d:setGlyph(114,55,116,3,7,0,5,4,14)
d:setGlyph(115,39,117,4,7,0,5,5,14)
d:setGlyph(116,122,66,4,9,0,3,5,14)
d:setGlyph(117,66,109,6,7,0,5,7,14)
d:setGlyph(118,111,107,5,7,0,5,6,14)
d:setGlyph(119,0,104,9,7,0,5,10,14)
d:setGlyph(120,101,107,5,7,0,5,6,14)
d:setGlyph(121,53,69,5,9,0,5,6,14)
d:setGlyph(122,47,117,4,7,0,5,5,14)
d:setGlyph(65,81,40,7,9,0,3,8,14)
d:setGlyph(66,78,60,6,9,0,3,7,14)
d:setGlyph(67,60,60,6,9,0,3,7,14)
d:setGlyph(68,102,39,7,9,0,3,8,14)
d:setGlyph(69,66,60,6,9,0,3,7,14)
d:setGlyph(70,103,67,5,9,0,3,6,14)
d:setGlyph(71,60,42,7,9,0,3,8,14)
d:setGlyph(72,109,39,7,9,0,3,8,14)
d:setGlyph(73,0,79,3,9,0,3,4,14)
d:setGlyph(74,118,66,4,9,0,3,5,14)
d:setGlyph(75,98,49,6,9,0,3,7,14)
d:setGlyph(76,68,69,5,9,0,3,6,14)
d:setGlyph(77,16,43,8,9,0,3,9,14)
d:setGlyph(78,67,42,7,9,0,3,8,14)
d:setGlyph(79,113,30,8,9,0,3,9,14)
d:setGlyph(80,42,60,6,9,0,3,7,14)
d:setGlyph(81,110,0,8,11,0,3,9,14)
d:setGlyph(82,48,60,6,9,0,3,7,14)
d:setGlyph(83,18,70,6,9,0,3,7,14)
d:setGlyph(84,74,42,7,9,0,3,8,14)
d:setGlyph(85,88,40,7,9,0,3,8,14)
d:setGlyph(86,95,40,7,9,0,3,8,14)
d:setGlyph(87,108,21,11,9,0,3,12,14)
d:setGlyph(88,30,61,6,9,0,3,7,14)
d:setGlyph(89,116,39,7,9,0,3,8,14)
d:setGlyph(90,110,48,6,9,0,3,7,14)
d:setGlyph(48,116,48,6,9,0,3,7,14)
d:setGlyph(49,108,66,5,9,1,3,7,14)
d:setGlyph(50,104,48,6,9,0,3,7,14)
d:setGlyph(51,122,48,6,9,0,3,7,14)
d:setGlyph(52,6,61,6,9,0,3,7,14)
d:setGlyph(53,18,61,6,9,0,3,7,14)
d:setGlyph(54,24,61,6,9,0,3,7,14)
d:setGlyph(55,12,61,6,9,0,3,7,14)
d:setGlyph(56,0,61,6,9,0,3,7,14)
d:setGlyph(57,36,60,6,9,0,3,7,14)
d:setGlyph(46,19,125,1,2,1,10,4,14)
d:setGlyph(44,45,124,2,4,0,10,4,14)
d:setGlyph(33,10,79,1,9,1,3,4,14)
d:setGlyph(63,48,69,5,9,0,3,6,14)
d:setGlyph(45,32,51,3,1,0,8,4,14)
d:setGlyph(43,78,102,7,7,0,5,8,14)
d:setGlyph(92,68,0,4,12,0,2,5,14)
d:setGlyph(47,64,0,4,12,0,2,5,14)
d:setGlyph(40,84,0,3,12,1,2,5,14)
d:setGlyph(41,72,0,3,12,1,2,5,14)
d:setGlyph(58,58,116,1,7,1,5,4,14)
d:setGlyph(59,6,79,2,9,0,5,4,14)
d:setGlyph(37,0,34,11,9,0,3,12,14)
d:setGlyph(38,14,52,7,9,0,3,8,14)
d:setGlyph(96,17,125,2,2,1,2,7,14)
d:setGlyph(39,16,125,1,3,1,2,3,14)
d:setGlyph(42,123,43,5,5,1,2,7,14)
d:setGlyph(35,28,52,7,9,1,3,9,14)
d:setGlyph(36,49,0,5,12,1,2,7,14)
d:setGlyph(61,7,125,6,3,1,6,9,14)
d:setGlyph(91,78,0,3,12,1,2,5,14)
d:setGlyph(93,81,0,3,12,1,2,5,14)
d:setGlyph(64,86,12,10,10,0,3,11,14)
d:setGlyph(94,28,124,7,4,1,3,9,14)
d:setGlyph(123,44,0,5,12,0,2,6,14)
d:setGlyph(125,39,0,5,12,0,2,6,14)
d:setGlyph(95,114,127,7,1,0,13,7,14)
d:setGlyph(126,48,124,7,3,1,7,9,14)
d:setGlyph(34,13,125,3,3,1,2,5,14)
d:setGlyph(62,78,109,6,7,1,5,9,14)
d:setGlyph(60,84,109,6,7,1,5,9,14)
d:setGlyph(150,114,127,7,1,0,8,7,14)
d:setGlyph(151,48,127,11,1,0,8,11,14)
d:setGlyph(124,87,0,1,12,2,2,5,14)
d:setGlyph(224,0,118,5,7,0,5,6,14)
d:setGlyph(225,12,24,6,10,0,2,7,14)
d:setGlyph(226,10,118,5,7,0,5,6,14)
d:setGlyph(227,43,117,4,7,0,5,5,14)
d:setGlyph(228,42,69,6,9,0,5,7,14)
d:setGlyph(229,12,111,6,7,0,5,7,14)
d:setGlyph(184,72,60,6,9,0,3,7,14)
d:setGlyph(230,45,102,9,7,0,5,10,14)
d:setGlyph(231,51,116,4,7,0,5,5,14)
d:setGlyph(232,0,111,6,7,0,5,7,14)
d:setGlyph(233,18,24,6,10,0,2,7,14)
d:setGlyph(234,24,110,6,7,0,5,6,14)
d:setGlyph(235,30,110,6,7,0,5,7,14)
d:setGlyph(236,85,102,7,7,0,5,8,14)
d:setGlyph(237,18,110,6,7,0,5,7,14)
d:setGlyph(238,48,109,6,7,0,5,7,14)
d:setGlyph(239,6,111,6,7,0,5,7,14)
d:setGlyph(240,84,58,6,9,0,5,7,14)
d:setGlyph(241,106,107,5,7,0,5,6,14)
d:setGlyph(242,96,109,5,7,0,5,6,14)
d:setGlyph(243,53,69,5,9,0,5,6,14)
d:setGlyph(244,17,0,9,12,0,2,10,14)
d:setGlyph(245,101,107,5,7,0,5,6,14)
d:setGlyph(246,96,58,6,9,0,5,7,14)
d:setGlyph(247,121,91,5,7,1,5,7,14)
d:setGlyph(248,113,99,7,7,0,5,8,14)
d:setGlyph(249,8,43,8,9,0,5,9,14)
d:setGlyph(250,60,109,6,7,0,5,7,14)
d:setGlyph(251,92,102,7,7,0,5,8,14)
d:setGlyph(252,15,118,5,7,0,5,6,14)
d:setGlyph(253,5,118,5,7,0,5,6,14)
d:setGlyph(254,70,102,8,7,0,5,9,14)
d:setGlyph(255,42,110,6,7,0,5,7,14)
d:setGlyph(192,81,40,7,9,0,3,8,14)
d:setGlyph(193,90,58,6,9,0,3,7,14)
d:setGlyph(194,78,60,6,9,0,3,7,14)
d:setGlyph(195,88,67,5,9,0,3,6,14)
d:setGlyph(196,22,12,7,11,0,3,8,14)
d:setGlyph(197,66,60,6,9,0,3,7,14)
d:setGlyph(168,35,12,6,11,0,1,7,14)
d:setGlyph(198,75,22,11,9,0,3,12,14)
d:setGlyph(199,83,69,5,9,0,3,6,14)
d:setGlyph(200,77,51,7,9,0,3,8,14)
d:setGlyph(201,26,0,7,12,0,0,8,14)
d:setGlyph(202,54,60,6,9,0,3,7,14)
d:setGlyph(203,84,49,7,9,0,3,8,14)
d:setGlyph(204,16,43,8,9,0,3,9,14)
d:setGlyph(205,109,39,7,9,0,3,8,14)
d:setGlyph(206,113,30,8,9,0,3,9,14)
d:setGlyph(207,70,51,7,9,0,3,8,14)
d:setGlyph(208,42,60,6,9,0,3,7,14)
d:setGlyph(209,60,60,6,9,0,3,7,14)
d:setGlyph(210,74,42,7,9,0,3,8,14)
d:setGlyph(211,102,58,6,9,0,3,7,14)
d:setGlyph(212,70,33,9,9,0,3,10,14)
d:setGlyph(213,30,61,6,9,0,3,7,14)
d:setGlyph(215,46,42,7,9,0,3,8,14)
d:setGlyph(214,15,13,7,11,0,3,8,14)
d:setGlyph(216,88,31,9,9,0,3,10,14)
d:setGlyph(217,100,0,10,11,0,3,11,14)
d:setGlyph(218,63,51,7,9,0,3,8,14)
d:setGlyph(219,105,30,8,9,0,3,9,14)
d:setGlyph(220,120,57,6,9,0,3,7,14)
d:setGlyph(221,0,70,6,9,0,3,7,14)
d:setGlyph(222,22,34,10,9,0,3,11,14)
d:setGlyph(223,12,70,6,9,0,3,7,14)
if not Fonts["Tahoma"] then Fonts["Tahoma"] = {} end
d = scrupp.addFont(t)
Fonts["Tahoma"][10] = d
d:setGlyph(32,0,0,0,0,4,16,4,16)
d:setGlyph(97,72,109,6,7,0,6,7,16)
d:setGlyph(98,6,24,6,10,0,3,7,16)
d:setGlyph(99,106,107,5,7,0,6,6,16)
d:setGlyph(100,24,23,6,10,0,3,7,16)
d:setGlyph(101,12,111,6,7,0,6,7,16)
d:setGlyph(102,48,23,5,10,0,3,5,16)
d:setGlyph(103,36,23,6,10,0,6,7,16)
d:setGlyph(104,30,23,6,10,0,3,7,16)
d:setGlyph(105,9,79,1,9,1,4,3,16)
d:setGlyph(106,75,0,3,12,0,4,4,16)
d:setGlyph(107,53,23,5,10,0,3,6,16)
d:setGlyph(108,74,23,1,10,1,3,3,16)
d:setGlyph(109,9,104,9,7,1,6,11,16)
d:setGlyph(110,120,75,6,7,0,6,7,16)
d:setGlyph(111,48,109,6,7,0,6,7,16)
d:setGlyph(112,0,24,6,10,0,6,7,16)
d:setGlyph(113,42,23,6,10,0,6,7,16)
d:setGlyph(114,35,117,4,7,0,6,5,16)
d:setGlyph(115,25,117,5,7,0,6,6,16)
d:setGlyph(116,122,66,4,9,0,4,4,16)
d:setGlyph(117,66,109,6,7,0,6,7,16)
d:setGlyph(118,111,107,5,7,0,6,6,16)
d:setGlyph(119,0,104,9,7,0,6,10,16)
d:setGlyph(120,101,107,5,7,0,6,6,16)
d:setGlyph(121,63,23,5,10,0,6,6,16)
d:setGlyph(122,30,117,5,7,0,6,6,16)
d:setGlyph(65,21,52,7,9,0,4,8,16)
d:setGlyph(66,6,70,6,9,0,4,7,16)
d:setGlyph(67,7,52,7,9,0,4,8,16)
d:setGlyph(68,102,39,7,9,0,4,8,16)
d:setGlyph(69,66,60,6,9,0,4,7,16)
d:setGlyph(70,114,57,6,9,0,4,7,16)
d:setGlyph(71,42,51,7,9,0,4,8,16)
d:setGlyph(72,109,39,7,9,0,4,8,16)
d:setGlyph(73,0,79,3,9,0,4,4,16)
d:setGlyph(74,118,66,4,9,0,4,5,16)
d:setGlyph(75,98,49,6,9,0,4,7,16)
d:setGlyph(76,68,69,5,9,0,4,6,16)
d:setGlyph(77,61,33,9,9,0,4,10,16)
d:setGlyph(78,67,42,7,9,0,4,8,16)
d:setGlyph(79,113,30,8,9,0,4,9,16)
d:setGlyph(80,108,57,6,9,0,4,7,16)
d:setGlyph(81,110,0,8,11,0,4,9,16)
d:setGlyph(82,49,51,7,9,0,4,8,16)
d:setGlyph(83,35,51,7,9,0,4,8,16)
d:setGlyph(84,74,42,7,9,0,4,8,16)
d:setGlyph(85,88,40,7,9,0,4,8,16)
d:setGlyph(86,56,51,7,9,0,4,8,16)
d:setGlyph(87,97,21,11,9,0,4,12,16)
d:setGlyph(88,39,42,7,9,0,4,8,16)
d:setGlyph(89,116,39,7,9,0,4,7,16)
d:setGlyph(90,110,48,6,9,0,4,7,16)
d:setGlyph(48,116,48,6,9,0,4,7,16)
d:setGlyph(49,108,66,5,9,1,4,7,16)
d:setGlyph(50,104,48,6,9,0,4,7,16)
d:setGlyph(51,122,48,6,9,0,4,7,16)
d:setGlyph(52,6,61,6,9,0,4,7,16)
d:setGlyph(53,18,61,6,9,0,4,7,16)
d:setGlyph(54,24,61,6,9,0,4,7,16)
d:setGlyph(55,12,61,6,9,0,4,7,16)
d:setGlyph(56,0,61,6,9,0,4,7,16)
d:setGlyph(57,36,60,6,9,0,4,7,16)
d:setGlyph(46,19,125,1,2,1,11,4,16)
d:setGlyph(44,45,124,2,4,0,11,4,16)
d:setGlyph(33,11,79,1,9,1,4,4,16)
d:setGlyph(63,113,66,5,9,0,4,6,16)
d:setGlyph(45,17,12,4,1,0,8,5,16)
d:setGlyph(43,78,102,7,7,0,5,8,16)
d:setGlyph(92,59,0,5,12,0,3,5,16)
d:setGlyph(47,54,0,5,12,0,3,5,16)
d:setGlyph(40,9,0,4,13,1,3,5,16)
d:setGlyph(41,13,0,4,13,0,3,5,16)
d:setGlyph(58,58,116,1,7,2,6,5,16)
d:setGlyph(59,6,79,2,9,1,6,5,16)
d:setGlyph(37,42,33,10,9,1,4,12,16)
d:setGlyph(38,97,30,8,9,0,4,9,16)
d:setGlyph(96,17,125,2,2,3,3,7,16)
d:setGlyph(39,47,124,1,4,1,3,3,16)
d:setGlyph(42,123,43,5,5,1,3,8,16)
d:setGlyph(35,28,52,7,9,1,4,9,16)
d:setGlyph(36,33,0,6,12,0,3,7,16)
d:setGlyph(61,35,124,7,4,1,7,9,16)
d:setGlyph(91,78,0,3,12,1,3,5,16)
d:setGlyph(93,81,0,3,12,1,3,5,16)
d:setGlyph(64,75,12,11,10,0,4,12,16)
d:setGlyph(94,20,124,8,4,1,4,9,16)
d:setGlyph(123,44,0,5,12,0,3,6,16)
d:setGlyph(125,39,0,5,12,0,3,6,16)
d:setGlyph(95,114,127,7,1,0,14,7,16)
d:setGlyph(126,48,124,7,3,1,7,9,16)
d:setGlyph(34,42,124,3,4,1,3,5,16)
d:setGlyph(62,106,100,7,7,1,5,9,16)
d:setGlyph(60,99,100,7,7,1,5,9,16)
d:setGlyph(150,114,127,7,1,0,8,7,16)
d:setGlyph(151,104,126,12,1,0,8,12,16)
d:setGlyph(124,87,0,1,12,2,3,6,16)
d:setGlyph(224,72,109,6,7,0,6,7,16)
d:setGlyph(225,12,24,6,10,0,3,7,16)
d:setGlyph(226,36,110,6,7,0,6,7,16)
d:setGlyph(227,43,117,4,7,0,6,5,16)
d:setGlyph(228,42,69,6,9,0,6,7,16)
d:setGlyph(229,12,111,6,7,0,6,7,16)
d:setGlyph(184,72,60,6,9,0,4,7,16)
d:setGlyph(230,45,102,9,7,0,6,10,16)
d:setGlyph(231,116,106,5,7,0,6,6,16)
d:setGlyph(232,0,111,6,7,0,6,7,16)
d:setGlyph(233,18,24,6,10,0,3,7,16)
d:setGlyph(234,24,110,6,7,0,6,6,16)
d:setGlyph(235,30,110,6,7,0,6,7,16)
d:setGlyph(236,85,102,7,7,0,6,8,16)
d:setGlyph(237,18,110,6,7,0,6,7,16)
d:setGlyph(238,48,109,6,7,0,6,7,16)
d:setGlyph(239,6,111,6,7,0,6,7,16)
d:setGlyph(240,0,24,6,10,0,6,7,16)
d:setGlyph(241,106,107,5,7,0,6,6,16)
d:setGlyph(242,96,109,5,7,0,6,6,16)
d:setGlyph(243,63,23,5,10,0,6,6,16)
d:setGlyph(244,0,0,9,13,0,3,10,16)
d:setGlyph(245,101,107,5,7,0,6,6,16)
d:setGlyph(246,96,58,6,9,0,6,7,16)
d:setGlyph(247,121,91,5,7,1,6,7,16)
d:setGlyph(248,36,103,9,7,0,6,10,16)
d:setGlyph(249,32,33,10,9,0,6,11,16)
d:setGlyph(250,60,109,6,7,0,6,7,16)
d:setGlyph(251,54,102,8,7,0,6,9,16)
d:setGlyph(252,54,109,6,7,0,6,7,16)
d:setGlyph(253,5,118,5,7,0,6,6,16)
d:setGlyph(254,18,103,9,7,0,6,10,16)
d:setGlyph(255,42,110,6,7,0,6,7,16)
d:setGlyph(192,21,52,7,9,0,4,8,16)
d:setGlyph(193,0,52,7,9,0,4,8,16)
d:setGlyph(194,6,70,6,9,0,4,7,16)
d:setGlyph(195,24,70,6,9,0,4,7,16)
d:setGlyph(196,118,0,8,11,0,4,9,16)
d:setGlyph(197,66,60,6,9,0,4,7,16)
d:setGlyph(168,35,12,6,11,0,2,7,16)
d:setGlyph(198,75,22,11,9,0,4,12,16)
d:setGlyph(199,122,48,6,9,0,4,7,16)
d:setGlyph(200,77,51,7,9,0,4,8,16)
d:setGlyph(201,26,0,7,12,0,1,8,16)
d:setGlyph(202,54,60,6,9,0,4,7,16)
d:setGlyph(203,0,43,8,9,0,4,9,16)
d:setGlyph(204,61,33,9,9,0,4,10,16)
d:setGlyph(205,109,39,7,9,0,4,8,16)
d:setGlyph(206,113,30,8,9,0,4,9,16)
d:setGlyph(207,70,51,7,9,0,4,8,16)
d:setGlyph(208,108,57,6,9,0,4,7,16)
d:setGlyph(209,7,52,7,9,0,4,8,16)
d:setGlyph(210,74,42,7,9,0,4,8,16)
d:setGlyph(211,102,58,6,9,0,4,7,16)
d:setGlyph(212,70,33,9,9,0,4,10,16)
d:setGlyph(213,39,42,7,9,0,4,8,16)
d:setGlyph(215,46,42,7,9,0,4,8,16)
d:setGlyph(214,0,13,8,11,0,4,9,16)
d:setGlyph(216,11,34,11,9,0,4,12,16)
d:setGlyph(217,88,0,12,11,0,4,12,16)
d:setGlyph(218,24,43,8,9,0,4,9,16)
d:setGlyph(219,52,33,9,9,0,4,10,16)
d:setGlyph(220,32,42,7,9,0,4,8,16)
d:setGlyph(221,53,42,7,9,0,4,8,16)
d:setGlyph(222,86,22,11,9,0,4,12,16)
d:setGlyph(223,91,49,7,9,0,4,8,16)
