Attribute VB_Name = "modBass"
'BASS 1.8 Multimedia Library
'---------------------------
'(c) 1999-2003 Ian Luck.
'Please report bugs/suggestions/etc... to bass@un4seen.com

'See the BASS.CHM file for more complete documentation

'NOTE: VB does not support 64-bit integers, so VB users only have access
'      to the low 32-bits of 64-bit return values. 64-bit parameters can
'      be specified though, using the "64" version of the function.

Global Const BASSTRUE As Long = 1   'Use this instead of VB Booleans
Global Const BASSFALSE As Long = 0  'Use this instead of VB Booleans

'***********************************************
'* Error codes returned by BASS_GetErrorCode() *
'***********************************************
Global Const BASS_OK = 0               'all is OK
Global Const BASS_ERROR_MEM = 1        'memory error
Global Const BASS_ERROR_FILEOPEN = 2   'can't open the file
Global Const BASS_ERROR_DRIVER = 3     'can't find a free sound driver
Global Const BASS_ERROR_BUFLOST = 4    'the sample buffer was lost
Global Const BASS_ERROR_HANDLE = 5     'invalid handle
Global Const BASS_ERROR_FORMAT = 6     'unsupported sample format
Global Const BASS_ERROR_POSITION = 7   'invalid playback position
Global Const BASS_ERROR_INIT = 8       'BASS_Init has not been successfully called
Global Const BASS_ERROR_START = 9      'BASS_Start has not been successfully called
Global Const BASS_ERROR_INITCD = 10    'can't initialize CD
Global Const BASS_ERROR_CDINIT = 11    'BASS_CDInit has not been successfully called
Global Const BASS_ERROR_NOCD = 12      'no CD in drive
Global Const BASS_ERROR_CDTRACK = 13   'can't play the selected CD track
Global Const BASS_ERROR_ALREADY = 14   'already initialized
Global Const BASS_ERROR_CDVOL = 15     'CD has no volume control
Global Const BASS_ERROR_NOPAUSE = 16   'not paused
Global Const BASS_ERROR_NOTAUDIO = 17  'not an audio track
Global Const BASS_ERROR_NOCHAN = 18    'can't get a free channel
Global Const BASS_ERROR_ILLTYPE = 19   'an illegal type was specified
Global Const BASS_ERROR_ILLPARAM = 20  'an illegal parameter was specified
Global Const BASS_ERROR_NO3D = 21      'no 3D support
Global Const BASS_ERROR_NOEAX = 22     'no EAX support
Global Const BASS_ERROR_DEVICE = 23    'illegal device number
Global Const BASS_ERROR_NOPLAY = 24    'not playing
Global Const BASS_ERROR_FREQ = 25      'illegal sample rate
Global Const BASS_ERROR_NOTFILE = 27   'the stream is not a file stream
Global Const BASS_ERROR_NOHW = 29      'no hardware voices available
Global Const BASS_ERROR_EMPTY = 31     'the MOD music has no sequence data
Global Const BASS_ERROR_NONET = 32     'no internet connection could be opened
Global Const BASS_ERROR_CREATE = 33    'couldn't create the file
Global Const BASS_ERROR_NOFX = 34      'effects are not available
Global Const BASS_ERROR_PLAYING = 35   'the channel is playing
Global Const BASS_ERROR_NOTAVAIL = 37  'requested data is not available
Global Const BASS_ERROR_DECODE = 38    'the channel is a "decoding channel"
Global Const BASS_ERROR_DX = 39        'a sufficient DirectX version is not installed
Global Const BASS_ERROR_TIMEOUT = 40   'connection timedout
Global Const BASS_ERROR_FILEFORM = 41  'unsupported file format
Global Const BASS_ERROR_SPEAKER = 42   'unavailable speaker
Global Const BASS_ERROR_UNKNOWN = -1   'some other mystery error

'************************
'* Initialization flags *
'************************
Global Const BASS_DEVICE_8BITS = 1     'use 8 bit resolution, else 16 bit
Global Const BASS_DEVICE_MONO = 2      'use mono, else stereo
Global Const BASS_DEVICE_3D = 4        'enable 3D functionality
' If the BASS_DEVICE_3D flag is not specified when initilizing BASS,
' then the 3D flags (BASS_SAMPLE_3D and BASS_MUSIC_3D) are ignored when
' loading/creating a sample/stream/music.
Global Const BASS_DEVICE_LEAVEVOL = 32 'leave volume as it is
Global Const BASS_DEVICE_NOTHREAD = 128 'update buffers manually (using BASS_Update)
Global Const BASS_DEVICE_LATENCY = 256 'calculate device latency (BASS_INFO struct)
Global Const BASS_DEVICE_VOL1000 = 512 '0-1000 volume range (else 0-100)
Global Const BASS_DEVICE_FLOATDSP = 1024 'all DSP uses 32-bit floating-point data
Global Const BASS_DEVICE_SPEAKERS = 2048 'force enabling of speaker assignment

'***********************************
'* BASS_INFO flags (from DSOUND.H) *
'***********************************
Global Const DSCAPS_CONTINUOUSRATE = 16
' supports all sample rates between min/maxrate
Global Const DSCAPS_EMULDRIVER = 32
' device does NOT have hardware DirectSound support
Global Const DSCAPS_CERTIFIED = 64
' device driver has been certified by Microsoft
' The following flags tell what type of samples are supported by HARDWARE
' mixing, all these formats are supported by SOFTWARE mixing
Global Const DSCAPS_SECONDARYMONO = 256    ' mono
Global Const DSCAPS_SECONDARYSTEREO = 512  ' stereo
Global Const DSCAPS_SECONDARY8BIT = 1024   ' 8 bit
Global Const DSCAPS_SECONDARY16BIT = 2048  ' 16 bit

'*****************************************
'* BASS_RECORDINFO flags (from DSOUND.H) *
'*****************************************
Global Const DSCCAPS_EMULDRIVER = DSCAPS_EMULDRIVER
' device does NOT have hardware DirectSound recording support
Global Const DSCCAPS_CERTIFIED = DSCAPS_CERTIFIED
' device driver has been certified by Microsoft

'******************************************************************
'* defines for formats field of BASS_RECORDINFO (from MMSYSTEM.H) *
'******************************************************************
Global Const WAVE_FORMAT_1M08 = &H1          ' 11.025 kHz, Mono,   8-bit
Global Const WAVE_FORMAT_1S08 = &H2          ' 11.025 kHz, Stereo, 8-bit
Global Const WAVE_FORMAT_1M16 = &H4          ' 11.025 kHz, Mono,   16-bit
Global Const WAVE_FORMAT_1S16 = &H8          ' 11.025 kHz, Stereo, 16-bit
Global Const WAVE_FORMAT_2M08 = &H10         ' 22.05  kHz, Mono,   8-bit
Global Const WAVE_FORMAT_2S08 = &H20         ' 22.05  kHz, Stereo, 8-bit
Global Const WAVE_FORMAT_2M16 = &H40         ' 22.05  kHz, Mono,   16-bit
Global Const WAVE_FORMAT_2S16 = &H80         ' 22.05  kHz, Stereo, 16-bit
Global Const WAVE_FORMAT_4M08 = &H100        ' 44.1   kHz, Mono,   8-bit
Global Const WAVE_FORMAT_4S08 = &H200        ' 44.1   kHz, Stereo, 8-bit
Global Const WAVE_FORMAT_4M16 = &H400        ' 44.1   kHz, Mono,   16-bit
Global Const WAVE_FORMAT_4S16 = &H800        ' 44.1   kHz, Stereo, 16-bit

'***************
'* Music flags *
'***************
Global Const BASS_MUSIC_RAMP = 1       ' normal ramping
Global Const BASS_MUSIC_RAMPS = 2      ' sensitive ramping
' Ramping doesn't take a lot of extra processing and improves
' the sound quality by removing "clicks". Sensitive ramping will
' leave sharp attacked samples, unlike normal ramping.
Global Const BASS_MUSIC_LOOP = 4       ' loop music
Global Const BASS_MUSIC_FT2MOD = 16    ' play .MOD as FastTracker 2 does
Global Const BASS_MUSIC_PT1MOD = 32    ' play .MOD as ProTracker 1 does
Global Const BASS_MUSIC_MONO = 64      ' force mono mixing (less CPU usage)
Global Const BASS_MUSIC_3D = 128       ' enable 3D functionality
Global Const BASS_MUSIC_POSRESET = 256 ' stop all notes when moving position
Global Const BASS_MUSIC_SURROUND = 512 'surround sound
Global Const BASS_MUSIC_SURROUND2 = 1024 'surround sound (mode 2)
Global Const BASS_MUSIC_STOPBACK = 2048 'stop the music on a backwards jump effect
Global Const BASS_MUSIC_FX = 4096      'enable old implementation of DX8 effects
Global Const BASS_MUSIC_CALCLEN = 8192 'calculate playback length
Global Const BASS_MUSIC_NONINTER = 16384 ' non-interpolated mixing
Global Const BASS_MUSIC_FLOAT = &H10000	'32-bit floating-point
Global Const BASS_MUSIC_DECODE = &H200000 'don't play the music, only decode (BASS_ChannelGetData)
Global Const BASS_MUSIC_NOSAMPLE = &H400000 ' don't load the samples

'*********************
'* Sample info flags *
'*********************
Global Const BASS_SAMPLE_8BITS = 1          ' 8 bit
Global Const BASS_SAMPLE_FLOAT = 256        ' 32-bit floating-point
Global Const BASS_SAMPLE_MONO = 2           ' mono, else stereo
Global Const BASS_SAMPLE_LOOP = 4           ' looped
Global Const BASS_SAMPLE_3D = 8             ' 3D functionality enabled
Global Const BASS_SAMPLE_SOFTWARE = 16      ' it's NOT using hardware mixing
Global Const BASS_SAMPLE_MUTEMAX = 32       ' muted at max distance (3D only)
Global Const BASS_SAMPLE_VAM = 64           ' uses the DX7 voice allocation & management
Global Const BASS_SAMPLE_FX = 128           ' old implementation of DX8 effects are enabled
Global Const BASS_SAMPLE_OVER_VOL = 65536   ' override lowest volume
Global Const BASS_SAMPLE_OVER_POS = 131072  ' override longest playing
Global Const BASS_SAMPLE_OVER_DIST = 196608 ' override furthest from listener (3D only)

Global Const BASS_MP3_SETPOS = 131072       ' enable pin-point seeking on the MP3/MP2/MP1

Global Const BASS_STREAM_AUTOFREE = 262144  ' automatically free the stream when it stop/ends
Global Const BASS_STREAM_RESTRATE = 524288  ' restrict the download rate of internet file streams
Global Const BASS_STREAM_BLOCK = 1048576    ' download/play internet file stream (MPx/OGG) in small blocks
Global Const BASS_STREAM_DECODE = &H200000  ' don't play the stream, only decode (BASS_ChannelGetData)
Global Const BASS_STREAM_META = &H400000    ' request metadata from a Shoutcast stream
Global Const BASS_STREAM_FILEPROC = &H800000 ' use a STREAMFILEPROC callback

' Speaker assignment flags
Global Const BASS_SPEAKER_FRONT = &H1000000 ' front speakers
Global Const BASS_SPEAKER_REAR = &H2000000  ' rear/side speakers
Global Const BASS_SPEAKER_CENLFE = &H3000000 ' center & LFE speakers (5.1)
Global Const BASS_SPEAKER_REAR2 = &H4000000 ' rear center speakers (7.1)
Global Const BASS_SPEAKER_LEFT = &H10000000 ' modifier: left
Global Const BASS_SPEAKER_RIGHT = &H20000000 ' modifier: right
Global Const BASS_SPEAKER_FRONTLEFT = BASS_SPEAKER_FRONT or BASS_SPEAKER_LEFT
Global Const BASS_SPEAKER_FRONTRIGHT = BASS_SPEAKER_FRONT or BASS_SPEAKER_RIGHT
Global Const BASS_SPEAKER_REARLEFT = BASS_SPEAKER_REAR or BASS_SPEAKER_LEFT
Global Const BASS_SPEAKER_REARRIGHT = BASS_SPEAKER_REAR or BASS_SPEAKER_RIGHT
Global Const BASS_SPEAKER_CENTER = BASS_SPEAKER_CENLFE or BASS_SPEAKER_LEFT
Global Const BASS_SPEAKER_LFE = BASS_SPEAKER_CENLFE or BASS_SPEAKER_RIGHT
Global Const BASS_SPEAKER_REAR2LEFT = BASS_SPEAKER_REAR2 or BASS_SPEAKER_LEFT
Global Const BASS_SPEAKER_REAR2RIGHT = BASS_SPEAKER_REAR2 or BASS_SPEAKER_RIGHT

'**********************************************
'* BASS_StreamGetTags flags : what's returned *
'**********************************************
Global Const BASS_TAG_ID3 = 0   'ID3v1 tags : 128 byte block
Global Const BASS_TAG_ID3V2 = 1 'ID3v2 tags : variable length block
Global Const BASS_TAG_OGG = 2   'OGG comments : array of null-terminated strings
Global Const BASS_TAG_HTTP = 3  'HTTP headers : array of null-terminated strings
Global Const BASS_TAG_ICY = 4   'ICY headers : array of null-terminated strings
Global Const BASS_TAG_META = 5  'ICY metadata : null-terminated string

'********************
'* 3D channel modes *
'********************
Global Const BASS_3DMODE_NORMAL = 0
' normal 3D processing
Global Const BASS_3DMODE_RELATIVE = 1
' The channel's 3D position (position/velocity/orientation) are relative to
' the listener. When the listener's position/velocity/orientation is changed
' with BASS_Set3DPosition, the channel's position relative to the listener does
' not change.
Global Const BASS_3DMODE_OFF = 2
' Turn off 3D processing on the channel, the sound will be played
' in the center.


'****************************************************
'* EAX environments, use with BASS_SetEAXParameters *
'****************************************************
Global Const EAX_ENVIRONMENT_GENERIC = 0
Global Const EAX_ENVIRONMENT_PADDEDCELL = 1
Global Const EAX_ENVIRONMENT_ROOM = 2
Global Const EAX_ENVIRONMENT_BATHROOM = 3
Global Const EAX_ENVIRONMENT_LIVINGROOM = 4
Global Const EAX_ENVIRONMENT_STONEROOM = 5
Global Const EAX_ENVIRONMENT_AUDITORIUM = 6
Global Const EAX_ENVIRONMENT_CONCERTHALL = 7
Global Const EAX_ENVIRONMENT_CAVE = 8
Global Const EAX_ENVIRONMENT_ARENA = 9
Global Const EAX_ENVIRONMENT_HANGAR = 10
Global Const EAX_ENVIRONMENT_CARPETEDHALLWAY = 11
Global Const EAX_ENVIRONMENT_HALLWAY = 12
Global Const EAX_ENVIRONMENT_STONECORRIDOR = 13
Global Const EAX_ENVIRONMENT_ALLEY = 14
Global Const EAX_ENVIRONMENT_FOREST = 15
Global Const EAX_ENVIRONMENT_CITY = 16
Global Const EAX_ENVIRONMENT_MOUNTAINS = 17
Global Const EAX_ENVIRONMENT_QUARRY = 18
Global Const EAX_ENVIRONMENT_PLAIN = 19
Global Const EAX_ENVIRONMENT_PARKINGLOT = 20
Global Const EAX_ENVIRONMENT_SEWERPIPE = 21
Global Const EAX_ENVIRONMENT_UNDERWATER = 22
Global Const EAX_ENVIRONMENT_DRUGGED = 23
Global Const EAX_ENVIRONMENT_DIZZY = 24
Global Const EAX_ENVIRONMENT_PSYCHOTIC = 25
' total number of environments
Global Const EAX_ENVIRONMENT_COUNT = 26

'*************************************************************
'* EAX presets, usage: BASS_SetEAXParametersVB(EAX_PRESET_xxx) *
'*************************************************************
Global Const EAX_PRESET_GENERIC = 1
Global Const EAX_PRESET_PADDEDCELL = 2
Global Const EAX_PRESET_ROOM = 3
Global Const EAX_PRESET_BATHROOM = 4
Global Const EAX_PRESET_LIVINGROOM = 5
Global Const EAX_PRESET_STONEROOM = 6
Global Const EAX_PRESET_AUDITORIUM = 7
Global Const EAX_PRESET_CONCERTHALL = 8
Global Const EAX_PRESET_CAVE = 9
Global Const EAX_PRESET_ARENA = 10
Global Const EAX_PRESET_HANGAR = 11
Global Const EAX_PRESET_CARPETEDHALLWAY = 12
Global Const EAX_PRESET_HALLWAY = 13
Global Const EAX_PRESET_STONECORRIDOR = 14
Global Const EAX_PRESET_ALLEY = 15
Global Const EAX_PRESET_FOREST = 16
Global Const EAX_PRESET_CITY = 17
Global Const EAX_PRESET_MOUNTAINS = 18
Global Const EAX_PRESET_QUARRY = 19
Global Const EAX_PRESET_PLAIN = 20
Global Const EAX_PRESET_PARKINGLOT = 21
Global Const EAX_PRESET_SEWERPIPE = 22
Global Const EAX_PRESET_UNDERWATER = 23
Global Const EAX_PRESET_DRUGGED = 24
Global Const EAX_PRESET_DIZZY = 25
Global Const EAX_PRESET_PSYCHOTIC = 26


'**********************************************************************
'* Sync types (with BASS_ChannelSetSync() "param" and SYNCPROC "data" *
'* definitions) & flags.                                              *
'**********************************************************************
' Sync when a music or stream reaches a position.
' if HMUSIC...
' param: LOWORD=order (0=first, -1=all) HIWORD=row (0=first, -1=all)
' data : LOWORD=order HIWORD=row
' if HSTREAM...
' param: position in bytes
' data : not used
Global Const BASS_SYNC_POS = 0
Global Const BASS_SYNC_MUSICPOS = 0
' Sync when an instrument (sample for the non-instrument based formats)
' is played in a music (not including retrigs).
' param: LOWORD=instrument (1=first) HIWORD=note (0=c0...119=b9, -1=all)
' data : LOWORD=note HIWORD=volume (0-64)
Global Const BASS_SYNC_MUSICINST = 1
' Sync when a music or file stream reaches the end.
' param: not used
' data : not used
Global Const BASS_SYNC_END = 2
' Sync when the "sync" effect (XM/MTM/MOD: E8x/Wxx, IT/S3M: S2x) is used.
' param: 0:data=pos, 1:data="x" value
' data : param=0: LOWORD=order HIWORD=row, param=1: "x" value
Global Const BASS_SYNC_MUSICFX = 3
' FLAG: post a Windows message (instead of callback)
' When using a window message "callback", the message to post is given in the "proc"
' parameter of BASS_ChannelSetSync, and is posted to the window specified in the BASS_Init
' call. The message parameters are: WPARAM = data, LPARAM = user.
Global Const BASS_SYNC_META = 4
' Sync when metadata is received in a Shoutcast stream.
' param: not used
' data : pointer to the metadata
Global Const BASS_SYNC_SLIDE = 5
' Sync when an attribute slide is completed.
' param: not used
' data : the type of slide completed (one of the BASS_SLIDE_xxx values)
Global Const BASS_SYNC_MESSAGE = &H20000000
'FLAG: sync at mixtime, else at playtime
Global Const BASS_SYNC_MIXTIME = &H40000000
' FLAG: sync only once, else continuously
Global Const BASS_SYNC_ONETIME = &H80000000

Global Const CDCHANNEL = 0       ' CD channel, for use with BASS_Channel functions
Global Const RECORDCHAN = 1      ' Recording channel, for use with BASS_Channel functions

' BASS_ChannelIsActive return values
Global Const BASS_ACTIVE_STOPPED = 0
Global Const BASS_ACTIVE_PLAYING = 1
Global Const BASS_ACTIVE_STALLED = 2
Global Const BASS_ACTIVE_PAUSED = 3

' BASS_ChannelIsSliding return flags
Global Const BASS_SLIDE_FREQ = 1
Global Const BASS_SLIDE_VOL = 2
Global Const BASS_SLIDE_PAN = 4

' CD ID flags, use with BASS_CDGetID
Global Const BASS_CDID_IDENTITY = 0
Global Const BASS_CDID_UPC = 1
Global Const BASS_CDID_CDDB = 2
Global Const BASS_CDID_CDDB2 = 3

' BASS_ChannelGetData flags
Global Const BASS_DATA_AVAILABLE= 0          ' query how much data is buffered
Global Const BASS_DATA_FFT512   = &H80000000 ' 512 sample FFT
Global Const BASS_DATA_FFT1024  = &H80000001 ' 1024 FFT
Global Const BASS_DATA_FFT2048  = &H80000002 ' 2048 FFT
Global Const BASS_DATA_FFT512S  = &H80000010 ' stereo 512 sample FFT
Global Const BASS_DATA_FFT1024S = &H80000011 ' stereo 1024 FFT
Global Const BASS_DATA_FFT2048S = &H80000012 ' stereo 2048 FFT
Global Const BASS_DATA_FFT_NOWINDOW = &H20   ' FFT flag: no Hanning window

' BASS_RecordSetInput flags
Global Const BASS_INPUT_OFF = &H10000
Global Const BASS_INPUT_ON = &H20000
Global Const BASS_INPUT_LEVEL = &H40000

Global Const BASS_INPUT_TYPE_MASK = &Hff000000
Global Const BASS_INPUT_TYPE_UNDEF = &H00000000
Global Const BASS_INPUT_TYPE_DIGITAL = &H01000000
Global Const BASS_INPUT_TYPE_LINE = &H02000000
Global Const BASS_INPUT_TYPE_MIC = &H03000000
Global Const BASS_INPUT_TYPE_SYNTH = &H04000000
Global Const BASS_INPUT_TYPE_CD = &H05000000
Global Const BASS_INPUT_TYPE_PHONE = &H06000000
Global Const BASS_INPUT_TYPE_SPEAKER = &H07000000
Global Const BASS_INPUT_TYPE_WAVE = &H08000000
Global Const BASS_INPUT_TYPE_AUX = &H09000000
Global Const BASS_INPUT_TYPE_ANALOG = &H0a000000

' BASS_SetNetConfig flags
Global Const BASS_NET_TIMEOUT = 0
Global Const BASS_NET_BUFFER = 1

' BASS_StreamGetFilePosition modes
Global Const BASS_FILEPOS_DECODE = 0
Global Const BASS_FILEPOS_DOWNLOAD = 1
Global Const BASS_FILEPOS_END = 2

' STREAMFILEPROC actions
Global Const BASS_FILE_CLOSE = 0
Global Const BASS_FILE_READ = 1
Global Const BASS_FILE_QUERY = 2
Global Const BASS_FILE_LEN = 3

'**************************************************************
'* DirectSound interfaces (for use with BASS_GetDSoundObject) *
'**************************************************************
Global Const BASS_OBJECT_DS = 1                     ' DirectSound
Global Const BASS_OBJECT_DS3DL = 2                  'IDirectSound3DListener

'******************************
'* DX7 voice allocation flags *
'******************************
' Play the sample in hardware. If no hardware voices are available then
' the "play" call will fail
Global Const BASS_VAM_HARDWARE = 1
' Play the sample in software (ie. non-accelerated). No other VAM flags
'may be used together with this flag.
Global Const BASS_VAM_SOFTWARE = 2

'******************************
'* DX7 voice management flags *
'******************************
' These flags enable hardware resource stealing... if the hardware has no
' available voices, a currently playing buffer will be stopped to make room for
' the new buffer. NOTE: only samples loaded/created with the BASS_SAMPLE_VAM
' flag are considered for termination by the DX7 voice management.

' If there are no free hardware voices, the buffer to be terminated will be
' the one with the least time left to play.
Global Const BASS_VAM_TERM_TIME = 4
' If there are no free hardware voices, the buffer to be terminated will be
' one that was loaded/created with the BASS_SAMPLE_MUTEMAX flag and is beyond
' it 's max distance. If there are no buffers that match this criteria, then the
' "play" call will fail.
Global Const BASS_VAM_TERM_DIST = 8
' If there are no free hardware voices, the buffer to be terminated will be
' the one with the lowest priority.
Global Const BASS_VAM_TERM_PRIO = 16

'**********************************************************************
'* software 3D mixing algorithm modes (used with BASS_Set3DAlgorithm) *
'**********************************************************************
' default algorithm (currently translates to BASS_3DALG_OFF)
Global Const BASS_3DALG_DEFAULT = 0
' Uses normal left and right panning. The vertical axis is ignored except for
'scaling of volume due to distance. Doppler shift and volume scaling are still
'applied, but the 3D filtering is not performed. This is the most CPU efficient
'software implementation, but provides no virtual 3D audio effect. Head Related
'Transfer Function processing will not be done. Since only normal stereo panning
'is used, a channel using this algorithm may be accelerated by a 2D hardware
'voice if no free 3D hardware voices are available.
Global Const BASS_3DALG_OFF = 1
' This algorithm gives the highest quality 3D audio effect, but uses more CPU.
' Requires Windows 98 2nd Edition or Windows 2000 that uses WDM drivers, if this
' mode is not available then BASS_3DALG_OFF will be used instead.
Global Const BASS_3DALG_FULL = 2
' This algorithm gives a good 3D audio effect, and uses less CPU than the FULL
' mode. Requires Windows 98 2nd Edition or Windows 2000 that uses WDM drivers, if
' this mode is not available then BASS_3DALG_OFF will be used instead.
Global Const BASS_3DALG_LIGHT = 3

Type BASS_INFO
    size As Long          'size of this struct (set this before calling the function)
    flags As Long         'device capabilities (DSCAPS_xxx flags)
    hwsize As Long        'size of total device hardware memory
    hwfree As Long        'size of free device hardware memory
    freesam As Long       'number of free sample slots in the hardware
    free3d As Long        'number of free 3D sample slots in the hardware
    minrate As Long       'min sample rate supported by the hardware
    maxrate As Long       'max sample rate supported by the hardware
    eax As Long           'device supports EAX? (always BASSFALSE if BASS_DEVICE_3D was not used)
    minbuf As Long        'recommended minimum buffer length in ms (requires BASS_DEVICE_LATENCY)
    dsver As Long         'DirectSound version
    latency As Long       'delay (in ms) before start of playback (requires BASS_DEVICE_LATENCY)
    initflags As Long     '"flags" parameter of BASS_Init call
    speakers As Long      'number of speakers available
End Type

Type BASS_RECORDINFO
    size As Long          ' size of this struct (set this before calling the function)
    flags As Long         ' device capabilities (DSCCAPS_xxx flags)
    formats As Long       ' supported standard formats (WAVE_FORMAT_xxx flags)
    inputs As Long        ' number of inputs
    singlein As Long      ' BASSTRUE = only 1 input can be set at a time
End Type

'*************************
'* Sample info structure *
'*************************
Type BASS_SAMPLE
    freq As Long          'default playback rate
    volume As Long        'default volume (0-100)
    pan As Long           'default pan (-100=left, 0=middle, 100=right)
    flags As Long         'BASS_SAMPLE_xxx flags
    Length As Long        'length (in samples, not bytes)
    max As Long           ' maximum simultaneous playbacks
    ' The following are the sample's default 3D attributes (if the sample
    ' is 3D, BASS_SAMPLE_3D is in flags) see BASS_ChannelSet3DAttributes
    mode3d As Long        'BASS_3DMODE_xxx mode
    mindist As Single     'minimum distance
    MAXDIST As Single     'maximum distance
    iangle As Long        'angle of inside projection cone
    oangle As Long        'angle of outside projection cone
    outvol As Long        'delta-volume outside the projection cone
    ' The following are the defaults used if the sample uses the DirectX 7
    ' voice allocation/management features.
    vam As Long           'voice allocation/management flags (BASS_VAM_xxx)
    priority As Long      'priority (0=lowest, &Hffffffff=highest)
End Type

'********************************************************
'* 3D vector (for 3D positions/velocities/orientations) *
'********************************************************
Type BASS_3DVECTOR
    X As Single           '+=right, -=left
    Y As Single           '+=up, -=down
    z As Single           '+=front, -=behind
End Type

' DX8 effect types, use with BASS_ChannelSetFX
Global Const BASS_FX_CHORUS = 0                 ' GUID_DSFX_STANDARD_CHORUS
Global Const BASS_FX_COMPRESSOR = 1             ' GUID_DSFX_STANDARD_COMPRESSOR
Global Const BASS_FX_DISTORTION = 2             ' GUID_DSFX_STANDARD_DISTORTION
Global Const BASS_FX_ECHO = 3                   ' GUID_DSFX_STANDARD_ECHO
Global Const BASS_FX_FLANGER = 4                ' GUID_DSFX_STANDARD_FLANGER
Global Const BASS_FX_GARGLE = 5                 ' GUID_DSFX_STANDARD_GARGLE
Global Const BASS_FX_I3DL2REVERB = 6    ' GUID_DSFX_STANDARD_I3DL2REVERB
Global Const BASS_FX_PARAMEQ = 7                ' GUID_DSFX_STANDARD_PARAMEQ
Global Const BASS_FX_REVERB = 8                 ' GUID_DSFX_WAVES_REVERB

Type BASS_FXCHORUS              ' DSFXChorus
    fWetDryMix As Single
    fDepth As Single
    fFeedback As Single
    fFrequency As Single
    lWaveform As Long   ' 0=triangle, 1=sine
    fDelay As Single
    lPhase As Long              ' BASS_FX_PHASE_xxx
End Type

Type BASS_FXCOMPRESSOR  ' DSFXCompressor
    fGain As Single
    fAttack As Single
    fRelease As Single
    fThreshold As Single
    fRatio As Single
    fPredelay As Single
End Type

Type BASS_FXDISTORTION  ' DSFXDistortion
    fGain As Single
    fEdge As Single
    fPostEQCenterFrequency As Single
    fPostEQBandwidth As Single
    fPreLowpassCutoff As Single
End Type

Type BASS_FXECHO                ' DSFXEcho
    fWetDryMix As Single
    fFeedback As Single
    fLeftDelay As Single
    fRightDelay As Single
    lPanDelay As Long
End Type

Type BASS_FXFLANGER             ' DSFXFlanger
    fWetDryMix As Single
    fDepth As Single
    fFeedback As Single
    fFrequency As Single
    lWaveform As Long   ' 0=triangle, 1=sine
    fDelay As Single
    lPhase As Long              ' BASS_FX_PHASE_xxx
End Type

Type BASS_FXGARGLE              ' DSFXGargle
    dwRateHz As Long               ' Rate of modulation in hz
    dwWaveShape As Long            ' 0=triangle, 1=square
End Type

Type BASS_FXI3DL2REVERB ' DSFXI3DL2Reverb
    lRoom As Long                    ' [-10000, 0]      default: -1000 mB
    lRoomHF As Long                  ' [-10000, 0]      default: 0 mB
    flRoomRolloffFactor As Single    ' [0.0, 10.0]      default: 0.0
    flDecayTime As Single            ' [0.1, 20.0]      default: 1.49s
    flDecayHFRatio As Single         ' [0.1, 2.0]       default: 0.83
    lReflections As Long             ' [-10000, 1000]   default: -2602 mB
    flReflectionsDelay As Single     ' [0.0, 0.3]       default: 0.007 s
    lReverb As Long                  ' [-10000, 2000]   default: 200 mB
    flReverbDelay As Single          ' [0.0, 0.1]       default: 0.011 s
    flDiffusion As Single            ' [0.0, 100.0]     default: 100.0 %
    flDensity As Single              ' [0.0, 100.0]     default: 100.0 %
    flHFReference As Single          ' [20.0, 20000.0]  default: 5000.0 Hz
End Type

Type BASS_FXPARAMEQ             ' DSFXParamEq
    fCenter As Single
    fBandwidth As Single
    fGain As Single
End Type

Type BASS_FXREVERB              ' DSFXWavesReverb
    fInGain As Single                ' [-96.0,0.0]            default: 0.0 dB
    fReverbMix As Single             ' [-96.0,0.0]            default: 0.0 db
    fReverbTime As Single            ' [0.001,3000.0]         default: 1000.0 ms
    fHighFreqRTRatio As Single       ' [0.001,0.999]          default: 0.001
End Type

Global Const BASS_FX_PHASE_NEG_180 = 0
Global Const BASS_FX_PHASE_NEG_90 = 1
Global Const BASS_FX_PHASE_ZERO = 2
Global Const BASS_FX_PHASE_90 = 3
Global Const BASS_FX_PHASE_180 = 4

Type GUID       ' used with BASS_SetCLSID
    Data1 As Long
    Data2 As Integer
    Data3 As Integer
    Data4(7) As Byte
End Type


Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" _
(ByRef Destination As Any, _
ByRef Source As Any, _
ByVal Length As Long)

Private Declare Function lstrlen Lib "kernel32" Alias "lstrlenA" _
(ByVal lpString As Long) As Long

' Retrieve the version number of BASS that is loaded. RETURN : The BASS version (LOWORD.HIWORD)
Declare Function BASS_GetVersion Lib "bass.dll" () As Long

' Gets a pointer to the text description of a device. This function can be used to enumerate the available devices.
' devnum: The device (0=First)
' RETURN : The text description of the device (NULL=error)
' NOTE: use BASS_GetDeviceDestriptionString to get the text description
Declare Function BASS_GetDeviceDescription Lib "bass.dll" (ByVal devnum As Long) As Long

' Set the amount that BASS mixes ahead new musics/streams.
' Changing this setting does not affect musics/streams
' that have already been loaded/created. Increasing the
' buffer length, decreases the chance of the sound
' possibly breaking-up on slower computers, but also
' requires more memory. The default length is 0.5 secs.
' length : The buffer length in seconds
' RETURN : The actual new buffer length
Declare Function BASS_SetBufferLength Lib "bass.dll" (ByVal Length As Single) As Single

' Set an internet config option.
' opt    : The option to set (one of the BASS_NET_xxx flags)
' value  : The new value
' RETURN : The old value (-1=error)
Declare Function BASS_SetNetConfig Lib "bass.dll" (ByVal opt As Long, ByVal value As Long) As Long

' Set the global music/sample/stream volume levels.
' musvol : MOD music global volume level (0-100, -1=leave current)
' samvol : Sample global volume level (0-100, -1=leave current)
' strvol : Stream global volume level (0-100, -1=leave current)
Declare Sub BASS_SetGlobalVolumes Lib "bass.dll" (ByVal musvol As Long, ByVal samvol As Long, ByVal strvol As Long)

' Retrieve the global music/sample/stream volume levels.
' musvol : MOD music global volume level (NULL=don't retrieve it)
' samvol : Sample global volume level (NULL=don't retrieve it)
' strvol : Stream global volume level (NULL=don't retrieve it)
Declare Sub BASS_GetGlobalVolumes Lib "bass.dll" (ByRef musvol As Long, ByRef samvol As Long, ByRef strvol As Long)

' Make the volume/panning values translate to a logarithmic curve,
' or a linear "curve" (the default)
' volume :   volume curve(BASSFALSE = linear, BASSTRUE = Log)
' pan    : panning curve (BASSFALSE=linear, BASSTRUE=log) */
Declare Sub BASS_SetLogCurves Lib "bass.dll" (ByVal volume As Long, ByVal pan As Long)

' Set the 3D algorithm for software mixed 3D channels (does not affect
' hardware mixed channels). Changing the mode only affects subsequently
' created or loaded samples/streams/musics, not those that already exist.
' Requires DirectX 7 or above.
' algo : algorithm flag (BASS_3DALG_xxx)
Declare Sub BASS_Set3DAlgorithm Lib "bass.dll" (ByVal algo As Long)

' Get the BASS_ERROR_xxx error code. Use this function to get the reason for an error.
Declare Function BASS_ErrorGetCode Lib "bass.dll" () As Long

' Set the class identifier of the object to create, that will be used
' to initialize DirectSound.
' clsid  : Class identifier of the object to create (NULL=use default)
Declare Sub BASS_SetCLSID Lib "bass.dll" (clsid As GUID)

' Initialize the digital output. This must be called before all
' following BASS functions (except CD and recording functions).
' The volume is initially set to 100 (the maximum),
' use BASS_SetVolume() to adjust it.
' device : Device to use (0=first, -1=default, -2=no sound)
' freq   : Output sample rate
' flags:     BASS_DEVICE_xxx flags (optional HIWORD=update period)
' win:       Owner window (0=current foreground window)

' NOTE: The "no sound" device (device=-2), allows loading
' and "playing" of MOD musics only (all sample/stream
' functions and most other functions fail). This is so
' that you can still use the MOD musics as synchronizers
' when there is no soundcard present. When using device -2,
' you should still set the other arguments as you would do
' normally.
Declare Function BASS_Init Lib "bass.dll" (ByVal device As Long, ByVal freq As Long, ByVal flags As Long, ByVal win As Long) As Long

' Free all resources used by the digital output, including  all musics and samples.
Declare Sub BASS_Free Lib "bass.dll" ()

' Retrieve a pointer to a DirectSound interface. This can be used by
' advanced users to "plugin" external functionality.
' object : The interface to retrieve (BASS_OBJECT_xxx)
' RETURN : A pointer to the requested interface (NULL=error)
Declare Function BASS_GetDSoundObject Lib "bass.dll" (ByVal object As Long) As Long

' Retrieve some information on the device being used.
' info   : Pointer to store info at
Declare Sub BASS_GetInfo Lib "bass.dll" (ByRef info As BASS_INFO)

' Update the HMUSIC/HSTREAM channel buffers.
Declare Function BASS_Update Lib "bass.dll" () As Long

' Get the current CPU usage of BASS. This includes the time taken to mix
' the MOD musics and sample streams, and also the time taken by any user
' DSP functions. It does not include plain sample mixing which is done by
' the output device (hardware accelerated) or DirectSound (emulated). Audio
' CD playback requires no CPU usage.
' RETURN : The CPU usage percentage (floating-point)
Declare Function BASS_GetCPU Lib "bass.dll" () As Single

' Start the digital output.
Declare Function BASS_Start Lib "bass.dll" () As Long

' Stop the digital output, stopping all musics/samples/streams.
Declare Function BASS_Stop Lib "bass.dll" () As Long

' Stop the digital output, pausing all musics/samples/
' streams. Use BASS_Start to resume the digital output.
Declare Function BASS_Pause Lib "bass.dll" () As Long

' Set the digital output master volume.
' volume : Desired volume level (0-100)
Declare Function BASS_SetVolume Lib "bass.dll" (ByVal volume As Long) As Long

' Get the digital output master volume.
' RETURN : The volume level (0-100, -1=error)
Declare Function BASS_GetVolume Lib "bass.dll" () As Long

' Set the factors that affect the calculations of 3D sound.
' distf  : Distance factor (0.0-10.0, 1.0=use meters, 0.3=use feet, <0.0=leave current)
'          By default BASS measures distances in meters, you can change this
'          setting if you are using a different unit of measurement.
' roolf  : Rolloff factor, how fast the sound quietens with distance
'          (0.0=no rolloff, 1.0=real world, 2.0=2x real... 10.0=max, <0.0=leave current)
' doppf  : Doppler factor (0.0=no doppler, 1.0=real world, 2.0=2x real... 10.0=max, <0.0=leave current)
'          The doppler effect is the way a sound appears to change frequency when it is
'          moving towards or away from you. The listener and sound velocity settings are
'          used to calculate this effect, this "doppf" value can be used to lessen or
'          exaggerate the effect.
Declare Function BASS_Set3DFactors Lib "bass.dll" (ByVal distf As Single, ByVal rollf As Single, ByVal doppf As Single) As Long

' Get the factors that affect the calculations of 3D sound.
' distf  : Distance factor (NULL=don't get it)
' roolf  : Rolloff factor (NULL=don't get it)
' doppf  : Doppler factor (NULL=don't get it)
Declare Function BASS_Get3DFactors Lib "bass.dll" (ByRef distf As Single, ByRef rollf As Single, ByRef doppf As Single) As Long

' Set the position/velocity/orientation of the listener (ie. the player/viewer).
' pos    : Position of the listener (NULL=leave current)
' vel    : listener 's velocity, used to calculate doppler effect (NULL=leave current)
' front  : Direction that listener's front is pointing (NULL=leave current)
' top    : Direction that listener's top is pointing (NULL=leave current)
' NOTE   : front & top must both be set in a single call
Declare Function BASS_Set3DPosition Lib "bass.dll" (ByRef pos As Any, ByRef vel As Any, ByRef front As Any, ByRef top As Any) As Long

' Get the position/velocity/orientation of the listener.
' pos    : Position of the listener (NULL=don't get it)
' vel    : listener 's velocity (NULL=don't get it)
' front  : Direction that listener's front is pointing (NULL=don't get it)
' top    : Direction that listener's top is pointing (NULL=don't get it)
' NOTE   : front & top must both be retrieved in a single call
Declare Function BASS_Get3DPosition Lib "bass.dll" (ByRef pos As Any, ByRef vel As Any, ByRef front As Any, ByRef top As Any) As Long

' Apply changes made to the 3D system. This must be called to apply any changes
' made with BASS_Set3DFactors, BASS_Set3DPosition, BASS_ChannelSet3DAttributes or
' BASS_ChannelSet3DPosition. It improves performance to have DirectSound do all the
' required recalculating at the same time like this, rather than recalculating after
' every little change is made. NOTE: This is automatically called when starting a 3D
' sample with BASS_SamplePlay3D/Ex.
Declare Function BASS_Apply3D Lib "bass.dll" () As Long

' Set the type of EAX environment and it's parameters. Obviously, EAX functions
' have no effect if no EAX supporting device (ie. SB Live) is used.
' env    : Reverb environment (EAX_ENVIRONMENT_xxx, -1=leave current)
' vol    : Volume of the reverb (0.0=off, 1.0=max, <0.0=leave current)
' decay  : Time in seconds it takes the reverb to diminish by 60dB (0.1-20.0, <0.0=leave current)
' damp   : The damping, high or low frequencies decay faster (0.0=high decays quickest,
'          1.0=low/high decay equally, 2.0=low decays quickest, <0.0=leave current)
Declare Function BASS_SetEAXParameters Lib "bass.dll" (ByVal env As Long, ByVal vol As Single, ByVal decay As Single, ByVal damp As Single) As Long

' Get the current EAX parameters.
' env    : Reverb environment (NULL=don't get it)
' vol    : Reverb volume (NULL=don't get it)
' decay  : Decay duration (NULL=don't get it)
' damp   : The damping (NULL=don't get it)
Declare Function BASS_GetEAXParameters Lib "bass.dll" (ByRef env As Long, ByRef vol As Single, ByRef decay As Single, ByRef damp As Single) As Long


' Load a music (MO3/XM/MOD/S3M/IT/MTM). The amplification and pan
' seperation are initially set to 50, use BASS_MusicSetAmplify()
' and BASS_MusicSetPanSep() to adjust them.
' mem    : BASSTRUE = Load music from memory
' f      : Filename (mem=BASSFALSE) or memory location (mem=BASSTRUE)
' offset : File offset to load the music from (only used if mem=BASSFALSE)
' length : Data length (only used if mem=BASSFALSE, 0=use to end of file)
' flags  : BASS_MUSIC_xxx flags
' RETURN : The loaded music's handle (NULL=error)
Declare Function BASS_MusicLoad Lib "bass.dll" (ByVal mem As Long, ByVal f As Any, ByVal offset As Long, ByVal Length As Long, ByVal flags As Long) As Long

'  Free a music's resources. handle =  Music handle
Declare Sub BASS_MusicFree Lib "bass.dll" (ByVal handle As Long)

' Retrieves a pointer to music's name.
' handle : Music handle
' RETURN : Pointer to the Music 's name (NULL=error)
' NOTE: Use the function BASS_MusicGetNameString to get the actual name as a string.
Declare Function BASS_MusicGetName Lib "bass.dll" (ByVal handle As Long) As Long

' Retrieves the length of a music in patterns (how many "orders" there are)
' or in output bytes (requires BASS_MUSIC_CALCLEN was used with BASS_MusicLoad).
' handle : Music handle
' playlen: BASSTRUE=get the playback length, BASSFALSE=get the pattern length
' RETURN : The length of the music (-1=error)
Declare Function BASS_MusicGetLength Lib "bass.dll" (ByVal handle As Long, ByVal playlen As Long) As Long

' Pre-buffer initial sample data ready for playback.
' handle : Handle of music
Declare Function BASS_MusicPreBuf Lib "bass.dll" (ByVal handle As Long) As Long

' Play a music. Playback continues from where it was last stopped/paused.
' Multiple musics may be played simultaneously.
' handle : Handle of music to play
Declare Function BASS_MusicPlay Lib "bass.dll" (ByVal handle As Long) As Long

' Play a music, specifying start position and playback flags.
' handle : Handle of music to play
' pos    : Position to start playback from, LOWORD=order HIWORD=row
' flags  : BASS_MUSIC_xxx flags. These flags overwrite the defaults
'          specified when the music was loaded. (-1=use current flags)
' reset  : BASSTRUE = Stop all current playing notes and reset bpm/etc...
Declare Function BASS_MusicPlayEx Lib "bass.dll" (ByVal handle As Long, ByVal pos As Long, ByVal flags As Long, ByVal reset As Long) As Long

' Set a music's amplification level.
' handle:    Music handle
' amp:       Amplification Level(0 - 100)
Declare Function BASS_MusicSetAmplify Lib "bass.dll" (ByVal handle As Long, amp As Long) As Long

' Set a music's pan seperation.
' handle:    Music handle
' pan:       pan seperation(0 - 100, 50 = linear)
Declare Function BASS_MusicSetPanSep Lib "bass.dll" (ByVal handle As Long, pan As Long) As Long

' Set a music's "GetPosition" scaler
' When you call BASS_ChannelGetPosition, the "row" (HIWORD) will be
' scaled by this value. By using a higher scaler, you can get a more
' precise position indication.
' handle:    Music handle
' Scale: The scaler(1 - 256)
Declare Function BASS_MusicSetPositionScaler Lib "bass.dll" (ByVal handle As Long, ByVal pscale As Long) As Long

' Set the volume level of a channel in a music
' handle : Music handle
' channel: Channel number (0=first)
' volume : Volume level (0-100)
Declare Function BASS_MusicSetChannelVol Lib "bass.dll" (ByVal handle As Long, ByVal channel As Long, ByVal volume As Long) As Long

' Get the volume level of a channel in a music
' handle : Music handle
' channel: Channel number (0=first)
' RETURN : The channel's volume (-1=error)
Declare Function BASS_MusicGetChannelVol Lib "bass.dll" (ByVal handle As Long, ByVal channel As Long) As Long


' Load a WAV/MP3/MP2/MP1 sample. If you're loading a sample with 3D
' functionality, then you should use BASS_GetInfo and BASS_SetInfo to set
' the default 3D parameters. You can also use these two functions to set
' the sample's default frequency/volume/pan/looping.
' mem    : BASSTRUE = Load sample from memory
' f      : Filename (mem=BASSFALSE) or memory location (mem=BASSTRUE)
' offset : File offset to load the sample from (only used if mem=BASSFALSE)
' length : Data length (only used if mem=BASSFALSE, 0=use to end of file)
' max    : Maximum number of simultaneous playbacks (1-65535)
' flags  : BASS_SAMPLE_xxx flags (only the LOOP/3D/SOFTWARE/DEFER/MUTEMAX/OVER_xxx flags are used)
' RETURN : The loaded sample's handle (NULL=error)
Declare Function BASS_SampleLoad Lib "bass.dll" (ByVal mem As Long, ByVal f As Any, ByVal offset As Long, ByVal Length As Long, ByVal max As Long, ByVal flags As Long) As Long

' Create a sample. This function allows you to generate custom samples, or
' load samples that are not in the WAV format. A pointer is returned to the
' memory location at which you should write the sample's data. After writing
' the data, call BASS_SampleCreateDone to get the new sample's handle.
' length:    The sample 's length (in samples, NOT bytes)
' freq   : default sample rate
' max    : Maximum number of simultaneous playbacks (1-65535)
' flags:     BASS_SAMPLE_xxx flags
' RETURN : Memory location to write the sample's data (NULL=error)
Declare Function BASS_SampleCreate Lib "bass.dll" (ByVal Length As Long, ByVal freq As Long, ByVal max As Long, ByVal flags As Long) As Long

' Finished creating a new sample.
' Return: The New sample 's handle (NULL=error)
Declare Function BASS_SampleCreateDone Lib "bass.dll" () As Long

' Free a sample's resources.
' handle:    Sample handle
Declare Sub BASS_SampleFree Lib "bass.dll" (ByVal handle As Long)

' Retrieve a sample's current default attributes.
' handle:    Sample handle
' info   : Pointer to store sample info
Declare Function BASS_SampleGetInfo Lib "bass.dll" (ByVal handle As Long, ByRef info As BASS_SAMPLE) As Long

'Set a sample's default attributes.
' handle:    Sample handle
' info   : Sample info, only the freq/volume/pan/3D attributes and
'          looping/override method flags are used
Declare Function BASS_SampleSetInfo Lib "bass.dll" (ByVal handle As Long, ByRef info As BASS_SAMPLE) As Long

' Play a sample, using the sample's default attributes.
' handle : Handle of sample to play
' RETURN : Handle of channel used to play the sample (NULL=error)
Declare Function BASS_SamplePlay Lib "bass.dll" (ByVal handle As Long) As Long

' Play a sample, using specified attributes.
' handle : Handle of sample to play
' start  : Playback start position (in samples, not bytes)
' freq:      Playback Rate(-1 = Default)
' volume : Volume (-1=default, 0=silent, 100=max)
' pan:       pan position(-101 = Default, -100 = Left, 0 = middle, 100 = Right)
' loop   : 1 = Loop sample (-1=default)
' RETURN : Handle of channel used to play the sample (NULL=error)
Declare Function BASS_SamplePlayEx Lib "bass.dll" (ByVal handle As Long, ByVal start As Long, ByVal freq As Long, ByVal volume As Long, ByVal pan As Long, ByVal pLoop As Long) As Long

' Play a 3D sample, setting it's 3D position, orientation and velocity.
' handle : Handle of sample to play
' pos    : position of the sound (NULL = x/y/z=0.0)
' orient : orientation of the sound, this is irrelevant if it's an
'          omnidirectional sound source (NULL = x/y/z=0.0)
' vel    : velocity of the sound (NULL = x/y/z=0.0)
' RETURN : Handle of channel used to play the sample (NULL=error)
Declare Function BASS_SamplePlay3D Lib "bass.dll" (ByVal handle As Long, ByRef pos As Any, ByRef orient As Any, ByRef vel As Any) As Long

' Play a 3D sample, using specified attributes.
' handle : Handle of sample to play
' pos    : position of the sound (NULL = x/y/z=0.0)
' orient : orientation of the sound, this is irrelevant if it's an
'          omnidirectional sound source (NULL = x/y/z=0.0)
' vel    : velocity of the sound (NULL = x/y/z=0.0)
' start  : Playback start position (in samples, not bytes)
' freq:      Playback Rate(-1 = Default)
' volume : Volume (-1=default, 0=silent, 100=max)
' loop   : 1 = Loop sample (-1=default)
' RETURN : Handle of channel used to play the sample (NULL=error)
Declare Function BASS_SamplePlay3DEx Lib "bass.dll" (ByVal handle As Long, ByRef pos As Any, ByRef orient As Any, ByRef vel As Any, ByVal start As Long, ByVal freq As Long, ByVal volume As Long, ByVal pLoop As Long) As Long

' Stops all instances of a sample. For example, if a sample is playing
' simultaneously 3 times, calling this function will stop all 3 of them,
' which is obviously simpler than calling BASS_ChannelStop() 3 times.
' handle : Handle of sample to stop
Declare Function BASS_SampleStop Lib "bass.dll" (ByVal handle As Long) As Long


' Create a user sample stream.
' freq   : Stream playback rate
' flags  : BASS_SAMPLE_xxx flags (only the 8BITS/FLOAT/MONO/3D flags are used)
' proc   : User defined stream writing function pass using "AddressOf STREAMPROC"
' user   : The 'user' value passed to the callback function
' RETURN : The created stream's handle (NULL=error)
Declare Function BASS_StreamCreate Lib "bass.dll" (ByVal freq As Long, ByVal flags As Long, ByVal proc As Long, ByVal user As Long) As Long

' Create a sample stream from an MP3/MP2/MP1/OGG or WAV file.
' mem    : BASSTRUE = Stream file from memory
' f      : Filename (mem=BASSFALSE) or memory location (mem=BASSTRUE)
' offset : File offset of the stream data
' length : File length (0=use whole file if mem=BASSFALSE)
' flags  : Flags
' RETURN : The created stream's handle (NULL=error)
Declare Function BASS_StreamCreateFile Lib "bass.dll" (ByVal mem As Long, ByVal f As Any, ByVal offset As Long, ByVal Length As Long, ByVal flags As Long) As Long

' Create a sample stream from an MP3/MP2/MP1/OGG or WAV file on the internet.
' url    : The URL (beginning with "http://" or "ftp://")
' offset : File offset of start streaming from
' flags  : Flags
' save   : Filename to save the streamed file as locally (""=don't save)
' RETURN : The created stream's handle (NULL=error)
Declare Function BASS_StreamCreateURL Lib "bass.dll" (ByVal url As String, ByVal offset As Long, ByVal flags As Long, ByVal save As String) As Long

' Free a sample stream's resources.
' stream:    stream handle
Declare Sub BASS_StreamFree Lib "bass.dll" (ByVal handle As Long)

' Retrieves the playback length (in bytes) of a file stream. It's not always
' possible to 100% accurately guess the length of a stream, so the length returned
' may be only an approximation when using some WAV codecs.
' handle :  Stream handle
' RETURN : The length (0=streaming in blocks, -1=error)
Declare Function BASS_StreamGetLength Lib "bass.dll" (ByVal handle As Long) As Long

' Retrieves the requested tags/headers, if available.
' handle : Stream handle
' tags   : A BASS_TAG_xxx flag
' RETURN : Pointer to the tags (NULL=error)
Declare Function BASS_StreamGetTags Lib "bass.dll" (ByVal handle As Long, ByVal tags As Long) As Long

' Pre-buffer initial sample data ready for playback.
' handle : Handle of stream
Declare Function BASS_StreamPreBuf Lib "bass.dll" (ByVal handle As Long) As Long

' Play a sample stream, optionally flushing the buffer first.
' handle : Handle of stream to play
' flush  : Flush buffer contents. If you stop a stream and then want to
'          continue it from where it stopped, don't flush it. Flushing
'          a file stream causes it to restart from the beginning.
' flags  : BASS_SAMPLE_xxx flags (only affects file streams, and only the
'          LOOP flag is used)
Declare Function BASS_StreamPlay Lib "bass.dll" (ByVal handle As Long, ByVal flush As Long, ByVal flags As Long) As Long

' Retrieves the file position of the decoding, the download (if streaming from
' the internet), or the end (total length). Obviously only works with file streams.
' handle : Stream handle
' mode   : The position to retrieve (0=decoding, 1=download, 2=end)
' RETURN : The position (-1=error)
Declare Function BASS_StreamGetFilePosition Lib "bass.dll" (ByVal handle As Long, ByVal mode As Long) As Long


' Initialize the CD functions, must be called before any other CD
' functions. The volume is initially set to 100 (the maximum), use
' BASS_ChannelSetAttributes() to adjust it.
' drive  : The CD drive, for example: "d:" (NULL=use default drive)
' flags  : BASS_DEVICE_LEAVEVOL = leave the volume as it is
Declare Function BASS_CDInit Lib "bass.dll" (ByVal drive As Any, ByVal flags As Long) As Long

' Free resources used by the CD
Declare Sub BASS_CDFree Lib "bass.dll" ()

' Check if there is a CD in the drive.
' RETURN : 1 if cd in drive, 0 if not.
Declare Function BASS_CDInDrive Lib "bass.dll" () As Long

' Opens or closes the CD door.
' dopen  : BASSTRUE=open the door
Declare Function BASS_CDDoor Lib "bass.dll" (ByVal dopen As Long) As Long

' Retrieves identification info from the CD in the drive.
' id     : One of the BASS_CDID_xxx flags
' RETURN : ID string (NULL=error)
' NOTE: Use BASS_CDGetIDString to get the actual string.
Declare Function BASS_CDGetID Lib "bass.dll" (ByVal id As Long) As Long

' Retrieves the number of tracks on the CD
' RETURN : The number of tracks (-1=error)
Declare Function BASS_CDGetTracks Lib "bass.dll" () As Long

' Play a CD track.
' track  : Track number to play (1=first)
' loop   : BASSTRUE = Loop the track
' wait   : BASSTRUE = don't return until playback has started
'          (some drives will always wait anyway)
Declare Function BASS_CDPlay Lib "bass.dll" (ByVal track As Long, ByVal pLoop As Long, ByVal wait As Long) As Long

' Retrieves the playback length (in milliseconds) of a cd track.
' track  : The CD track (1=first)
' RETURN : The length (-1=error)
Declare Function BASS_CDGetTrackLength Lib "bass.dll" (ByVal track As Long) As Long


' Get the text description of a recording device. This function can be
' used to enumerate the available devices.
' devnum : The device (0=first)
' RETURN : The text description of the device (NULL=error)
Declare Function BASS_RecordGetDeviceDescription Lib "bass.dll" (ByVal devnum As Long) As Long

' Initialize a recording device.
' device : Device to use (0=first, -1=default)
Declare Function BASS_RecordInit Lib "bass.dll" (ByVal device As Long) As Long

' Free all resources used by the recording device.
Declare Sub BASS_RecordFree Lib "bass.dll" ()

' Retrieve some information on the recording device being used.
' info   : Pointer to store info at
Declare Sub BASS_RecordGetInfo Lib "bass.dll" (ByRef info As BASS_RECORDINFO)

' Get the text description of a recording input.
' inputn : Input number (0=first)
' RETURN : The text description (0=error)
Declare Function BASS_RecordGetInputName Lib "bass.dll" (ByVal inputn As Long) As Long

' Adjust the setting of a recording input.
' inputn : Input number (0=first)
' setting: BASS_INPUT flags (if BASS_INPUT_LEVEL used, LOWORD=volume)
Declare Function BASS_RecordSetInput Lib "bass.dll" (ByVal inputn As Long, ByVal setting As Long) As Long

' Retrieve the setting of a recording input.
' inputn : Input number (0=first)
' RETURN : The setting (LOWORD=volume, with BASS_INPUT_OFF flag if off, -1=error)
Declare Function BASS_RecordGetInput Lib "bass.dll" (ByVal inputn As Long) As Long

' Start recording.
' freq   : Sampling rate
' flags  : BASS_SAMPLE_8BITS/FLOAT/MONO flags (optional HIWORD=update period)
' proc   : User defined function to receive the recorded data
' user   : The 'user' value passed to the callback function
Declare Function BASS_RecordStart Lib "bass.dll" (ByVal freq As Long, ByVal flags As Long, ByVal proc As Long, ByVal user As Long) As Long


'*******************************************************************************
'* A "channel" can be a playing sample (HCHANNEL), a MOD music (HMUSIC), a     *
'* sample stream (HSTREAM), the CD (CDCHANNEL), or recording (RECORDCHAN). The *
'* following functions can be used with one or more of these channel types.    *
'*******************************************************************************

' Translate a byte position into time (seconds)
' handle : Handle of channel (HCHANNEL/HMUSIC/HSTREAM, also HSAMPLE)
' pos    : The position to translate
' RETURN : The millisecond position (-1=error)
Private Declare Function BASS_ChannelBytes2Seconds64 Lib "bass.dll" Alias "BASS_ChannelBytes2Seconds" (ByVal handle As Long, ByVal pos As Long, ByVal poshigh As Long) As Single

' Translate a time (seconds) position into bytes
' handle : Handle of channel (HCHANNEL/HMUSIC/HSTREAM, also HSAMPLE)
' pos    : The position to translate
' RETURN : The byte position (-1=error)
Declare Function BASS_ChannelSeconds2Bytes Lib "bass.dll" (ByVal handle As Long, ByVal pos As Single) As Long

' Check if a channel is active (playing) or stalled.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL/RECORDCHAN)
' RETURN : One of the BASS_ACTIVE_xxx values
Declare Function BASS_ChannelIsActive Lib "bass.dll" (ByVal handle As Long) As Long

' Get some info about a channel.
' handle:    channel handle(HCHANNEL / HMUSIC / HSTREAM)
' RETURN : BASS_SAMPLE_xxx flags (-1=error)
Declare Function BASS_ChannelGetFlags Lib "bass.dll" (ByVal handle As Long) As Long

' Stop a channel.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL/RECORDCHAN)
Declare Function BASS_ChannelStop Lib "bass.dll" (ByVal handle As Long) As Long

' Pause a channel.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL/RECORDCHAN)
Declare Function BASS_ChannelPause Lib "bass.dll" (ByVal handle As Long) As Long

' Resume a paused channel.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL/RECORDCHAN)
Declare Function BASS_ChannelResume Lib "bass.dll" (ByVal handle As Long) As Long

' Update a channel's attributes. The actual setting may not be exactly
' as specified, depending on the accuracy of the device and drivers.
' NOTE: Only the volume can be adjusted for the CD "channel", but not all
' soundcards allow controlling of the CD volume level.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL)
' freq   : Playback rate (-1=leave current)
' volume : Volume (-1=leave current, 0=silent, 100=max)
' pan    : pan position(-101 = current, -100 = Left, 0 = middle, 100 = Right)
'          panning has no effect on 3D channels
Declare Function BASS_ChannelSetAttributes Lib "bass.dll" (ByVal handle As Long, ByVal freq As Long, ByVal volume As Long, ByVal pan As Long) As Long

' Retrieve a channel's attributes. Only the volume is available for
' the CD "channel" (if allowed by the soundcard/drivers).
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL)
' freq   : Pointer to store playback rate (NULL=don't retrieve it)
' volume : Pointer to store volume (NULL=don't retrieve it)
' pan    : Pointer to store pan position (NULL=don't retrieve it)
Declare Function BASS_ChannelGetAttributes Lib "bass.dll" (ByVal handle As Long, ByRef freq As Long, ByRef volume As Long, ByRef pan As Long) As Long

' Slide a channel's attributes over a period of time.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL)
' freq   : Playback rate (100-100000, -1=leave current)
' volume : Volume (-1=leave current, 0=silent, 100=max, -2 = fadeout and stop channel)
' pan    : Pan position (-101=current, -100=left, 0=middle, 100=right)
' time   : Duration of the slide (in milliseconds)
Declare Function BASS_ChannelSlideAttributes Lib "bass.dll" (ByVal handle As Long, ByVal freq As Long, ByVal volume As Long, ByVal pan As Long, ByVal time As Long) As Long

' Check if a channel's attributes (freq/vol/pan) are sliding.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL)
' RETURN : Combination of the BASS_SLIDE_xxx flags
Declare Function BASS_ChannelIsSliding Lib "bass.dll" (ByVal handle As Long) As Long

' Set a channel's 3D attributes.
' handle : channel handle(HCHANNEL / HSTREAM / HMUSIC)
' mode   : BASS_3DMODE_xxx mode (-1=leave current setting)
' min    : minimum distance, volume stops increasing within this distance (<0.0=leave current)
' max    : maximum distance, volume stops decreasing past this distance (<0.0=leave current)
' iangle : angle of inside projection cone in degrees (360=omnidirectional, -1=leave current)
' oangle : angle of outside projection cone in degrees (-1=leave current)
'          NOTE: iangle & oangle must both be set in a single call
' outvol : delta-volume outside the projection cone (0=silent, 100=same as inside)
' The iangle/oangle angles decide how wide the sound is projected around the
' orientation angle. Within the inside angle the volume level is the channel
' level as set with BASS_ChannelSetAttributes, from the inside to the outside
' angles the volume gradually changes by the "outvol" setting.
Declare Function BASS_ChannelSet3DAttributes Lib "bass.dll" (ByVal handle As Long, ByVal mode As Long, ByVal min As Single, ByVal max As Single, ByVal iangle As Long, ByVal oangle As Long, ByVal outvol As Long) As Long

' Retrieve a channel's 3D attributes.
' handle : channel handle(HCHANNEL / HSTREAM / HMUSIC)
' mode   : BASS_3DMODE_xxx mode (NULL=don't retrieve it)
' min    : minumum distance (NULL=don't retrieve it)
' max    : maximum distance (NULL=don't retrieve it)
' iangle : angle of inside projection cone (NULL=don't retrieve it)
' oangle : angle of outside projection cone (NULL=don't retrieve it)
'          NOTE: iangle & oangle must both be retrieved in a single call
' outvol : delta-volume outside the projection cone (NULL=don't retrieve it)
Declare Function BASS_ChannelGet3DAttributes Lib "bass.dll" (ByVal handle As Long, ByRef mode As Long, ByRef min As Single, ByRef max As Single, ByRef iangle As Long, ByRef oangle As Long, ByRef outvol As Long) As Long

' Update a channel's 3D position, orientation and velocity. The velocity
' is only used to calculate the doppler effect.
' handle : channel handle(HCHANNEL / HSTREAM / HMUSIC)
' pos    : position of the sound (NULL=leave current)
' orient : orientation of the sound, this is irrelevant if it's an
'          omnidirectional sound source (NULL=leave current)
' vel    : velocity of the sound (NULL=leave current)
Declare Function BASS_ChannelSet3DPosition Lib "bass.dll" (ByVal handle As Long, ByRef pos As Any, ByRef orient As Any, ByRef vel As Any) As Long

' Retrieve a channel's current 3D position, orientation and velocity.
' handle : channel handle(HCHANNEL / HSTREAM / HMUSIC)
' pos    : position of the sound (NULL=don't retrieve it)
' orient : orientation of the sound, this is irrelevant if it's an
'          omnidirectional sound source (NULL=don't retrieve it)
' vel    : velocity of the sound (NULL=don't retrieve it)
Declare Function BASS_ChannelGet3DPosition Lib "bass.dll" (ByVal handle As Long, ByRef pos As Any, ByRef orient As Any, ByRef vel As Any) As Long

' Set the current playback position of a channel.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL)
' pos    : the position
'          if HCHANNEL: position in bytes
'          if HMUSIC: LOWORD=order HIWORD=row ... use MAKELONG(order,row)
'          if HSTREAM: position in bytes, file streams only
'          if CDCHANNEL: position in milliseconds from start of track
Private Declare Function BASS_ChannelSetPosition64 Lib "bass.dll" Alias "BASS_ChannelSetPosition" (ByVal handle As Long, ByVal pos As Long, ByVal poshigh As Long) As Long

' Get the current playback position of a channel.
' handle : Channel handle (HCHANNEL/HMUSIC/HSTREAM, or CDCHANNEL)
' RETURN : the position (-1=error)
'          if HCHANNEL: position in bytes
'          if HMUSIC: LOWORD=order HIWORD=row (use GetLoWord(position), GetHiWord(Position))
'          if HSTREAM: total bytes played since the stream was last flushed
'          if CDCHANNEL: position in milliseconds from start of track
Declare Function BASS_ChannelGetPosition Lib "bass.dll" (ByVal handle As Long) As Long

' Calculate a channel's current output level.
' handle : channel handle(HMUSIC / HSTREAM, or RECORDCHAN)
' RETURN : LOWORD=left level (0-128) HIWORD=right level (0-128) (-1=error)
'          Use GetLoWord and GetHiWord functions on return function.
Declare Function BASS_ChannelGetLevel Lib "bass.dll" (ByVal handle As Long) As Long

' Retrieves upto "length" bytes of the channel's current sample data. This is
' useful if you wish to "visualize" the sound.
' handle:  Channel handle(HMUSIC / HSTREAM, or RECORDCHAN)
' buffer : Location to write the data
' length : Number of bytes wanted, or a BASS_DATA_xxx flag
' RETURN : Number of bytes actually written to the buffer (-1=error)
Declare Function BASS_ChannelGetData Lib "bass.dll" (ByVal handle As Long, ByRef buffer As Any, ByVal Length As Long) As Long

' Setup a sync on a channel. Multiple syncs may be used per channel.
' handle : Channel handle (currently there are only HMUSIC syncs)
' atype  : Sync type (BASS_SYNC_xxx type & flags)
' param  : Sync parameters (see the BASS_SYNC_xxx type description)
' proc   : User defined callback function (use AddressOf SYNCPROC)
' user   : The 'user' value passed to the callback function
' Return : Sync handle(Null = Error)
Private Declare Function BASS_ChannelSetSync64 Lib "bass.dll" Alias "BASS_ChannelSetSync" (ByVal handle As Long, ByVal atype As Long, ByVal param As Long, ByVal paramhigh As Long, ByVal proc As Long, ByVal user As Long) As Long

' Remove a sync from a channel
' handle : channel handle(HMUSIC)
' sync   : Handle of sync to remove
Declare Function BASS_ChannelRemoveSync Lib "bass.dll" (ByVal handle As Long, ByVal sync As Long) As Long

' Setup a user DSP function on a channel. When multiple DSP functions
' are used on a channel, they are called in the order that they were added.
' handle:  channel handle(HMUSIC / HSTREAM)
' proc   : User defined callback function
' user   : The 'user' value passed to the callback function
' RETURN : DSP handle (NULL=error)
Declare Function BASS_ChannelSetDSP Lib "bass.dll" (ByVal handle As Long, ByVal proc As Long, ByVal user As Long) As Long

' Remove a DSP function from a channel
' handle : channel handle(HMUSIC / HSTREAM)
' dsp    : Handle of DSP to remove */
' RETURN : BASSTRUE / BASSFALSE
Declare Function BASS_ChannelRemoveDSP Lib "bass.dll" (ByVal handle As Long, ByVal dsp As Long) As Long

' Set the wet(reverb)/dry(no reverb) mix ratio on the channel. By default
' the distance of the sound from the listener is used to calculate the mix.
' NOTE: The channel must have 3D functionality enabled for the EAX environment
' to have any affect on it.
' handle : channel handle(HCHANNEL / HSTREAM / HMUSIC)
' mix    : The ratio (0.0=reverb off, 1.0=max reverb, -1.0=let EAX calculate
'          the reverb mix based on the distance)
Declare Function BASS_ChannelSetEAXMix Lib "bass.dll" (ByVal handle As Long, ByVal mix As Single) As Long

' Get the wet(reverb)/dry(no reverb) mix ratio on the channel.
' handle:    channel handle(HCHANNEL / HSTREAM / HMUSIC)
' mix    : Pointer to store the ratio at
Declare Function BASS_ChannelGetEAXMix Lib "bass.dll" (ByVal handle As Long, ByRef mix As Single) As Long

' Set a link between 2 channels. When the 1st is played/stopped/paused/resumed
' the 2nd channel is also played/stopped/paused/resumed.
' handle : Handle of channel to link "chan" to (HMUSIC/HSTREAM)
' chan   : Handle of channel to link to "handle" (HMUSIC/HSTREAM)
Declare Function BASS_ChannelSetLink Lib "bass.dll" (ByVal handle As Long, ByVal chan As Long) As Long

' Remove a link from a channel.
' handle : Handle of channel to unlink with "chan" (HMUSIC/HSTREAM)
' chan   : Handle of channel to unlink from "handle" (HMUSIC/HSTREAM)
Declare Function BASS_ChannelRemoveLink Lib "bass.dll" (ByVal handle As Long, ByVal chan As Long) As Long

' Setup a DX8 effect on a channel. Use BASS_FXSetParameters
' to set the effect parameters. Obviously requires DX8.
' handle : Channel handle (HMUSIC/HSTREAM)
' atype   : Type of effect to setup (BASS_FX_xxx)
' RETURN : FX handle (NULL=error)
Declare Function BASS_ChannelSetFX Lib "bass.dll" (ByVal handle As Long, ByVal atype As Long) As Long

' Remove a DX8 effect from a channel.
' handle : Channel handle (HMUSIC/HSTREAM)
' fx     : Handle of FX to remove
Declare Function BASS_ChannelRemoveFX Lib "bass.dll" (ByVal handle As Long, ByVal fx As Long) As Long

' Set the parameters of a DX8 effect.
' handle : FX handle
' par    : Pointer to the parameter structure
Declare Function BASS_FXSetParameters Lib "bass.dll" (ByVal handle As Long, ByRef par As Any) As Long

' Retrieve the parameters of a DX8 effect.
' handle : FX handle
' par    : Pointer to the parameter structure
Declare Function BASS_FXGetParameters Lib "bass.dll" (ByVal handle As Long, ByRef par As Any) As Long


'*******************************************
' 32-bit wrappers for 64-bit BASS functions
'*******************************************
Function BASS_ChannelBytes2Seconds(ByVal handle As Long, ByVal pos As Long) As Single
BASS_ChannelBytes2Seconds = BASS_ChannelBytes2Seconds64(handle, pos, 0)
End Function

Function BASS_ChannelSetPosition(ByVal handle As Long, ByVal pos As Long) As Long
BASS_ChannelSetPosition = BASS_ChannelSetPosition64(handle, pos, 0)
End Function

Function BASS_ChannelSetSync(ByVal handle As Long, ByVal atype As Long, ByVal param As Long, ByVal proc As Long, ByVal user As Long) As Long
BASS_ChannelSetSync = BASS_ChannelSetSync64(handle, atype, param, 0, proc, user)
End Function


' Gets the text description of a device. This function can be used to enumerate the available devices.
' devnum: The device(0 = First)
' Return: device description
' NOTE: use BASS_GetDeviceCount to get the number of available devices
Function BASS_GetDeviceDescriptionString(ByVal devnum As Long) As String
Dim pstring As Long
Dim sstring As String
On Error Resume Next
pstring = BASS_GetDeviceDescription(devnum)
If pstring Then
    sstring = VBStrFromAnsiPtr(pstring)
End If
BASS_GetDeviceDescriptionString = sstring
End Function

' Gets the number of available devices.
' Since the first device is 0, the last device is (BASS_GetDeviceCount - 1)
Function BASS_GetDeviceCount() As Integer
Dim count As Integer
On Error Resume Next
Do While BASS_GetDeviceDescription(count)
    count = count + 1
Loop
BASS_GetDeviceCount = count
End Function

'Retrieves a music's name.
'handle :  Music handle
'Return:   Music's name
Public Function BASS_MusicGetNameString(ByVal handle As Long) As String
Dim pstring As Long
Dim sstring As String
On Error Resume Next
pstring = BASS_MusicGetName(handle)
If pstring Then
    sstring = VBStrFromAnsiPtr(pstring)
End If
BASS_MusicGetNameString = sstring
End Function

'Retrieves identification info from CD as a String
' id:           a BASS_CDID_xxx flag
' RETURN:       identification info
Public Function BASS_CDGetIDString(ByVal id As Long) As String
Dim pstring As Long
Dim sstring As String
On Error Resume Next
pstring = BASS_CDGetID(id)
If pstring Then
        sstring = VBStrFromAnsiPtr(pstring)
End If
BASS_CDGetIDString = sstring
End Function

Function STREAMPROC(ByVal handle As Long, ByVal buffer As Long, ByVal Length As Long, ByVal user As Long) As Long
    
    'CALLBACK FUNCTION !!!
    
    ' User stream callback function
    ' NOTE: A stream function should obviously be as quick
    ' as possible, other streams (and MOD musics) can't be mixed until it's finished.
    ' handle : The stream that needs writing
    ' buffer : Buffer to write the samples in
    ' length : Number of bytes to write
    ' user   : The 'user' parameter value given when calling BASS_StreamCreate
    ' RETURN : Number of bytes written. If less than "length" then the
    '          stream is assumed to be at the end, and is stopped.
    
End Function

Function STREAMFILEPROC(ByVal action As Long, ByVal param1 As Long, ByVal param2 As Long, ByVal user As Long) As Long
    
    'CALLBACK FUNCTION !!!
    
    ' User file stream callback function.
    ' action : The action to perform, one of BASS_FILE_xxx values.
    ' param1 : Depends on "action"
    ' param2 : Depends on "action"
    ' user   : The 'user' parameter value given when calling BASS_StreamCreate
    ' RETURN : Depends on "action"
    
End Function

Sub SYNCPROC(ByVal handle As Long, ByVal channel As Long, ByVal data As Long, ByVal user As Long)
    
    'CALLBACK FUNCTION !!!
    
    'Similarly in here, write what to do when sync function
    'is called, i.e screen flash etc.
    
    ' NOTE: a sync callback function should be very quick as other
    ' syncs cannot be processed until it has finished.
    ' handle : The sync that has occured
    ' channel: Channel that the sync occured in
    ' data   : Additional data associated with the sync's occurance
    ' user   : The 'user' parameter given when calling BASS_ChannelSetSync */
    
End Sub

Sub DSPPROC(ByVal handle As Long, ByVal channel As Long, ByVal buffer As Long, ByVal Length As Long, ByVal user As Long)

    'CALLBACK FUNCTION !!!

    ' VB doesn't support pointers, so you should copy the buffer into an array,
    ' process it, and then copy it back into the buffer.

    ' DSP callback function. NOTE: A DSP function should obviously be as quick as
    ' possible... other DSP functions, streams and MOD musics can not be processed
    ' until it's finished.
    ' handle : The DSP handle
    ' channel: Channel that the DSP is being applied to
    ' buffer : Buffer to apply the DSP to
    ' length : Number of bytes in the buffer
    ' user   : The 'user' parameter given when calling BASS_ChannelSetDSP
    
End Sub

Function RECORDPROC(ByVal buffer As Long, ByVal Length As Long, ByVal user As Long) As Long

    'CALLBACK FUNCTION !!!

    ' Recording callback function.
    ' buffer : Buffer containing the recorded samples
    ' length : Number of bytes
    ' user   : The 'user' parameter value given when calling BASS_RecordStart
    ' RETURN : BASSTRUE = continue recording, BASSFALSE = stop

End Function

Function BASS_SetEAXParametersVB(Preset) As Long
' This function is a workaround, because VB doesn't support multiple comma seperated
' paramaters for each Global Const, simply pass the EAX_PRESET_XXXX value to this function
' instead of BASS_SetEasParamaets as you would to in C++
Select Case Preset
    Case EAX_PRESET_GENERIC
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_GENERIC, 0.5, 1.493, 0.5)
    Case EAX_PRESET_PADDEDCELL
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_PADDEDCELL, 0.25, 0.1, 0)
    Case EAX_PRESET_ROOM
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_ROOM, 0.417, 0.4, 0.666)
    Case EAX_PRESET_BATHROOM
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_BATHROOM, 0.653, 1.499, 0.166)
    Case EAX_PRESET_LIVINGROOM
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_LIVINGROOM, 0.208, 0.478, 0)
    Case EAX_PRESET_STONEROOM
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_STONEROOM, 0.5, 2.309, 0.888)
    Case EAX_PRESET_AUDITORIUM
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_AUDITORIUM, 0.403, 4.279, 0.5)
    Case EAX_PRESET_CONCERTHALL
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_CONCERTHALL, 0.5, 3.961, 0.5)
    Case EAX_PRESET_CAVE
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_CAVE, 0.5, 2.886, 1.304)
    Case EAX_PRESET_ARENA
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_ARENA, 0.361, 7.284, 0.332)
    Case EAX_PRESET_HANGAR
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_HANGAR, 0.5, 10, 0.3)
    Case EAX_PRESET_CARPETEDHALLWAY
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_CARPETEDHALLWAY, 0.153, 0.259, 2)
    Case EAX_PRESET_HALLWAY
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_HALLWAY, 0.361, 1.493, 0)
    Case EAX_PRESET_STONECORRIDOR
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_STONECORRIDOR, 0.444, 2.697, 0.638)
    Case EAX_PRESET_ALLEY
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_ALLEY, 0.25, 1.752, 0.776)
    Case EAX_PRESET_FOREST
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_FOREST, 0.111, 3.145, 0.472)
    Case EAX_PRESET_CITY
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_CITY, 0.111, 2.767, 0.224)
    Case EAX_PRESET_MOUNTAINS
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_MOUNTAINS, 0.194, 7.841, 0.472)
    Case EAX_PRESET_QUARRY
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_QUARRY, 1, 1.499, 0.5)
    Case EAX_PRESET_PLAIN
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_PLAIN, 0.097, 2.767, 0.224)
    Case EAX_PRESET_PARKINGLOT
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_PARKINGLOT, 0.208, 1.652, 1.5)
    Case EAX_PRESET_SEWERPIPE
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_SEWERPIPE, 0.652, 2.886, 0.25)
    Case EAX_PRESET_UNDERWATER
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_UNDERWATER, 1, 1.499, 0)
    Case EAX_PRESET_DRUGGED
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_DRUGGED, 0.875, 8.392, 1.388)
    Case EAX_PRESET_DIZZY
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_DIZZY, 0.139, 17.234, 0.666)
    Case EAX_PRESET_PSYCHOTIC
        BASS_SetEAXParametersVB = BASS_SetEAXParameters(EAX_ENVIRONMENT_PSYCHOTIC, 0.486, 7.563, 0.806)
End Select
End Function

Function BASS_GetStringVersion() As String
    'This function will return the string version
    'of the BASS DLL. For example the provided function within the DLL
    '"BASS_GetVersion" will return 393216, whereas this function works
    'out the actual version string as you would need to see it.
    BASS_GetStringVersion = Trim(Str(GetLoWord(BASS_GetVersion))) & "." & Trim(Str(GetHiWord(BASS_GetVersion)))
End Function

Public Function GetHiWord(lparam As Long) As Long
    ' This is the HIWORD of the lParam:
    GetHiWord = lparam \ &H10000 And &HFFFF&
End Function
Public Function GetLoWord(lparam As Long) As Long
    ' This is the LOWORD of the lParam:
    GetLoWord = lparam And &HFFFF&
End Function

Public Function BASS_GetErrorDescription(ErrorCode As Long) As String
Select Case ErrorCode
    Case BASS_OK
        BASS_GetErrorDescription = "All is OK"
    Case BASS_ERROR_MEM
        BASS_GetErrorDescription = "Memory Error"
    Case BASS_ERROR_FILEOPEN
        BASS_GetErrorDescription = "Can't Open the File"
    Case BASS_ERROR_DRIVER
        BASS_GetErrorDescription = "Can't Find a Free Sound Driver"
    Case BASS_ERROR_BUFLOST
        BASS_GetErrorDescription = "The Sample Buffer Was Lost - Please Report This!"
    Case BASS_ERROR_HANDLE
        BASS_GetErrorDescription = "Invalid Handle"
    Case BASS_ERROR_FORMAT
        BASS_GetErrorDescription = "Unsupported Format"
    Case BASS_ERROR_POSITION
        BASS_GetErrorDescription = "Invalid Playback Position"
    Case BASS_ERROR_INIT
        BASS_GetErrorDescription = "BASS_Init Has Not Been Successfully Called"
    Case BASS_ERROR_START
        BASS_GetErrorDescription = "BASS_Start Has Not Been Successfully Called"
    Case BASS_ERROR_INITCD
        BASS_GetErrorDescription = "Can't Initialize CD"
    Case BASS_ERROR_CDINIT
        BASS_GetErrorDescription = "BASS_CDInit Has Not Been Successfully Called"
    Case BASS_ERROR_NOCD
        BASS_GetErrorDescription = "No CD in drive"
    Case BASS_ERROR_CDTRACK
        BASS_GetErrorDescription = "Can't Play the Selected CD Track"
    Case BASS_ERROR_ALREADY
        BASS_GetErrorDescription = "Already Initialized"
    Case BASS_ERROR_CDVOL
        BASS_GetErrorDescription = "CD Has No Volume Control"
    Case BASS_ERROR_NOPAUSE
        BASS_GetErrorDescription = "Not Paused"
    Case BASS_ERROR_NOTAUDIO
        BASS_GetErrorDescription = "Not An Audio Track"
    Case BASS_ERROR_NOCHAN
        BASS_GetErrorDescription = "Can't Get a Free Channel"
    Case BASS_ERROR_ILLTYPE
        BASS_GetErrorDescription = "An Illegal Type Was Specified"
    Case BASS_ERROR_ILLPARAM
        BASS_GetErrorDescription = "An Illegal Parameter Was Specified"
    Case BASS_ERROR_NO3D
        BASS_GetErrorDescription = "No 3D Support"
    Case BASS_ERROR_NOEAX
        BASS_GetErrorDescription = "No EAX Support"
    Case BASS_ERROR_DEVICE
        BASS_GetErrorDescription = "Illegal Device Number"
    Case BASS_ERROR_NOPLAY
        BASS_GetErrorDescription = "Not Playing"
    Case BASS_ERROR_FREQ
        BASS_GetErrorDescription = "Illegal Sample Rate"
    Case BASS_ERROR_NOTFILE
        BASS_GetErrorDescription = "The Stream is Not a File Stream (WAV/MP3)"
    Case BASS_ERROR_NOHW
        BASS_GetErrorDescription = "No Hardware Voices Available"
    Case BASS_ERROR_EMPTY
        BASS_GetErrorDescription = "The MOD music has no sequence data"
    Case BASS_ERROR_NONET
        BASS_GetErrorDescription = "No Internet connection could be opened"
    Case BASS_ERROR_CREATE
        BASS_GetErrorDescription = "Couldn't create the file"
    Case BASS_ERROR_NOFX
        BASS_GetErrorDescription = "Effects are not enabled"
    Case BASS_ERROR_PLAYING
        BASS_GetErrorDescription = "The channel is playing"
    Case BASS_ERROR_NOTAVAIL
        BASS_GetErrorDescription = "The requested data is not available"
    Case BASS_ERROR_DECODE
        BASS_GetErrorDescription = "The channel is a 'decoding channel'"
    Case BASS_ERROR_UNKNOWN
        BASS_GetErrorDescription = "Some Other Mystery Error"
End Select
End Function

Function MakeLong(LoWord As Long, HiWord As Long) As Long
'Replacement for the c++ Function MAKELONG
'You need this to pass values to certain function calls.
'i.e BASS_ChannelSetPosition needs to pass a value
'using make long, i.e BASS_ChannelSetPosition Handle,MakeLong(Order,Row)
MakeLong = LoWord Or LShift(HiWord, 16)
End Function

Public Function LShift(ByVal lValue As Long, ByVal lNumberOfBitsToShift As Long) As Long
    Const ksCallname As String = "LShift"
    On Error GoTo Procedure_Error
    LShift = lValue * (2 ^ lNumberOfBitsToShift)
    
Procedure_Exit:
    Exit Function
    
Procedure_Error:
    Err.Raise Err.Number, ksCallname, Err.Description, Err.HelpFile, Err.HelpContext
    Resume Procedure_Exit
End Function

Public Function RShift(ByVal lValue As Long, ByVal lNumberOfBitsToShift As Long) As Long

    Const ksCallname As String = "RShift"
    On Error GoTo Procedure_Error
    RShift = lValue \ (2 ^ lNumberOfBitsToShift)
    
Procedure_Exit:
    Exit Function
    
Procedure_Error:
    Err.Raise Err.Number, ksCallname, Err.Description, Err.HelpFile, Err.HelpContext
    Resume Procedure_Exit
End Function

Public Function VBStrFromAnsiPtr(ByVal lpStr As Long) As String
Dim bStr() As Byte
Dim cChars As Long
On Error Resume Next
' Get the number of characters in the buffer
cChars = lstrlen(lpStr)

' Resize the byte array
ReDim bStr(0 To cChars - 1) As Byte

' Grab the ANSI buffer
Call CopyMemory(bStr(0), ByVal lpStr, cChars)

' Now convert to a VB Unicode string
VBStrFromAnsiPtr = StrConv(bStr, vbUnicode)
End Function
