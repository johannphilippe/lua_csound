<CsoundSynthesizer>

<CsOptions>
-odac
</CsOptions>

<CsInstruments>
sr = 48000
ksmps = 64
nchnls = 2
0dbfs = 1

ierr lua_dofile "./array_test.lua"
print ierr

; kperf 
instr 1 
    iArr[] init 1 
    kArr[] init 1 
    ; Random curve 
    kArr[0] = rspline:k(1, 10, 0.1, 5)
    kres[] = lua_obj_arr("myclass", iArr, kArr, 2)
    printk2 kres[0]
    printk2 kres[1], 30

    kfq = ((kres[1] + 1) * .5) * 500 + 50

    ao = vco2(0.2, kfq) * kres[0] 
    outs ao, ao
endin
; aperf 
instr 2 
    iArr[] init 1 
    aArr[] init 1 
    ; Random curve 
    aArr[0] = rspline:a(50, 300, 0.1, 5)
    ares[] = lua_obj_arr("myclass", iArr, aArr)
    ares[0] = ares[0] * 0.2
    ares[1] = ares[1] * 0.2
    outs ares[0], ares[1]
endin

</CsInstruments>

<CsScore>
i 1 0 10
i 2 10 10
</CsScore>

</CsoundSynthesizer>

