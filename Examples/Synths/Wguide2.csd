<Cabbage>
form caption("Impulse waveguide 2") size(550, 325), colour("black"),pluginID("imp2") 
rslider bounds(15, 10, 100, 100) channel("semi"), range(-12,12,-12), caption("Coarse"), colour("HotPink  ")
rslider bounds(120, 10, 100, 100) channel("spread"), range(0,1,.5), caption("spread"), colour("HotPink  ")
rslider bounds(225, 10, 100, 100) channel("detune"), range(-1,1,0), caption("Detune"), colour("HotPink  ")
rslider bounds(330, 10, 100, 100) channel("filter1"), range(100,20000,2000), caption("Cutoff1"), colour("HotPink  ")
rslider bounds(435, 10, 100, 100) channel("filter2"), range(100,20000,2000), caption("Cutoff2"), colour("HotPink  ")


rslider bounds(15, 120, 100, 100), caption("Impulse1"), colour("HotPink  "), channel("random1"), range(0,3,.1)
rslider bounds(120, 120, 100, 100), caption("Impulse2"), colour("HotPink  "), channel("random2"), range(0,3,.1)
rslider bounds(225, 120, 100, 100), caption("Rev level"), colour("HotPink  "), channel("level"), range(0,1,.8)
rslider bounds(330, 120, 100, 100), caption("Rev size"), colour("HotPink  "), channel("revsize"), range(.1,.99,.7)

checkbox  bounds(450,120, 160, 20),channel("onoff"), text("Rev On/Off"), value(0)
keyboard pos(15,230), size(520, 60)


</Cabbage>
<CsoundSynthesizer>
<CsOptions>
-d -n -+rtmidi=NULL -M0 
</CsOptions>
<CsInstruments>

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

;Author: Giorgio Zucco (2012)

seed	0
ga1	init	0
ga2	init	0


instr	1
gkrnd1	chnget	"random1"
gkrnd2	chnget	"random2"
gkrate	chnget	"rate"
gksemi	chnget	"semi"
gkspread	chnget	"spread"
gkdetune	chnget	"detune"
gkfilt1	chnget	"filter1"
gkfilt2	chnget	"filter2"
iamp	ampmidi	1
inote	notnum
kfreq1	=	cpsmidinn(inote+int(gksemi))
kfreq2	=	cpsmidinn(inote+int(gksemi)+gkdetune)

kdev	=	rnd(.01)
krnd1	randomh	kdev,.01+gkrnd1,10
asig1	mpulse	iamp,0.01+krnd1
krnd2	randomh	kdev,.01+gkrnd2,10
asig2	mpulse	iamp,0.01+krnd2
awg1	wguide2	asig1,kfreq1,kfreq1*.5,gkfilt1,gkfilt2,.24,.24
awg2	wguide2	asig1,kfreq2,kfreq2*.5,gkfilt1,gkfilt2,.24,.24
aL	clip	awg1,0,0dbfs
aR	clip	awg2,0,0dbfs
kenv	mxadsr	.001,1,1,.5

aoutL = ((aL * gkspread) + (aR * (1 - gkspread))) 
aoutR = ((aL * (1-gkspread)) + (aR * gkspread))   

outs	aoutL*kenv,aoutR*kenv
vincr	ga1,aoutL*kenv
vincr	ga2,aoutR*kenv
endin

instr	10	;reverb
ktrig	chnget	"onoff"
klevel	chnget	"level"
ksize	chnget	"revsize"
if	ktrig	=	1	then
	
al,ar	reverbsc	ga1,ga2,ksize,12000,sr,0
asum1	dcblock2	al
asum2	dcblock2	ar
else
asum1	=	0
asum2	=	0
endif

outs	asum1*klevel,asum2*klevel
clear	ga1,ga2
endin


</CsInstruments>
<CsScore>


i1	0	36000
i10	0	36000
</CsScore>
</CsoundSynthesizer>
