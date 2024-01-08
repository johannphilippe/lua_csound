function squarewave(phasor, amp)
	local arr = {}
	for i = 1, #phasor do
		if(phasor[i]  < 0.5) then
			arr[i] = amp[i]
		else
			arr[i] = amp[i] * -1
		end
		--print(i, arr[i])
	end
	return arr
end

function simple_square(phasor, amp)
	if(phasor < 0.5) then 
		return amp
	else
		return amp * - 1
	end
end

