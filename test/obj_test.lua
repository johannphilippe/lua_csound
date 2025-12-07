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
    self.kout = 0
    self.aout = {}
    self.phase = 0
end

function myclass:kperf(freq)
    self.freq = 2 * math.pi * freq
    self.phase_incr = self.freq / (self.sr / self.ksmps)
    self.kout = math.sin(self.phase)
    self.phase = self.phase + self.phase_incr
    if(self.phase > 2.0 * math.pi) then 
        self.phase = self.phase - 2.0 * math.pi
    end
    return self.kout
end

function myclass:aperf(freq) 
    for i = 1, self.ksmps do 
        self.freq = 2 * math.pi * freq[i]
        self.phase_incr = self.freq / (self.sr)
        self.aout[i] = math.sin(self.phase)
        self.phase = self.phase + self.phase_incr
        if(self.phase > 2.0 * math.pi) then 
            self.phase = self.phase - 2.0 * math.pi
        end
    end
    return self.aout
end