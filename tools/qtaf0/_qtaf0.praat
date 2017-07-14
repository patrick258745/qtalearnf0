#########################################################################
############################## Input Formular ###########################
#########################################################################

form quantitativeTargetApproximation
	optionmenu Task: 1
		option Search for optimal qTA parameters
		option F0 resynthesis with qTA parameters
	comment General options options:
		integer f0_range_min_(Hz) 100
		integer f0_range_max_(Hz) 600
		real syllable_shift 0
		integer filter_order 5
		word corpus_directory ../../test/corpus/
		word target_file ../../test/qta.targets
	comment Parameter search options:
		real slope_range_min_(st/s) -100
		real slope_range_max_(st/s) 100
		real offset_range_min_(st) 80
		real offset_range_max_(st) 120
		real strength_range_min_(1/st) 1
		real strength_range_max_(1/st) 80
	comment F0 resynthesis options:
		word output_directory ../../test/qta/
endform

#########################################################################
############################## Main Script ##############################
#########################################################################

##### get input values
f0_min = f0_range_min
f0_max = f0_range_max
m_min = slope_range_min
m_max = slope_range_max
b_min = offset_range_min
b_max = offset_range_max
l_min = strength_range_min
l_max = strength_range_max
shift = syllable_shift
n = filter_order
cnt = 0
if task = 1
	flag$ = "-s"
else
	flag$ = "-r"
endif

##### check validity
assert f0_min < f0_max
assert m_min < m_max
assert b_min < b_max
assert l_min < l_max
assert n > 0

##### find audio files
Create Strings as file list... list 'corpus_directory$'*.wav
numberOfFiles = Get number of strings
if !numberOfFiles
	exit There are no sound files in the folder!
else
	more_file = 1
endif

##### load targets if neccessary
if task = 2
	Read Table from comma-separated file... 'target_file$'
	Rename... targetsTable
endif

##### iterate over all aufio files
for current_file from 1 to numberOfFiles

	##### get current audio file
	select Strings list
	fileName$ = Get string... current_file
	name$ = fileName$ - ".wav" - ".WAV"
	Read from file... 'corpus_directory$''fileName$'

	##### load TextGrid file
	if fileReadable ("'corpus_directory$''name$'.TextGrid")
		Read from file... 'corpus_directory$''name$'.TextGrid
	else
		exit Missing TextGrid File 'name$'
	endif

	nintervals = Get number of intervals... 1
	wordStart = Get start time of interval... 1 2
	wordEnd = Get start time of interval... 1 nintervals

	##### Get F0 PitchTier
	if fileReadable ("'corpus_directory$''name$'.PitchTier")
		if fileReadable ("'corpus_directory$''name$'.PitchTier")
			Read from file... 'corpus_directory$''name$'.PitchTier
		else
			exit Missing pulse File 'name$'
		endif
		select PitchTier 'name$'
		Formula... 12 * ln (self) / ln(2); semitone
		Rename... semitoneF0
	else
		select Sound 'name$'
		To Manipulation... 0.01 f0_min f0_max
		Extract pitch tier
		Formula... 12 * ln (self) / ln(2); semitone
		Rename... semitoneF0
		select Manipulation 'name$'
		Remove
	endif
	
	##### get qta F0 and save targets
	call qtaAnalysis
	select TableOfReal targets
	Write to headerless spreadsheet file... 'corpus_directory$''name$'.targets
	Remove

	if task = 2
		##### save result
		select PitchTier semitoneF0
		Save as PitchTier spreadsheet file... 'corpus_directory$''name$'.origf0
		select PitchTier qtaF0
		Save as PitchTier spreadsheet file... 'corpus_directory$''name$'.qtaf0

		##### create output directory
		createDirectory: "'output_directory$'"
		
		##### create resynthesized audio file
		call qtaResynthesis
		
		##### create plot
		call generatePlot
	endif
		
	##### clean up
	select PitchTier semitoneF0
	Remove
	select PitchTier qtaF0
	Remove
	select TextGrid 'name$'
	Remove
	select Sound 'name$'
	Remove
endfor

##### create ensemble target file if needed
if task = 1
	call assembleTargetFile
endif

##### final clean up
if task = 2
	select Table targetsTable
	Remove
endif
select Strings list
Remove

#########################################################################
############################## Procedures ###############################
#########################################################################

########## (Procedure) determine qTA F0 ##########
procedure qtaAnalysis
	
	##### store results
	Create PitchTier... qtaF0 wordStart wordEnd

	Create TableOfReal... targets 1 6
	Set column label (index)... 1 slope
	Set column label (index)... 2 offset
	Set column label (index)... 3 strength
	Set column label (index)... 4 duration
	Set column label (index)... 5 rmse
	Set column label (index)... 6 corr
	
	##### counter variables
	interval_t = 0
	if task = 2
		select Table targetsTable
		targetNumber = Search column... name 'name$'
	endif

	##### iterate over syllables
	for nm from 1 to nintervals
		select TextGrid 'name$'
		label$ = Get label of interval... 1 nm
		
		##### consider syllable shift
		tmpS = Get starting point... 1 nm
		if tmpS = wordStart
			start = tmpS
		else
			start = tmpS - shift/1000
		endif

		tmpE = Get end point... 1 nm
		if tmpE = wordEnd
			end = tmpE
		else
			end = tmpE - shift/1000
		endif

		interval_dur = end - start

		if not label$ = ""
			interval_t = interval_t + 1

			select PitchTier semitoneF0
			index_first = Get high index from time... start
			index_last = Get low index from time... end
			# syllable shift can lead to problems if there is no F0 in interval
			if index_last = 0
				index_last = 1
			endif
			tInitial = Get time from index... index_first
			xInitial = Get value at index... index_first
			tFinal = end

			if interval_t == 1
				deriv[1] = xInitial
				for i from 2 to n
    				deriv[i] = 0
				endfor
			else
				for i from 1 to n
    				deriv[i] = derivFinal[i]
				endfor
			endif

			##### determine target values
			if task = 2
				select Table targetsTable
				m_min = Get value... targetNumber slope
				m_max = m_min	
				b_min = Get value... targetNumber offset
				b_max = b_min
				l_min = Get value... targetNumber strength
				l_max = l_min
			endif
			
			##### create input file for ./qtatarget
			filedelete 'corpus_directory$'input
			fileappend 'corpus_directory$'input 'm_min' 'm_max''newline$'
			fileappend 'corpus_directory$'input 'b_min' 'b_max''newline$'
			fileappend 'corpus_directory$'input 'l_min' 'l_max''newline$'
			fileappend 'corpus_directory$'input 'n''newline$'
			for i from 1 to n
				val = deriv[i]
    			fileappend 'corpus_directory$'input 'val' 
			endfor
			fileappend 'corpus_directory$'input 'newline$'
			fileappend 'corpus_directory$'input 'start' 'end''newline$'
			
			select PitchTier semitoneF0
			Down to TableOfReal... Hertz
			nsamples = Get number of rows
			fileappend 'corpus_directory$'input 'nsamples''newline$'
			for i from 1 to nsamples
				time = Get value... i 1
				freq = Get value... i 2
				fileappend 'corpus_directory$'input 'time' 'freq''newline$'
			endfor
			Remove

			if fileReadable("./qtamodel")
				cnt = cnt + 1
				runSystem: "printf '\r\t processed targets ['cnt']: '"
				runSystem: "./qtamodel 'flag$' --in 'corpus_directory$'input --out 'corpus_directory$'output"
			else
				printline Cannot find ./qtamodel!
				exit
			endif
			
			##### analyze output file
			buff$ < 'corpus_directory$'output
			
			#First line: qTA parameters
			len_buff = length(buff$)
			line_index = index(buff$,newline$)
			sbuff$ = left$(buff$,line_index-1)
			buff$ = mid$(buff$,line_index+1,len_buff)
			len_sbuff = length(sbuff$)
	
			#First line: m
			sep_index = index(sbuff$," ")
			pbuff$ = left$(sbuff$,sep_index-1)
			sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
			mBest'interval_t' = 'pbuff$'
			m = mBest'interval_t'
	
			#First line: b
			sep_index = index(sbuff$," ")
			pbuff$ = left$(sbuff$,sep_index-1)
			sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
			bBest'interval_t' = 'pbuff$'
			b = bBest'interval_t'
	
			#First line: lambda
			lambdaBest'interval_t' = 'sbuff$'
			lambda = lambdaBest'interval_t'
	
			duration'interval_t' = end - start
			d = duration'interval_t'
			
			#Second line: final F0 articulatory state
			len_buff = length(buff$)
			line_index = index(buff$,newline$)
			sbuff$ = left$(buff$,line_index-1)
			buff$ = mid$(buff$,line_index+1,len_buff)
			len_sbuff = length(sbuff$)
	
			#Second line: Final state
			for i from 1 to n-1
    			sep_index = index(sbuff$," ")
				pbuff$ = left$(sbuff$,sep_index-1)
				sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
				derivFinal[i] = 'pbuff$'
			endfor

			#Second line: last derivative
			derivFinal[n] = 'sbuff$'
	
			#Third line: rmse and correlation results
			len_buff = length(buff$)
			line_index = index(buff$,newline$)
			sbuff$ = left$(buff$,line_index-1)
			buff$ = mid$(buff$,line_index+1,len_buff)
			len_sbuff = length(sbuff$)
	
			#Third line: rmse
			sep_index = index(sbuff$," ")
			pbuff$ = left$(sbuff$,sep_index-1)
			sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
			rmse'interval_t' = 'pbuff$'
			rmse = rmse'interval_t'
	
			#Third line: correlation
			corr'interval_t' = 'sbuff$'
			corr = corr'interval_t'
	
			#Forth line: number of sample
			len_buff = length(buff$)
			line_index = index(buff$,newline$)
			sbuff$ = left$(buff$,line_index-1)
			buff$ = mid$(buff$,line_index+1,len_buff)
			len_sbuff = length(sbuff$)
			number_of_row = 'sbuff$'
	
			#Remaining: time and resynthesized f0
			for i from 1 to number_of_row
				len_buff = length(buff$)
				line_index = index(buff$,newline$)
				sbuff$ = left$(buff$,line_index-1)
				buff$ = mid$(buff$,line_index+1,len_buff)
				len_sbuff = length(sbuff$)
	
				sep_index = index(sbuff$," ")
				pbuff$ = left$(sbuff$,sep_index-1)
				sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
				sampletime = 'pbuff$'
				fvalue = 'sbuff$'
	
				select PitchTier qtaF0
				Add point... sampletime fvalue
			endfor
			
			#Save targets
			select TableOfReal targets
			nrows = Get number of rows
			Set value... nrows 1 m
			Set value... nrows 2 b
			Set value... nrows 3 lambda			
			Set value... nrows 4 d
			Set value... nrows 5 rmse
			Set value... nrows 6 corr
			Insert row (index)... nrows + 1

		endif
	endfor

	filedelete 'corpus_directory$'input
	filedelete 'corpus_directory$'output

	##### last row is empty
	select TableOfReal targets
	nrows = Get number of rows
	Remove row (index)... nrows
endproc

########## (Procedure) Create ensemble target file ##########
procedure assembleTargetFile

	##### create result container
	writeFileLine: "'target_file$'", "name,slope,offset,strength,duration,rmse,corr"

	##### iterate over all files
	for current_file from 1 to numberOfFiles

		##### get current target file
		select Strings list
		fileName$ = Get string... current_file
		name$ = fileName$ - ".wav" - ".WAV"
		
		Read TableOfReal from headerless spreadsheet file... 'corpus_directory$''name$'.targets
		filedelete 'corpus_directory$''name$'.targets
		select TableOfReal 'name$'
		nrows = Get number of rows

		##### iterate over rows (syllable targets)
		for i from 1 to nrows
			#read targets
			select TableOfReal 'name$'
			m = Get value... i 1
			b = Get value... i 2
			l = Get value... i 3
			d = Get value... i 4
			r = Get value... i 5
			c = Get value... i 6
			
			#write targets
			appendFile: "'target_file$'",name$,",",m,",",b,",",l,",",d,",",r,",",c,newline$
		endfor
		select TableOfReal 'name$'
		Remove
	endfor
endproc

########## (Procedure) resynthesize with approximated F0	 ##########
procedure qtaResynthesis
	select Sound 'name$'
	To Manipulation... 0.01 f0_min f0_max
	select PitchTier qtaF0
	Formula... exp(self*ln(2)/12)
	plus Manipulation 'name$'
	Replace pitch tier
	select Manipulation 'name$'
	Get resynthesis (overlap-add)
	createDirectory: "'output_directory$'audios"
	nowarn Save as WAV file: "'output_directory$'/audios/'name$'_qta.wav"
	Remove
	select Manipulation 'name$'
	Remove
endproc

########## (Procedure) plot F0 contours and targets ##########
procedure generatePlot
	if fileReadable("./qtatools")
		runSystem: "printf '\n\t plotted files...'"
		runSystem: "./qtatools -p --dir 'corpus_directory' --label 'name$'"
	else
		printline Cannot find ./qtatools!
		exit
	endif
	
	##### clean
	filedelete 'corpus_directory$''name$'.p
	filedelete 'corpus_directory$''name$'.origf0
	filedelete 'corpus_directory$''name$'.qtaf0
	filedelete 'corpus_directory$''name$'.targets
	createDirectory: "'output_directory$'plots"
	
	##### copy
	#Read from file... 'corpus_directory$''name$'.png
	#Save as PNG file... 'output_directory$'/plots/'name$'.png
endproc
