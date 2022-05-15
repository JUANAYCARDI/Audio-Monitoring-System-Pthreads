/***( Manifest constants for user-defined queuing system  )********************/

#define     BUFSIZE       8     /* number of slots in queues */
#define     NUM_QUEUES    5     /* number of queues */
#define     INTERFACE_Q   0     /* queue 0: interface */
#define     COMM_Q		  1     /* queue 1: communication */
#define     CONTROL_Q     2     /* queue 2: control */
#define     AUDIO_Q       3     /* queue 3: audio */
#define     ENV_Q         4     /* queue 4: environment */


/***( User-defined message structure )*****************************************/

typedef struct
{
    int   signal;

    //Interface
    int maxNoise;
    int intervalTime;

    //Comm
    int decibels;
    int contact;
    int date1;
    int date2;

    //Audio
    int newSound;
    int newInterval;
} msg_t;

/***( Signals )****************************************************/

typedef enum{
  sTurnOn,
  sTurnOff,
  sConf,
  sConfData,
  sNewConf
} TO_INTER;

typedef enum{
  sStartComm,
  sStopComm,
  sAlert,
  sData
}TO_COMM;

typedef enum{
  sStartCtrl,
  sStopCtrl,
  sConfCtrl,
  sNewConfData,
  sInterval
}TO_CONTROL;

typedef enum{
  sStartAudio,
  sStopAudio,
  sSound,
  sNewTime
}TO_AUDIO;

typedef enum{
  sStartEnv
}TO_ENV;

/***( User-defined EFSM states )************************************************/

typedef enum{
    InterIdleOn,
    InterIdleOff,
    Display,
    SendNewConf
} INTER_STATES;

typedef enum{
  ControlOn,
  ControlOff,
  WaitingNewConf
} CTRL_STATES;

typedef enum{
    CommOn,
    CommOff
} COMM_STATES;

typedef enum{
  AudioOff,
  AudioOn
} AUDIO_STATES;