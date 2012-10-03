;  BASS 1.8 Multimedia Library
;  ----------------------------
;  (c) 1999-2003 Ian Luck.
;  Please report bugs/suggestions/etc... to bass@un4seen.com

;  See the BASS.CHM file for more complete documentation

;  Differences:
;   BASS_SetEAXParameters [call] -> BASS_SetEAXParametersASM [macro]
;   BASS_INFO.size               -> BASS_INFO.ssize
;   BASS_INFO.eax                -> BASS_INFO.haseax
;   BASS_RECORDINFO.size         -> BASS_RECORDINFO.ssize
;   BASS_SAMPLE.length           -> BASS_SAMPLE.slength

float TYPEDEF REAL4

; Error codes returned by BASS_GetErrorCode()
BASS_OK                 equ 0   ; all is OK
BASS_ERROR_MEM          equ 1   ; memory error
BASS_ERROR_FILEOPEN     equ 2   ; can't open the file
BASS_ERROR_DRIVER       equ 3   ; can't find a free/valid driver
BASS_ERROR_BUFLOST      equ 4   ; the sample buffer was lost - please report this!
BASS_ERROR_HANDLE       equ 5   ; invalid handle
BASS_ERROR_FORMAT       equ 6   ; unsupported sample format
BASS_ERROR_POSITION     equ 7   ; invalid playback position
BASS_ERROR_INIT         equ 8   ; BASS_Init has not been successfully called
BASS_ERROR_START        equ 9   ; BASS_Start has not been successfully called
BASS_ERROR_INITCD       equ 10  ; can't initialize CD
BASS_ERROR_CDINIT       equ 11  ; BASS_CDInit has not been successfully called
BASS_ERROR_NOCD         equ 12  ; no CD in drive
BASS_ERROR_CDTRACK      equ 13  ; can't play the selected CD track
BASS_ERROR_ALREADY      equ 14  ; already initialized
BASS_ERROR_CDVOL        equ 15  ; CD has no volume control
BASS_ERROR_NOPAUSE      equ 16  ; not paused
BASS_ERROR_NOTAUDIO     equ 17  ; not an audio track
BASS_ERROR_NOCHAN       equ 18  ; can't get a free channel
BASS_ERROR_ILLTYPE      equ 19  ; an illegal type was specified
BASS_ERROR_ILLPARAM     equ 20  ; an illegal parameter was specified
BASS_ERROR_NO3D         equ 21  ; no 3D support
BASS_ERROR_NOEAX        equ 22  ; no EAX support
BASS_ERROR_DEVICE       equ 23  ; illegal device number
BASS_ERROR_NOPLAY       equ 24  ; not playing
BASS_ERROR_FREQ         equ 25  ; illegal sample rate
BASS_ERROR_NOTFILE      equ 27  ; the stream is not a file stream
BASS_ERROR_NOHW         equ 29  ; no hardware voices available
BASS_ERROR_EMPTY        equ 31  ; the MOD music has no sequence data
BASS_ERROR_NONET        equ 32  ; no internet connection could be opened
BASS_ERROR_CREATE       equ 33  ; couldn't create the file
BASS_ERROR_NOFX         equ 34  ; effects are not enabled
BASS_ERROR_PLAYING      equ 35  ; the channel is playing
BASS_ERROR_NOTAVAIL     equ 37  ; requested data is not available
BASS_ERROR_DECODE       equ 38  ; the channel is a "decoding channel"
BASS_ERROR_DX           equ 39  ; a sufficient DirectX version is not installed
BASS_ERROR_TIMEOUT      equ 40  ; connection timedout
BASS_ERROR_FILEFORM     equ 41  ; unsupported file format
BASS_ERROR_SPEAKER      equ 42  ; unavailable speaker
BASS_ERROR_UNKNOWN      equ -1  ; some other mystery error

; Initialization flags
BASS_DEVICE_8BITS       equ 1   ; use 8 bit resolution, else 16 bit
BASS_DEVICE_MONO        equ 2   ; use mono, else stereo
BASS_DEVICE_3D          equ 4   ; enable 3D functionality
BASS_DEVICE_LEAVEVOL    equ 32  ; leave the volume as it is
BASS_DEVICE_NOTHREAD    equ 128 ; update buffers manually (using BASS_Update)
BASS_DEVICE_LATENCY     equ 256 ; calculate device latency (BASS_INFO struct)
BASS_DEVICE_VOL1000     equ 512 ; 0-1000 volume range (else 0-100)
BASS_DEVICE_FLOATDSP    equ 1024; all DSP uses 32-bit floating-point data
BASS_DEVICE_SPEAKERS    equ 2048; force enabling of speaker assignment

; DirectSound interfaces (for use with BASS_GetDSoundObject)
BASS_OBJECT_DS          equ 1   ; IDirectSound
BASS_OBJECT_DS3DL       equ 2   ; IDirectSound3DListener

BASS_INFO struct
        ssize   DWORD   ?       ; size of this struct (set this before calling the function)
        flags   DWORD   ?       ; device capabilities (DSCAPS_xxx flags)
        hwsize  DWORD   ?       ; size of total device hardware memory
        hwfree  DWORD   ?       ; size of free device hardware memory
        freesam DWORD   ?       ; number of free sample slots in the hardware
        free3d  DWORD   ?       ; number of free 3D sample slots in the hardware
        minrate DWORD   ?       ; min sample rate supported by the hardware
        maxrate DWORD   ?       ; max sample rate supported by the hardware
        haseax  DWORD   ?       ; device supports EAX? (always FALSE if BASS_DEVICE_3D was not used)
		minbuf  DWORD   ?       ; recommended minimum buffer length in ms (requires BASS_DEVICE_LATENCY)
        dsver   DWORD   ?       ; DirectSound version
        latency DWORD   ?       ; delay (in ms) before start of playback (requires BASS_DEVICE_LATENCY)
        initflags DWORD ?       ; "flags" parameter of BASS_Init call
        speakers DWORD  ?       ; number of speakers available
BASS_INFO ends

; BASS_INFO flags (from DSOUND.H)
; supports all sample rates between min/maxrate
DSCAPS_CONTINUOUSRATE   equ     0x00000010
; device does NOT have hardware DirectSound support
DSCAPS_EMULDRIVER       equ     0x00000020
; device driver has been certified by Microsoft
DSCAPS_CERTIFIED        equ     0x00000040
; The following flags tell what type of samples are supported by HARDWARE
; mixing, all these formats are supported by SOFTWARE mixing
DSCAPS_SECONDARYMONO    equ     0x00000100      ; mono
DSCAPS_SECONDARYSTEREO  equ     0x00000200      ; stereo
DSCAPS_SECONDARY8BIT    equ     0x00000400      ; 8 bit
DSCAPS_SECONDARY16BIT   equ     0x00000800      ; 16 bit

BASS_RECORDINFO struct
        ssize    DWORD   ?      ; size of this struct (set this before calling the function)
        flags    DWORD   ?      ; device capabilities (DSCCAPS_xxx flags)
        formats  DWORD   ?      ; supported standard formats (WAVE_FORMAT_xxx flags)
        inputs   DWORD   ?      ; number of inputs
        singelin DWORD   ?      ; TRUE = only 1 input can be set at a time
BASS_RECORDINFO ends

; BASS_RECORDINFO flags (from DSOUND.H)
; device does NOT have hardware DirectSound recording support
DSCCAPS_EMULDRIVER      equ     DSCAPS_EMULDRIVER
; device driver has been certified by Microsoft
; defines for formats field of BASS_RECORDINFO (from MMSYSTEM.H)
DSCCAPS_CERTIFIED       equ     DSCAPS_CERTIFIED

; Ramping doesn't take a lot of extra processing and improves
; the sound quality by removing "clicks". Sensitive ramping will
; leave sharp attacked samples, unlike normal ramping.
BASS_MUSIC_RAMP         equ     1       ; normal ramping
BASS_MUSIC_RAMPS        equ     2       ; sensitive ramping
BASS_MUSIC_LOOP         equ     4       ; loop music
BASS_MUSIC_FT2MOD       equ    16       ; play .MOD as FastTracker 2 does
BASS_MUSIC_PT1MOD       equ    32       ; play .MOD as ProTracker 1 does
BASS_MUSIC_MONO         equ    64       ; force mono mixing (less CPU usage)
BASS_MUSIC_3D           equ   128       ; enable 3D functionality
BASS_MUSIC_POSRESET     equ   256       ; stop all notes when moving position
BASS_MUSIC_SURROUND     equ   512       ; surround sound
BASS_MUSIC_SURROUND2    equ  1024       ; surround sound (mode 2)
BASS_MUSIC_STOPBACK     equ  2048       ; stop the music on a backwards jump effect
BASS_MUSIC_FX           equ  4096       ; enable old implementation of DX8 effects
BASS_MUSIC_CALCLEN      equ  8192       ; calculate playback length
BASS_MUSIC_NONINTER     equ 16384       ; non-interpolated mixing
BASS_MUSIC_FLOAT        equ  0x10000    ; 32-bit floating-point
BASS_MUSIC_DECODE       equ  0x200000   ; don't play the music, only decode (BASS_ChannelGetData)
BASS_MUSIC_NOSAMPLE     equ  0x400000   ; don't load the samples

; Sample info structure & flags
BASS_SAMPLE struct
        freq     DWORD   ?              ; default playback rate
        volume   DWORD   ?              ; default volume (0-100)
        pan      BYTE    ?              ; default pan (-100=left, 0=middle, 100=right)
        flags    DWORD   ?              ; BASS_SAMPLE_xxx flags
        slength  DWORD   ?              ; length (in samples, not bytes)
        max      DWORD   ?              ; maximum simultaneous playbacks
        mode3d   DWORD   ?              ; BASS_3DMODE_xxx mode
        mindist  float   ?              ; minimum distance
        maxdist  float   ?              ; maximum distance
        iangle   DWORD   ?              ; angle of inside projection cone
        oangle   DWORD   ?              ; angle of outside projection cone
        outvol   DWORD   ?              ; delta-volume outside the projection cone
        vam      DWORD   ?              ; voice allocation/management flags (BASS_VAM_xxx)
        priority DWORD   ?              ; priority (0=lowest, 0xffffffff=highest)
BASS_SAMPLE ends

BASS_SAMPLE_8BITS       equ    1        ; 8 bit
BASS_SAMPLE_FLOAT       equ    256      ; 32-bit floating-point
BASS_SAMPLE_MONO        equ    2        ; mono, else stereo
BASS_SAMPLE_LOOP        equ    4        ; looped
BASS_SAMPLE_3D          equ    8        ; 3D functionality enabled
BASS_SAMPLE_SOFTWARE    equ    16       ; it's NOT using hardware mixing
BASS_SAMPLE_MUTEMAX     equ    32       ; muted at max distance (3D only)
BASS_SAMPLE_VAM         equ    64       ; uses the DX7 voice allocation & management
BASS_SAMPLE_FX          equ    128      ; old implementation of DX8 effects are enabled
BASS_SAMPLE_OVER_VOL    equ    0x10000  ; override lowest volume
BASS_SAMPLE_OVER_POS    equ    0x20000  ; override longest playing
BASS_SAMPLE_OVER_DIST   equ    0x30000  ; override furthest from listener (3D only)
BASS_MP3_SETPOS         equ    0x20000  ; enable pin-point seeking on the MP3/MP2/MP1
BASS_STREAM_AUTOFREE    equ    0x40000  ; automatically free the stream when it stop/ends
BASS_STREAM_RESTRATE    equ    0x80000  ; restrict the download rate of internet file streams
BASS_STREAM_BLOCK       equ    0x100000 ; download/play internet file stream (MPx/OGG) in small blocks
BASS_STREAM_DECODE      equ    0x200000 ; don't play the stream, only decode (BASS_ChannelGetData)
BASS_STREAM_META        equ    0x400000 ; request metadata from a Shoutcast stream
BASS_STREAM_FILEPROC    equ    0x800000 ; use a STREAMFILEPROC callback

; Speaker assignment flags
BASS_SPEAKER_FRONT      equ 0x1000000	// front speakers
BASS_SPEAKER_REAR       equ 0x2000000	// rear/side speakers
BASS_SPEAKER_CENLFE     equ 0x3000000	// center & LFE speakers (5.1)
BASS_SPEAKER_REAR2      equ 0x4000000	// rear center speakers (7.1)
BASS_SPEAKER_LEFT       equ 0x10000000	// modifier: left
BASS_SPEAKER_RIGHT      equ 0x20000000	// modifier: right
BASS_SPEAKER_FRONTLEFT  equ BASS_SPEAKER_FRONT or BASS_SPEAKER_LEFT
BASS_SPEAKER_FRONTRIGHT equ BASS_SPEAKER_FRONT or BASS_SPEAKER_RIGHT
BASS_SPEAKER_REARLEFT   equ BASS_SPEAKER_REAR or BASS_SPEAKER_LEFT
BASS_SPEAKER_REARRIGHT  equ BASS_SPEAKER_REAR or BASS_SPEAKER_RIGHT
BASS_SPEAKER_CENTER     equ BASS_SPEAKER_CENLFE or BASS_SPEAKER_LEFT
BASS_SPEAKER_LFE        equ BASS_SPEAKER_CENLFE or BASS_SPEAKER_RIGHT
BASS_SPEAKER_REAR2LEFT  equ BASS_SPEAKER_REAR2 or BASS_SPEAKER_LEFT
BASS_SPEAKER_REAR2RIGHT equ BASS_SPEAKER_REAR2 or BASS_SPEAKER_RIGHT

; BASS_StreamGetFilePosition modes
BASS_FILEPOS_DECODE     equ    0
BASS_FILEPOS_DOWNLOAD   equ    1
BASS_FILEPOS_END        equ    2

; STREAMFILEPROC actions
BASS_FILE_CLOSE         equ    0
BASS_FILE_READ          equ    1
BASS_FILE_QUERY         equ    2
BASS_FILE_LEN           equ    3

BASS_VAM_HARDWARE       equ    1
BASS_VAM_SOFTWARE       equ    2
BASS_VAM_TERM_TIME      equ    4
BASS_VAM_TERM_DIST      equ    8
BASS_VAM_TERM_PRIO      equ    16
BASS_3DMODE_NORMAL      equ    0
BASS_3DMODE_RELATIVE    equ    1
BASS_3DMODE_OFF         equ    2

BASS_3DVECTOR struct
        x float ?       ; +=right, -=left
        y float ?       ; +=up, -=down
        z float ?       ; +=front, -=behind
BASS_3DVECTOR ends

EAX_PRESET_GENERIC         equ 0
EAX_PRESET_PADDEDCELL      equ 1
EAX_PRESET_ROOM            equ 2
EAX_PRESET_BATHROOM        equ 3
EAX_PRESET_LIVINGROOM      equ 4
EAX_PRESET_STONEROOM       equ 5
EAX_PRESET_AUDITORIUM      equ 6
EAX_PRESET_CONCERTHALL     equ 7
EAX_PRESET_CAVE            equ 8
EAX_PRESET_ARENA           equ 9
EAX_PRESET_HANGAR          equ 10
EAX_PRESET_CARPETEDHALLWAY equ 11
EAX_PRESET_HALLWAY         equ 12
EAX_PRESET_STONECORRIDOR   equ 13
EAX_PRESET_ALLEY           equ 14
EAX_PRESET_FOREST          equ 15
EAX_PRESET_CITY            equ 16
EAX_PRESET_MOUNTAINS       equ 17
EAX_PRESET_QUARRY          equ 18
EAX_PRESET_PLAIN           equ 19
EAX_PRESET_PARKINGLOT      equ 20
EAX_PRESET_SEWERPIPE       equ 21
EAX_PRESET_UNDERWATER      equ 22
EAX_PRESET_DRUGGED         equ 23
EAX_PRESET_DIZZY           equ 24
EAX_PRESET_PSYCHOTIC       equ 25
EAX_PRESET_COUNT           equ 26          ; total number of environments
EAX_PRESET_DISABLE         equ 999

BASS_SetEAXParametersASM MACRO EAX_PRESET :REQ
   IF EAX_PRESET EQ EAX_PRESET_GENERIC  
    invoke BASS_SetEAXParameters, EAX_PRESET_GENERIC,3F000000h,3FBF1AA0h,3F000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_PADDEDCELL
    invoke BASS_SetEAXParameters, EAX_PRESET,3E800000h,3DCCCCCDh,00000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_ROOM     
    invoke BASS_SetEAXParameters, EAX_PRESET_ROOM,3ED58106h,3ECCCCCDh,3F2A7EFAh
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_BATHROOM
    invoke BASS_SetEAXParameters, EAX_PRESET_BATHROOM,3F272B02h,3FBFDF3Bh,3E29FBE7h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_LIVINGROOM
    invoke BASS_SetEAXParameters, EAX_PRESET_LIVINGROOM,3E54FDF4h,3EF4BC6Ah,00000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_STONEROOM
    invoke BASS_SetEAXParameters, EAX_PRESET_STONEROOM,3F000000h,4013C6A8h,3F6353F8h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_AUDITORIUM
    invoke BASS_SetEAXParameters, EAX_PRESET_AUDITORIUM,3ECE5604h,4088ED91h,3F000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_CONCERTHALL
    invoke BASS_SetEAXParameters, EAX_PRESET_CONCERTHALL,3F000000h,407D8106h,3F000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_CAVE
    invoke BASS_SetEAXParameters, EAX_PRESET_CAVE,3F000000h,4038B439h,3FA6E979h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_ARENA
    invoke BASS_SetEAXParameters, EAX_PRESET_ARENA,3EB8D4FEh,40E91687h,3EA9FBE7h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_HANGAR
    invoke BASS_SetEAXParameters, EAX_PRESET_HANGAR,3F000000h,41200000h,3E99999Ah
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_CARPETEDHALLWAY
    invoke BASS_SetEAXParameters, EAX_PRESET_CARPETEDHALLWAY,3E1CAC08h,3E849BA6h,40000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_HALLWAY
    invoke BASS_SetEAXParameters, EAX_PRESET_HALLWAY,3EB8D4FEh,3FBF1AA0h,00000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_STONECORRIDOR
    invoke BASS_SetEAXParameters, EAX_PRESET_STONECORRIDOR,3EE353F8h,402C9BA6h,3F2353F8h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_ALLEY
    invoke BASS_SetEAXParameters, EAX_PRESET_ALLEY,3E800000h,3FE04189h,3F46A7F0h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_FOREST
    invoke BASS_SetEAXParameters, EAX_PRESET_FOREST,3DE353F8h,404947AEh,3EF1A9FCh
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_CITY
    invoke BASS_SetEAXParameters, EAX_PRESET_CITY,3DE353F8h,40311687h,3E656042h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_MOUNTAINS
    invoke BASS_SetEAXParameters, EAX_PRESET_MOUNTAINS,3E46A7F0h,40FAE979h,3EF1A9FCh
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_QUARRY
    invoke BASS_SetEAXParameters, EAX_PRESET_QUARRY,3F800000h,3FBFDF3Bh,3F000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_PLAIN
    invoke BASS_SetEAXParameters, EAX_PRESET_PLAIN,3DC6A7F0h,40311687h,3E656042h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_PARKINGLOT
    invoke BASS_SetEAXParameters, EAX_PRESET_PARKINGLOT,3E54FDF4h,3FD374BCh,3FC00000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_SEWERPIPE
    invoke BASS_SetEAXParameters, EAX_PRESET_SEWERPIPE,3F26E979h,4038B439h,3E800000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_UNDERWATER
    invoke BASS_SetEAXParameters, EAX_PRESET_UNDERWATER,3F800000h,3FBFDF3Bh,00000000h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_DRUGGED
    invoke BASS_SetEAXParameters, EAX_PRESET_DRUGGED,3F600000h,410645A2h,3FB1A9FCh
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_DIZZY
    invoke BASS_SetEAXParameters, EAX_PRESET_DIZZY,3E0E5604h,4189DF3Bh,3F2A7EFAh
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_PSYCHOTIC
    invoke BASS_SetEAXParameters, EAX_PRESET_PSYCHOTIC,3EF8D4FEh,40F20419h,3F4E5604h
   ENDIF
   IF EAX_PRESET EQ EAX_PRESET_DISABLE
    invoke BASS_SetEAXParameters, -1,0,-1,-1
   ENDIF   
ENDM

BASS_3DALG_DEFAULT      equ 0
BASS_3DALG_OFF          equ 1
BASS_3DALG_FULL         equ 2
BASS_3DALG_LIGHT        equ 3

BASS_SYNC_MUSICPOS      equ 0
BASS_SYNC_POS           equ 0
BASS_SYNC_MUSICINST     equ 1
BASS_SYNC_END           equ 2
BASS_SYNC_MUSICFX       equ 3
BASS_SYNC_META          equ 4
BASS_SYNC_SLIDE         equ 5
BASS_SYNC_MESSAGE       equ 0x20000000   ; FLAG: post a Windows message (instead of callback)
BASS_SYNC_MIXTIME       equ 0x40000000   ; FLAG: sync at mixtime, else at playtime
BASS_SYNC_ONETIME       equ 0x80000000   ; FLAG: sync only once, else continuously

BASS_DATA_AVAILABLE     equ 0            ; query how much data is buffered
BASS_DATA_FFT512        equ 0x80000000   ; 512 sample FFT
BASS_DATA_FFT1024       equ 0x80000001   ; 1024 FFT
BASS_DATA_FFT2048       equ 0x80000002   ; 2048 FFT
BASS_DATA_FFT512S       equ 0x80000010   ; stereo 512 sample FFT
BASS_DATA_FFT1024S      equ 0x80000011   ; stereo 1024 FFT
BASS_DATA_FFT2048S      equ 0x80000012   ; stereo 2048 FFT
BASS_DATA_FFT_NOWINDOW  equ 0x20         ; FFT flag: no Hanning window

BASS_TAG_ID3            equ 0            ; ID3v1 tags : 128 byte block
BASS_TAG_ID3V2          equ 1            ; ID3v2 tags : variable length block
BASS_TAG_OGG            equ 2            ; OGG comments : array of null-terminated strings
BASS_TAG_HTTP           equ 3            ; HTTP headers : array of null-terminated strings
BASS_TAG_ICY            equ 4            ; ICY headers : array of null-terminated strings
BASS_TAG_META           equ 5            ; ICY metadata : null-terminated string

BASS_FX_CHORUS          equ 0            ; GUID_DSFX_STANDARD_CHORUS
BASS_FX_COMPRESSOR      equ 1            ; GUID_DSFX_STANDARD_COMPRESSOR
BASS_FX_DISTORTION      equ 2            ; GUID_DSFX_STANDARD_DISTORTION
BASS_FX_ECHO            equ 3            ; GUID_DSFX_STANDARD_ECHO
BASS_FX_FLANGER         equ 4            ; GUID_DSFX_STANDARD_FLANGER
BASS_FX_GARGLE          equ 5            ; GUID_DSFX_STANDARD_GARGLE
BASS_FX_I3DL2REVERB     equ 6            ; GUID_DSFX_STANDARD_I3DL2REVERB
BASS_FX_PARAMEQ         equ 7            ; GUID_DSFX_STANDARD_PARAMEQ
BASS_FX_REVERB          equ 8            ; GUID_DSFX_WAVES_REVERB

BASS_FXCHORUS struct
  fWetDryMix float ?
  fDepth     float ?
  fFeedback  float ?
  fFrequency float ?
  lWaveform  DWORD ?     ; 0=triangle, 1=sine
  fDelay     float ?
  lPhase     DWORD ?     ; BASS_FX_PHASE_xxx
BASS_FXCHORUS ends       ; DSFXChorus

BASS_FXCOMPRESSOR struct
  fGain      float ?
  fAttack    float ?
  fRelease   float ?
  fThreshold float ?
  fRatio     float ?
  fPredelay  float ?
BASS_FXCOMPRESSOR ends    ; DSFXCompressor

BASS_FXDISTORTION struct
  fGain                   float ?
  fEdge                   float ?
  fPostEQCenterFrequency  float ?
  fPostEQBandwidth        float ?
  fPreLowpassCutoff       float ?
BASS_FXDISTORTION ends    ; DSFXDistortion

BASS_FXECHO struct
  fWetDryMix  float ?
  fFeedback   float ?
  fLeftDelay  float ?
  fRightDelay float ?
  lPanDelay   BYTE  ?
BASS_FXECHO ends          ; DSFXEcho

BASS_FXFLANGER struct
  fWetDryMix  float ?
  fDepth      float ? 
  fFeedback   float ?
  fFrequency  float ?
  lWaveform   DWORD ?     ; 0=triangle, 1=sine
  fDelay      float ?
  lPhase      DWORD ?     ; BASS_FX_PHASE_xxx
BASS_FXFLANGER ends       ; DSFXFlanger

BASS_FXGARGLE struct
 dwRateHz     DWORD ?     ; Rate of modulation in hz
 dwWaveShape  DWORD ?     ; 0=triangle, 1=square
BASS_FXGARGLE ends        ; DSFXGargle

BASS_FXPARAMEQ struct
  fCenter    float ?
  fBandwidth float ?
  fGain      float ?
BASS_FXPARAMEQ ends       ; DSFXParamEq

BASS_FXREVERB struct 
  fInGain          float ?  
  fReverbMix       float ?  
  fReverbTime      float ?    
  fHighFreqRTRatio float ?
BASS_FXREVERB ends                ; DSFXWavesReverb

BASS_FXI3DL2REVERB struct 
  lRoom                  BYTE  ?         ; [-10000, 0] default: -1000 mB
  lRoomHF                BYTE  ?         ; [-10000, 0] default: 0 mB
  flRoomRolloffFactor    float ?         ; [0.0, 10.0] default: 0.0
  flDecayTime            float ?         ; [0.1, 20.0] default: 1.49s
  flDecayHFRatio         float ?         ; [0.1, 2.0] default: 0.83
  lReflections           BYTE  ?         ; [-10000, 1000] default: -2602 mB
  flReflectionsDelay     float ?         ; [0.0, 0.3] default: 0.007 s
  lReverb                BYTE  ?         ; [-10000, 2000] default: 200 mB
  flReverbDelay          float ?         ; [0.0, 0.1] default: 0.011 s
  flDiffusion            float ?         ; [0.0, 100.0] default: 100.0 %
  flDensity              float ?         ; [0.0, 100.0] default: 100.0 %
  flHFReference          float ?         ; [20.0, 20000.0] default: 5000.0 Hz
BASS_FXI3DL2REVERB ends   ; DSFXI3DL2Reverb

BASS_FX_PHASE_NEG_180   equ 0
BASS_FX_PHASE_NEG_90    equ 1
BASS_FX_PHASE_ZERO      equ 2
BASS_FX_PHASE_90        equ 3
BASS_FX_PHASE_180       equ 4

CDCHANNEL               equ 0       ; CD channel, for use with BASS_Channel functions
RECORDCHAN              equ 1       ; Recording channel, for use with BASS_Channel functions

; BASS_CDGetID flags
BASS_CDID_IDENTITY      equ 0
BASS_CDID_UPC           equ 1
BASS_CDID_CDDB          equ 2
BASS_CDID_CDDB2         equ 3

; BASS_ChannelIsActive return values
BASS_ACTIVE_STOPPED     equ 0
BASS_ACTIVE_PLAYING     equ 1
BASS_ACTIVE_STALLED     equ 2
BASS_ACTIVE_PAUSED      equ 3

; BASS_ChannelIsSliding return flags
BASS_SLIDE_FREQ         equ 1
BASS_SLIDE_VOL          equ 2
BASS_SLIDE_PAN          equ 4

; BASS_RecordSetInput flags
BASS_INPUT_OFF          equ 0x10000
BASS_INPUT_ON           equ 0x20000
BASS_INPUT_LEVEL        equ 0x40000

; BASS_SetNetConfig flags
BASS_NET_TIMEOUT        equ 0
BASS_NET_BUFFER         equ 1


; prototypes
; check BASS.CHM for informations

BASS_Apply3D                            PROTO
BASS_CDDoor                             PROTO :DWORD
BASS_CDFree                             PROTO
BASS_CDGetID                            PROTO :DWORD
BASS_CDGetTrackLength                   PROTO :DWORD
BASS_CDGetTracks                        PROTO
BASS_CDInDrive                          PROTO
BASS_CDInit                             PROTO :DWORD,:DWORD
BASS_CDPlay                             PROTO :DWORD,:DWORD,:DWORD
BASS_ChannelBytes2Seconds               PROTO :DWORD,:QWORD
BASS_ChannelGet3DAttributes             PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_ChannelGet3DPosition               PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_ChannelGetAttributes               PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_ChannelGetData                     PROTO :DWORD,:DWORD,:DWORD
BASS_ChannelGetEAXMix                   PROTO :DWORD,:DWORD
BASS_ChannelGetFlags                    PROTO :DWORD
BASS_ChannelGetLevel                    PROTO :DWORD
BASS_ChannelGetPosition                 PROTO :DWORD
BASS_ChannelIsActive                    PROTO :DWORD
BASS_ChannelIsSliding                   PROTO :DWORD
BASS_ChannelPause                       PROTO :DWORD
BASS_ChannelRemoveDSP                   PROTO :DWORD,:DWORD
BASS_ChannelRemoveFX                    PROTO :DWORD,:DWORD
BASS_ChannelRemoveLink                  PROTO :DWORD,:DWORD
BASS_ChannelRemoveSync                  PROTO :DWORD,:DWORD
BASS_ChannelResume                      PROTO :DWORD
BASS_ChannelSeconds2Bytes               PROTO :DWORD,:DWORD
BASS_ChannelSet3DAttributes             PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_ChannelSet3DPosition               PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_ChannelSetAttributes               PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_ChannelSetDSP                      PROTO :DWORD,:DWORD,:DWORD
BASS_ChannelSetEAXMix                   PROTO :DWORD,:DWORD
BASS_ChannelSetFX                       PROTO :DWORD,:DWORD
BASS_ChannelSetLink                     PROTO :DWORD,:DWORD
BASS_ChannelSetPosition                 PROTO :DWORD,:QWORD
BASS_ChannelSetSync                     PROTO :DWORD,:DWORD,:QWORD,:DWORD,:DWORD
BASS_ChannelSlideAttributes             PROTO :DWORD,:DWORD,:QWORD,:DWORD,:DWORD
BASS_ChannelStop                        PROTO :DWORD
BASS_ErrorGetCode                       PROTO
BASS_FXGetParameters                    PROTO :DWORD,:DWORD
BASS_FXSetParameters                    PROTO :DWORD,:DWORD
BASS_Free                               PROTO
BASS_Get3DFactors                       PROTO :DWORD,:DWORD,:DWORD
BASS_Get3DPosition                      PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_GetCPU                             PROTO
BASS_GetDSoundObject                    PROTO :DWORD
BASS_GetDeviceDescription               PROTO :DWORD
BASS_GetEAXParameters                   PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_GetGlobalVolumes                   PROTO :DWORD,:DWORD,:DWORD
BASS_GetInfo                            PROTO :DWORD
BASS_GetVersion                         PROTO
BASS_GetVolume                          PROTO
BASS_Init                               PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_MusicFree                          PROTO :DWORD
BASS_MusicGetChannelVol                 PROTO :DWORD,:DWORD
BASS_MusicGetLength                     PROTO :DWORD,:DWORD
BASS_MusicGetName                       PROTO :DWORD
BASS_MusicLoad                          PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_MusicPlay                          PROTO :DWORD
BASS_MusicPlayEx                        PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_MusicPreBuf                        PROTO :DWORD
BASS_MusicSetAmplify                    PROTO :DWORD,:DWORD
BASS_MusicSetChannelVol                 PROTO :DWORD,:DWORD,:DWORD
BASS_MusicSetPanSep                     PROTO :DWORD,:DWORD
BASS_MusicSetPositionScaler             PROTO :DWORD,:DWORD
BASS_Pause                              PROTO
BASS_RecordFree                         PROTO
BASS_RecordGetDeviceDescription         PROTO :DWORD
BASS_RecordGetInfo                      PROTO :DWORD
BASS_RecordGetInput                     PROTO :DWORD
BASS_RecordGetInputName                 PROTO :DWORD
BASS_RecordInit                         PROTO :DWORD
BASS_RecordSetInput                     PROTO :DWORD,:DWORD
BASS_RecordStart                        PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_SampleCreate                       PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_SampleCreateDone                   PROTO
BASS_SampleFree                         PROTO :DWORD
BASS_SampleGetInfo                      PROTO :DWORD,:DWORD
BASS_SampleLoad                         PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_SamplePlay                         PROTO :DWORD
BASS_SamplePlay3D                       PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_SamplePlay3DEx                     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_SamplePlayEx                       PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_SampleSetInfo                      PROTO :DWORD,:DWORD
BASS_SampleStop                         PROTO :DWORD
BASS_Set3DAlgorithm                     PROTO :DWORD
BASS_Set3DFactors                       PROTO :DWORD,:DWORD,:DWORD
BASS_Set3DPosition                      PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_SetEAXParameters                   PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_SetBufferLength                    PROTO :DWORD
BASS_SetCLSID                           PROTO :DWORD
BASS_SetGlobalVolumes                   PROTO :DWORD,:DWORD,:DWORD
BASS_SetLogCurves                       PROTO :DWORD,:DWORD
BASS_SetNetConfig                       PROTO :DWORD,:DWORD
BASS_SetVolume                          PROTO :DWORD
BASS_Start                              PROTO
BASS_Stop                               PROTO
BASS_StreamCreate                       PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_StreamCreateFile                   PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
BASS_StreamCreateURL                    PROTO :DWORD,:DWORD,:DWORD,:DWORD
BASS_StreamFree                         PROTO :DWORD
BASS_StreamGetFilePosition              PROTO :DWORD,:DWORD
BASS_StreamGetLength                    PROTO :DWORD
BASS_StreamGetTags                      PROTO :DWORD,:DWORD
BASS_StreamPlay                         PROTO :DWORD,:DWORD,:DWORD
BASS_StreamPreBuf                       PROTO :DWORD
BASS_Update                             PROTO

