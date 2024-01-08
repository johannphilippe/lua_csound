myclass = {} -- first create an empty table

-- Base constructor for lua classes
function myclass:new(o) 
	o=o or {}
	setmetatable(o,self)
	self.__index = self
	return o
end
    
-- Init function allows to store and initialize members
function myclass:init(something) 
end

    
-- when performance doesn't need arguments, it is still necessary to pass at leas one dummy argument
function myclass:kperf(something) 
end

-- if opcode is running at k-rate, it will call kperf, else if will call aperf (ksmps times) every k-pass.  
function myclass:kperf(something) 

end
