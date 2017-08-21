#########################################################################
############################## Input Formular ###########################
#########################################################################

form quantitativeTargetApproximation
	optionmenu Task: 1
		option Search for optimal qTA parameters
		option F0 resynthesis with qTA parameters
	comment General options options:
		integer f0_range_min_(Hz) 100
		integer f0_range_max_(Hz) 500
		integer speaker_mean_(Hz) 232.86
		real syllable_shift 0
		integer filter_order 5
		word corpus_directory ../../test/corpus/
		word target_file ../../test/qta.target
	comment Parameter search options:
		real slope_range_min_(st/s) -100
		real slope_range_max_(st/s) 100
		real offset_range_min_(st) 80
		real offset_range_max_(st) 120
		real strength_range_min_(1/st) 1
		real strength_range_max_(1/st) 80
		real regularization_parameter 0.0
	comment F0 resynthesis options:
		boolean store 1
		word output_directory ../../test/qta/
endform

#########################################################################
############################## Main Script ##############################
#########################################################################

##### get input values
f0_min = f0_range_min
f0_max = f0_range_max
s_mean = 12*log2(speaker_mean)
m_min = slope_range_min
m_max = slope_range_max
b_min = offset_range_min
b_max = offset_range_max
l_min = strength_range_min
l_max = strength_range_max
shift = syllable_shift
lambda = regularization_parameter
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

##### print message
if task = 1
	writeInfoLine: "[praat] search for optimal qta parameters"
else
	writeInfoLine: "[praat] resynthesize f0 with given qta parameters"
endif

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
	wCnt = 0
endif

##### iterate over all aufio files
for current_file from 1 to numberOfFiles

	##### get current audio file
	select Strings list
	fileName$ = Get string... current_file
	name$ = fileName$ - ".wav" - ".WAV"
	Read from file... 'corpus_directory$''fileName$'

	##### only process targets from file
	pos = 1
	if task = 2
		select Table targetsTable
		pos = Search column... name 'name$'
	endif
	
	if pos > 0		
		##### print status
		cnt = cnt + 1
		# printline [praat] ('cnt') Process 'name$' 
	
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
			Formula... 12*log2(self); semitone
			Rename... semitoneF0
		else
			select Sound 'name$'
			To Manipulation... 0.01 f0_min f0_max
			Extract pitch tier
			Formula... 12*log2(self); semitone
			Rename... semitoneF0
			select Manipulation 'name$'
			Remove
		endif
		
		##### get qta F0 and save targets
		call qtaAnalysis
		select TableOfReal targets
		Write to headerless spreadsheet file... 'corpus_directory$''name$'.target
		Remove
		
		select TableOfReal measures
		Write to headerless spreadsheet file... 'corpus_directory$''name$'.measures
		Remove
	
		if task = 2
			##### save result
			select PitchTier semitoneF0
			Down to TableOfReal... Hertz
			Save as headerless spreadsheet file... 'corpus_directory$''name$'.origf0
			Remove
			select PitchTier qtaF0
			Down to TableOfReal... Hertz
			Save as headerless spreadsheet file... 'corpus_directory$''name$'.qtaf0
			Remove
	
			##### create output directory
			createDirectory: "'output_directory$'"
	
			if store				
				##### create resynthesized audio file
				call qtaResynthesis
				
				##### create plot
				call generatePlot
			endif
				
			##### clean files
			filedelete 'corpus_directory$''name$'.origf0
			filedelete 'corpus_directory$''name$'.qtaf0
			filedelete 'corpus_directory$''name$'.target
			
			wCnt = wCnt+1
		endif
			
		##### clean up
		select PitchTier semitoneF0
		Remove
		select PitchTier qtaF0
		Remove
		select TextGrid 'name$'
	Remove
	endif
	
	##### clean up
	select Sound 'name$'
	Remove
endfor

##### create ensemble target file if needed
call assembleMeasuresFile
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

##### print message
if task = 1
	writeInfoLine: "'tab$'processed 'numberOfFiles' utterances successfully"
else
	writeInfoLine: "'tab$'processed 'wCnt' utterances successfully"
endif

#########################################################################
############################## Procedures ###############################
#########################################################################

########## (Procedure) determine qTA F0 ##########
procedure qtaAnalysis
	
	##### store results
	Create PitchTier... qtaF0 wordStart wordEnd

	Create TableOfReal... targets 1 4
	Set column label (index)... 1 slope
	Set column label (index)... 2 offset
	Set column label (index)... 3 strength
	Set column label (index)... 4 duration
	
	Create TableOfReal... measures 1 3
	Set column label (index)... 1 mae
	Set column label (index)... 2 rmse
	Set column label (index)... 3 corr
	
	##### counter variables
	interval_t = 0
	if task = 2
		select Table targetsTable
		targetNumber = Search column... name 'name$'
	endif
	
	##### count intervals
	for nm1 from 1 to nintervals
		select TextGrid 'name$'
		label$ = Get label of interval... 1 nm1
		if not label$ = ""
			interval_t = interval_t + 1
		endif
	endfor
	
	##### create input file for ./qtatarget
	filedelete 'corpus_directory$'input
	fileappend 'corpus_directory$'input 'n''newline$'
	fileappend 'corpus_directory$'input 's_mean''newline$'
	fileappend 'corpus_directory$'input 'interval_t''newline$'
	interval_t = 0

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

		if not label$ = ""
			interval_t = interval_t + 1

			##### determine target values
			if task = 2
				select Table targetsTable
				m_min = Get value... targetNumber slope
				m_max = m_min	
				b_min = Get value... targetNumber offset
				b_max = b_min
				l_min = Get value... targetNumber strength
				l_max = l_min
				targetNumber = targetNumber + 1
			endif
			
			##### add target information to input file
			fileappend 'corpus_directory$'input 'm_min' 'm_max' 'b_min' 'b_max' 'l_min' 'l_max' 'start' 'end''newline$'
			
			#####Save interval duration
			interval_dur = end - start
			select TableOfReal targets
			nrows = Get number of rows
			Set value... nrows 4 interval_dur
			Insert row (index)... nrows + 1
		endif
	endfor
		
	##### write original f0 to file
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

	##### call C++ program
	if fileReadable("./qtamodel")
		runSystem: "./qtamodel 'flag$' --in 'corpus_directory$'input --out 'corpus_directory$'output -l 'lambda'"
	else
		printline Cannot find ./qtamodel!
		exit
	endif
		
	##### analyze output file
	buff$ < 'corpus_directory$'output
	
	#first line: error measures
	len_buff = length(buff$)
	line_index = index(buff$,newline$)
	sbuff$ = left$(buff$,line_index-1)
	buff$ = mid$(buff$,line_index+1,len_buff)
	len_sbuff = length(sbuff$)
	
	#mae
	sep_index = index(sbuff$," ")
	pbuff$ = left$(sbuff$,sep_index-1)
	sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
	mae'interval_t' = 'pbuff$'
	mae = mae'interval_t'
	
	#rmse
	sep_index = index(sbuff$," ")
	pbuff$ = left$(sbuff$,sep_index-1)
	sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
	rmse'interval_t' = 'pbuff$'
	rmse = rmse'interval_t'

	#corr
	corr'interval_t' = 'sbuff$'
	corr = corr'interval_t'
	
	#Save error measures
	select TableOfReal measures
	Set value... 1 1 mae			
	Set value... 1 2 rmse
	Set value... 1 3 corr
		
	# following lines: get target values
	for nm from 1 to interval_t
		#qTA parameters
		len_buff = length(buff$)
		line_index = index(buff$,newline$)
		sbuff$ = left$(buff$,line_index-1)
		buff$ = mid$(buff$,line_index+1,len_buff)
		len_sbuff = length(sbuff$)

		#m
		sep_index = index(sbuff$," ")
		pbuff$ = left$(sbuff$,sep_index-1)
		sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
		mBest'interval_t' = 'pbuff$'
		m = mBest'interval_t'

		#b
		sep_index = index(sbuff$," ")
		pbuff$ = left$(sbuff$,sep_index-1)
		sbuff$ = mid$(sbuff$,sep_index+1,len_sbuff)
		bBest'interval_t' = 'pbuff$'
		b = bBest'interval_t'
		
		#lambda
		lambdaBest'interval_t' = 'sbuff$'
		lambda = lambdaBest'interval_t'
	
		duration'interval_t' = end - start
		d = duration'interval_t'
		
		#Save targets
		select TableOfReal targets
		nrows = Get number of rows
		Set value... nm 1 m
		Set value... nm 2 b
		Set value... nm 3 lambda			
	endfor
			
	#next line: number of sample
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
	measures_file$ = target_file$ - ".target" + ".measures"
	writeFileLine: "'target_file$'", "name,slope,offset,strength,duration"
	writeFileLine: "'measures_file$'", "name,mae,rmse,corr"

	##### iterate over all files
	for current_file from 1 to numberOfFiles

		##### get current target file
		select Strings list
		fileName$ = Get string... current_file
		name$ = fileName$ - ".wav" - ".WAV"

		##### only take test targets from file
		pos = 1
		if task = 2
			select Table targetsTable
			pos = Search column... name 'name$'
		endif
		
		if pos > 0	
			Read TableOfReal from headerless spreadsheet file... 'corpus_directory$''name$'.target
			filedelete 'corpus_directory$''name$'.target
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
				
				#write targets
				appendFile: "'target_file$'",name$,",",m,",",b,",",l,",",d,newline$
			endfor
			select TableOfReal 'name$'
			Remove
		endif
	endfor
endproc

########## (Procedure) Create ensemble measures file ##########
procedure assembleMeasuresFile

	##### create result container
	measures_file$ = target_file$ - ".target" + ".measures"
	writeFileLine: "'measures_file$'", "name,mae,rmse,corr"

	##### iterate over all files
	for current_file from 1 to numberOfFiles

		##### get current target file
		select Strings list
		fileName$ = Get string... current_file
		name$ = fileName$ - ".wav" - ".WAV"

		##### only take test targets from file
		pos = 1
		if task = 2
			select Table targetsTable
			pos = Search column... name 'name$'
		endif
		
		if pos > 0
			
			##### get current measures file	
			Read TableOfReal from headerless spreadsheet file... 'corpus_directory$''name$'.measures
			filedelete 'corpus_directory$''name$'.measures
	
			#read measures
			select TableOfReal 'name$'
			mae = Get value... 1 1
			rmse = Get value... 1 2
			corr = Get value... 1 3
			
			#write targets
			appendFile: "'measures_file$'",name$,",",mae,",",rmse,",",corr,newline$
	
			select TableOfReal 'name$'
			Remove
		endif
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
	nowarn Save as WAV file: "'output_directory$'audios/'name$'_qta.wav"
	Remove
	select Manipulation 'name$'
	Remove
endproc

########## (Procedure) plot F0 contours and targets ##########
procedure generatePlot
	createDirectory: "'output_directory$'plots"
	if fileReadable("./qtatools")
		runSystem: "./qtatools -p --dir 'corpus_directory$' --label 'name$' --out 'output_directory$'plots/'name$'.png"
	else
		printline Cannot find ./qtatools!
		exit
	endif
	
	##### clean
	filedelete 'corpus_directory$''name$'.plot
endproc
