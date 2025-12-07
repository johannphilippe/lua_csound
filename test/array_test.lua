myclass = {} -- first create an empty table

function myclass:new(o) -- base constructor for lua classes
	o=o or {}
	setmetatable(o,self)
	self.__index = self
	return o
end

--[[
    Sync Sine and Cosine
]]--
function myclass:init(nothing) -- then the init function allows to store and initialize members
    self.kout = {0, 0}
    self.aout = {{}, {}}
    self.phase = 0
end

function myclass:kperf(freq) -- when performance doesn't need arguments, it is still necessary to pass at leas one dummy argument
    self.freq = 2 * math.pi * freq
    self.phase_incr = self.freq / (self.sr / self.ksmps)
    self.kout[1] = math.sin(self.phase)
    self.kout[2] = math.cos(self.phase)
    self.phase = self.phase + self.phase_incr
    if(self.phase > 2.0 * math.pi) then 
        self.phase = self.phase - 2.0 * math.pi
    end
    self.kout[0] = 1.0
    return self.kout
end

function myclass:aperf(freq) -- if opcode is running at k-rate, it will call kperf, else if will call aperf (ksmps times) every k-pass.  
    for i = 1, self.ksmps do 
        self.freq = 2 * math.pi * freq[i]
        self.phase_incr = self.freq / (self.sr)
        self.aout[1][i] = math.sin(self.phase)
        self.aout[2][i] = math.cos(self.phase)
        self.phase = self.phase + self.phase_incr
        if(self.phase > 2.0 * math.pi) then 
            self.phase = self.phase - 2.0 * math.pi
        end
    end
    return self.aout
end